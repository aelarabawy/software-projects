/*
 * wpa_supplicant_client_dbus_controller.h
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_

#include "wpa_supplicant_dbus_common.h"
#include "wpa_supplicant_dbus_client.h"
#include "wpa_supplicant_dbus_name_watcher.h"
#include "wpa_supplicant_dbus_introspectable.h"


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

	wpa_supplicant_dbus_nameWatcher *m_nameWatcher;
	wpa_supplicant_dbus_introspectable *m_proxyIntrospectable;

	GDBusConnection * m_connection;
	GMainLoop * m_loop;

} wpa_supplicant_dbus_client;


#endif /* WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_ */
