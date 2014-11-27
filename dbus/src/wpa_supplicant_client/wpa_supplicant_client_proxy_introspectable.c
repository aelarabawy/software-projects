/*
 * wpa_supplicant_client_proxy_introspectable.c
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_proxy_introspectable.h"

wpa_supplicantClient_ProxyIntrospectable *wpa_supplicantClient_proxyIntrospectable_init() {
	//First Create the object
	wpa_supplicantClient_ProxyIntrospectable * proxy = malloc (sizeof(wpa_supplicantClient_ProxyObjectManager));
	if (!proxy){
		printf ("Can not allocate a wpa_supplicantClient_ProxyObjectManager Object ...Exiting\n");
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicantClient_ProxyObjectManager));

	return proxy;
}


void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicantClient_ProxyIntrospectable *proxy,
		                                             GDBusConnection *connection) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Start \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Start() \n");
		return;
	}

	GError * error = NULL;
#if SYNC_API
	proxy->m_introspectionProxy = g_dbus_proxy_new_sync (connection,
			                                             G_DBUS_PROXY_FLAGS_NONE,
														 NULL,
														 "fi.w1.wpa_supplicant1",
														 "/fi/w1/wpa_supplicant1",
														 "org.freedesktop.DBus.Introspectable",
														 NULL,
														 &error);
	if (!proxy->m_introspectionProxy){
		printf("Can not create the Introspection proxy \n");
		printf("Error Message: %s \n", error->message);
		return;
	} else {
		printf("Created the Introspection Proxy Interface successfully \n");
	}


    GVariant *v = g_dbus_proxy_call_sync (introspectionProxy,
    		                              "Introspect",
				 					      NULL,
										  G_DBUS_CALL_FLAGS_NONE,
										  -1,
										  NULL,
										  &error);
	if (!v){
		printf("Failed to Introspect wpa_supplicant \n");
		printf("Error Message: %s \n", error->message);
		return;
	} else {
		printf("Performed the Introspection Successfully \n");
	}

    gchar * xmlData = NULL;
   	g_variant_get (v, "(s)", &xmlData);
   	printf ("Result is : \n %s \n", xmlData);


	return;
}


void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Stop \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Stop() \n");
		return;
	}

	return;
}


void wpa_supplicantClient_proxyIntrospectable_Cleanup(wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Cleanup \n");

	if (!proxy) {
		printf("NULL is passed to wpa_supplicantClient_proxyIntrospectable_Cleanup() ... Exiting \n");
		return;
	}

	//finally free the object
	free(proxy);

	return;
}
