/*
 * wpa_supplicant_client_bss_dbus_controller.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss_dbus_controller.h"

wpa_supplicantClient_bssDbusController *wpa_supplicantClient_bssDbusController_Init (void) {
    wpa_supplicantClient_bssDbusController *controller = malloc(sizeof(wpa_supplicantClient_bssDbusController));
    if (!controller) {
    	printf("Failed to allocate the BSS D-Bus Object ... Exiting\n");
    	return NULL;
    }

    return controller;
}

void wpa_supplicantClient_bssDbusController_Start (wpa_supplicantClient_bssDbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bssDbusController_Stop (wpa_supplicantClient_bssDbusController *controller) {
	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bssDbusController_Destroy (wpa_supplicantClient_bssDbusController *controller) {
	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}


