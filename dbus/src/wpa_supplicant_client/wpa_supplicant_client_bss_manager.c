/*
 * wpa_supplicant_client_bss_manager.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss_manager.h"

wpa_supplicantClient_bssManager *wpa_supplicantClient_bssManager_Init (void) {
	wpa_supplicantClient_bssManager *manager = malloc(sizeof(wpa_supplicantClient_bssManager));
    if (!manager) {
    	printf("Failed to allocate the BSS Manager Object ... Exiting\n");
    	return NULL;
    }

    return manager;
}

void wpa_supplicantClient_bssManager_Start (wpa_supplicantClient_bssManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bssManager_Stop (wpa_supplicantClient_bssManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bssManager_Destroy (wpa_supplicantClient_bssManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

