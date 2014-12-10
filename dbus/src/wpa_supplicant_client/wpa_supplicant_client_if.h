/*
 * wpa_supplicant_client_if.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_IF_H_
#define WPA_SUPPLICANT_CLIENT_IF_H_

#include "common.h"
#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_if_dbus_controller.h"
#include "wpa_supplicant_client_bss_manager.h"
#include "wpa_supplicant_client_network_mgr.h"

typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_pathName [MAX_LEN_NAME];

	wpa_supplicantClient_if_dbusController *m_dbusController;

	wpa_supplicantClient_bssManager *m_bssManager;
	wpa_supplicantClient_networkMgr *m_networkMgr;


	IfState m_state; //RO
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

} wpa_supplicantClient_if;

wpa_supplicantClient_if *wpa_supplicantClient_if_Init (char *, //Bus Name
		                                               char *, //Interface Object Path
													   void *); //D-Bus Connection
void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *);

char *wpa_supplicantClient_if_GetIfPathName (wpa_supplicantClient_if *);


//Get & Set of Properties
IfState wpa_supplicantClient_if_GetState (wpa_supplicantClient_if *);

bool wpa_supplicantClient_if_GetIsScanning (wpa_supplicantClient_if *);

unsigned long wpa_supplicantClient_if_GetApScan (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetApScan(wpa_supplicantClient_if *, unsigned long);

unsigned long wpa_supplicantClient_if_GetBssExpireAge (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetBssExpireAge (wpa_supplicantClient_if *, unsigned long);

unsigned long wpa_supplicantClient_if_GetBssExpireCount (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetBssExpireCount (wpa_supplicantClient_if *, unsigned long);

char *wpa_supplicantClient_if_GetCountry (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetCountry (wpa_supplicantClient_if *, char *);

char *wpa_supplicantClient_if_GetIfName (wpa_supplicantClient_if *);

char *wpa_supplicantClient_if_GetBridgeIfName (wpa_supplicantClient_if *);

char *wpa_supplicantClient_if_GetDriver (wpa_supplicantClient_if *);

bool wpa_supplicantClient_if_GetFastReauth (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetFastReauth (wpa_supplicantClient_if *, bool);

unsigned long wpa_supplicantClient_if_GetScanInterval (wpa_supplicantClient_if *);
void wpa_supplicantClient_if_SetScanInterval (wpa_supplicantClient_if *, unsigned long);


#endif /* WPA_SUPPLICANT_CLIENT_IF_H_ */
