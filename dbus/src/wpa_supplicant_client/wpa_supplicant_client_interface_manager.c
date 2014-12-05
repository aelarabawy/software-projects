/*
 * wpa_supplicant_client_interface_manager.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_interface_manager.h"


wpa_supplicantClient_ifManager *wpa_supplicantClient_ifManager_Init () {
	wpa_supplicantClient_ifManager *manager = malloc(sizeof(wpa_supplicantClient_ifManager));
    if (!manager) {
    	printf("Failed to allocate the Interface Manager Object ... Exiting\n");
    	return NULL;
    }

	return manager;
}

void wpa_supplicantClient_ifManager_Start (wpa_supplicantClient_ifManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_ifManager_Stop (wpa_supplicantClient_ifManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_ifManager_Destroy (wpa_supplicantClient_ifManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

