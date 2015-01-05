/*
 * wpa_supplicant_client_bss.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_H_
#define WPA_SUPPLICANT_CLIENT_BSS_H_

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_dbus_bss.h"

#define NUM_RATES 100

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	//Properties
	BssMode m_mode;

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
	void *m_dbusHandle;

} wpa_supplicant_client_bss;


//Public Methods
wpa_supplicant_client_bss *wpa_supplicant_client_bss_Init (char *,  //Bus Name
		                                                   char *,  //Object Path
									                       void *); //D-Bus Connection
void wpa_supplicant_client_bss_Start (wpa_supplicant_client_bss *);
void wpa_supplicant_client_bss_Stop (wpa_supplicant_client_bss *);
void wpa_supplicant_client_bss_Destroy (wpa_supplicant_client_bss *);

char *wpa_supplicant_client_bss_GetPathName (wpa_supplicant_client_bss *);

//Get & Set of Properties
BssMode wpa_supplicant_client_bss_GetMode (wpa_supplicant_client_bss *);
unsigned char *wpa_supplicant_client_bss_GetBssid (wpa_supplicant_client_bss *, unsigned long *);
unsigned char* wpa_supplicant_client_bss_GetSsid (wpa_supplicant_client_bss *, unsigned long *);

unsigned long wpa_supplicant_client_bss_Getfreq (wpa_supplicant_client_bss *);
long wpa_supplicant_client_bss_GetSignal (wpa_supplicant_client_bss *);

unsigned long *wpa_supplicant_client_bss_getRates (wpa_supplicant_client_bss *, unsigned long *);
bool wpa_supplicant_client_bss_isPrivacySupprtd (wpa_supplicant_client_bss *);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_H_ */
