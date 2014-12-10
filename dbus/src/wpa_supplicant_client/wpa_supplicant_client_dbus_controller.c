/*
 * wpa_supplicant_client_dbus_controller.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_dbus_controller.h"

//Prototypes for Private Functions
static void OnNameWatchEventCb (void *, NameWatcherEventType, void *);
static void detectedNameUp(wpa_supplicantClient_dbusController *, GDBusConnection *);
static void detectedNameDown(wpa_supplicantClient_dbusController *, GDBusConnection *);
static void getProperties (wpa_supplicantClient_dbusController *);
static void registerSignals (wpa_supplicantClient_dbusController *);
//End of Prototypes for Private Functions

//Arrays  to match with enums (don't change order)
char *eapMethodList[] = {"none", "MD5", "TLS", "MSCHAPV2", "PEAP", "TTLS", "GTC", "OTP", "SIM", "LEAP", "PSK",
		                 "AKA", "AKA'", "FAST", "PAX", "SAKE", "GPSK", "WSC", "IKEV2", "TNC", "PWD"};
//End of Arrays

wpa_supplicantClient_dbusController *wpa_supplicantClient_dbusController_Init (char *busName,
		                                                                       char *objectPath,
																			   void *notifyCb,
		                                                                       void *parent) {
    wpa_supplicantClient_dbusController *controller = malloc(sizeof(wpa_supplicantClient_dbusController));
    if (!controller) {
    	printf("Failed to allocate the Client D-Bus Object ... Exiting\n");
    	goto FAIL_CONTROLLER;
    }
	memset (controller, 0, sizeof(wpa_supplicantClient_dbusController));

	//Set the Notification parameters
	controller->m_notifyCb = notifyCb;
	controller->m_parent = parent;
	strcpy(controller->m_busName, busName);
	strcpy(controller->m_objectPath, objectPath);


	//Initialize the Name Watcher for the wpa_supplicant name on the bus
	controller->m_nameWatcher = wpa_supplicantClient_nameWatcher_Init(busName,
			                                                          OnNameWatchEventCb,
																      (void *) controller);
	if (!controller->m_nameWatcher) {
		printf("Failed to initialize the Name Watcher ... exiting\n");

		goto FAIL_NAME_WATCHER;
	}

	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init( busName,
			                                                                           objectPath);
	if (!controller->m_proxyIntrospectable) {
		printf("Failed to Initialize the Proxy Introspectable Object... Exiting\n");

		goto FAIL_PROXY_INTROSPECTABLE;
	}


	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);
FAIL_NAME_WATCHER:
	free(controller);
FAIL_CONTROLLER:
    return NULL;

SUCCESS:
    return controller;
}

void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Start the Name Watcher
	wpa_supplicantClient_nameWatcher_Start(controller->m_nameWatcher);

    //Start the Event Loop
	controller->m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(controller->m_loop);

	return;
}

void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Clean-up Event loop
	g_main_loop_unref (controller->m_loop);

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(controller->m_proxyIntrospectable);


	//Stop the Name Watcher
	wpa_supplicantClient_nameWatcher_Stop(controller->m_nameWatcher);

	return;
}

void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}


	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);


	//Destroy the Name watcher
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);

	//Finally, we free the controller
	free (controller);

	return;
}


//Set functions
void wpa_supplicantClient_dbusController_SetDbgLvl (wpa_supplicantClient_dbusController *controller,
		                                            ClientDbgLvl lvl) {
	char dbgLvl [MAX_LEN_NAME];

	switch (lvl) {
	case CLIENT_DBG_LVL_VERBOSE:
		strcpy(dbgLvl, "msgdump");
		break;

	case  CLIENT_DBG_LVL_DEBUG:
		strcpy(dbgLvl, "debug");
		break;

	case CLIENT_DBG_LVL_INFO:
		strcpy(dbgLvl, "info");
		break;

	case CLIENT_DBG_LVL_WRN:
		strcpy(dbgLvl, "warning");
		break;

	case CLIENT_DBG_LVL_ERR:
		strcpy(dbgLvl, "erro");
		break;
    }

	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
			                          "DebugLevel",
									  g_variant_new ("s", dbgLvl));

	return;
}

void wpa_supplicantClient_dbusController_SetShowTS (wpa_supplicantClient_dbusController *controller, bool show) {

	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
			                          "DebugTimestamp",
								 	  g_variant_new ("b", show));

	return;
}

void wpa_supplicantClient_dbusController_SetShowKeys (wpa_supplicantClient_dbusController *controller, bool show) {
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "DebugShowKeys",
									  g_variant_new ("b", show));

	return;
}


//Private Functions
///////////////////
static void resumeStart (wpa_supplicantClient_dbusController *controller,
                         GDBusConnection *connection)
{
	GError * error = NULL;

	//Inform the Parent
	controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_READY, (void *)connection);

	controller->m_objectProxy = g_dbus_object_proxy_new (connection,
			                                             controller->m_objectPath);
	if (!controller->m_objectProxy) {
		printf ("Failed to create an object proxy for the Client \n");
		return;
	}

	controller->m_mainIfProxy = g_dbus_proxy_new_sync (connection,
			                                           G_DBUS_PROXY_FLAGS_NONE,
						   						       NULL,
												       controller->m_busName,
												       controller->m_objectPath,
												       WPA_SUPPLICANT_MAIN_INTERFACE,
												       NULL,
												       &error);
	if (!controller->m_mainIfProxy) {
		printf("Can not create an interface proxy for the main interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the main interface successfully \n");
	}

	controller->m_ifIfProxy = g_dbus_proxy_new_sync (connection,
			                                         G_DBUS_PROXY_FLAGS_NONE,
					   			  				     NULL,
												     controller->m_busName,
												     controller->m_objectPath,
												     WPA_SUPPLICANT_INTERFACE_INTERFACE,
												     NULL,
												     &error);
	if (!controller->m_ifIfProxy) {
		printf("Can not create an interface proxy for the Interface interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the Interface interface successfully \n");
	}

	//Then Register for the signals
	registerSignals(controller);

	//Get the Properties
	getProperties(controller);
}

static void tempStop (wpa_supplicantClient_dbusController *controller) {
	//Inform the Parent
	controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_NOT_READY, NULL);

	//Free the dbus proxy Interface
	g_object_unref(controller->m_mainIfProxy);
	controller->m_mainIfProxy = NULL;

	g_object_unref(controller->m_ifIfProxy);
	controller->m_ifIfProxy = NULL;

    controller->m_connection = NULL;
}

static void detectedNameUp(wpa_supplicantClient_dbusController *controller,
                           GDBusConnection *connection) {
	printf ("Entering the detectedNameUp() \n");

	if (controller->m_connection) {
		printf("Something is wrong: Stored Connection should be NULL\n");
		return;
	}

	controller->m_connection = connection;

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(controller->m_proxyIntrospectable,
			                                       connection);

	//Now we need to resume the start process
	resumeStart(controller, connection);

	return;
}

static void detectedNameDown(wpa_supplicantClient_dbusController *controller,
		                     GDBusConnection *connection) {
	printf ("Entering detectedNameDown() \n");

	if ((!controller->m_connection)  || (controller->m_connection != connection)) {
		printf("Something is wrong: Stored Connection should be the same as the received value\n");
		return;
	}

	controller->m_connection = NULL;

	//Now we need to disable the Object and interface proxies
    tempStop(controller);

	return;
}

static void OnNameWatchEventCb (void *parent,
		                        NameWatcherEventType type,
						 void *args) {
	GDBusConnection *connection;

	if (!parent) {
		printf ("NULL pointer passed for parent....Exiting\n");
		return;
	}
	wpa_supplicantClient_dbusController *controller = (wpa_supplicantClient_dbusController *)parent;

	switch (type) {
	case  NAME_WATCHER_EVENT_NAME_UP:
		connection = (GDBusConnection *)args;
		detectedNameUp (controller, connection);
		break;

	case NAME_WATCHER_EVENT_NAME_DOWN:
		connection = (GDBusConnection *)args;
		detectedNameDown (controller, connection);
		break;

	default:
		printf("Invalid Name Watch Event\n");
	}

	return;
}


static void signalNotify (GDBusProxy *dbusIfProxy,
		                  gchar *sender,
                          gchar *signal,
                          GVariant *parameter,
                          gpointer userData) {
	printf("Entering Client signalNotify() :\n");
	printf("------ Sender: %s\n", sender);
	printf("------ Signal: %s\n", signal);

	//First Get the controller
	wpa_supplicantClient_dbusController *controller = (wpa_supplicantClient_dbusController *)userData;


	if (!strcmp(signal,"InterfaceAdded")) {
	    GVariantIter *iter;
	    GVariant *v;
	    gchar *str;

		printf("Received InterfaceAdded Signal\n");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    printf("Added Interface : %s\n", str);

	    /*Note that although the parameter includes a dictionary of the interface properties,
	     * I am not going to parse it here....
	     * Instead, the interface D-Bus controller will query for it
	     * */

	    //Call the call back function
	    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
	} else if (!strcmp(signal, "InterfaceRemoved")) {
	    gchar *str;

	    printf("Received InterfaceRemoved Signal\n");
	    g_variant_get(parameter, "(o)", &str);

	    printf("Removed Interface : %s\n", str);

	    //Call the call back function
        controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_REM_IF, (void *)str );

	} else if (!strcmp(signal, "PropertiesChanged")) {
		printf("Received PropertiesChanged Signal");
	} else if (!strcmp(signal, "NetworkRequest")) {
		printf("Received NetworkRequest Signal");
	} else {
		printf("Received an Invalid Signal");
	}
}


