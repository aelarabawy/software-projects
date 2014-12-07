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

#include "common.h"
#include "wpa_supplicant_client_dbus_controller.h"
#include "wpa_supplicant_client_interface_manager.h"

typedef enum {
	CLIENT_STATE_IDLE = 0,
	CLIENT_STATE_READY,
} ClientState;

#define MAX_EAP_METHODS 100

//Main Structure for the Client
typedef struct {
	wpa_supplicantClient_dbusController *m_dbusController;
	wpa_supplicantClient_ifManager *m_ifManager;

	//Properties of the Client
	ClientState m_state;  //For Internal use
	ClientDbgLvl m_dbgLvl; //(RW)
	bool m_dbgShowTS; //(RW)
	bool m_dbgShowKeys; //(RW)

	int m_eapMethodCount; //(RO)
	EapMethod m_eapMethods [MAX_EAP_METHODS];  //(RO)

} wpa_supplicantClient;

//Public Methods
//==============
wpa_supplicantClient *wpa_supplicantClient_Init ();
void wpa_supplicantClient_Start (wpa_supplicantClient *);
void wpa_supplicantClient_Stop (wpa_supplicantClient *);
void wpa_supplicantClient_Destroy (wpa_supplicantClient *);

//Get/Set
ClientDbgLvl getDbgLvl (wpa_supplicantClient *);
void setDbgLvl(wpa_supplicantClient *, ClientDbgLvl);

bool getDbgShowTS (wpa_supplicantClient *);
void setDbgShowTS (wpa_supplicantClient *, bool);

bool getDbgShowKeys (wpa_supplicantClient *);
void setDbgShowKeys (wpa_supplicantClient *, bool);

int getEapMethodCount (wpa_supplicantClient *);
EapMethod getDEapMethod (wpa_supplicantClient *, int);



//Internal Methods
//================



#endif /* WPA_SUPPLICANT_CLIENT */
