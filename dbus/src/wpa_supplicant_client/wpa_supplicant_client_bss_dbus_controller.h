/*
 * wpa_supplicant_client_bss_dbus_engine.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_
#define WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_

#include "common.h"

typedef struct {


} wpa_supplicantClient_bssDbusController;

wpa_supplicantClient_bssDbusController *wpa_supplicantClient_bssDbusController_Init (void);
void wpa_supplicantClient_bssDbusController_Start (wpa_supplicantClient_bssDbusController *);
void wpa_supplicantClient_bssDbusController_Stop (wpa_supplicantClient_bssDbusController *);
void wpa_supplicantClient_bssDbusController_Destroy (wpa_supplicantClient_bssDbusController *);

#endif /* WPA_SUPPLICANT_CLIENT_BSS_DBUS_CONTROLLER_H_ */
