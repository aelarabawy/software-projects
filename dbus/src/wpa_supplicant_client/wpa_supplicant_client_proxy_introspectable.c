/*
 * wpa_supplicant_client_proxy_introspectable.c
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

wpa_supplicantClient_ProxyIntrospectable *wpa_supplicantClient_proxyIntrospectable_Init() {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Init() \n");

	//First Create the object
	wpa_supplicantClient_ProxyIntrospectable *proxy = malloc (sizeof(wpa_supplicantClient_ProxyIntrospectable));
	if (!proxy){
		printf ("Can not allocate a wpa_supplicantClient_ProxyIntrospectable Object ...Exiting\n");
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicantClient_ProxyIntrospectable));

	return proxy;
}


void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Start() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Start() \n");
		return;
	}

	return;
}

void wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(wpa_supplicantClient_ProxyIntrospectable *proxy,
			                                                    char **descPtr,
																GDBusConnection *connection) {
	GError * error = NULL;

#if SYNC_API
	proxy->m_introspectionProxy = g_dbus_proxy_new_sync (connection,
			                                             G_DBUS_PROXY_FLAGS_NONE,
														 NULL,
														 WPA_SUPPLICANT_BUS_NAME,
														 WPA_SUPPLICANT_MAIN_OBJECT_PATH,
														 WPA_SUPPLICANT_INTROSPECTABLE_INTERFACE,
														 NULL,
														 &error);
	if (!proxy->m_introspectionProxy){
		printf("Can not create the Introspection proxy \n");
		printf("Error Message: %s \n", error->message);
		return;
	} else {
		printf("Created the Introspection Proxy Interface successfully \n");
	}


    GVariant *v = g_dbus_proxy_call_sync (proxy->m_introspectionProxy,
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
#endif

   	if (*descPtr) {
   		free (*descPtr); //TODO this should be changed to gfree.... check with the api
   		*descPtr = NULL;
   	}

	g_variant_get(v, "(s)", descPtr);
	if (!descPtr){
		printf("Failed to allocate the string for XML Description\n");
		return;
	}

	return;
}

void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Stop() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Stop() \n");
		return;
	}

	return;
}


void wpa_supplicantClient_proxyIntrospectable_Cleanup(wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Cleanup() \n");

	if (!proxy) {
		printf("NULL is passed to wpa_supplicantClient_proxyIntrospectable_Cleanup() ... Exiting \n");
		return;
	}

	//finally free the object
	free(proxy);

	return;
}
