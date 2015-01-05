/*
 * wpa_supplicant_dbus_client.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_dbus_client_int.h"

//Prototypes for Private Functions
static void OnNameWatchEventCb (void *, NameWatcherEventType, void *);
static void detectedNameUp(wpa_supplicant_dbus_client *, GDBusConnection *);
static void detectedNameDown(wpa_supplicant_dbus_client *, GDBusConnection *);
static void getProperties (wpa_supplicant_dbus_client *);
static void registerSignals (wpa_supplicant_dbus_client *);
//End of Prototypes for Private Functions

//Arrays  to match with enums (don't change order)
char *eapMethodList[] = {"none", "MD5", "TLS", "MSCHAPV2", "PEAP", "TTLS", "GTC", "OTP", "SIM", "LEAP", "PSK",
		                 "AKA", "AKA'", "FAST", "PAX", "SAKE", "GPSK", "WSC", "IKEV2", "TNC", "PWD"};
//End of Arrays

void *wpa_supplicant_dbus_client_Init (char *busName,
		                               char *objectPath,
									   void *notifyCb,
		                               void *parent) {
	ENTER();

    wpa_supplicant_dbus_client *client = malloc(sizeof(wpa_supplicant_dbus_client));
    if (!client) {
    	ALLOC_FAIL("client");
    	goto FAIL_CLIENT;
    }
	memset (client, 0, sizeof(wpa_supplicant_dbus_client));

	//Set the Notification parameters
	client->m_notifyCb = notifyCb;
	client->m_parent = parent;
	strcpy(client->m_busName, busName);
	strcpy(client->m_objectPath, objectPath);


	//Initialize the Name Watcher for the wpa_supplicant name on the bus
	client->m_nameWatcher = wpa_supplicant_dbus_nameWatcher_Init(busName,
			                                                     OnNameWatchEventCb,
																 (void *) client);
	if (!client->m_nameWatcher) {
		ERROR("Failed to initialize the Name Watcher ... exiting");

		goto FAIL_NAME_WATCHER;
	}

	client->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init(busName,
			                                                                      objectPath);
	if (!client->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}


	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	wpa_supplicant_dbus_nameWatcher_Destroy(client->m_nameWatcher);

FAIL_NAME_WATCHER:
	free(client);

FAIL_CLIENT:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
    return (void *)client;
}

void wpa_supplicant_dbus_client_Start (void *handle) {

	ENTER();

	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	if (!client){
		NULL_POINTER("client");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Name Watcher
	wpa_supplicant_dbus_nameWatcher_Start(client->m_nameWatcher);

    //Start the Event Loop
	client->m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(client->m_loop);

    EXIT();
	return;
}

void wpa_supplicant_dbus_client_Stop (void *handle) {

	ENTER();

	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	if (!client){
		NULL_POINTER("client");
		EXIT_WITH_ERROR();
		return;
	}

	//Clean-up Event loop
	g_main_loop_unref (client->m_loop);

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(client->m_proxyIntrospectable);


	//Stop the Name Watcher
	wpa_supplicant_dbus_nameWatcher_Stop(client->m_nameWatcher);

	EXIT();
	return;
}

void wpa_supplicant_dbus_client_Destroy (void *handle) {

	ENTER();

	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	if (!client){
		NULL_POINTER("client");
		EXIT_WITH_ERROR();
		return;
	}


	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(client->m_proxyIntrospectable);


	//Destroy the Name watcher
	wpa_supplicant_dbus_nameWatcher_Destroy(client->m_nameWatcher);

	//Finally, we free the client
	free (client);

	EXIT();
	return;
}


//Set functions
void wpa_supplicant_dbus_client_SetDbgLvl (void *handle,
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

	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	g_dbus_proxy_set_cached_property (client->m_mainIfProxy,
			                          "DebugLevel",
									  g_variant_new ("s", dbgLvl));

	EXIT();
	return;
}

void wpa_supplicant_dbus_client_SetShowTS (void *handle, bool show) {

	ENTER();
	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	g_dbus_proxy_set_cached_property (client->m_mainIfProxy,
			                          "DebugTimestamp",
								 	  g_variant_new ("b", show));

	EXIT();
	return;
}

void wpa_supplicant_dbus_client_SetShowKeys (void *handle, bool show) {

	ENTER();

	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)handle;
	g_dbus_proxy_set_cached_property (client->m_mainIfProxy,
		 	                          "DebugShowKeys",
									  g_variant_new ("b", show));

	EXIT();
	return;
}


//Private Functions
///////////////////
static void resumeStart (wpa_supplicant_dbus_client *client,
                         GDBusConnection *connection)
{
	ENTER();

	GError * error = NULL;

	//Inform the Parent
	client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_READY, (void *)connection);

	client->m_objectProxy = g_dbus_object_proxy_new (connection,
			                                             client->m_objectPath);
	if (!client->m_objectProxy) {
		ERROR("Failed to create an object proxy for the Client ");
		EXIT_WITH_ERROR();
		return;
	}

	client->m_mainIfProxy = g_dbus_proxy_new_sync (connection,
			                                       G_DBUS_PROXY_FLAGS_NONE,
						   						   NULL,
												   client->m_busName,
												   client->m_objectPath,
												   WPA_SUPPLICANT_MAIN_INTERFACE,
												   NULL,
												   &error);
	if (!client->m_mainIfProxy) {
		ERROR("Can not create an interface proxy for the main interface ");
		ERROR("Error Message: %s ", error->message);
		EXIT_WITH_ERROR();
		return;
	}

	PROGRESS("Created the proxy for the main interface successfully ");

	client->m_ifIfProxy = g_dbus_proxy_new_sync (connection,
			                                     G_DBUS_PROXY_FLAGS_NONE,
					   			  				 NULL,
												 client->m_busName,
												 client->m_objectPath,
												 WPA_SUPPLICANT_INTERFACE_INTERFACE,
												 NULL,
												 &error);
	if (!client->m_ifIfProxy) {
		ERROR("Can not create an interface proxy for the Interface interface ");
		ERROR("Error Message: %s ", error->message);

		EXIT_WITH_ERROR();
		return;
	}
	PROGRESS("Created the proxy for the Interface interface successfully ");

	//Then Register for the signals
	registerSignals(client);

	//Get the Properties
	getProperties(client);

	EXIT();
	return;
}

static void tempStop (wpa_supplicant_dbus_client *client) {
	ENTER();

	//Inform the Parent
	client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_NOT_READY, NULL);

	//Free the dbus proxy Interface
	g_object_unref(client->m_mainIfProxy);
	client->m_mainIfProxy = NULL;

	g_object_unref(client->m_ifIfProxy);
	client->m_ifIfProxy = NULL;

    client->m_connection = NULL;

    EXIT();
}

static void detectedNameUp(wpa_supplicant_dbus_client *client,
                           GDBusConnection *connection) {
	ENTER();

	if (client->m_connection) {
		ERROR("Something is wrong: Stored Connection should be NULL");
		EXIT_WITH_ERROR();
		return;
	}

	client->m_connection = connection;

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(client->m_proxyIntrospectable,
			                                       connection);

	//Now we need to resume the start process
	resumeStart(client, connection);

	EXIT();
	return;
}

static void detectedNameDown(wpa_supplicant_dbus_client *client,
		                     GDBusConnection *connection) {
	ENTER();

	if ((!client->m_connection)  || (client->m_connection != connection)) {
		ERROR("Something is wrong: Stored Connection should be the same as the received value");
		EXIT_WITH_ERROR();
		return;
	}

	client->m_connection = NULL;

	//Now we need to disable the Object and interface proxies
    tempStop(client);

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
	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)parent;

	switch (type) {
	case  NAME_WATCHER_EVENT_NAME_UP:
		connection = (GDBusConnection *)args;
		detectedNameUp (client, connection);
		break;

	case NAME_WATCHER_EVENT_NAME_DOWN:
		connection = (GDBusConnection *)args;
		detectedNameDown (client, connection);
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

	//First Get the client
	wpa_supplicant_dbus_client *client = (wpa_supplicant_dbus_client *)userData;


	if (!strcmp(signal,"InterfaceAdded")) {
	    GVariantIter *iter;
	    gchar *str;

		INFO("Received InterfaceAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("Interface Added : %s", str);

	    /*Note that although the parameter includes a dictionary of the interface properties,
	     * I am not going to parse it here....
	     * Instead, the interface D-Bus client will query for it
	     * */

	    //Call the call back function
	    client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
	} else if (!strcmp(signal, "InterfaceRemoved")) {
	    gchar *str;

	    INFO("Received InterfaceRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("Interface Removed : %s", str);

	    //Call the call back function
        client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_REM_IF, (void *)str );

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


static void getProp_dbgLvl(wpa_supplicant_dbus_client *client) {
	ENTER();

    GVariant *v;
    ClientDbgLvl dbgLvl;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (client->m_mainIfProxy,
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
    client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_SET_DBG_LEVEL, (void *)dbgLvl );

    EXIT();
    return;
}

static void getProp_dbgShowTS(wpa_supplicant_dbus_client *client) {
	ENTER();

	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (client->m_mainIfProxy,
	  	                                  "DebugTimestamp");
    if (!v) {
    	ERROR("Failed to get the property for DebugTimestamp");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &show);
    INFO("DebugTimestamp Property is %d",(int)show);

    //Call the call back function
    client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_TS , (void *)show );

    EXIT();
    return;
}

