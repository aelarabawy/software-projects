/*
 * wpa_supplicant_client_bss.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss.h"

wpa_supplicantClient_bss *wpa_supplicantClient_bss_Init (void) {
	wpa_supplicantClient_bss *bss = malloc(sizeof(wpa_supplicantClient_bss));
    if (!bss) {
    	printf("Failed to allocate the BSS Object ... Exiting\n");
    	return NULL;
    }

	return bss;
}

void wpa_supplicantClient_bss_Start (wpa_supplicantClient_bss *bss) {

	if (!bss){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bss_Stop (wpa_supplicantClient_bss *bss) {

	if (!bss){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_bss_Destroy (wpa_supplicantClient_bss *bss) {

	if (!bss){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

enum BSS_MODE wpa_supplicant_bss_GetMode (wpa_supplicantClient_bss* bss) {
	return bss->m_mode;
}

unsigned char* wpa_supplicant_bss_GetBssid (wpa_supplicantClient_bss* bss) {
	return bss->m_bssid;
}

unsigned char* wpa_supplicant_bss_GetSsid (wpa_supplicantClient_bss* bss) {
	return bss->m_ssid;
}

unsigned int wpa_supplicant_bss_Getfreq (wpa_supplicantClient_bss* bss) {
	return bss->m_freq;
}

int wpa_supplicant_bss_GetSignal(wpa_supplicantClient_bss* bss) {
	return bss->m_signal;
}

unsigned int* wpa_supplicant_bss_getRates (wpa_supplicantClient_bss* bss) {
	return bss->m_rates;
}

bool wpa_supplicant_bss_isPrivacySupprtd (wpa_supplicantClient_bss* bss) {
	return bss->m_isPrivacySupprtd;
}
