/*
 * wpa_supplicant_client_if_dbus_iface.c
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_common.h"
#include "wpa_supplicant_dbus_iface_int.h"

//Prototypes for Private Functions
static void getProperties (wpa_supplicant_dbus_iface *);
static void registerSignals (wpa_supplicant_dbus_iface *);
//End of Prototypes for Private Functions

//Arrays  to match with enums (don't change order)
char *interfaceStateList[] = {"unknown", "inactive", "scanning", "authenticating", "associating", "associated",
		                      "4way_handshake", "group_handshake", "completed", "disconnected"};
//End of Arrays

void *wpa_supplicant_dbus_iface_Init (char *busName,
		                              char *objectPath,
									  void *connection,
		                              void *notifyCb,
		                              void *parent) {
	ENTER();

    wpa_supplicant_dbus_iface *iface = malloc(sizeof(wpa_supplicant_dbus_iface));
    if (!iface) {
    	ALLOC_FAIL("iface");
    	goto FAIL_IFACE;
    }
	memset (iface, 0, sizeof(wpa_supplicant_dbus_iface));

	//Set the Notification parameters
	strcpy(iface->m_busName, busName);
	strcpy(iface->m_objectPath, objectPath);
	iface->m_connection = connection;
	iface->m_notifyCb = notifyCb;
	iface->m_parent = parent;


	iface->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init(busName, objectPath);
	if (!iface->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	free(iface);

FAIL_IFACE:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
    EXIT();
    return (void *)iface;
}

void wpa_supplicant_dbus_iface_Start (void *handle) {
	ENTER();
	GError * error = NULL;

	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	if (!iface){
		NULL_POINTER("iface");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(iface->m_proxyIntrospectable,
			                                       iface->m_connection);


	iface->m_objectProxy = g_dbus_object_proxy_new (iface->m_connection,
			                                        iface->m_objectPath);
	if (!iface->m_objectProxy) {
		ERROR("Failed to create an object proxy for the Client");
		EXIT_WITH_ERROR();
		return;
	}

	iface->m_mainIfProxy = g_dbus_proxy_new_sync (iface->m_connection,
			                                      G_DBUS_PROXY_FLAGS_NONE,
						   						  NULL,
												  iface->m_busName,
												  iface->m_objectPath,
												  WPA_SUPPLICANT_INTERFACE_INTERFACE,
												  NULL,
												  &error);
	if (!iface->m_mainIfProxy) {
		ERROR("Can not create an interface proxy for the main interface");
		ERROR("Error Message: %s", error->message);
		EXIT_WITH_ERROR();
		return;
	} else {
		PROGRESS("Created the proxy for the main interface successfully");
	}


	//Then Register for the signals
	registerSignals(iface);

	//Get the Properties
	getProperties(iface);

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_Stop (void *ifaceHandle) {

	ENTER();

	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)ifaceHandle;
	if (!iface){
		NULL_POINTER("iface");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the dbus proxy Interface
	g_object_unref(iface->m_mainIfProxy);
	iface->m_mainIfProxy = NULL;

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(iface->m_proxyIntrospectable);

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_Destroy (void *handle) {

	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	if (!iface){
		NULL_POINTER("iface");
		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(iface->m_proxyIntrospectable);


	//Finally, we free the iface
	free (iface);

	EXIT();
	return;
}


//Setting of properties
void wpa_supplicant_dbus_iface_SetApScan (void *handle,
		                                  unsigned int scan) {
	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
		 	                          "ApScan",
									  g_variant_new ("u", scan));

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_SetBssExpireAge (void *handle,
		                                        unsigned int age) {
	ENTER();

	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
		 	                          "BSSExpireAge",
									  g_variant_new ("u", age));

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_SetBssExpireCount (void *handle,
		                                          unsigned int count) {
	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
		 	                          "BSSExpireCount",
									   g_variant_new ("u", count));

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_SetCountry (void *handle,
		                                   char *country) {
	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
		 	                          "Country",
									  g_variant_new ("s", country));
	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_SetFastReauth (void *handle,
		                                      bool fastReauth) {
	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
		 	                          "FastReauth",
									  g_variant_new ("b", fastReauth));

	EXIT();
	return;
}

void wpa_supplicant_dbus_iface_SetScanInterval (void *handle,
		                                        unsigned int interval) {
	ENTER();
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)handle;
	g_dbus_proxy_set_cached_property (iface->m_mainIfProxy,
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

	//First Get the iface
	wpa_supplicant_dbus_iface *iface = (wpa_supplicant_dbus_iface *)userData;


	if (!strcmp(signal,"NetworkAdded")) {
	    GVariantIter *iter;
	    gchar *str;

		INFO("Received NetworkAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("New Network Added: %s", str);

	    /*Note that although the parameter includes a dictionary of the network properties,
	     * I am not going to parse it here....
	     * Instead, the Network D-Bus iface will query for it
	     * */

	    //Call the call back function
	    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_ADD_NW, (void *)str );

	} else if (!strcmp(signal, "NetworkRemoved")) {
	    gchar *str;

	    INFO("Received NetworkRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("Network Removed : %s", str);

	    //Call the call back function
        iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_REM_NW, (void *)str );

	}else if (!strcmp(signal,"BSSAdded")) {
	    GVariantIter *iter;
	    gchar *str;

		INFO("Received BSSAdded Signal");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    PROGRESS("New BSS Added : %s", str);

	    /*Note that although the parameter includes a dictionary of the BSS properties,
	     * I am not going to parse it here....
	     * Instead, the BSS D-Bus iface will query for it
	     * */

	    //Call the call back function
	    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_ADD_BSS, (void *)str );

	} else if (!strcmp(signal, "BSSRemoved")) {
	    gchar *str;

	    INFO("Received BSSRemoved Signal");
	    g_variant_get(parameter, "(o)", &str);

	    PROGRESS("BSS Removed : %s", str);

	    //Call the call back function
        iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_REM_BSS, (void *)str );

	} else {
		ERROR("Received an Invalid/UnSupported Signal");
	}

	EXIT();
	return;
}


