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
	 GDBusObjectManager *m_objectManager;
} wpa_supplicantClient_ProxyObjectManager;

//Methods
wpa_supplicantClient_ProxyObjectManager * wpa_supplicantClient_ProxyObjectManager_Init ();
void wpa_supplicantClient_ProxyObjectManager_Start (wpa_supplicantClient_ProxyObjectManager *,
		                                            GDBusConnection *connection);
void wpa_supplicantClient_ProxyObjectManager_Stop (wpa_supplicantClient_ProxyObjectManager *);
void wpa_supplicantClient_ProxyObjectManager_Cleanup(wpa_supplicantClient_ProxyObjectManager *);

#endif /* WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_MANAGER_H_ */
