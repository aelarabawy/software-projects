/*
 * wpa_supplicant_dbus_bss_int.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_BSS_INT_H_
#define WPA_SUPPLICANT_DBUS_BSS_INT_H_

#include "wpa_supplicant_dbus_common.h"
#include "wpa_supplicant_dbus_bss.h"
#include "wpa_supplicant_dbus_introspectable.h"

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

	wpa_supplicant_dbus_introspectable *m_proxyIntrospectable;

} wpa_supplicant_dbus_bss;


// Set of Properties
//////////////////////////


#endif /* WPA_SUPPLICANT_DBUS_BSS_INT_H_ */
