/*
 * wpa_supplicant_client.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 *
 *  The Well-Known Bus Name is:
 *    fi.w1.wpa_supplicant1
 *
 *
 *  The wpa_supplicant offers the following API's on the D-Bus
 *  fi.w1.wpa_supplicant1
 *  fi.w1.wpa_supplicant1.Interface
 *  fi.w1.wpa_supplicant1.Interface.WPS
 *  fi.w1.wpa_supplicant1.BSS
 *  fi.w1.wpa_supplicant1.Network
 */
#ifndef WPA_SUPPLICANT_CLIENT_H_
#define WPA_SUPPLICANT_CLIENT_H_

#include "common.h"
#include "wpa_supplicant_client_interface_manager.h"
#include "wpa_supplicant_client_name_watcher.h"
#include "wpa_supplicant_client_proxy_object_manager.h"
#include "wpa_supplicant_client_proxy_introspectable.h"


//Main Structure for the Client
typedef struct {
	wpa_supplicantClient_ifManager *m_ifManager;

	char *xmlDescription;
	wpa_supplicantClient_nameWatcher *m_nameWatcher;
	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;
	wpa_supplicantClient_ProxyObjectManager *m_proxyObjectManager;

	GDBusConnection * m_connection;
	GMainLoop * m_loop;
} wpa_supplicantClient;

//Methods
wpa_supplicantClient *wpa_supplicantClient_Init ();
void wpa_supplicantClient_Start (wpa_supplicantClient *);
void wpa_supplicantClient_Stop (wpa_supplicantClient *);
void wpa_supplicantClient_Cleanup (wpa_supplicantClient *);


#endif /* WPA_SUPPLICANT_CLIENT */
