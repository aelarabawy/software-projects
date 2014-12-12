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
	ENTER();

    wpa_supplicantClient_dbusController *controller = malloc(sizeof(wpa_supplicantClient_dbusController));
    if (!controller) {
    	ALLOC_FAIL("controller");
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
		ERROR("Failed to initialize the Name Watcher ... exiting");

		goto FAIL_NAME_WATCHER;
	}

	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init( busName,
			                                                                           objectPath);
	if (!controller->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}


	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);

FAIL_NAME_WATCHER:
	free(controller);

FAIL_CONTROLLER:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
    return controller;
}

void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *controller) {

	ENTER();

	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Name Watcher
	wpa_supplicantClient_nameWatcher_Start(controller->m_nameWatcher);

    //Start the Event Loop
	controller->m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(controller->m_loop);

    EXIT();
	return;
}

void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *controller) {

	ENTER();

	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}

	//Clean-up Event loop
	g_main_loop_unref (controller->m_loop);

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(controller->m_proxyIntrospectable);


	//Stop the Name Watcher
	wpa_supplicantClient_nameWatcher_Stop(controller->m_nameWatcher);

	EXIT();
	return;
}

void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *controller) {

	ENTER();

	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}


	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);


	//Destroy the Name watcher
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);

	//Finally, we free the controller
	free (controller);

	EXIT();
	return;
}


//Set functions
void wpa_supplicantClient_dbusController_SetDbgLvl (wpa_supplicantClient_dbusController *controller,
		                                            ClientDbgLvl lvl) {

	ENTER();

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

	EXIT();
	return;
}

void wpa_supplicantClient_dbusController_SetShowTS (wpa_supplicantClient_dbusController *controller, bool show) {

	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
			                          "DebugTimestamp",
								 	  g_variant_new ("b", show));

	EXIT();
	return;
}

void wpa_supplicantClient_dbusController_SetShowKeys (wpa_supplicantClient_dbusController *controller, bool show) {

	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "DebugShowKeys",
									  g_variant_new ("b", show));

	EXIT();
	return;
}


//Private Functions
///////////////////
static void resumeStart (wpa_supplicantClient_dbusController *controller,
                         GDBusConnection *connection)
{
	ENTER();

	GError * error = NULL;

	//Inform the Parent
	controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_READY, (void *)connection);

	controller->m_objectProxy = g_dbus_object_proxy_new (connection,
			                                             controller->m_objectPath);
	if (!controller->m_objectProxy) {
		ERROR("Failed to create an object proxy for the Client ");
		EXIT_WITH_ERROR();
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
		ERROR("Can not create an interface proxy for the main interface ");
		ERROR("Error Message: %s ", error->message);
		EXIT_WITH_ERROR();
		return;
	}

	PROGRESS("Created the proxy for the main interface successfully ");

	controller->m_ifIfProxy = g_dbus_proxy_new_sync (connection,
			                                         G_DBUS_PROXY_FLAGS_NONE,
					   			  				     NULL,
												     controller->m_busName,
												     controller->m_objectPath,
												     WPA_SUPPLICANT_INTERFACE_INTERFACE,
												     NULL,
												     &error);
	if (!controller->m_ifIfProxy) {
		ERROR("Can not create an interface proxy for the Interface interface ");
		ERROR("Error Message: %s ", error->message);

		EXIT_WITH_ERROR();
		return;
	}
	PROGRESS("Created the proxy for the Interface interface successfully ");

	//Then Register for the signals
	registerSignals(controller);

	//Get the Properties
	getProperties(controller);

	EXIT();
	return;
}

static void tempStop (wpa_supplicantClient_dbusController *controller) {
	ENTER();

	//Inform the Parent
	controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_NOT_READY, NULL);

	//Free the dbus proxy Interface
	g_object_unref(controller->m_mainIfProxy);
	controller->m_mainIfProxy = NULL;

	g_object_unref(controller->m_ifIfProxy);
	controller->m_ifIfProxy = NULL;

    controller->m_connection = NULL;

    EXIT();
}

static void detectedNameUp(wpa_supplicantClient_dbusController *controller,
                           GDBusConnection *connection) {
	ENTER();

	if (controller->m_connection) {
		ERROR("Something is wrong: Stored Connection should be NULL");
		EXIT_WITH_ERROR();
		return;
	}

	controller->m_connection = connection;

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(controller->m_proxyIntrospectable,
			                                       connection);

	//Now we need to resume the start process
	resumeStart(controller, connection);

	EXIT();
	return;
}

static void detectedNameDown(wpa_supplicantClient_dbusController *controller,
		                     GDBusConnection *connection) {
	ENTER();

	if ((!controller->m_connection)  || (controller->m_connection != connection)) {
		ERROR("Something is wrong: Stored Connection should be the same as the received value");
		EXIT_WITH_ERROR();
		return;
	}

	controller->m_connection = NULL;

	//Now we need to disable the Object and interface proxies
    tempStop(controller);

    EXIT();
	return;
}

