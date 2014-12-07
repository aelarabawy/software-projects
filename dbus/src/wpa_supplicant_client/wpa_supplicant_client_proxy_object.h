/*
 * wpa_supplicant_client_proxy_object.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_H_
#define WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_H_

#include "common.h"
#include "wpa_supplicant_client_defines.h"

typedef struct wpa_supplicantClient_ProxyObject {
	GDBusObjectProxy *m_objectProxy;
	GDBusProxy *m_mainIfProxy;
	GDBusProxy *m_ifIfProxy;

	//Call Back function to the parent
	void (*m_notifyCb) (void *, ClientEventType, void*);
	void *m_parent;
} wpa_supplicantClient_ProxyObject;

//Methods
wpa_supplicantClient_ProxyObject * wpa_supplicantClient_proxyObject_Init (void *cb, void *parent);
void wpa_supplicantClient_proxyObject_Start (wpa_supplicantClient_ProxyObject *);
void wpa_supplicantClient_proxyObject_Stop (wpa_supplicantClient_ProxyObject *);
void wpa_supplicantClient_proxyObject_Destroy(wpa_supplicantClient_ProxyObject *);

void wpa_supplicantClient_proxyObject_StartFollowing(wpa_supplicantClient_ProxyObject *,
		                                             GDBusConnection *);
void wpa_supplicantClient_proxyObject_StopFollowing(wpa_supplicantClient_ProxyObject *);

#endif /* WPA_SUPPLICANT_CLIENT_PROXY_OBJECT_H_ */