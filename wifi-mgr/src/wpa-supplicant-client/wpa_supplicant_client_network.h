/*
 * wpa_supplicant_client_network.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_H_

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_dbus_network.h"

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	void *m_dbusHandle;

	//Properties of the Client
	bool m_isEnabled; //(RW)
} wpa_supplicant_client_network;


wpa_supplicant_client_network *wpa_supplicant_client_network_Init (char *,  //Bus Name
		                                                           char *,  //Object Path
										                           void *); //D-Bus Connection
void wpa_supplicant_client_network_Start (wpa_supplicant_client_network *);
void wpa_supplicant_client_network_Stop (wpa_supplicant_client_network *);
void wpa_supplicant_client_network_Destroy (wpa_supplicant_client_network *);

char *wpa_supplicant_client_network_GetPathName (wpa_supplicant_client_network *);

//Get & Set of Properties
bool wpa_supplicant_client_network_GetEnabled (wpa_supplicant_client_network *);
void wpa_supplicant_client_network_SetEnabled (wpa_supplicant_client_network *,
		                                       bool);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_H_ */
