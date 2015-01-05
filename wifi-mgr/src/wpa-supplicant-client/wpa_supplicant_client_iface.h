/*
 * wpa_supplicant_client_if.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_IF_H_
#define WPA_SUPPLICANT_CLIENT_IF_H_

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_dbus_iface.h"

#include "wpa_supplicant_client_bss_mgr.h"
#include "wpa_supplicant_client_network_mgr.h"

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_pathName [MAX_LEN_NAME];

	void *m_dbusHandle;

	wpa_supplicant_client_bssMgr *m_bssManager;
	wpa_supplicant_client_networkMgr *m_networkMgr;


	IfaceState m_state; //RO
	bool m_scanning; //RO
	unsigned long m_apScan; //RW
	unsigned long m_bssExpireAge; //RW
	unsigned long m_bssExpireCount; //RW
	char m_country [MAX_LEN_NAME]; //RW
	char m_ifName [MAX_LEN_NAME]; //RO
	char m_bridgeIfName [MAX_LEN_NAME]; //RO
	char m_driver [MAX_LEN_NAME];  //RO
	bool m_fastReauth; //RW
	unsigned long m_scanInterval; //RW

	//TODO
	//Capabilities
	//DisconnectReason
	//CurrentAuthMode
	//PKCS11EnginePath
	//PKCS11ModulePath
	//Blobs
	//currentNetwork (RO)
	//currentBss (RO)

} wpa_supplicant_client_iface;

wpa_supplicant_client_iface *wpa_supplicant_client_iface_Init (char *, //Bus Name
		                                                       char *, //Interface Object Path
										                       void *); //D-Bus Connection
void wpa_supplicant_client_iface_Start (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_Stop (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_Destroy (wpa_supplicant_client_iface *);

char *wpa_supplicant_client_iface_GetIfacePathName (wpa_supplicant_client_iface *);

//Get & Set of Properties
IfaceState wpa_supplicant_client_iface_GetState (wpa_supplicant_client_iface *);

bool wpa_supplicant_client_iface_GetIsScanning (wpa_supplicant_client_iface *);

unsigned long wpa_supplicant_client_iface_GetApScan (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetApScan(wpa_supplicant_client_iface *, unsigned long);

unsigned long wpa_supplicant_client_iface_GetBssExpireAge (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetBssExpireAge (wpa_supplicant_client_iface *, unsigned long);

unsigned long wpa_supplicant_client_iface_GetBssExpireCount (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetBssExpireCount (wpa_supplicant_client_iface *, unsigned long);

char *wpa_supplicant_client_iface_GetCountry (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetCountry (wpa_supplicant_client_iface *, char *);

char *wpa_supplicant_client_iface_GetIfName (wpa_supplicant_client_iface *);

char *wpa_supplicant_client_iface_GetBridgeIfName (wpa_supplicant_client_iface *);

char *wpa_supplicant_client_iface_GetDriver (wpa_supplicant_client_iface *);

bool wpa_supplicant_client_iface_GetFastReauth (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetFastReauth (wpa_supplicant_client_iface *, bool);

unsigned long wpa_supplicant_client_iface_GetScanInterval (wpa_supplicant_client_iface *);
void wpa_supplicant_client_iface_SetScanInterval (wpa_supplicant_client_iface *, unsigned long);


#endif /* WPA_SUPPLICANT_CLIENT_IF_H_ */
