/*
 * wpa_supplicant_client_interface.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_INTERFACE_H_
#define WPA_SUPPLICANT_CLIENT_INTERFACE_H_

#include "common.h"
#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_bss_manager.h"
#include "wpa_supplicant_client_network.h"

typedef struct {
	wpa_supplicantClient_bssManager *m_bssManager;
	wpa_supplicantClient_network *m_network;

	char m_pathName [MAX_SIZE_PATH_NAME];
} wpa_supplicantClient_if;

wpa_supplicantClient_if *wpa_supplicantClient_if_Init (char *);
void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *);

char *wpa_supplicantClient_if_GetIfPathName (wpa_supplicantClient_if *);

#endif /* WPA_SUPPLICANT_CLIENT_INTERFACE_H_ */
