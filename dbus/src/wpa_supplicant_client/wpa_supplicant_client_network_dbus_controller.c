/*
 * wpa_supplicant_client_network_dbus_controller.c
 *
 *  Created on: Dec 8, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_network_dbus_controller.h"

//Prototypes for Private Functions
static void register4Signals(wpa_supplicantClient_network_dbusController *);
static void getProperties(wpa_supplicantClient_network_dbusController *);
//End of Prototypes for Private Functions


wpa_supplicantClient_network_dbusController *wpa_supplicantClient_network_dbusController_Init (char *busName,
		                                                                                       char *objectPath,
																							   void *connection,
																			                   void *notifyCb,
		                                                                                       void *parent) {
	wpa_supplicantClient_network_dbusController *controller = malloc(sizeof(wpa_supplicantClient_network_dbusController));
    if (!controller) {
    	printf("Failed to allocate the Network D-Bus Object ... Exiting\n");
    	goto FAIL_CONTROLLER;
    }
	memset (controller, 0, sizeof(wpa_supplicantClient_network_dbusController));

    //Set the bus/object Names
	strcpy(controller->m_busName, busName);
	strcpy(controller->m_objectPath, objectPath);

	controller->m_connection = connection;

	//Set the Notification parameters
	controller->m_notifyCb = notifyCb;
	controller->m_parent = parent;

	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init( busName,
			                                                                           objectPath);
	if (!controller->m_proxyIntrospectable) {
		printf("Failed to Initialize the Proxy Introspectable for Network Object... Exiting\n");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	free(controller);
FAIL_CONTROLLER:
    return NULL;

SUCCESS:
    return controller;
}

void wpa_supplicantClient_network_dbusController_Start (wpa_supplicantClient_network_dbusController *controller) {
	GError * error = NULL;

	if (!controller){
		printf("Passing NULL for Controller to the function ...Exiting\n");
		return;
	}

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(controller->m_proxyIntrospectable,
			                                       controller->m_connection);


	controller->m_objectProxy = g_dbus_object_proxy_new (controller->m_connection,
			                                             controller->m_objectPath);
	if (!controller->m_objectProxy) {
		printf ("Failed to create an object proxy for the Network \n");
		return;
	}

	controller->m_ifProxy = g_dbus_proxy_new_sync(controller->m_connection,
			                                      G_DBUS_PROXY_FLAGS_NONE,
						   						  NULL,
												  controller->m_busName,
												  controller->m_objectPath,
												  WPA_SUPPLICANT_NETWORK_INTERFACE,
												  NULL,
												  &error);
	if (!controller->m_ifProxy) {
		printf("Can not create an interface proxy for the Network \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the interface proxy for the Network interface successfully \n");
	}

	//Register for Signals
	register4Signals(controller);

	//Collect Property values
    getProperties(controller);

	return;
}

void wpa_supplicantClient_network_dbusController_Stop (wpa_supplicantClient_network_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Free the proxy
	g_object_unref(controller->m_ifProxy);
	controller->m_ifProxy = NULL;

	//TODO Need to free the m_objectProxy (figure out how)
	controller->m_objectProxy = NULL;

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(controller->m_proxyIntrospectable);


	return;
}

void wpa_supplicantClient_network_dbusController_Destroy (wpa_supplicantClient_network_dbusController *controller) {
	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);

	//Finally, we free the controller
	free (controller);

	return;
}


void wpa_supplicantClient_network_dbusController_SetEnabled (wpa_supplicantClient_network_dbusController *controller,
		                                                     bool enabled) {
	g_dbus_proxy_set_cached_property (controller->m_ifProxy,
				                      "Enabled",
									   g_variant_new ("b", enabled));

	return;
}



//Private Functions
///////////////////

static void signalNotify (GDBusProxy *dbusIfProxy,
		                  gchar *sender,
                          gchar *signal,
                          GVariant *parameter,
                          gpointer userData) {
	printf("Entering Network signalNotify() :\n");
	printf("------ Sender: %s\n", sender);
	printf("------ Signal: %s\n", signal);

	//First Get the Controller pointer
	wpa_supplicantClient_network_dbusController *controller = (wpa_supplicantClient_network_dbusController *)userData;

	printf("Received an Invalid/UnSupported Signal");
}

static void register4Signals(wpa_supplicantClient_network_dbusController *controller) {

    g_signal_connect (controller->m_ifProxy,
    			      "g-signal",
    				  G_CALLBACK(signalNotify),
    				  (void *)controller);

    return;
}

static void getProp_enabled(wpa_supplicantClient_network_dbusController *controller) {
	GVariant *v;
    bool isEnabled;

    v = g_dbus_proxy_get_cached_property (controller->m_ifProxy,
	  	                                  "Enabled");
    if (!v) {
    	printf("Failed to get the property for Enabled\n");
    	return;
    }

    g_variant_get(v, "b", &isEnabled);
    printf("Enabled Property is %d\n",(int)isEnabled);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, NETWORK_EVENT_TYPE_SET_ENABLED, (void *)isEnabled);


    //Cleanup
    g_variant_unref(v);
}

static void getProperties(wpa_supplicantClient_network_dbusController *controller) {
	getProp_enabled(controller);
}
