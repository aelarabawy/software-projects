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

enum BSS_INTERNAL_ACTIONS {
	BSS_SIGNAL_UPDATED = 0
};

enum BSS_MODE {
	BSS_MODE_AD_HOC         = 0,
	BSS_MODE_INFRASTRUCTURE = 1
};


typedef struct {
	enum BSS_MODE m_mode;

	unsigned char m_bssid [6];  //BSSID: MAC Address of the BSS (Read Only)
	unsigned char m_ssid [100]; //SSID : Human Readable BSS name (Read Only)


	unsigned int m_freq; //Frequency in MHz (Read Only)
	int m_signal;  //Signal Strength (read Only)

	unsigned int m_rates[NUM_RATES];  //Array of supported Rates (Read Only)

	bool m_isPrivacySupprtd;   //(Read Only)

	//WPA
	//RSN
	//IEs

	//DBus Engine
	wpa_supplicantClient_bssDbusController *m_dbusController;

} wpa_supplicantClient_bss;

//Public Methods
wpa_supplicantClient_bss *wpa_supplicantClient_bss_Init (void);
void wpa_supplicantClient_bss_Start (wpa_supplicantClient_bss *);
void wpa_supplicantClient_bss_Stop (wpa_supplicantClient_bss *);
void wpa_supplicantClient_bss_Destroy (wpa_supplicantClient_bss *);


enum BSS_MODE  wpa_supplicant_bss_GetMode  (wpa_supplicantClient_bss*);
unsigned char* wpa_supplicant_bss_GetBssid (wpa_supplicantClient_bss*);
unsigned char* wpa_supplicant_bss_GetSsid  (wpa_supplicantClient_bss*);

unsigned int   wpa_supplicant_bss_Getfreq   (wpa_supplicantClient_bss*);
int            wpa_supplicant_bss_GetSignal (wpa_supplicantClient_bss*);
unsigned int*  wpa_supplicant_bss_getRates  (wpa_supplicantClient_bss*);
bool           wpa_supplicant_bss_isPrivacySupprtd (wpa_supplicantClient_bss*);

//Internal Methods
void dbusEngineCallBack (enum BSS_INTERNAL_ACTIONS, void*);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_H_ */
