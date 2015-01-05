/*
 * wpa_supplicant_dbus_network.c
 *
 *  Created on: Dec 8, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_dbus_network_int.h"

//Prototypes for Private Functions
static void register4Signals(wpa_supplicant_dbus_network *);
static void getProperties(wpa_supplicant_dbus_network *);
//End of Prototypes for Private Functions


void *wpa_supplicant_dbus_network_Init (char *busName,
		                                char *objectPath,
										void *connection,
										void *notifyCb,
		                                void *parent) {
	ENTER();

	wpa_supplicant_dbus_network *network = malloc(sizeof(wpa_supplicant_dbus_network));
    if (!network) {
    	ALLOC_FAIL("network");
    	goto FAIL_NETWORK;
    }
	memset (network, 0, sizeof(wpa_supplicant_dbus_network));

    //Set the bus/object Names
	strcpy(network->m_busName, busName);
	strcpy(network->m_objectPath, objectPath);

	network->m_connection = connection;

	//Set the Notification parameters
	network->m_notifyCb = notifyCb;
	network->m_parent = parent;

	network->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init( busName,
			                                                                           objectPath);
	if (!network->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable for Network Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	free(network);

FAIL_NETWORK:
	EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
    return (void *)network;
}

void wpa_supplicant_dbus_network_Start (void *nwHandle) {
	ENTER();

	wpa_supplicant_dbus_network *network = (wpa_supplicant_dbus_network *)nwHandle;
	GError * error = NULL;

	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(network->m_proxyIntrospectable,
			                                       network->m_connection);


	network->m_objectProxy = g_dbus_object_proxy_new (network->m_connection,
			                                             network->m_objectPath);
	if (!network->m_objectProxy) {
		ERROR("Failed to create an object proxy for the Network");
		EXIT_WITH_ERROR();
		return;
	}

	network->m_ifProxy = g_dbus_proxy_new_sync(network->m_connection,
			                                      G_DBUS_PROXY_FLAGS_NONE,
						   						  NULL,
												  network->m_busName,
												  network->m_objectPath,
												  WPA_SUPPLICANT_NETWORK_INTERFACE,
												  NULL,
												  &error);
	if (!network->m_ifProxy) {
		ERROR("Can not create an interface proxy for the Network");
		ERROR("Error Message: %s", error->message);

		EXIT_WITH_ERROR();
		return;
	} else {
		PROGRESS("Created the interface proxy for the Network interface successfully ");
	}

	//Register for Signals
	register4Signals(network);

	//Collect Property values
    getProperties(network);

    EXIT();
	return;
}

void wpa_supplicant_dbus_network_Stop (void *nwHandle) {

	ENTER();

	wpa_supplicant_dbus_network *network = (wpa_supplicant_dbus_network *)nwHandle;
	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the proxy
	g_object_unref(network->m_ifProxy);
	network->m_ifProxy = NULL;

	//TODO Need to free the m_objectProxy (figure out how)
	network->m_objectProxy = NULL;

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(network->m_proxyIntrospectable);

	EXIT();
	return;
}

void wpa_supplicant_dbus_network_Destroy (void *nwHandle) {

	ENTER();

	wpa_supplicant_dbus_network *network = (wpa_supplicant_dbus_network *)nwHandle;
	if (!network){
		NULL_POINTER("network");

		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(network->m_proxyIntrospectable);

	//Finally, we free the network
	free (network);

	EXIT();
	return;
}


void wpa_supplicant_dbus_network_SetEnabled (void *nwHandle,
		                                     bool enabled) {
	ENTER();

	wpa_supplicant_dbus_network *network = (wpa_supplicant_dbus_network *)nwHandle;

	g_dbus_proxy_set_cached_property (network->m_ifProxy,
				                      "Enabled",
									   g_variant_new ("b", enabled));

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

	//First Get the network pointer
	//wpa_supplicant_dbus_network *network = (wpa_supplicant_dbus_network *)userData;

	ERROR("Received an Invalid/UnSupported Signal");
	EXIT_WITH_ERROR();
}

static void register4Signals(wpa_supplicant_dbus_network *network) {
    ENTER();

    g_signal_connect (network->m_ifProxy,
    			      "g-signal",
    				  G_CALLBACK(signalNotify),
    				  (void *)network);

    EXIT();
    return;
}

static void getProp_enabled(wpa_supplicant_dbus_network *network) {
	ENTER();

	GVariant *v;
    bool isEnabled;

    v = g_dbus_proxy_get_cached_property (network->m_ifProxy,
	  	                                  "Enabled");
    if (!v) {
    	ERROR("Failed to get the property for Enabled");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &isEnabled);
    INFO("Enabled Property is %d",(int)isEnabled);

    //Call the call back function
    network->m_notifyCb(network->m_parent, NETWORK_EVENT_TYPE_SET_ENABLED, (void *)isEnabled);


    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProperties(wpa_supplicant_dbus_network *network) {
	ENTER();

	getProp_enabled(network);

	EXIT();
	return;
}
