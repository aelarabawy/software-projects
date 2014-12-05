/*
 * wpa_supplicant_client_network.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_network.h"

wpa_supplicantClient_network *wpa_supplicantClient_network_Init (void)  {
	wpa_supplicantClient_network * network = malloc(sizeof(wpa_supplicantClient_network));
    if (!network) {
    	printf("Failed to allocate the Network Object ... Exiting\n");
    	return NULL;
    }

	return network;
}

void wpa_supplicantClient_network_Start (wpa_supplicantClient_network *network)  {

	if (!network){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_network_Stop (wpa_supplicantClient_network *network)  {

	if (!network){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_network_Destroy (wpa_supplicantClient_network *network) {

	if (!network){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}
