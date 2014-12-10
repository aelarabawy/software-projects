/*
 * wpa_supplicant_client_if_manager.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_IF_MANAGER_H_
#define WPA_SUPPLICANT_CLIENT_IF_MANAGER_H_

#include "common.h"
#include "wpa_supplicant_client_if.h"

typedef struct interfaceList interfaceList;
typedef struct interfaceListHead interfaceListHead;

struct interfaceListHead {
	void *placeHolder;
	interfaceList*  m_next;
};

struct interfaceList {
  wpa_supplicantClient_if *m_interface;
  interfaceList *m_next;
};


typedef struct {
	char m_busName [MAX_LEN_NAME];
	void *m_dbusConnection;
	interfaceListHead  m_interfaceGroup;
} wpa_supplicantClient_ifManager;

wpa_supplicantClient_ifManager *wpa_supplicantClient_ifManager_Init (char *); //Bus Name
void wpa_supplicantClient_ifManager_Start (wpa_supplicantClient_ifManager *,
		                                   void *); //D-Bus Connection
void wpa_supplicantClient_ifManager_Stop (wpa_supplicantClient_ifManager *);
void wpa_supplicantClient_ifManager_Destroy (wpa_supplicantClient_ifManager *);

void wpa_supplicantClient_ifManager_AddIf (wpa_supplicantClient_ifManager*, char*);
void wpa_supplicantClient_ifManager_RemIf (wpa_supplicantClient_ifManager*, char*);

#endif /* WPA_SUPPLICANT_CLIENT_IF_MANAGER_H_ */
