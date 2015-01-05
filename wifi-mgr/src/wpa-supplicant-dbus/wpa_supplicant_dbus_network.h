/*
 * wpa_supplicant_dbus_network.h
 *
 *  Created on: Dec 8, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_NETWORK_H_
#define WPA_SUPPLICANT_DBUS_NETWORK_H_

typedef enum {
	NETWORK_EVENT_TYPE_FIRST = 0,
	NETWORK_EVENT_TYPE_SET_ENABLED,
	NETWORK_EVENT_TYPE_LAST
} NetworkEventType;

void *wpa_supplicant_dbus_network_Init (char *, //Bus Name
		                                char *, //Object Path
										void *, //Connection
   										void *, //Call back Function
										void *); //Parent

void wpa_supplicant_dbus_network_Start (void *); //handler
void wpa_supplicant_dbus_network_Stop (void *); //handler
void wpa_supplicant_dbus_network_Destroy (void *); //handler


void wpa_supplicant_dbus_network_SetEnabled (void *, //handler
		                                     bool); //isEnabled

#endif /* WPA_SUPPLICANT_DBUS_NETWORK_H_ */
