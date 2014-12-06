/*
 * wpa_supplicant_client_defines.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_DEFINES_H_
#define WPA_SUPPLICANT_CLIENT_DEFINES_H_

//Well-Known Bus Name
#define WPA_SUPPLICANT_BUS_NAME            "fi.w1.wpa_supplicant1"

//Object Path
#define WPA_SUPPLICANT_MAIN_OBJECT_PATH    "/fi/w1/wpa_supplicant1"

//Interfaces
#define WPA_SUPPLICANT_MAIN_INTERFACE      "fi.w1.wpa_supplicant1"
#define WPA_SUPPLICANT_INTERFACE_INTERFACE "fi.w1.wpa_supplicant1.Interface"
#define WPA_SUPPLICANT_INTROSPECTABLE_INTERFACE    "org.freedesktop.DBus.Introspectable"

#endif /* WPA_SUPPLICANT_CLIENT_DEFINES_H_ */
