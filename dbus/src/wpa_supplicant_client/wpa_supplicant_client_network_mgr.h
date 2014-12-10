/*
 * wpa_supplicant_client_network_mgr.h
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_

#include "common.h"
#include "wpa_supplicant_client_network.h"

typedef struct networkList networkList;
typedef struct networkListHead networkListHead;

struct networkListHead {
	void *placeHolder;
	networkList*  m_next;
};

struct networkList {
  wpa_supplicantClient_network *m_network;
  networkList *m_next;
};


typedef struct {
	char m_busName [MAX_LEN_NAME];
	void *m_dbusConnection;

	networkListHead  m_networkGroup;
} wpa_supplicantClient_networkMgr;

wpa_supplicantClient_networkMgr *wpa_supplicantClient_networkMgr_Init (char *,  //Bus Name
		                                                               void *); //D-Bus Connection
void wpa_supplicantClient_networkMgr_Start (wpa_supplicantClient_networkMgr *);
void wpa_supplicantClient_networkMgr_Stop (wpa_supplicantClient_networkMgr *);
void wpa_supplicantClient_networkMgr_Destroy (wpa_supplicantClient_networkMgr *);

void wpa_supplicantClient_networkMgr_AddNetwork (wpa_supplicantClient_networkMgr*, char*);
void wpa_supplicantClient_networkMgr_RemNetwork (wpa_supplicantClient_networkMgr*, char*);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_MGR_H_ */
