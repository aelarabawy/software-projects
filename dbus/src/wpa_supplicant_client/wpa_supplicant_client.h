/*
 * wpa_supplicant_client.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */
#ifndef WPA_SUPPLICANT_CLIENT_H_
#define WPA_SUPPLICANT_CLIENT_H_

#include "common.h"
#include "wpa_supplicant_client_name_watcher.h"
#include "wpa_supplicant_client_proxy_object_manager.h"


//Main Structure for the Client
typedef struct wpa_supplicantClient {
	wpa_supplicantClient_nameWatcher * m_nameWatcher;
	wpa_supplicantClient_ProxyObjectManager * m_proxyObjectManager;

	GDBusConnection * m_connection;

	GMainLoop * m_loop;
} wpa_supplicantClient;

//Methods
wpa_supplicantClient *wpa_supplicantClient_Init();
void wpa_supplicantClient_Start(wpa_supplicantClient *);
void wpa_supplicantClient_Stop(wpa_supplicantClient *);
void wpa_supplicantClient_Cleanup (wpa_supplicantClient *);


#endif /* WPA_SUPPLICANT_CLIENT */
