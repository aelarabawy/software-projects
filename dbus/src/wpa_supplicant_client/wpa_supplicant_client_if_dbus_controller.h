/*
 * wpa_supplicant_client_if_dbus_controller.h
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_IF_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_IF_DBUS_CONTROLLER_H_

#include "common.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

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

//Don't change Order
typedef enum {
	IF_STATE_UNKNOWN = 0,
	IF_STATE_INACTIVE,
	IF_STATE_SCANNING,
	IF_STATE_AUTHENTICATING,
	IF_STATE_ASSOCIATING,
	IF_STATE_ASSOCIATED,
	IF_STATE_4_WAY_HANDSHAKE,
	IF_STATE_GROUP_HANDSHAKE,
	IF_STATE_COMPLETED,
	IF_STATE_DISCONNECTED,
	IF_STATE_LAST
} IfState;

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	GDBusConnection * m_connection;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, IfEventType, void*);
	void *m_parent;

	//Proxies
	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_mainIfProxy;


	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;
} wpa_supplicantClient_if_dbusController;

wpa_supplicantClient_if_dbusController *wpa_supplicantClient_if_dbusController_Init (char *, //Bus Name
		                                                                             char *, //Object Path
																					 void *, //Connection
		                                                                             void *, //Callback
																					 void *); //parent
void wpa_supplicantClient_if_dbusController_Start (wpa_supplicantClient_if_dbusController *);
void wpa_supplicantClient_if_dbusController_Stop (wpa_supplicantClient_if_dbusController *);
void wpa_supplicantClient_if_dbusController_Destroy (wpa_supplicantClient_if_dbusController *);


// Set of Properties
//////////////////////////
void wpa_supplicantClient_if_dbusController_SetApScan (wpa_supplicantClient_if_dbusController *,
		                                               unsigned int);

void wpa_supplicantClient_if_dbusController_SetBssExpireAge (wpa_supplicantClient_if_dbusController *,
		                                                     unsigned int);

void wpa_supplicantClient_if_dbusController_SetBssExpireCount (wpa_supplicantClient_if_dbusController *,
		                                                       unsigned int);

void wpa_supplicantClient_if_dbusController_SetCountry (wpa_supplicantClient_if_dbusController *,
		                                                char *);

void wpa_supplicantClient_if_dbusController_SetFastReauth (wpa_supplicantClient_if_dbusController *,
		                                                   bool);

void wpa_supplicantClient_if_dbusController_SetScanInterval (wpa_supplicantClient_if_dbusController *,
		                                                     unsigned int);


#endif /* WPA_SUPPLICANT_CLIENT_IF_DBUS_CONTROLLER_H_ */
