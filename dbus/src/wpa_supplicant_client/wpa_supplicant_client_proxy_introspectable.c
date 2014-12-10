/*
 * wpa_supplicant_client_proxy_introspectable.c
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

//Prototype for Private Functions
static void query4XmlDescription (wpa_supplicantClient_ProxyIntrospectable *, GDBusConnection *);
//End of Prototypes for Private Functions

wpa_supplicantClient_ProxyIntrospectable *wpa_supplicantClient_proxyIntrospectable_Init(char *busName,
		                                                                                char *objectPath) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Init() \n");

	//First Create the object
	wpa_supplicantClient_ProxyIntrospectable *proxy = malloc(sizeof(wpa_supplicantClient_ProxyIntrospectable));
	if (!proxy){
		printf ("Can not allocate a wpa_supplicantClient_ProxyIntrospectable Object ...Exiting\n");
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicantClient_ProxyIntrospectable));

	strcpy(proxy->m_busName, busName);
	strcpy(proxy->m_objectPath, objectPath);

	return proxy;
}


void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicantClient_ProxyIntrospectable *proxy,
		                                             GDBusConnection *connection) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Start() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Start() \n");
		return;
	}

	if (!connection) {
		printf("NULL is passed for the connection to the function wpa_supplicantClient_proxyIntrospectable_Start() \n");
		return;
	}

	if (proxy->m_xmlDescription) {
		printf("XML Description is already there, no need to query for it again...No action is performed\n");
	} else {
        query4XmlDescription(proxy, connection);
	}

	printf("XML Description:\n %s \n", proxy->m_xmlDescription);

	return;
}

void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Stop() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_proxyIntrospectable_Stop() \n");
		return;
	}

	//Free the XML Description string if it is allocated
	g_free(proxy->m_xmlDescription);
	proxy->m_xmlDescription = NULL;

	//Free the dbus proxy Object
	g_object_unref(proxy->m_introspectionProxy);
	proxy->m_introspectionProxy = NULL;

	return;
}


void wpa_supplicantClient_proxyIntrospectable_Destroy(wpa_supplicantClient_ProxyIntrospectable *proxy) {
	printf("Entering wpa_supplicantClient_proxyIntrospectable_Destroy() \n");

	if (!proxy) {
		printf("NULL is passed to wpa_supplicantClient_proxyIntrospectable_Destroy() ... Exiting \n");
		return;
	}

	//finally free the object
	free(proxy);

	return;
}

char *wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(wpa_supplicantClient_ProxyIntrospectable *proxy) {
	return proxy->m_xmlDescription;
}


//Private Functions
///////////////////
static void query4XmlDescription (wpa_supplicantClient_ProxyIntrospectable *proxy,
 		       				      GDBusConnection *connection) {
	GError * error = NULL;

#if SYNC_API
	proxy->m_introspectionProxy = g_dbus_proxy_new_sync (connection,
			                                             G_DBUS_PROXY_FLAGS_NONE,
														 NULL,
														 proxy->m_busName,
														 proxy->m_objectPath,
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

		//Free the proxy
		g_object_unref(proxy->m_introspectionProxy);
		proxy->m_introspectionProxy = NULL;

		return;
	} else {
		printf("Performed the Introspection Successfully \n");
	}
#endif

	g_variant_get(v, "(s)", &proxy->m_xmlDescription);
	if (!proxy->m_xmlDescription) {
		printf("Failed to allocate the string for XML Description\n");

		g_variant_unref(v);
		//Free the proxy
		g_object_unref(proxy->m_introspectionProxy);
		proxy->m_introspectionProxy = NULL;

		return;
	}

	//Now we free the GVariant
	g_variant_unref(v);

	return;
}

