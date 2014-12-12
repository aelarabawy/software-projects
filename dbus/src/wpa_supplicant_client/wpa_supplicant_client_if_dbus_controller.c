/*
 * wpa_supplicant_client_if_dbus_controller.c
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_if_dbus_controller.h"

//Prototypes for Private Functions
static void getProperties (wpa_supplicantClient_if_dbusController *);
static void registerSignals (wpa_supplicantClient_if_dbusController *);
//End of Prototypes for Private Functions

//Arrays  to match with enums (don't change order)
char *interfaceStateList[] = {"unknown", "inactive", "scanning", "authenticating", "associating", "associated",
		                      "4way_handshake", "group_handshake", "completed", "disconnected"};
//End of Arrays

wpa_supplicantClient_if_dbusController *wpa_supplicantClient_if_dbusController_Init (char *busName,
		                                                                             char *objectPath,
																					 void *connection,
		                                                                             void *notifyCb,
		                                                                             void *parent) {
	ENTER();

    wpa_supplicantClient_if_dbusController *controller = malloc(sizeof(wpa_supplicantClient_if_dbusController));
    if (!controller) {
    	ALLOC_FAIL("controller");
    	goto FAIL_CONTROLLER;
    }
	memset (controller, 0, sizeof(wpa_supplicantClient_if_dbusController));

	//Set the Notification parameters
	strcpy(controller->m_busName, busName);
	strcpy(controller->m_objectPath, objectPath);
	controller->m_connection = connection;
	controller->m_notifyCb = notifyCb;
	controller->m_parent = parent;


	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init(busName, objectPath);
	if (!controller->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	free(controller);

FAIL_CONTROLLER:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
    EXIT();
    return controller;
}

void wpa_supplicantClient_if_dbusController_Start (wpa_supplicantClient_if_dbusController *controller) {

	ENTER();
	GError * error = NULL;

	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(controller->m_proxyIntrospectable,
			                                       controller->m_connection);


	controller->m_objectProxy = g_dbus_object_proxy_new (controller->m_connection,
			                                             controller->m_objectPath);
	if (!controller->m_objectProxy) {
		ERROR("Failed to create an object proxy for the Client");
		EXIT_WITH_ERROR();
		return;
	}

	controller->m_mainIfProxy = g_dbus_proxy_new_sync (controller->m_connection,
			                                           G_DBUS_PROXY_FLAGS_NONE,
						   						       NULL,
												       controller->m_busName,
												       controller->m_objectPath,
													   WPA_SUPPLICANT_INTERFACE_INTERFACE,
												       NULL,
												       &error);
	if (!controller->m_mainIfProxy) {
		ERROR("Can not create an interface proxy for the main interface");
		ERROR("Error Message: %s", error->message);
		EXIT_WITH_ERROR();
		return;
	} else {
		PROGRESS("Created the proxy for the main interface successfully");
	}


	//Then Register for the signals
	registerSignals(controller);

	//Get the Properties
	getProperties(controller);

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_Stop (wpa_supplicantClient_if_dbusController *controller) {

	ENTER();
	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the dbus proxy Interface
	g_object_unref(controller->m_mainIfProxy);
	controller->m_mainIfProxy = NULL;

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(controller->m_proxyIntrospectable);

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_Destroy (wpa_supplicantClient_if_dbusController *controller) {

	if (!controller){
		NULL_POINTER("controller");
		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);


	//Finally, we free the controller
	free (controller);

	EXIT();
	return;
}


//Setting of properties
void wpa_supplicantClient_if_dbusController_SetApScan (wpa_supplicantClient_if_dbusController *controller,
		                                               unsigned int scan) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "ApScan",
									  g_variant_new ("u", scan));

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_SetBssExpireAge (wpa_supplicantClient_if_dbusController *controller,
		                                                     unsigned int age) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "BSSExpireAge",
									  g_variant_new ("u", age));

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_SetBssExpireCount (wpa_supplicantClient_if_dbusController *controller,
		                                                       unsigned int count) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "BSSExpireCount",
									  g_variant_new ("u", count));

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_SetCountry (wpa_supplicantClient_if_dbusController *controller,
		                                                char *country) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "Country",
									  g_variant_new ("s", country));
	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_SetFastReauth (wpa_supplicantClient_if_dbusController *controller,
		                                                   bool fastReauth) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "FastReauth",
									  g_variant_new ("b", fastReauth));

	EXIT();
	return;
}

void wpa_supplicantClient_if_dbusController_SetScanInterval (wpa_supplicantClient_if_dbusController *controller,
		                                                     unsigned int interval) {
	ENTER();
	g_dbus_proxy_set_cached_property (controller->m_mainIfProxy,
		 	                          "ScanInterval",
									  g_variant_new ("i", interval));

	EXIT();
	return;
}

//Private Functions
///////////////////

static void signalNotify (GDBusProxy *dbusIfProxy,
		                  gchar *sender,
                          gchar *signal,
                          GVariant *parameter,
                          gpointer userData) {
	ENTER();
	INFO("------ Sender: %s", sender);
	INFO("------ Signal: %s", signal);

	//First Get the controller
	wpa_supplicantClient_if_dbusController *controller = (wpa_supplicantClient_if_dbusController *)userData;


	if (!strcmp(signal,"NetworkAdded")) {
	    GVariantIter *iter;
	    GVariant *v;
	    gchar *str;

		INFO("Received NetworkAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("New Network Added: %s", str);

	    /*Note that although the parameter includes a dictionary of the network properties,
	     * I am not going to parse it here....
	     * Instead, the Network D-Bus controller will query for it
	     * */

	    //Call the call back function
	    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_ADD_NW, (void *)str );

	} else if (!strcmp(signal, "NetworkRemoved")) {
	    gchar *str;

	    INFO("Received NetworkRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("Network Removed : %s", str);

	    //Call the call back function
        controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_REM_NW, (void *)str );

	}else if (!strcmp(signal,"BSSAdded")) {
	    GVariantIter *iter;
	    GVariant *v;
	    gchar *str;

		INFO("Received BSSAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("New BSS Added : %s", str);

	    /*Note that although the parameter includes a dictionary of the BSS properties,
	     * I am not going to parse it here....
	     * Instead, the BSS D-Bus controller will query for it
	     * */

	    //Call the call back function
	    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_ADD_BSS, (void *)str );

	} else if (!strcmp(signal, "BSSRemoved")) {
	    gchar *str;

	    INFO("Received BSSRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("BSS Removed : %s", str);

	    //Call the call back function
        controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_REM_BSS, (void *)str );

	} else {
		ERROR("Received an Invalid/UnSupported Signal");
	}

	EXIT();
	return;
}


