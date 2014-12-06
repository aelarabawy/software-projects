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

typedef enum {
	NAME_WATCHER_EVENT_NAME_UP = 0,
	NAME_WATCHER_EVENT_NAME_DOWN,
	NAME_WATCHER_EVENT_LAST
} NameWatcherEventType;

typedef struct wpa_supplicantClient_nameWatcher {
	char m_busName [BUS_NAME_MAX_LEN];
	guint m_watchId;

	//Notification Call back function
	void (*m_notifyCb) (void *, NameWatcherEventType, void*);
	void *m_parent;
} wpa_supplicantClient_nameWatcher;

wpa_supplicantClient_nameWatcher * wpa_supplicantClient_nameWatcher_Init (char *,  //bus name
                                                                          void *,  //Notification Cb
																		  void *); //parent
void wpa_supplicantClient_nameWatcher_Start (wpa_supplicantClient_nameWatcher *);
void wpa_supplicantClient_nameWatcher_Stop (wpa_supplicantClient_nameWatcher *);
void wpa_supplicantClient_nameWatcher_Destroy (wpa_supplicantClient_nameWatcher *);

#endif /* WPA_SUPPLICANT_CLIENT_NAME_WATCHER_H_ */
