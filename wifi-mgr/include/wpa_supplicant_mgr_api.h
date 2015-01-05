/*
 * wpa-supplicant-mgr-api.h
 *
 *  Created on: Dec 12, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_MGR_API_H_
#define WPA_SUPPLICANT_MGR_API_H_

#include "common.h"

typedef void* wpaHandle;

/***
 * API For the wpa-supplicant-mgr
 */
//API to Init/Start/Stop/Destroy the wpa-supplicant-mgr
wpaHandle wpa_supplicant_mgr_Init ();
void wpa_supplicant_mgr_Start (wpaHandle); //wpa_supplicant manager handle
void wpa_supplicant_mgr_Stop (wpaHandle);
void wpa_supplicant_mgr_Destroy (wpaHandle);

#if 0
//Will fill those later
//API to get a handle to a WiFi Interface
wpaHandle wpa_supplicant_mgr_GetWiFiIfaceByName(wpaHandle,  //wpa_supplicant manager handle
		                                        char *);    //WiFi Interface name (such as wlan0)

unsigned int  wpa_supplicant_mgr_GetWiFiIfaceNames(wpaHandle,  //wpa_supplicant manager handle
		                                           char ** );  //Array of strings to fill with interface Names (count is returned)

//API to get the wpa_supplicant parameters

typedef struct {
	ClientDbgLvl m_dbgLvl;
	unsigned long m_dbgShowTS;  //Boolean (1/0) value
	unsigned long m_dbgShowKeys;  //Boolean (1/0) value
	unsigned long m_eapMethodSupport;  //Bit mask for EAP Methods
} wpa_supplicant_mgr_attribs;


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

#endif /* WPA_SUPPLICANT_MANAGER_API_H_ */