static void getProp_state (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    gchar *state;
    IfState ifState;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "State");
    if (!v) {
    	ERROR("Failed to get the property for State");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &state);
    INFO("State Property is %s", state);

    int i;
    for (i = 0; i < (int)IF_STATE_LAST ; i++) {
    	if (!strcmp(state, interfaceStateList[i])) {
    		ifState = (IfState)i;
    		break;
    	}
    }
    if (i == (int)IF_STATE_LAST) {
    	ERROR("Found Invalid Interface State: %s", state);
    } else {
    	//Call the call back function
    	controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_STATE, (void *)ifState );
    }

    //Cleanup
    g_free(state);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_scanning (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    bool scanning;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Scanning");
    if (!v) {
    	ERROR("Failed to get the property for Scanning");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &scanning);

    INFO("Scanning Property is %d",(int)scanning);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_SCANNING , (void *)scanning);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_apScan (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    unsigned long apScan;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "ApScan");
    if (!v) {
    	ERROR("Failed to get the property for ApScan");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &apScan);

    INFO("ApScan Property is %d",(int)apScan);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_AP_SCAN , (void *)apScan);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bssExpireAge (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    unsigned long age;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "BSSExpireAge");
    if (!v) {
    	ERROR("Failed to get the property for BSSExpireAge");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &age);

    INFO("BSSExpireAge Property is %d",(int)age);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE , (void *)age);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bssExpireCount (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    unsigned long count;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "BSSExpireCount");
    if (!v) {
    	ERROR("Failed to get the property for BSSExpireCount");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &count);

    INFO("BSSExpireCount Property is %d",(int)count);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT, (void *)count);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_country (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();
	GVariant *v;
    gchar *country;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Country");
    if (!v) {
    	ERROR("Failed to get the property for Country");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &country);
    INFO("Country Property is %s",country);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_COUNTRY , (void *)country);

    //Cleanup
    g_free(country);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_ifName (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    gchar *name;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Ifname");
    if (!v) {
    	ERROR("Failed to get the property for Ifname");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &name);
    INFO("Ifname Property is %s",name);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_IF_NAME , (void *)name);

    //Cleanup
    g_free(name);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bridgeIfName (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    gchar *name;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "BridgeIfname");
    if (!v) {
    	ERROR("Failed to get the property for BridgeIfname");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &name);
    INFO("BridgeIfname Property is %s",name);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_BRIDGE_IF_NAME , (void *)name);

    //Cleanup
    g_free(name);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_driver (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    gchar *driver;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Driver");
    if (!v) {
    	ERROR("Failed to get the property for Driver");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &driver);
    INFO("Driver Property is %s",driver);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_DRIVER , (void *)driver);

    //Cleanup
    g_free(driver);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_fastReauth (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    bool fastReauth;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "FastReauth");
    if (!v) {
    	ERROR("Failed to get the property for FastReauth");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &fastReauth);
    INFO("FastReauth Property is %d",(int)fastReauth);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_FAST_REAUTH , (void *)fastReauth);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_scanInterval (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	GVariant *v;
    unsigned long interval;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "ScanInterval");
    if (!v) {
    	ERROR("Failed to get the property for ScanInterval");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "i", &interval);

    INFO("ScanInterval Property is %d",(int)interval);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_SET_SCAN_INTERVAL, (void *)interval);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}


static void getProp_networks (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "Networks");
    if (!v) {
    	ERROR("Failed to get the property for Networks");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "ao", &iter);
    while (g_variant_iter_loop(iter, "o", &str)) {
    	INFO("New Network: %s", str);

    	//Call the call back function
        controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_ADD_NW, (void *)str );
    }

    EXIT();
    return;
}

static void getProp_bsss(wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "BSSs");
    if (!v) {
    	ERROR("Failed to get the property for BSSs");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "ao", &iter);
    while (g_variant_iter_loop(iter, "o", &str)) {
    	INFO("New BSS: %s", str);

    	//Call the call back function
        controller->m_notifyCb(controller->m_parent, IF_EVENT_TYPE_ADD_BSS, (void *)str );
    }

    EXIT();
    return;
}

static void getProperties (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

    getProp_state(controller);
	getProp_scanning(controller);
	getProp_apScan(controller);
	getProp_bssExpireAge(controller);
	getProp_bssExpireCount(controller);
	getProp_country(controller);
	getProp_ifName(controller);
	getProp_bridgeIfName(controller);
	getProp_driver(controller);
	getProp_fastReauth(controller);
	getProp_scanInterval(controller);

	getProp_networks(controller);
	getProp_bsss(controller);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicantClient_if_dbusController *controller) {
	ENTER();

	//Register for signals for the main interface
	g_signal_connect (controller->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)controller);

	EXIT();
	return;

}