static void getProp_dbgLvl(wpa_supplicantClient_dbusController *controller) {
    GVariant *v;
    ClientDbgLvl dbgLvl;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "DebugLevel");
    if (!v) {
    	printf("Failed to get the property for DebugLevel\n");
    	return;
    }

    g_variant_get(v, "s",&str);
    printf("Debug Level Property is %s\n",str);

    if (!strcmp(str, "msgdump")) {
    	dbgLvl = CLIENT_DBG_LVL_VERBOSE;
    } else if (!strcmp(str, "debug")) {
    	dbgLvl = CLIENT_DBG_LVL_DEBUG;
    } else if (!strcmp(str, "info")) {
    	dbgLvl = CLIENT_DBG_LVL_INFO;
    } else if (!strcmp(str, "warning")) {
    	dbgLvl = CLIENT_DBG_LVL_WRN;
    } else if (!strcmp(str, "erro")) {
    	dbgLvl = CLIENT_DBG_LVL_ERR;
    } else {
    	printf("Received Invalid Debug Level: %s\n", str);
    }

    g_free(str);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_DBG_LEVEL, (void *)dbgLvl );
}

static void getProp_dbgShowTS(wpa_supplicantClient_dbusController *controller) {
	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "DebugTimestamp");
    if (!v) {
    	printf("Failed to get the property for DebugTimestamp\n");
    	return;
    }

    g_variant_get(v, "b", &show);
    printf("DebugTimestamp Property is %d\n",(int)show);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_TS , (void *)show );
}

