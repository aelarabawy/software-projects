/*
 * wpa_supplicant_client_proxy_introspectable.h
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 *
 *  This Class is responsible for Introspection procedure for all objects
 *  Each D-Bus Controller will own an instance of this class
 */

#ifndef WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_
#define WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_

#include "common.h"

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	char *m_xmlDescription; //It is allocated by the called function

	GDBusProxy *m_introspectionProxy;
} wpa_supplicantClient_ProxyIntrospectable;

//Methods
wpa_supplicantClient_ProxyIntrospectable *wpa_supplicantClient_proxyIntrospectable_Init(char *,   //Bus Name
		                                                                                char *);  //Object Path
void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicantClient_ProxyIntrospectable *,
		                                             GDBusConnection *);
void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicantClient_ProxyIntrospectable *);
void wpa_supplicantClient_proxyIntrospectable_Destroy(wpa_supplicantClient_ProxyIntrospectable *);

char *wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(wpa_supplicantClient_ProxyIntrospectable *);

#endif /* WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_ */
