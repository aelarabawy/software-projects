/*
 * wpa_supplicant_client_interface.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_interface.h"

wpa_supplicantClient_if *wpa_supplicantClient_if_Init (void)  {
	wpa_supplicantClient_if * interface = malloc(sizeof(wpa_supplicantClient_if));
    if (!interface) {
    	printf("Failed to allocate the Interface Object ... Exiting\n");
    	return NULL;
    }

	return interface;
}

void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *interface)  {

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *interface)  {

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *interface) {

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}
