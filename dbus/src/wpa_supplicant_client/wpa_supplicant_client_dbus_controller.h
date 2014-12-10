/*
 * wpa_supplicant_client_dbus_controller.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_

#include "common.h"

#include "wpa_supplicant_client_name_watcher.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

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


typedef enum {
	CLIENT_DBG_LVL_VERBOSE = 0,
	CLIENT_DBG_LVL_DEBUG,
	CLIENT_DBG_LVL_INFO,
	CLIENT_DBG_LVL_WRN,
	CLIENT_DBG_LVL_ERR
} ClientDbgLvl;

//Don't edit or re-order
typedef enum {
	EAP_NONE = 0,
	EAP_MD5,
	EAP_TLS,
	EAP_MSCHAPV2,
	EAP_PEAP,
	EAP_TTLS,
	EAP_GTC,
	EAP_OTP,
	EAP_SIM,
	EAP_LEAP,
	EAP_PSK,
	EAP_AKA,
	EAP_AKA_DASH,
	EAP_FAST,
	EAP_PAX,
	EAP_SAKE,
	EAP_GPSK,
	EAP_WSC,
	EAP_IKEV2,
	EAP_TNC,
	EAP_PWD,
	EAP_LAST
} EapMethod;

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	//Proxies
	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_mainIfProxy;
	GDBusProxy *m_ifIfProxy;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, ClientEventType, void*);
	void *m_parent;

	wpa_supplicantClient_nameWatcher *m_nameWatcher;
	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;

	GDBusConnection * m_connection;
	GMainLoop * m_loop;

} wpa_supplicantClient_dbusController;

wpa_supplicantClient_dbusController *wpa_supplicantClient_dbusController_Init (char *, //Bus Name
		                                                                       char *, //Object Path
																			   void *, //Call back Function
																			   void *); //Parent
void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *);

void wpa_supplicantClient_dbusController_SetDbgLvl (wpa_supplicantClient_dbusController *, ClientDbgLvl);
void wpa_supplicantClient_dbusController_SetShowTS (wpa_supplicantClient_dbusController *, bool);
void wpa_supplicantClient_dbusController_SetShowKeys (wpa_supplicantClient_dbusController *, bool);

#endif /* WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_ */
