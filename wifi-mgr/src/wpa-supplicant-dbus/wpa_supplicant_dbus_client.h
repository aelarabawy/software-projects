/*
 * wpa_supplicant_dbus_client.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_CLIENT_H_
#define WPA_SUPPLICANT_DBUS_CLIENT_H_

//This list is for events for the fi.w1.wpa.supplicant1 Interface
typedef enum {
	CLIENT_EVENT_TYPE_NOT_READY = 0,
	CLIENT_EVENT_TYPE_READY,
	CLIENT_EVENT_TYPE_ADD_IF,
	CLIENT_EVENT_TYPE_REM_IF,
	CLIENT_EVENT_TYPE_SET_DBG_LEVEL,
	CLIENT_EVENT_TYPE_SET_SHOW_TS,
	CLIENT_EVENT_TYPE_SET_SHOW_KEYS,
	CLIENT_EVENT_TYPE_ADD_EAP_METHOD,
	CLIENT_EVENT_TYPE_LAST
} ClientEventType;


void *wpa_supplicant_dbus_client_Init (char *, //Bus Name
		                               char *, //Object Path
									   void *, //Call back Function
									   void *); //Parent
void wpa_supplicant_dbus_client_Start (void *);
void wpa_supplicant_dbus_client_Stop (void *);
void wpa_supplicant_dbus_client_Destroy (void *);

void wpa_supplicant_dbus_client_SetDbgLvl (void *, ClientDbgLvl);
void wpa_supplicant_dbus_client_SetShowTS (void *, bool);
void wpa_supplicant_dbus_client_SetShowKeys (void *, bool);

#endif /* WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_ */
