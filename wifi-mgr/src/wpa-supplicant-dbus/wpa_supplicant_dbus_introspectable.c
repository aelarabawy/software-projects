/*
 * wpa_supplicant_client_proxy_introspectable.c
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_dbus_introspectable.h"

//Prototype for Private Functions
static void query4XmlDescription (wpa_supplicant_dbus_introspectable *, GDBusConnection *);
//End of Prototypes for Private Functions

wpa_supplicant_dbus_introspectable *wpa_supplicantClient_proxyIntrospectable_Init(char *busName,
		                                                                                char *objectPath) {
	ENTER();

	//First Create the object
	wpa_supplicant_dbus_introspectable *proxy = malloc(sizeof(wpa_supplicant_dbus_introspectable));
	if (!proxy){
		ALLOC_FAIL("Can not allocate a wpa_supplicant_dbus_introspectable Object ...Exiting");
		EXIT_WITH_ERROR();
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicant_dbus_introspectable));

	strcpy(proxy->m_busName, busName);
	strcpy(proxy->m_objectPath, objectPath);

	EXIT();
	return proxy;
}


void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicant_dbus_introspectable *proxy,
		                                             GDBusConnection *connection) {
	ENTER();

	if (!proxy) {
		NULL_POINTER("proxy");
		EXIT_WITH_ERROR();
		return;
	}

	if (!connection) {
		NULL_POINTER("connection");
		EXIT_WITH_ERROR();
		return;
	}

	if (proxy->m_xmlDescription) {
		WARN("XML Description is already there, no need to query for it again...No action is performed");
	} else {
        query4XmlDescription(proxy, connection);
	}

	INFO("XML Description: %s", proxy->m_xmlDescription);

	EXIT();
	return;
}

void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicant_dbus_introspectable *proxy) {
	ENTER();

	if (!proxy) {
		NULL_POINTER("proxy");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the XML Description string if it is allocated
	g_free(proxy->m_xmlDescription);
	proxy->m_xmlDescription = NULL;

	//Free the dbus proxy Object
	g_object_unref(proxy->m_introspectionProxy);
	proxy->m_introspectionProxy = NULL;

	EXIT();
	return;
}


void wpa_supplicantClient_proxyIntrospectable_Destroy(wpa_supplicant_dbus_introspectable *proxy) {
	ENTER();

	if (!proxy) {
		NULL_POINTER("proxy");
		EXIT_WITH_ERROR();
	}

	//finally free the object
	free(proxy);

	EXIT();
	return;
}

char *wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(wpa_supplicant_dbus_introspectable *proxy) {
	ENTER();
	EXIT();
	return proxy->m_xmlDescription;
}


//Private Functions
///////////////////
static void query4XmlDescription (wpa_supplicant_dbus_introspectable *proxy,
 		       				      GDBusConnection *connection) {
	ENTER();
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
		ERROR("Can not create the Introspection proxy");
		ERROR("Error Message: %s", error->message);
		EXIT_WITH_ERROR();
		return;
	} else {
		PROGRESS("Created the Introspection Proxy Interface successfully");
	}


    GVariant *v = g_dbus_proxy_call_sync (proxy->m_introspectionProxy,
    		                              "Introspect",
				 					      NULL,
										  G_DBUS_CALL_FLAGS_NONE,
										  -1,
										  NULL,
										  &error);
	if (!v){
		ERROR("Failed to Introspect wpa_supplicant");
		ERROR("Error Message: %s", error->message);

		//Free the proxy
		g_object_unref(proxy->m_introspectionProxy);
		proxy->m_introspectionProxy = NULL;

		EXIT_WITH_ERROR();
		return;
	} else {
		PROGRESS("Performed the Introspection Successfully");
	}
#endif

	g_variant_get(v, "(s)", &proxy->m_xmlDescription);
	if (!proxy->m_xmlDescription) {
		ERROR("Failed to allocate the string for XML Description");

		g_variant_unref(v);
		//Free the proxy
		g_object_unref(proxy->m_introspectionProxy);
		proxy->m_introspectionProxy = NULL;

		EXIT_WITH_ERROR();
		return;
	}

	//Now we free the GVariant
	g_variant_unref(v);

	EXIT();
	return;
}
