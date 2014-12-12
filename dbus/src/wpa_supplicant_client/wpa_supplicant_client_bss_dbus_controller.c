/*
 * wpa_supplicant_client_bss_dbus_controller.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */
#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_bss_dbus_controller.h"

//Prototypes for Private Functions
static void getProperties (wpa_supplicantClient_bss_dbusController *);
static void registerSignals (wpa_supplicantClient_bss_dbusController *);
//End of Prototypes for Private Functions

wpa_supplicantClient_bss_dbusController *wpa_supplicantClient_bss_dbusController_Init (char *busName,
		                                                                               char *objectPath,
																					   void *connection,
		                                                                               void *notifyCb,
		                                                                               void *parent) {
	ENTER();

    wpa_supplicantClient_bss_dbusController *controller = malloc(sizeof(wpa_supplicantClient_bss_dbusController));
    if (!controller) {
    	ALLOC_FAIL("controller");
    	goto FAIL_CONTROLLER;
    }
	memset (controller, 0, sizeof(wpa_supplicantClient_bss_dbusController));

	//Set the Notification parameters
	strcpy(controller->m_busName, busName);
	strcpy(controller->m_objectPath, objectPath);
	controller->m_connection = connection;
	controller->m_notifyCb = notifyCb;
	controller->m_parent = parent;


	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init(busName, objectPath);
	if (!controller->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable BSS Object... Exiting");

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

void wpa_supplicantClient_bss_dbusController_Start (wpa_supplicantClient_bss_dbusController *controller) {

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
		ERROR("Failed to create an object proxy for the BSS ");
		EXIT_WITH_ERROR();
		return;
	}

	controller->m_mainIfProxy = g_dbus_proxy_new_sync (controller->m_connection,
			                                           G_DBUS_PROXY_FLAGS_NONE,
						   						       NULL,
												       controller->m_busName,
												       controller->m_objectPath,
													   WPA_SUPPLICANT_BSS_INTERFACE,
												       NULL,
												       &error);
	if (!controller->m_mainIfProxy) {
		ERROR("Can not create a proxy for the BSS interface ");
		ERROR("Error Message: %s ", error->message);

		EXIT_WITH_ERROR();
		return;
	}

	PROGRESS("Created the proxy for the BSS interface successfully ");


	//Then Register for the signals
	registerSignals(controller);

	//Get the Properties
	getProperties(controller);

	EXIT();
	return;
}


void wpa_supplicantClient_bss_dbusController_Stop (wpa_supplicantClient_bss_dbusController *controller) {

	ENTER();

	if (!controller) {
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


void wpa_supplicantClient_bss_dbusController_Destroy (wpa_supplicantClient_bss_dbusController *controller) {

	ENTER();

	if (!controller) {
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
	wpa_supplicantClient_bss_dbusController *controller = (wpa_supplicantClient_bss_dbusController *)userData;

	ERROR("Received an Invalid/UnSupported Signal");
	EXIT_WITH_ERROR();
	return;
}

static void getProp_bssid(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned char bssid[10];
    unsigned char index;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "BSSID");
    if (!v) {
    	ERROR("Failed to get the property for BSSID");
    	EXIT_WITH_ERROR();
    	return;
    }
    index = 1;
    g_variant_get(v, "ay", &iter);
    while (g_variant_iter_loop(iter, "y", &bssid[index])) {
    	INFO("BSSID[%d] = %d", index -1, bssid[index]);
    	index++;
    }

    //Now put the count in the array
    bssid[0] = index -1;

	//Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_BSSID, (void *)bssid );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_ssid(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned char ssid[10];
    unsigned char index;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "SSID");
    if (!v) {
    	ERROR("Failed to get the property for SSID");
    	EXIT_WITH_ERROR();
    	return;
    }
    index = 1;
    g_variant_get(v, "ay", &iter);
    while (g_variant_iter_loop(iter, "y", &ssid[index])) {
    	INFO("SSID[%d] = %d", index -1, ssid[index]);
    	index++;
    }

    //Now put the count in the array
    ssid[0] = index -1;

	//Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_SSID, (void *)ssid );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_privacy(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

	GVariant *v;
    bool privacy;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Privacy");
    if (!v) {
    	ERROR("Failed to get the property for Privacy");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &privacy);
    INFO("Privacy Property is %d",(int)privacy);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_PRIVACY , (void *)privacy);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_mode(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

    GVariant *v;
    BSS_MODE mode;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "Mode");
    if (!v) {
    	ERROR("Failed to get the property for Mode");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "s",&str);
    INFO("Mode Property is %s",str);

    if (!strcmp(str, "ad-hoc")) {
    	mode = BSS_MODE_AD_HOC;
    } else if (!strcmp(str, "infrastructure")) {
    	mode = BSS_MODE_INFRASTRUCTURE;
    } else {
    	ERROR("Received Invalid Mode: %s", str);
    }

    g_free(str);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_MODE, (void *)mode );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_freq(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

	GVariant *v;
    unsigned long freq;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Frequency");
    if (!v) {
    	ERROR("Failed to get the property for Frequency");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "q", &freq);

    INFO("Frequency Property is %d",(int)freq);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_FREQ, (void *)freq);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_signal(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

	GVariant *v;
    long signal;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
	  	                                  "Signal");
    if (!v) {
    	ERROR("Failed to get the property for Signal");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "n", &signal);

    INFO("Signal Property is %d",(int)signal);

    //Call the call back function
    controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_SET_SIGNAL, (void *)signal);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_rates(wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned long rate;

    v = g_dbus_proxy_get_cached_property (controller->m_mainIfProxy,
  	                                      "Rates");
    if (!v) {
    	ERROR("Failed to get the property for Rates");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "au", &iter);
    while (g_variant_iter_loop(iter, "u", &rate)) {
    	INFO("New Rate: %d", rate);

    	//Call the call back function
        controller->m_notifyCb(controller->m_parent, BSS_EVENT_TYPE_ADD_RATE, (void *)rate );
    }

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}


static void getProperties (wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

    getProp_bssid(controller);
	getProp_ssid(controller);
	getProp_privacy(controller);
	getProp_mode(controller);
	getProp_freq(controller);
	getProp_signal(controller);

	getProp_rates(controller);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicantClient_bss_dbusController *controller) {
	ENTER();

	//Register for signals for the BSS interface
	g_signal_connect (controller->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)controller);
	EXIT();
	return;
}
