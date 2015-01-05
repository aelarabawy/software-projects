/*
 * wpa_supplicant_dbus_common.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_DBUS_COMMON_H_
#define WPA_SUPPLICANT_DBUS_COMMON_H_

#include <gio/gio.h>

#include "wpa_supplicant_common.h"
#include "logger.h"

#define SYNC_API 1
#define MAX_LEN_NAME 100

//Interfaces
#define WPA_SUPPLICANT_MAIN_INTERFACE           "fi.w1.wpa_supplicant1"
#define WPA_SUPPLICANT_INTERFACE_INTERFACE      "fi.w1.wpa_supplicant1.Interface"
#define WPA_SUPPLICANT_INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"
#define WPA_SUPPLICANT_BSS_INTERFACE            "fi.w1.wpa_supplicant1.BSS"
#define WPA_SUPPLICANT_NETWORK_INTERFACE        "fi.w1.wpa_supplicant1.Network"



#endif /* WPA_SUPPLICANT_DBUS_COMMON_H_ */