static void getProp_dbgShowKeys(wpa_supplicantClient_dbusController *controller) {
	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "DebugShowKeys");
    if (!v) {
    	printf("Failed to get the property for DebugShowKeys\n");
    	return;
    }

    g_variant_get(v, "b", &show);
    printf("DebugShowKeys Property is %d\n",(int)show);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_KEYS, (void *)show );
}


static void getProp_eapMethods(wpa_supplicantClient_dbusController *controller) {
    GVariant *v;
    GVariantIter *iter;
    gchar *str;
    EapMethod method;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "EapMethods");
    if (!v) {
    	printf("Failed to get the property for EapMethods\n");
    	return;
    }

    g_variant_get(v, "as", &iter);
    while (g_variant_iter_loop(iter, "s", &str)) {
    	printf("EAP Method : %s\n", str);

    	int i;
    	for (i = 1; i < (int)EAP_LAST ; i++) {
    		if (!strcmp(str,eapMethodList[i])) {
    			method = (EapMethod)i;
    			break;
    		}
    	}
    	if (i == (int)EAP_LAST) {
    		printf("Found Invalid EAP Method: %s\n", str);
    	} else {
        	//Call the call back function
    		controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_EAP_METHOD, (void *)method );
    	}
    }
}

static void getProp_interfaces (wpa_supplicantClient_dbusController *controller) {
    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "Interfaces");
    if (!v) {
    	printf("Failed to get the property for Interfaces\n");
    	return;
    }

    g_variant_get(v, "ao", &iter);
    while (g_variant_iter_loop(iter, "o", &str)) {
    	printf("Interface : %s\n", str);

    	//Call the call back function
        controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
    }
}

static void getProperties (wpa_supplicantClient_dbusController *controller) {
    getProp_dbgLvl(controller);
	getProp_dbgShowTS(controller);
	getProp_dbgShowKeys(controller);
	getProp_eapMethods(controller);
	getProp_interfaces(controller);
}

static void registerSignals (wpa_supplicantClient_dbusController *controller) {
	//Register for signals for the main interface
	g_signal_connect (controller->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)controller);

	//Register for the signals for Interface Interface
	g_signal_connect (controller->m_ifIfProxy,
			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)controller);
}
