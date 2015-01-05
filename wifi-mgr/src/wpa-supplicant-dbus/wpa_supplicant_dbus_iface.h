/*
 * wpa_supplicant_dbus_iface.h
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_IFACE_H_
#define WPA_SUPPLICANT_DBUS_IFACE_H_


typedef enum {
	IF_EVENT_TYPE_FIRST = 0,
	IF_EVENT_TYPE_ADD_NW,
	IF_EVENT_TYPE_REM_NW,
	IF_EVENT_TYPE_ADD_BSS,
	IF_EVENT_TYPE_REM_BSS,
	IF_EVENT_TYPE_SET_STATE,
	IF_EVENT_TYPE_SET_SCANNING,
	IF_EVENT_TYPE_SET_AP_SCAN,
	IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE,
	IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT,
	IF_EVENT_TYPE_SET_COUNTRY,
	IF_EVENT_TYPE_SET_IF_NAME,
	IF_EVENT_TYPE_SET_BRIDGE_IF_NAME,
	IF_EVENT_TYPE_SET_DRIVER,
	IF_EVENT_TYPE_SET_FAST_REAUTH,
	IF_EVENT_TYPE_SET_SCAN_INTERVAL,
	IF_EVENT_TYPE_LAST
} IfEventType;

void *wpa_supplicant_dbus_iface_Init (char *, //Bus Name
		                              char *, //Object Path
									  void *, //Connection
		                              void *, //Callback
									  void *); //parent
void wpa_supplicant_dbus_iface_Start (void *);
void wpa_supplicant_dbus_iface_Stop (void *);
void wpa_supplicant_dbus_iface_Destroy (void *);


// Set of Properties
//////////////////////////
void wpa_supplicant_dbus_iface_SetApScan (void *,
		                                  unsigned int);

void wpa_supplicant_dbus_iface_SetBssExpireAge (void *,
		                                        unsigned int);

void wpa_supplicant_dbus_iface_SetBssExpireCount (void *,
		                                          unsigned int);

void wpa_supplicant_dbus_iface_SetCountry (void *,
		                                   char *);

void wpa_supplicant_dbus_iface_SetFastReauth (void *,
		                                      bool);

void wpa_supplicant_dbus_iface_SetScanInterval (void *,
		                                        unsigned int);


#endif /* WPA_SUPPLICANT_DBUS_IFACE_H_ */
