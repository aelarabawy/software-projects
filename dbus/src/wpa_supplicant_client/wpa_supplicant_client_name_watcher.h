/*
 * wpa_supplicant_client_name_watcher.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_
#define WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_

#include "common.h"
#include <gio/gio.h>

typedef struct wpa_supplicantClient_nameWatcher {
	char m_busName [BUS_NAME_MAX_LEN];
	guint m_watchId;

	void (*m_onNameUpCallBack)(void*, GDBusConnection *);
	void (*m_onNameDownCallBack)(void*, GDBusConnection *);
	void * parent;
} wpa_supplicantClient_nameWatcher;

wpa_supplicantClient_nameWatcher * wpa_supplicantClient_nameWatcher_Init (char *,
                                                                          void (*onNameUp) (void*, GDBusConnection *),
																		  void (*onNameDown) (void*, GDBusConnection *),
																		  void * parent);
void wpa_supplicantClient_nameWatcher_Start (wpa_supplicantClient_nameWatcher *);
void wpa_supplicantClient_nameWatcher_Stop (wpa_supplicantClient_nameWatcher *);
void wpa_supplicantClient_nameWatcher_Cleanup (wpa_supplicantClient_nameWatcher *);

#endif /* WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_ */
