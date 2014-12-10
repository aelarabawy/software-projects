/*
 * wpa_supplicant_client_network_dbus_controller.h
 *
 *  Created on: Dec 8, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NETWORK_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_NETWORK_DBUS_CONTROLLER_H_

#include "common.h"
#include "wpa_supplicant_client_proxy_introspectable.h"

typedef enum {
	NETWORK_EVENT_TYPE_FIRST = 0,
	NETWORK_EVENT_TYPE_SET_ENABLED,
	NETWORK_EVENT_TYPE_LAST
} NetworkEventType;

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	GDBusConnection * m_connection;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, NetworkEventType, void*);
	void *m_parent;

	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_ifProxy;

	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;

} wpa_supplicantClient_network_dbusController;

wpa_supplicantClient_network_dbusController *wpa_supplicantClient_network_dbusController_Init (char *, //Bus Name
		                                                                                       char *, //Object Path
																							   void *, //Connection
																			                   void *, //Call back Function
																			                   void *); //Parent

void wpa_supplicantClient_network_dbusController_Start (wpa_supplicantClient_network_dbusController *);
void wpa_supplicantClient_network_dbusController_Stop (wpa_supplicantClient_network_dbusController *);
void wpa_supplicantClient_network_dbusController_Destroy (wpa_supplicantClient_network_dbusController *);

// Set of Properties
//////////////////////////
void wpa_supplicantClient_network_dbusController_SetEnabled (wpa_supplicantClient_network_dbusController *, bool);

#endif /* WPA_SUPPLICANT_CLIENT_NETWORK_DBUS_CONTROLLER_H_ */