static void getProp_dbgShowKeys(wpa_supplicant_dbus_client *client) {
	ENTER();

	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (client->m_mainIfProxy,
	  	                                  "DebugShowKeys");
    if (!v) {
    	ERROR("Failed to get the property for DebugShowKeys");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &show);
    INFO("DebugShowKeys Property is %d",(int)show);

    //Call the call back function
    client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_KEYS, (void *)show );

    EXIT();
    return;
}


static void getProp_eapMethods(wpa_supplicant_dbus_client *client) {
	ENTER();
    GVariant *v;
    GVariantIter *iter;
    gchar *str;
    EapMethod method;

    v = g_dbus_proxy_get_cached_property (client->m_mainIfProxy,
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
    		client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_ADD_EAP_METHOD, (void *)method );
    	}
    }

    EXIT();
    return;
}

static void getProp_interfaces (wpa_supplicant_dbus_client *client) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (client->m_mainIfProxy,
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
        client->m_notifyCb(client->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );
    }

    EXIT();
    return;
}

static void getProperties (wpa_supplicant_dbus_client *client) {
	ENTER();

    getProp_dbgLvl(client);
	getProp_dbgShowTS(client);
	getProp_dbgShowKeys(client);
	getProp_eapMethods(client);
	getProp_interfaces(client);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicant_dbus_client *client) {
	ENTER();

	//Register for signals for the main interface
	g_signal_connect (client->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)client);

	//Register for the signals for Interface Interface
	g_signal_connect (client->m_ifIfProxy,
			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)client);

	EXIT();
	return;
}
