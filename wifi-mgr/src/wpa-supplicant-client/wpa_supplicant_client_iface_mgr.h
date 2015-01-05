/*
 * wpa_supplicant_client_iface_mgr.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_IFACE_MGR_H_
#define WPA_SUPPLICANT_CLIENT_IFACE_MGR_H_

#include "wpa_supplicant_client_iface.h"

typedef struct ifaceList ifaceList;
typedef struct ifaceListHead ifaceListHead;

struct ifaceListHead {
	void *placeHolder;
	ifaceList*  m_next;
};

struct ifaceList {
  wpa_supplicant_client_iface *m_iface;
  ifaceList *m_next;
};


typedef struct {
	char m_busName [MAX_LEN_NAME];
	void *m_dbusConnection;
	ifaceListHead  m_ifaceGroup;
} wpa_supplicant_client_ifaceMgr;

wpa_supplicant_client_ifaceMgr *wpa_supplicant_client_ifaceMgr_Init (char *); //Bus Name
void wpa_supplicant_client_ifaceMgr_Start (wpa_supplicant_client_ifaceMgr *,
		                                   void *); //D-Bus Connection
void wpa_supplicant_client_ifaceMgr_Stop (wpa_supplicant_client_ifaceMgr *);
void wpa_supplicant_client_ifaceMgr_Destroy (wpa_supplicant_client_ifaceMgr *);

void wpa_supplicant_client_ifaceMgr_AddIface (wpa_supplicant_client_ifaceMgr *, char*);
void wpa_supplicant_client_ifaceMgr_RemIface (wpa_supplicant_client_ifaceMgr *, char*);

#endif /* WPA_SUPPLICANT_CLIENT_IFACE_MGR_H_ */
