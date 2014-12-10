/*
 * wpa_supplicant_client_network.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_H_

#include "common.h"
#include "wpa_supplicant_client_network_dbus_controller.h"

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	wpa_supplicantClient_network_dbusController *m_dbusController;

	//Properties of the Client
	bool m_isEnabled; //(RW)
} wpa_supplicantClient_network;

wpa_supplicantClient_network *wpa_supplicantClient_network_Init (char *,  //Bus Name
		                                                         char *,  //Object Path
																 void *); //D-Bus Connection
void wpa_supplicantClient_network_Start (wpa_supplicantClient_network *);
void wpa_supplicantClient_network_Stop (wpa_supplicantClient_network *);
void wpa_supplicantClient_network_Destroy (wpa_supplicantClient_network *);

char *wpa_supplicantClient_network_GetPathName (wpa_supplicantClient_network *);

//Get & Set of Properties
bool wpa_supplicantClient_network_GetEnabled (wpa_supplicantClient_network *);
void wpa_supplicantClient_network_SetEnabled (wpa_supplicantClient_network *,
		                                      bool);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_H_ */
