/*
 * wpa_supplicant_client_interface.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_interface.h"

wpa_supplicantClient_if *wpa_supplicantClient_if_Init (char *pathName)  {
	printf("Entered wpa_supplicantClient_if_Init() with pathName = %s \n", pathName);

	wpa_supplicantClient_if * interface = malloc(sizeof(wpa_supplicantClient_if));
    if (!interface) {
    	printf("Failed to allocate the Interface Object ... Exiting\n");
    	return NULL;
    }
    memset(interface, 0, sizeof(wpa_supplicantClient_if));

    strcpy (interface->m_pathName, pathName);

	return interface;
}

void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *interface)  {
	printf("Entered wpa_supplicantClient_if_Start() \n");
	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *interface)  {
	printf("Entered wpa_supplicantClient_if_Stop() \n");

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *interface) {
	printf("Entered wpa_supplicantClient_if_Destroy()\n");

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Finally Free the Interface
	free(interface);

	return;
}

char* wpa_supplicantClient_if_GetIfPathName(wpa_supplicantClient_if *interface) {
	return interface->m_pathName;
}
