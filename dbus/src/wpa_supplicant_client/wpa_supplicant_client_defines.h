/*
 * wpa_supplicant_client_defines.h
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#ifndef WPA_SUPPLICANT_CLIENT_DEFINES_H_
#define WPA_SUPPLICANT_CLIENT_DEFINES_H_

#define MAX_SIZE_PATH_NAME 100

//Well-Known Bus Name
#define WPA_SUPPLICANT_BUS_NAME            "fi.w1.wpa_supplicant1"

//Object Path
#define WPA_SUPPLICANT_MAIN_OBJECT_PATH    "/fi/w1/wpa_supplicant1"

//Interfaces
#define WPA_SUPPLICANT_MAIN_INTERFACE      "fi.w1.wpa_supplicant1"
#define WPA_SUPPLICANT_INTERFACE_INTERFACE "fi.w1.wpa_supplicant1.Interface"
#define WPA_SUPPLICANT_INTROSPECTABLE_INTERFACE    "org.freedesktop.DBus.Introspectable"

//This list is for events for the fi.w1.wpa.supplicant1 Interface
typedef enum {
	CLIENT_EVENT_TYPE_READY = 0,
	CLIENT_EVENT_TYPE_ADD_IF,
	CLIENT_EVENT_TYPE_REM_IF,
	CLIENT_EVENT_TYPE_SET_DBG_LEVEL,
	CLIENT_EVENT_TYPE_SET_SHOW_TS,
	CLIENT_EVENT_TYPE_SET_SHOW_KEYS,
	CLIENT_EVENT_TYPE_ADD_EAP_METHOD,
	CLIENT_EVENT_TYPE_LAST
} ClientEventType;

typedef enum {
	CLIENT_DBG_LVL_VERBOSE = 0,
	CLIENT_DBG_LVL_DEBUG,
	CLIENT_DBG_LVL_INFO,
	CLIENT_DBG_LVL_WRN,
	CLIENT_DBG_LVL_ERR
} ClientDbgLvl;

//Don't edit or re-order
typedef enum {
	EAP_NONE = 0,
	EAP_MD5,
	EAP_TLS,
	EAP_MSCHAPV2,
	EAP_PEAP,
	EAP_TTLS,
	EAP_GTC,
	EAP_OTP,
	EAP_SIM,
	EAP_LEAP,
	EAP_PSK,
	EAP_AKA,
	EAP_AKA_DASH,
	EAP_FAST,
	EAP_PAX,
	EAP_SAKE,
	EAP_GPSK,
	EAP_WSC,
	EAP_IKEV2,
	EAP_TNC,
	EAP_PWD,
	EAP_LAST
} EapMethod;

#endif /* WPA_SUPPLICANT_CLIENT_DEFINES_H_ */
