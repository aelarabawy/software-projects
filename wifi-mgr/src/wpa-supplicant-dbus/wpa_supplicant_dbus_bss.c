/*
 * wpa_supplicant_dbus_bss.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_dbus_bss_int.h"

//Prototypes for Private Functions
static void getProperties (wpa_supplicant_dbus_bss *);
static void registerSignals (wpa_supplicant_dbus_bss *);
//End of Prototypes for Private Functions

void *wpa_supplicant_dbus_bss_Init (char *busName,
		                            char *objectPath,
									void *connection,
		                            void *notifyCb,
		                            void *parent) {
	ENTER();

    wpa_supplicant_dbus_bss *bss = malloc(sizeof(wpa_supplicant_dbus_bss));
    if (!bss) {
    	ALLOC_FAIL("bss");
    	goto FAIL_BSS;
    }
	memset (bss, 0, sizeof(wpa_supplicant_dbus_bss));

	//Set the Notification parameters
	strcpy(bss->m_busName, busName);
	strcpy(bss->m_objectPath, objectPath);
	bss->m_connection = connection;
	bss->m_notifyCb = notifyCb;
	bss->m_parent = parent;


	bss->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init(busName, objectPath);
	if (!bss->m_proxyIntrospectable) {
		ERROR("Failed to Initialize the Proxy Introspectable BSS Object... Exiting");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_INTROSPECTABLE:
	free(bss);

FAIL_BSS:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
    return (void *)bss;
}

void wpa_supplicant_dbus_bss_Start (void *handle) {

	ENTER();

	GError * error = NULL;

	wpa_supplicant_dbus_bss *bss = (wpa_supplicant_dbus_bss *)handle;
	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(bss->m_proxyIntrospectable,
			                                       bss->m_connection);


	bss->m_objectProxy = g_dbus_object_proxy_new (bss->m_connection,
			                                      bss->m_objectPath);
	if (!bss->m_objectProxy) {
		ERROR("Failed to create an object proxy for the BSS ");
		EXIT_WITH_ERROR();
		return;
	}

	bss->m_mainIfProxy = g_dbus_proxy_new_sync (bss->m_connection,
			                                    G_DBUS_PROXY_FLAGS_NONE,
						   						NULL,
												bss->m_busName,
												bss->m_objectPath,
												WPA_SUPPLICANT_BSS_INTERFACE,
												NULL,
												&error);
	if (!bss->m_mainIfProxy) {
		ERROR("Can not create a proxy for the BSS interface ");
		ERROR("Error Message: %s ", error->message);

		EXIT_WITH_ERROR();
		return;
	}

	PROGRESS("Created the proxy for the BSS interface successfully ");


	//Then Register for the signals
	registerSignals(bss);

	//Get the Properties
	getProperties(bss);

	EXIT();
	return;
}


void wpa_supplicant_dbus_bss_Stop (void *handle) {

	ENTER();

	wpa_supplicant_dbus_bss *bss = (wpa_supplicant_dbus_bss *)handle;
	if (!bss) {
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}


	//Free the dbus proxy Interface
	g_object_unref(bss->m_mainIfProxy);
	bss->m_mainIfProxy = NULL;

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(bss->m_proxyIntrospectable);

	EXIT();
	return;
}


void wpa_supplicant_dbus_bss_Destroy (void *handle) {

	ENTER();

	wpa_supplicant_dbus_bss *bss = (wpa_supplicant_dbus_bss *)handle;
	if (!bss) {
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(bss->m_proxyIntrospectable);


	//Finally, we free the bss
	free (bss);

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

	//First Get the bss
	//wpa_supplicant_dbus_bss *bss = (wpa_supplicant_dbus_bss *)userData;

	ERROR("Received an Invalid/UnSupported Signal");
	EXIT_WITH_ERROR();
	return;
}

static void getProp_bssid(wpa_supplicant_dbus_bss *bss) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned char bssid[10];
    unsigned char index;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
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
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_BSSID, (void *)bssid );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_ssid(wpa_supplicant_dbus_bss *bss) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned char ssid[10];
    unsigned char index;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
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
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_SSID, (void *)ssid );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_privacy(wpa_supplicant_dbus_bss *bss) {
	ENTER();

	GVariant *v;
    bool privacy;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
	  	                                  "Privacy");
    if (!v) {
    	ERROR("Failed to get the property for Privacy");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "b", &privacy);
    INFO("Privacy Property is %d",(int)privacy);

    //Call the call back function
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_PRIVACY , (void *)privacy);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_mode(wpa_supplicant_dbus_bss *bss) {
	ENTER();

    GVariant *v;
    BssMode mode;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
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
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_MODE, (void *)mode );

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_freq(wpa_supplicant_dbus_bss *bss) {
	ENTER();

	GVariant *v;
    unsigned long freq;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
	  	                                  "Frequency");
    if (!v) {
    	ERROR("Failed to get the property for Frequency");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "q", &freq);

    INFO("Frequency Property is %d",(int)freq);

    //Call the call back function
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_FREQ, (void *)freq);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_signal(wpa_supplicant_dbus_bss *bss) {
	ENTER();

	GVariant *v;
    long signal;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
	  	                                  "Signal");
    if (!v) {
    	ERROR("Failed to get the property for Signal");
    	EXIT_WITH_ERROR();
    	return;
    }

    g_variant_get(v, "n", &signal);

    INFO("Signal Property is %d",(int)signal);

    //Call the call back function
    bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_SET_SIGNAL, (void *)signal);

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}

static void getProp_rates(wpa_supplicant_dbus_bss *bss) {
	ENTER();

    GVariant *v;
    GVariantIter *iter;
    unsigned long rate;

    v = g_dbus_proxy_get_cached_property (bss->m_mainIfProxy,
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
        bss->m_notifyCb(bss->m_parent, BSS_EVENT_TYPE_ADD_RATE, (void *)rate );
    }

    //Cleanup
    g_variant_unref(v);

    EXIT();
    return;
}


static void getProperties (wpa_supplicant_dbus_bss *bss) {
	ENTER();

    getProp_bssid(bss);
	getProp_ssid(bss);
	getProp_privacy(bss);
	getProp_mode(bss);
	getProp_freq(bss);
	getProp_signal(bss);

	getProp_rates(bss);

	EXIT();
	return;
}

static void registerSignals (wpa_supplicant_dbus_bss *bss) {
	ENTER();

	//Register for signals for the BSS interface
	g_signal_connect (bss->m_mainIfProxy,
 			          "g-signal",
				      G_CALLBACK(signalNotify),
				      (void *)bss);
	EXIT();
	return;
}
