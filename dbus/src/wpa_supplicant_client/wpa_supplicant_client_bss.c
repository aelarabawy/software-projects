/*
 * wpa_supplicant_client_bss.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss.h"

//Prototype for the call back function from the D-Bus Controller
static void dbusControllerNotify (void *, BssEventType, void *);

wpa_supplicantClient_bss *wpa_supplicantClient_bss_Init (char *busName,
                                                         char *objectPath,
		                                                 void *connection)  {
	ENTER_FUNC("pathName = %s", objectPath);

	wpa_supplicantClient_bss *bss = malloc(sizeof(wpa_supplicantClient_bss));
    if (!bss) {
    	ALLOC_FAIL("bss");
    	goto FAIL_BSS;
    }
    memset(bss, 0, sizeof(wpa_supplicantClient_bss));

    strcpy(bss->m_busName, busName);
    strcpy(bss->m_objectPath, objectPath);

	//Initialize the D-Bus Controller
	bss->m_dbusController = wpa_supplicantClient_bss_dbusController_Init(busName,
			                                                             objectPath,
																		 connection,
																		 dbusControllerNotify,
			                                                             (void *)bss);
	if (!bss->m_dbusController) {
		ERROR("Failed to initialize the BSS D-BUS Controller ... exiting");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Success
	goto SUCCESS;

FAIL_DBUS_CONTROLLER:
    free(bss);

FAIL_BSS:
    EXIT_WITH_ERROR();
    return NULL;
SUCCESS:
	EXIT();
	return bss;
}


void wpa_supplicantClient_bss_Start (wpa_supplicantClient_bss *bss) {

	ENTER();

	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_bss_dbusController_Start(bss->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_bss_Stop (wpa_supplicantClient_bss *bss) {
	ENTER();

	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_bss_dbusController_Stop(bss->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_bss_Destroy (wpa_supplicantClient_bss *bss) {
	ENTER();

	if (!bss){
		NULL_POINTER("bss");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_bss_dbusController_Destroy(bss->m_dbusController);

	//Free the BSS Object
	free(bss);

	EXIT();
	return;
}

char* wpa_supplicantClient_bss_GetPathName (wpa_supplicantClient_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_objectPath;
}

// Get & Set of Properties
//////////////////////////
BSS_MODE wpa_supplicant_bss_GetMode (wpa_supplicantClient_bss* bss) {
	ENTER();
	EXIT();
	return bss->m_mode;
}

unsigned char *wpa_supplicant_bss_GetBssid (wpa_supplicantClient_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_bssidLen;
	EXIT();
	return bss->m_bssid;
}

unsigned char *wpa_supplicant_bss_GetSsid (wpa_supplicantClient_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_ssidLen;
	EXIT();
	return bss->m_ssid;
}

unsigned long wpa_supplicant_bss_Getfreq (wpa_supplicantClient_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_freq;
}

long wpa_supplicant_bss_GetSignal(wpa_supplicantClient_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_signal;
}

unsigned long *wpa_supplicant_bss_getRates (wpa_supplicantClient_bss *bss, unsigned long *ptrCount) {
	ENTER();
	*ptrCount = bss->m_rateCount;
	EXIT();
	return bss->m_rates;
}

bool wpa_supplicant_bss_isPrivacySupprtd (wpa_supplicantClient_bss *bss) {
	ENTER();
	EXIT();
	return bss->m_isPrivacySupprtd;
}


//Private Functions
///////////////////
//Notification function (call back from dbus controller)
static void dbusControllerNotify (void *parent, BssEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args);

	wpa_supplicantClient_bss *bss = (wpa_supplicantClient_bss *)parent;

	unsigned char *byteArray;
	switch (type) {
	case BSS_EVENT_TYPE_SET_MODE:
		INFO("BSS_EVENT_TYPE_SET_MODE");
		bss->m_mode = (BSS_MODE)args;
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
