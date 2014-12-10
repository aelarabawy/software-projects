/*
 * wpa_supplicant_client_bss.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_H_
#define WPA_SUPPLICANT_CLIENT_BSS_H_

#include "common.h"
#include "wpa_supplicant_client_bss_dbus_controller.h"

#define NUM_RATES 10

typedef enum {
	BSS_SIGNAL_UPDATED = 0
} BSS_INTERNAL_ACTIONS;



typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	//Properties
	BSS_MODE m_mode;

	unsigned char m_bssid [8];  //BSSID: MAC Address of the BSS (Read Only)
	unsigned long m_bssidLen;

	unsigned char m_ssid [MAX_LEN_NAME]; //SSID : Human Readable BSS name (Read Only)
    unsigned long m_ssidLen;

	unsigned long m_freq; //Frequency in MHz (Read Only)
	long m_signal;  //Signal Strength (read Only)

	unsigned long m_rates[NUM_RATES];  //Array of supported Rates (Read Only)
	unsigned long m_rateCount;

	bool m_isPrivacySupprtd;   //(Read Only)

	//WPA
	//RSN
	//IEs

	//DBus Engine
	wpa_supplicantClient_bss_dbusController *m_dbusController;

} wpa_supplicantClient_bss;

//Public Methods
wpa_supplicantClient_bss *wpa_supplicantClient_bss_Init (char *,  //Bus Name
		                                                 char *,  //Object Path
														 void *); //D-Bus Connection
void wpa_supplicantClient_bss_Start (wpa_supplicantClient_bss *);
void wpa_supplicantClient_bss_Stop (wpa_supplicantClient_bss *);
void wpa_supplicantClient_bss_Destroy (wpa_supplicantClient_bss *);

char *wpa_supplicantClient_bss_GetPathName (wpa_supplicantClient_bss *);

//Get & Set of Properties
BSS_MODE wpa_supplicant_bss_GetMode (wpa_supplicantClient_bss *);
unsigned char *wpa_supplicant_bss_GetBssid (wpa_supplicantClient_bss *, unsigned long *);
unsigned char* wpa_supplicant_bss_GetSsid (wpa_supplicantClient_bss *, unsigned long *);

unsigned long wpa_supplicant_bss_Getfreq (wpa_supplicantClient_bss *);
long wpa_supplicant_bss_GetSignal (wpa_supplicantClient_bss *);
unsigned long *wpa_supplicant_bss_getRates (wpa_supplicantClient_bss *, unsigned long *);
bool wpa_supplicant_bss_isPrivacySupprtd (wpa_supplicantClient_bss *);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_H_ */
