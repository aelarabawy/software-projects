/*
 * wpa_supplicant_client_bss.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss.h"

//Prototype for the call back function from the D-Bus Driver
static void dbusNotify (void *, BssEventType, void *);

wpa_supplicant_client_bss *wpa_supplicant_client_bss_Init (char *busName,
                                                           char *objectPath,
		                                                   void *connection)  {
	ENTER_FUNC("pathName = %s", objectPath);

	wpa_supplicant_client_bss *bss = malloc(sizeof(wpa_supplicant_client_bss));
    if (!bss) {
    	ALLOC_FAIL("bss");
    	goto FAIL_BSS;
    }
    memset(bss, 0, sizeof(wpa_supplicant_client_bss));

    strcpy(bss->m_busName, busName);
    strcpy(bss->m_objectPath, objectPath);

	//Initialize the D-Bus Handler
	bss->m_dbusHandle = wpa_supplicant_dbus_bss_Init(busName,
			                                         objectPath,
												  	 connection,
													 dbusNotify,
			                                         (void *)bss);
	if (!bss->m_dbusHandle) {
		ERROR("Failed to initialize the BSS D-BUS Handle ... exiting");

		goto FAIL_DBUS_HANDLE;
	}

	//Success
	goto SUCCESS;

FAIL_DBUS_HANDLE:
    free(bss);

FAIL_BSS:
    EXIT_WITH_ERROR();
    return NULL;
SUCCESS:
	EXIT();
	return bss;
}


void wpa_supplicant_client_bss_Start (wpa_supplicant_client_bss *bss) {

	ENTER();
	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_bss_Start(bss->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_bss_Stop (wpa_supplicant_client_bss *bss) {
	ENTER();

	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_bss_Stop(bss->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_bss_Destroy (wpa_supplicant_client_bss *bss) {
	ENTER();

	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_bss_Destroy(bss->m_dbusHandle);

	//Free the BSS Object
	free(bss);

	EXIT();
	return;
}

char* wpa_supplicant_client_bss_GetPathName (wpa_supplicant_client_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_objectPath;
}

// Get & Set of Properties
//////////////////////////
BssMode wpa_supplicant_bss_GetMode (wpa_supplicant_client_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_mode;
}

unsigned char *wpa_supplicant_bss_GetBssid (wpa_supplicant_client_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_bssidLen;
	EXIT();
	return bss->m_bssid;
}

unsigned char *wpa_supplicant_bss_GetSsid (wpa_supplicant_client_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_ssidLen;
	EXIT();
	return bss->m_ssid;
}

unsigned long wpa_supplicant_bss_Getfreq (wpa_supplicant_client_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_freq;
}

long wpa_supplicant_bss_GetSignal(wpa_supplicant_client_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_signal;
}

unsigned long *wpa_supplicant_bss_getRates (wpa_supplicant_client_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_rateCount;
	EXIT();
	return bss->m_rates;
}

bool wpa_supplicant_bss_isPrivacySupprtd (wpa_supplicant_client_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_isPrivacySupprtd;
}


//Private Functions
///////////////////
//Notification function (call back from dbus driver)
static void dbusNotify (void *parent, BssEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args);

	wpa_supplicant_client_bss *bss = (wpa_supplicant_client_bss *)parent;

	unsigned char *byteArray;
	switch (type) {
	case BSS_EVENT_TYPE_SET_MODE:
		INFO("BSS_EVENT_TYPE_SET_MODE");
		bss->m_mode = (BssMode)args;
		break;

	case BSS_EVENT_TYPE_SET_BSSID:
		INFO("BSS_EVENT_TYPE_SET_BSSID");
		byteArray = (unsigned char *)args;
		bss->m_bssidLen = byteArray[0];
		memcpy(bss->m_bssid, &byteArray[1],bss->m_bssidLen);
		break;

	case BSS_EVENT_TYPE_SET_SSID:
		INFO("BSS_EVENT_TYPE_SET_SSID");
		byteArray = (unsigned char *)args;
		bss->m_ssidLen = byteArray[0];
		memcpy(bss->m_ssid, &byteArray[1],bss->m_ssidLen);
		break;

	case BSS_EVENT_TYPE_SET_FREQ:
		INFO("BSS_EVENT_TYPE_SET_FREQ");
		bss->m_freq = (unsigned long)args;
		break;

	case BSS_EVENT_TYPE_SET_SIGNAL:
		INFO("BSS_EVENT_TYPE_SET_SIGNAL");
		bss->m_signal = (long)args;
		break;

	case BSS_EVENT_TYPE_ADD_RATE:
		INFO("BSS_EVENT_TYPE_ADD_RATE");
		bss->m_rates[bss->m_rateCount++] = (unsigned long)args;
		break;

	case BSS_EVENT_TYPE_SET_PRIVACY:
		INFO("BSS_EVENT_TYPE_SET_PRIVACY");
		bss->m_isPrivacySupprtd = (bool)args;
		break;

	default:
		ERROR("Invalid/UnSupported BSS Event Type %d", type);
		break;
	}

	EXIT();
	return;
}
