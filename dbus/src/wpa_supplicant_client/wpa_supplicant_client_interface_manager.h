/*
 * wpa_supplicant_client_interface_manager.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_INTERFACE_MANAGER_H_
#define WPA_SUPPLICANT_CLIENT_INTERFACE_MANAGER_H_

#include "common.h"
#include "wpa_supplicant_client_interface.h"

typedef struct interfaceList interfaceList;
struct interfaceList {
  wpa_supplicantClient_if *m_interface;

  interfaceList *m_next;
};

typedef struct {
  interfaceList  m_interfaceGroup;
} wpa_supplicantClient_ifManager;

wpa_supplicantClient_ifManager *wpa_supplicantClient_ifManager_Init (void);
void wpa_supplicantClient_ifManager_Start (wpa_supplicantClient_ifManager *);
void wpa_supplicantClient_ifManager_Stop (wpa_supplicantClient_ifManager *);
void wpa_supplicantClient_ifManager_Destroy (wpa_supplicantClient_ifManager *);

#endif /* WPA_SUPPLICANT_CLIENT_INTERFACE_MANAGER_H_ */
