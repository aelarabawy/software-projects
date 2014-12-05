/*
 * wpa_supplicant_client_network.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_H_

#include "common.h"

typedef struct {

} wpa_supplicantClient_network;

wpa_supplicantClient_network *wpa_supplicantClient_network_Init (void);
void wpa_supplicantClient_network_Start (wpa_supplicantClient_network *);
void wpa_supplicantClient_network_Stop (wpa_supplicantClient_network *);
void wpa_supplicantClient_network_Destroy (wpa_supplicantClient_network *);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_H_ */
