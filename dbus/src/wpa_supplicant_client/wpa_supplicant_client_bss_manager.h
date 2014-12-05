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
struct bssList {
  wpa_supplicantClient_bss *m_bss;

  struct bssList *m_next;
};

typedef struct {
	bssList m_bssGroup;
} wpa_supplicantClient_bssManager;

wpa_supplicantClient_bssManager *wpa_supplicantClient_bssManager_Init (void);
void wpa_supplicantClient_bssManager_Start (wpa_supplicantClient_bssManager *);
void wpa_supplicantClient_bssManager_Stop (wpa_supplicantClient_bssManager *);
void wpa_supplicantClient_bssManager_Destroy (wpa_supplicantClient_bssManager *);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_MANAGER_H_ */
