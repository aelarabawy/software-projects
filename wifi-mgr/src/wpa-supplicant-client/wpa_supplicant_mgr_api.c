/*
 * wpa_supplicant_mgr_api.c
 *
 *  Created on: Dec 13, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_mgr_api.h"
#include "wpa_supplicant_client.h"


/***
 * API For the wpa-supplicant-mgr
 */

wpaHandle wpa_supplicant_mgr_Init () {
	ENTER();

	wpaHandle client = (wpaHandle) wpa_supplicant_client_Init ("fi.w1.wpa_supplicant1",   //Bus Name
			                                                   "/fi/w1/wpa_supplicant1"); //Object Path
	if (!client){
		ALLOC_FAIL("client");
		EXIT_WITH_ERROR();
		return NULL;
	}

	EXIT();
	return client;
}

void wpa_supplicant_mgr_Start (wpaHandle client) {
	ENTER();

	//Starting Operation
	PROGRESS("Starting the Client");
	wpa_supplicant_client_Start((wpa_supplicant_client *)client);

	EXIT();
}

void wpa_supplicant_mgr_Stop (wpaHandle client) {
	ENTER();

	//Stopping Operation
	PROGRESS("Stopping the Client");
	wpa_supplicant_client_Stop((wpa_supplicant_client *)client);

	EXIT();
}

void wpa_supplicant_mgr_Destroy (wpaHandle client) {
	ENTER();

	wpa_supplicant_client_Destroy((wpa_supplicant_client *)client);

	EXIT();
}


#if 0
//Will fill those later


//API to get a handle to a WiFi Interface
wpaHandle wpa_supplicant_mgr_GetWiFiIfaceByName(wpaHandle client,
		                                        char * IfaceName) {
	ENTER();


	EXIT();

}

unsigned int  wpa_supplicant_mgr_GetWiFiIfaceNames(wpaHandle,  //wpa_supplicant manager handle
		                                           char ** ) {  //Array of strings to fill with interface Names (count is returned)
	ENTER();


	EXIT();
}

//API to get the wpa_supplicant parameters

void wpa_supplicant_mgr_GetAttribs(wpaHandle, //wpa_supplicant manager handle
		                           wpa_supplicant_mgr_attribs *); //Pointer to structure to fill




/***
 * API for the WiFi Interfaces
 */
//To get the WiFi Interface Attributes

typedef struct {
	Wifi_Iface_State m_state;
	bool m_scanning;
	unsigned long m_apScan;
	unsigned long m_bssExpireAge;
	unsigned long m_bssExpireCount;
	char m_country [MAX_LEN_NAME];
	char m_ifName [MAX_LEN_NAME];
	char m_bridgeIfName [MAX_LEN_NAME];
	char m_driver [MAX_LEN_NAME];
	bool m_fastReauth;
	unsigned long m_scanInterval;
} wpa_supplicant_wifi_iface_attribs;

void wpa_supplicant_wifi_iface_GetAttribs(wpaHandle, //wifi handle
		                                  wpa_supplicant_wifi_iface_attribs *); //Pointer to structure to fill


//To Get BSSs detected by this WiFi Interface
unsigned int wpa_spplicant_wifi_iface_GetBssNames (wpaHandle,
		                                            char **);

//To get the BSS that the Wifi Interface is connected to
char *wpa_supplicant_wifi_GetCurrentBss (wpaHandle);

//Get the BSS Handle from its SSID (name)
wpaHandle wpa_supplicant_wifi_GetBss (char *);

#endif

