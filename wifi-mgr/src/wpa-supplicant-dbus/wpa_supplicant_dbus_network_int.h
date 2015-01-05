/*
 * wpa_supplicant_dbus_network_int.h
 *
 *  Created on: Dec 8, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_NETWORK_INT_H_
#define WPA_SUPPLICANT_DBUS_NETWORK_INT_H_

#include "wpa_supplicant_dbus_common.h"
#include "wpa_supplicant_dbus_network.h"
#include "wpa_supplicant_dbus_introspectable.h"


typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	GDBusConnection * m_connection;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, NetworkEventType, void*);
	void *m_parent;

	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_ifProxy;

	wpa_supplicant_dbus_introspectable *m_proxyIntrospectable;

} wpa_supplicant_dbus_network;


#endif /* WPA_SUPPLICANT_DBUS_NETWORK_INT_H_ */