static void getProp_state (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    gchar *state;
    IfaceState ifState;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
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
    		ifState = (IfaceState)i;
    		break;
    	}
    }
    if (i == (int)IF_STATE_LAST) {
    	ERROR("Found Invalid Interface State: %s", state);
    } else {
    	//Call the call back function
    	iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_STATE, (void *)ifState );
    }

    //Cleanup
    g_free(state);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_scanning (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    bool scanning;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "Scanning");
    if (!v) {
    	ERROR("Failed to get the property for Scanning");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &scanning);

    INFO("Scanning Property is %d",(int)scanning);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_SCANNING , (void *)scanning);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_apScan (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    unsigned long apScan;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "ApScan");
    if (!v) {
    	ERROR("Failed to get the property for ApScan");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &apScan);

    INFO("ApScan Property is %d",(int)apScan);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_AP_SCAN , (void *)apScan);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bssExpireAge (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    unsigned long age;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "BSSExpireAge");
    if (!v) {
    	ERROR("Failed to get the property for BSSExpireAge");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &age);

    INFO("BSSExpireAge Property is %d",(int)age);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE , (void *)age);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bssExpireCount (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    unsigned long count;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "BSSExpireCount");
    if (!v) {
    	ERROR("Failed to get the property for BSSExpireCount");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "u", &count);

    INFO("BSSExpireCount Property is %d",(int)count);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT, (void *)count);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_country (wpa_supplicant_dbus_iface *iface) {
	ENTER();
	GVariant *v;
    gchar *country;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "Country");
    if (!v) {
    	ERROR("Failed to get the property for Country");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &country);
    INFO("Country Property is %s",country);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_COUNTRY , (void *)country);

    //Cleanup
    g_free(country);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_ifName (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    gchar *name;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "Ifname");
    if (!v) {
    	ERROR("Failed to get the property for Ifname");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &name);
    INFO("Ifname Property is %s",name);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_IF_NAME , (void *)name);

    //Cleanup
    g_free(name);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_bridgeIfName (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    gchar *name;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "BridgeIfname");
    if (!v) {
    	ERROR("Failed to get the property for BridgeIfname");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &name);
    INFO("BridgeIfname Property is %s",name);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_BRIDGE_IF_NAME , (void *)name);

    //Cleanup
    g_free(name);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_driver (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    gchar *driver;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "Driver");
    if (!v) {
    	ERROR("Failed to get the property for Driver");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s", &driver);
    INFO("Driver Property is %s",driver);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_DRIVER , (void *)driver);

    //Cleanup
    g_free(driver);
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_fastReauth (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    bool fastReauth;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "FastReauth");
    if (!v) {
    	ERROR("Failed to get the property for FastReauth");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &fastReauth);
    INFO("FastReauth Property is %d",(int)fastReauth);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_FAST_REAUTH , (void *)fastReauth);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_scanInterval (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	GVariant *v;
    unsigned long interval;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
	  	                                  "ScanInterval");
    if (!v) {
    	ERROR("Failed to get the property for ScanInterval");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "i", &interval);

    INFO("ScanInterval Property is %d",(int)interval);

    //Call the call back function
    iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_SET_SCAN_INTERVAL, (void *)interval);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}


static void getProp_networks (wpa_supplicant_dbus_iface *iface) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
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
        iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_ADD_NW, (void *)str );
    }

    EXIT();
    return;
}

static void getProp_bsss(wpa_supplicant_dbus_iface *iface) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (iface->m_mainIfProxy,
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
        iface->m_notifyCb(iface->m_parent, IF_EVENT_TYPE_ADD_BSS, (void *)str );
    }

    EXIT();
    return;
}

static void getProperties (wpa_supplicant_dbus_iface *iface) {
	ENTER();

    getProp_state(iface);
	getProp_scanning(iface);
	getProp_apScan(iface);
	getProp_bssExpireAge(iface);
	getProp_bssExpireCount(iface);
	getProp_country(iface);
	getProp_ifName(iface);
	getProp_bridgeIfName(iface);
	getProp_driver(iface);
	getProp_fastReauth(iface);
	getProp_scanInterval(iface);

	getProp_networks(iface);
	getProp_bsss(iface);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicant_dbus_iface *iface) {
	ENTER();

	//Register for signals for the main interface
	g_signal_connect (iface->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)iface);

	EXIT();
	return;

}
