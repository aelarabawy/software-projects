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
#include "wpa_supplicant_client_proxy_object_manager.h"
#include "wpa_supplicant_client_proxy_introspectable.h"


typedef struct {
	char *xmlDescription;
	wpa_supplicantClient_nameWatcher *m_nameWatcher;
	wpa_supplicantClient_ProxyIntrospectable *m_proxyIntrospectable;
	wpa_supplicantClient_ProxyObjectManager *m_proxyObjectManager;

	GDBusConnection * m_connection;
	GMainLoop * m_loop;

} wpa_supplicantClient_dbusController;

wpa_supplicantClient_dbusController *wpa_supplicantClient_dbusController_Init (void);
void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *);
void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *);


#endif /* WPA_SUPPLICANT_CLIENT_DBUS_CONTROLLER_H_ */