static void OnNameWatchEventCb (void *parent,
		                        NameWatcherEventType type,
								void *args) {
	ENTER();

	GDBusConnection *connection;

	if (!parent) {
		NULL_POINTER("parent");
		EXIT_WITH_ERROR();
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
		ERROR("Invalid Name Watch Event");
	}

	EXIT();
	return;
}


static void signalNotify (GDBusProxy *dbusIfProxy,
		                  gchar *sender,
                          gchar *signal,
                          GVariant *parameter,
                          gpointer userData) {
	ENTER();
	INFO("------ Sender: %s", sender);
	INFO("------ Signal: %s", signal);

	//First Get the controller
	wpa_supplicantClient_dbusController *controller = (wpa_supplicantClient_dbusController *)userData;


	if (!strcmp(signal,"InterfaceAdded")) {
	    GVariantIter *iter;
	    GVariant *v;
	    gchar *str;

		INFO("Received InterfaceAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("Interface Added : %s", str);

	    /*Note that although the parameter includes a dictionary of the interface properties,
	     * I am not going to parse it here....
	     * Instead, the interface D-Bus controller will query for it
	     * */

	    //Call the call back function
	    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
	} else if (!strcmp(signal, "InterfaceRemoved")) {
	    gchar *str;

	    INFO("Received InterfaceRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("Interface Removed : %s", str);

	    //Call the call back function
        controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_REM_IF, (void *)str );

	} else if (!strcmp(signal, "PropertiesChanged")) {
		INFO("Received PropertiesChanged Signal");
	} else if (!strcmp(signal, "NetworkRequest")) {
		INFO("Received NetworkRequest Signal");
	} else {
		INFO("Received an Invalid Signal");
	}

	EXIT();
	return;
}


static void getProp_dbgLvl(wpa_supplicantClient_dbusController *controller) {
	ENTER();

    GVariant *v;
    ClientDbgLvl dbgLvl;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "DebugLevel");
    if (!v) {
    	ERROR("Failed to get the property for DebugLevel");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s",&str);
    INFO("Debug Level Property is %s",str);

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
    	ERROR("Received Invalid Debug Level: %s", str);
    }

    g_free(str);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_DBG_LEVEL, (void *)dbgLvl );

    EXIT();
    return;
}

static void getProp_dbgShowTS(wpa_supplicantClient_dbusController *controller) {
	ENTER();

	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "DebugTimestamp");
    if (!v) {
    	ERROR("Failed to get the property for DebugTimestamp");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &show);
    INFO("DebugTimestamp Property is %d",(int)show);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_TS , (void *)show );

    EXIT();
    return;
}

static void getProp_dbgShowKeys(wpa_supplicantClient_dbusController *controller) {
	ENTER();

	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "DebugShowKeys");
    if (!v) {
    	ERROR("Failed to get the property for DebugShowKeys");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &show);
    INFO("DebugShowKeys Property is %d",(int)show);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_KEYS, (void *)show );

    EXIT();
    return;
}


static void getProp_eapMethods(wpa_supplicantClient_dbusController *controller) {
	ENTER();
    GVariant *v;
    GVariantIter *iter;
    gchar *str;
    EapMethod method;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "EapMethods");
    if (!v) {
    	ERROR("Failed to get the property for EapMethods");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "as", &iter);
    while (g_variant_iter_loop(iter, "s", &str)) {
    	INFO("EAP Method : %s", str);

    	int i;
    	for (i = 1; i < (int)EAP_LAST ; i++) {
    		if (!strcmp(str,eapMethodList[i])) {
    			method = (EapMethod)i;
    			break;
    		}
    	}
    	if (i == (int)EAP_LAST) {
    		ERROR("Found Invalid EAP Method: %s", str);
    	} else {
        	//Call the call back function
    		controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_EAP_METHOD, (void *)method );
    	}
    }

    EXIT();
    return;
}

static void getProp_interfaces (wpa_supplicantClient_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "Interfaces");
    if (!v) {
    	ERROR("Failed to get the property for Interfaces");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "ao", &iter);
    while (g_variant_iter_loop(iter, "o", &str)) {
    	INFO("Interface : %s", str);

    	//Call the call back function
        controller->m_notifyCb(controller->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
    }

    EXIT();
    return;
}

static void getProperties (wpa_supplicantClient_dbusController *controller) {
	ENTER();

    getProp_dbgLvl(controller);
	getProp_dbgShowTS(controller);
	getProp_dbgShowKeys(controller);
	getProp_eapMethods(controller);
	getProp_interfaces(controller);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicantClient_dbusController *controller) {
	ENTER();

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

	EXIT();
	return;
}
