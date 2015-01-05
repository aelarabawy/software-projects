/*
 * wpa_supplicant_client_name_watcher.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_
#define WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_

#include "wpa_supplicant_dbus_common.h"

typedef enum {
	NAME_WATCHER_EVENT_NAME_UP = 0,
	NAME_WATCHER_EVENT_NAME_DOWN,
	NAME_WATCHER_EVENT_LAST
} NameWatcherEventType;

typedef struct wpa_supplicant_dbus_nameWatcher {
	char m_busName [MAX_LEN_NAME];
	guint m_watchId;

	//Notification Call back function
	void (*m_notifyCb) (void *, NameWatcherEventType, void*);
	void *m_parent;
} wpa_supplicant_dbus_nameWatcher;

wpa_supplicant_dbus_nameWatcher * wpa_supplicant_dbus_nameWatcher_Init (char *,  //bus name
                                                                          void *,  //Notification Cb
																		  void *); //parent
void wpa_supplicant_dbus_nameWatcher_Start (wpa_supplicant_dbus_nameWatcher *);
void wpa_supplicant_dbus_nameWatcher_Stop (wpa_supplicant_dbus_nameWatcher *);
void wpa_supplicant_dbus_nameWatcher_Destroy (wpa_supplicant_dbus_nameWatcher *);

#endif /* WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_ */
