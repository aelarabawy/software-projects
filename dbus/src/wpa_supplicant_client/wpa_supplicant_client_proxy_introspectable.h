/*
 * wpa_supplicant_client_proxy_introspectable.h
 *
 *  Created on: Nov 25, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_
#define WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_

#include "common.h"

typedef struct {
	GDBusProxy *m_introspectionProxy;
} wpa_supplicantClient_ProxyIntrospectable;

//Methods
wpa_supplicantClient_ProxyIntrospectable *wpa_supplicantClient_proxyIntrospectable_Init();
void wpa_supplicantClient_proxyIntrospectable_Start (wpa_supplicantClient_ProxyIntrospectable *);
void wpa_supplicantClient_proxyIntrospectable_Stop (wpa_supplicantClient_ProxyIntrospectable *);
void wpa_supplicantClient_proxyIntrospectable_Destroy(wpa_supplicantClient_ProxyIntrospectable *);

void wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(wpa_supplicantClient_ProxyIntrospectable *,
			                                                    char **,
																GDBusConnection *);


#endif /* WPA_SUPPLICANT_CLIENT_PROXY_INTROSPECTABLE_H_ */
