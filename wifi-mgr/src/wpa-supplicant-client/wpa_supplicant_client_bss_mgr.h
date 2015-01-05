/*
 * wpa_supplicant_client_bss_mgr.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_MGR_H_
#define WPA_SUPPLICANT_CLIENT_BSS_MGR_H_

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_client_bss.h"

typedef struct bssList bssList;
typedef struct bssListHead bssListHead;

struct bssListHead {
	void *placeHolder;
	bssList*  m_next;
};

struct bssList {
  wpa_supplicant_client_bss *m_bss;
  bssList *m_next;
};

typedef struct {
	char m_busName[MAX_LEN_NAME];
	void *m_dbusConnection;

	bssListHead  m_bssGroup;
} wpa_supplicant_client_bssMgr;


wpa_supplicant_client_bssMgr *wpa_supplicant_client_bssMgr_Init (char *,  //Bus Name
		                                                              void *); //D-Bus Connection
void wpa_supplicant_client_bssMgr_Start (wpa_supplicant_client_bssMgr *);
void wpa_supplicant_client_bssMgr_Stop (wpa_supplicant_client_bssMgr *);
void wpa_supplicant_client_bssMgr_Destroy (wpa_supplicant_client_bssMgr *);

void wpa_supplicant_client_bssMgr_AddBss (wpa_supplicant_client_bssMgr *, char*);
void wpa_supplicant_client_bssMgr_RemBss (wpa_supplicant_client_bssMgr *, char*);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_MGR_H_ */
