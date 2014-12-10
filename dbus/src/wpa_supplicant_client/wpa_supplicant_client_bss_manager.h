/*
 * wpa_supplicant_client_bss_manager.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_MANAGER_H_
#define WPA_SUPPLICANT_CLIENT_BSS_MANAGER_H_

#include "common.h"
#include "wpa_supplicant_client_bss.h"

typedef struct bssList bssList;
typedef struct bssListHead bssListHead;

struct bssListHead {
	void *placeHolder;
	bssList*  m_next;
};

struct bssList {
  wpa_supplicantClient_bss *m_bss;
  bssList *m_next;
};

typedef struct {
	char m_busName[MAX_LEN_NAME];
	void *m_dbusConnection;

	bssListHead  m_bssGroup;
} wpa_supplicantClient_bssManager;

wpa_supplicantClient_bssManager *wpa_supplicantClient_bssManager_Init (char *,  //Bus Name
		                                                               void *); //D-Bus Connection
void wpa_supplicantClient_bssManager_Start (wpa_supplicantClient_bssManager *);
void wpa_supplicantClient_bssManager_Stop (wpa_supplicantClient_bssManager *);
void wpa_supplicantClient_bssManager_Destroy (wpa_supplicantClient_bssManager *);

void wpa_supplicantClient_bssManager_AddBss (wpa_supplicantClient_bssManager*, char*);
void wpa_supplicantClient_bssManager_RemBss (wpa_supplicantClient_bssManager*, char*);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_MANAGER_H_ */
