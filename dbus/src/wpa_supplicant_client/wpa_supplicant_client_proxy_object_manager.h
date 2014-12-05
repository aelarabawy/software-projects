/*
 * wpa_supplicant_client_proxy_object_manager.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_MANAGER_H_
#define WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_MANAGER_H_

#include "common.h"

typedef struct wpa_supplicantClient_ProxyObjectManager {
	GDBusObjectProxy *mainObject;
	GDBusProxy *mainInterfaceProxy;
	GDBusProxy *interfaceInterfaceProxy;
	//GDBusObjectManager *m_objectManager;
} wpa_supplicantClient_ProxyObjectManager;

//Methods
wpa_supplicantClient_ProxyObjectManager * wpa_supplicantClient_proxyObjectManager_Init ();
void wpa_supplicantClient_proxyObjectManager_Start (wpa_supplicantClient_ProxyObjectManager *);
void wpa_supplicantClient_proxyObjectManager_Stop (wpa_supplicantClient_ProxyObjectManager *);
void wpa_supplicantClient_proxyObjectManager_Destroy(wpa_supplicantClient_ProxyObjectManager *);

void wpa_supplicantClient_proxyObjectManager_StartFollowing(wpa_supplicantClient_ProxyObjectManager *,
		                                                    GDBusConnection *);
void wpa_supplicantClient_proxyObjectManager_StopFollowing(wpa_supplicantClient_ProxyObjectManager *);

#endif /* WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_MANAGER_H_ */
