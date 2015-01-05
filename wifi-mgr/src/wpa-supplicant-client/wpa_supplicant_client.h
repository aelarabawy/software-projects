/*
 * wpa_supplicant_client.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 *
 *  The Well-Known Bus Name is:
 *    fi.w1.wpa_supplicant1
 *
 *
 *  The wpa_supplicant offers the following API's on the D-Bus
 *  fi.w1.wpa_supplicant1
 *  fi.w1.wpa_supplicant1.Interface
 *  fi.w1.wpa_supplicant1.Interface.WPS
 *  fi.w1.wpa_supplicant1.BSS
 *  fi.w1.wpa_supplicant1.Network
 */
#ifndef WPA_SUPPLICANT_CLIENT_H_
#define WPA_SUPPLICANT_CLIENT_H_

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_dbus_client.h"

#include "wpa_supplicant_client_iface_mgr.h"

typedef enum {
	CLIENT_STATE_IDLE = 0,
	CLIENT_STATE_READY,
} ClientState;

#define MAX_EAP_METHODS 100

//Main Structure for the Client
typedef struct {
	char m_busName [MAX_LEN_NAME];
	char m_objectPath [MAX_LEN_NAME];

	void *m_dbusHandle;
	wpa_supplicant_client_ifaceMgr *m_ifaceMgr;

	//For Internal use
	ClientState m_state;

	//Properties of the Client
	ClientDbgLvl m_dbgLvl; //(RW)
	bool m_dbgShowTS; //(RW)
	bool m_dbgShowKeys; //(RW)

	int m_eapMethodCount; //(RO)
	EapMethod m_eapMethods [MAX_EAP_METHODS];  //(RO)
} wpa_supplicant_client;



//Public Methods
//==============
wpa_supplicant_client *wpa_supplicant_client_Init (char *, //Bus Name
	                                               char *); //Object Path
void wpa_supplicant_client_Start (wpa_supplicant_client *);
void wpa_supplicant_client_Stop (wpa_supplicant_client *);
void wpa_supplicant_client_Destroy (wpa_supplicant_client *);

//Get & Set
ClientDbgLvl wpa_supplicant_client_GetDbgLvl (wpa_supplicant_client *);
void wpa_supplicant_client_SetDbgLvl(wpa_supplicant_client *, ClientDbgLvl);

bool wpa_supplicant_client_GetDbgShowTS (wpa_supplicant_client *);
void wpa_supplicant_client_SetDbgShowTS (wpa_supplicant_client *, bool);

bool wpa_supplicant_client_GetDbgShowKeys (wpa_supplicant_client *);
void wpa_supplicant_client_SetDbgShowKeys (wpa_supplicant_client *, bool);

int wpa_supplicant_client_GetEapMethodCount (wpa_supplicant_client *);
EapMethod wpa_supplicant_client_GetDEapMethod (wpa_supplicant_client *, int);


#endif /* WPA_SUPPLICANT_CLIENT */
