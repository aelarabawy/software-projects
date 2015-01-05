/*
 * wpa_supplicant_dbus_bss.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_BSS_H_
#define WPA_SUPPLICANT_DBUS_BSS_H_


typedef enum {
	BSS_EVENT_TYPE_FIRST = 0,
	BSS_EVENT_TYPE_SET_MODE,
	BSS_EVENT_TYPE_SET_BSSID,
	BSS_EVENT_TYPE_SET_SSID,
	BSS_EVENT_TYPE_SET_FREQ,
	BSS_EVENT_TYPE_SET_SIGNAL,
	BSS_EVENT_TYPE_ADD_RATE,
	BSS_EVENT_TYPE_SET_PRIVACY,
	BSS_EVENT_TYPE_LAST
} BssEventType;


typedef enum {
	BSS_MODE_AD_HOC         = 0,
	BSS_MODE_INFRASTRUCTURE
} BssMode;


void *wpa_supplicant_dbus_bss_Init (char *, //Bus Name
                                    char *, //Object Path
		                            void *, //Connection
                                    void *, //Call back Function
                                    void *); //Parent
void wpa_supplicant_dbus_bss_Start (void *);
void wpa_supplicant_dbus_bss_Stop (void *);
void wpa_supplicant_dbus_bss_Destroy (void *);

// Set of Properties
//////////////////////////


#endif /* WPA_SUPPLICANT_DBUS_BSS_H_ */
