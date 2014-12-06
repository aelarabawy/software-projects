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
#include "wpa_supplicant_client_proxy_object.h"

typedef enum {
	CLIENT_EVENT_TYPE_READY = 0,
	CLIENT_EVENT_TYPE_ADD_IF,
	CLIENT_EVENT_TYPE_DEL_IF,
	CLIENT_EVENT_TYPE_SET_DBG_LEVEL,
	CLIENT_EVENT_TYPE_SET_SHOW_TS,
	CLIENT_EVENT_TYPE_SET_SHOW_KEYS,
	CLIENT_EVENT_TYPE_LAST
} ClientEventType;


typedef struct {

	//Call Back function to the parent
	void (*m_notifyCb) (void *, ClientEventType, void*);
	void *m_parent;

	char *xmlDescription;
	wpa_supplicantClient_nameWatcher *m_nameWatcher;
	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;
	wpa_supplicantClient_ProxyObject *m_proxyObject;

	GDBusConnection * m_connection;
	GMainLoop * m_loop;

} wpa_supplicantClient_dbusController;

wpa_supplicantClient_dbusController *wpa_supplicantClient_dbusController_Init (void *cb, void *parent);
void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *);


#endif /* WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_ */
