/*
 * wpa_supplicant_client_network_mgr.h
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_

#include "wpa_supplicant_client_network.h"

typedef struct networkList networkList;
typedef struct networkListHead networkListHead;

struct networkListHead {
	void *placeHolder;
	networkList*  m_next;
};

struct networkList {
  wpa_supplicant_client_network *m_network;
  networkList *m_next;
};


typedef struct {
	char m_busName [MAX_LEN_NAME];
	void *m_dbusConnection;

	networkListHead  m_networkGroup;
} wpa_supplicant_client_networkMgr;

wpa_supplicant_client_networkMgr *wpa_supplicant_client_networkMgr_Init (char *,  //Bus Name
		                                                                 void *); //D-Bus Connection
void wpa_supplicant_client_networkMgr_Start (wpa_supplicant_client_networkMgr *);
void wpa_supplicant_client_networkMgr_Stop (wpa_supplicant_client_networkMgr *);
void wpa_supplicant_client_networkMgr_Destroy (wpa_supplicant_client_networkMgr *);

void wpa_supplicant_client_networkMgr_AddNetwork (wpa_supplicant_client_networkMgr*, char*);
void wpa_supplicant_client_networkMgr_RemNetwork (wpa_supplicant_client_networkMgr*, char*);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_ */
