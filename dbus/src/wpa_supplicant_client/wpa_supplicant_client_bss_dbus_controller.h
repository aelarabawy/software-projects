/*
 * wpa_supplicant_client_bss_dbus_engine.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_

#include "common.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

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
} BSS_MODE;

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	GDBusConnection * m_connection;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, BssEventType, void*);
	void *m_parent;

	//Proxies
	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_mainIfProxy;

	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;

} wpa_supplicantClient_bss_dbusController;

wpa_supplicantClient_bss_dbusController *wpa_supplicantClient_bss_dbusController_Init (char *, //Bus Name
                                                                                       char *, //Object Path
		                                                                               void *, //Connection
                                                                                       void *, //Call back Function
                                                                                       void *); //Parent
void wpa_supplicantClient_bss_dbusController_Start (wpa_supplicantClient_bss_dbusController *);
void wpa_supplicantClient_bss_dbusController_Stop (wpa_supplicantClient_bss_dbusController *);
void wpa_supplicantClient_bss_dbusController_Destroy (wpa_supplicantClient_bss_dbusController *);

// Set of Properties
//////////////////////////


#endif /* WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_ */
