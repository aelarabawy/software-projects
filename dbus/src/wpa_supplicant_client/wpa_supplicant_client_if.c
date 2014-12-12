/*
 * wpa_supplicant_client_if.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_if.h"

//Prototype for the call back function from the D-Bus Controller
static void dbusControllerNotify (void *, IfEventType, void*);


wpa_supplicantClient_if *wpa_supplicantClient_if_Init (char *busName,
		                                               char *pathName,
													   void *connection)  {
	ENTER_FUNC("pathName = %s", pathName);

	wpa_supplicantClient_if * interface = malloc(sizeof(wpa_supplicantClient_if));
    if (!interface) {
    	ALLOC_FAIL("interface");
    	goto FAIL_IF;
    }
    memset(interface, 0, sizeof(wpa_supplicantClient_if));

    strcpy(interface->m_busName, busName);
    strcpy(interface->m_pathName, pathName);

	//Initialize the D-Bus Controller
	interface->m_dbusController = wpa_supplicantClient_if_dbusController_Init(busName,
			                                                                  pathName,
																			  connection,
			                                                                  dbusControllerNotify,
			                                                                  (void *)interface);
	if (!interface->m_dbusController) {
		ERROR("Failed to initialize the Interface D-BUS Controller ... exiting");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Initialize the Network Manager
	interface->m_networkMgr = wpa_supplicantClient_networkMgr_Init(busName, connection);
	if (!interface->m_networkMgr) {
		ERROR("Failed to initialize the Interface Network Manager ... exiting");

		goto FAIL_NW_MANAGER;
	}

	//Initialize the BSS Manager
	interface->m_bssManager = wpa_supplicantClient_bssManager_Init(busName, connection);
	if (!interface->m_bssManager) {
		ERROR("Failed to initialize the Interface BSS Manager ... exiting");

		goto FAIL_BSS_MANAGER;
	}

	//Success
	goto SUCCESS;

FAIL_BSS_MANAGER:
	wpa_supplicantClient_networkMgr_Destroy(interface->m_networkMgr);

FAIL_NW_MANAGER:
	wpa_supplicantClient_if_dbusController_Destroy(interface->m_dbusController);

FAIL_DBUS_CONTROLLER:
	free(interface);

FAIL_IF:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return interface;
}


void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *interface)  {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_if_dbusController_Start(interface->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *interface)  {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_if_dbusController_Stop(interface->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *interface) {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicantClient_if_dbusController_Destroy(interface->m_dbusController);

	//Finally Free the Interface
	free(interface);

	EXIT();
	return;
}

char* wpa_supplicantClient_if_GetIfPathName(wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_pathName;
}


// Get & Set of Properties
//////////////////////////
IfState wpa_supplicantClient_if_GetState (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_state;
}

bool wpa_supplicantClient_if_GetIsScanning (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_scanning;
}

unsigned long wpa_supplicantClient_if_GetApScan (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_apScan;
}

void wpa_supplicantClient_if_SetApScan(wpa_supplicantClient_if *interface, unsigned long scan) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetApScan(interface->m_dbusController, scan);
	interface->m_apScan = scan;
	EXIT();
}

unsigned long wpa_supplicantClient_if_GetBssExpireAge (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_bssExpireAge;
}

void wpa_supplicantClient_if_SetBssExpireAge (wpa_supplicantClient_if *interface, unsigned long age) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetBssExpireAge(interface->m_dbusController, age);
	interface->m_bssExpireAge = age;
	EXIT();
}

unsigned long wpa_supplicantClient_if_GetBssExpireCount (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_bssExpireCount;
}

void wpa_supplicantClient_if_SetBssExpireCount (wpa_supplicantClient_if *interface, unsigned long count) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetBssExpireCount(interface->m_dbusController, count);
	interface->m_bssExpireCount = count;
	EXIT();
}

char *wpa_supplicantClient_if_GetCountry (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_country;
}

void wpa_supplicantClient_if_SetCountry (wpa_supplicantClient_if *interface, char *country) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetCountry(interface->m_dbusController, country);
	strcpy(interface->m_country, country);
	EXIT();
}

char *wpa_supplicantClient_if_GetIfName (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_ifName;
}

char *wpa_supplicantClient_if_GetBridgeIfName (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_bridgeIfName;
}

char *wpa_supplicantClient_if_GetDriver (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_driver;
}

bool wpa_supplicantClient_if_GetFastReauth (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_fastReauth;
}

void wpa_supplicantClient_if_SetFastReauth (wpa_supplicantClient_if *interface, bool fastReauth) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetFastReauth(interface->m_dbusController, fastReauth);
	interface->m_fastReauth = fastReauth;
	EXIT();
}

unsigned long wpa_supplicantClient_if_GetScanInterval (wpa_supplicantClient_if *interface) {
	ENTER();
	EXIT();
	return interface->m_scanInterval;
}

void wpa_supplicantClient_if_SetScanInterval (wpa_supplicantClient_if *interface, unsigned long interval) {
	ENTER();
	wpa_supplicantClient_if_dbusController_SetScanInterval(interface->m_dbusController, interval);
	interface->m_scanInterval = interval;
	EXIT();
}


// Private Functions
////////////////////

//Notification function (call back from dbus controller)
static void dbusControllerNotify (void *parent, IfEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args);

	wpa_supplicantClient_if *interface = (wpa_supplicantClient_if *)parent;

	switch (type) {
	case IF_EVENT_TYPE_SET_STATE:
		INFO("IF_EVENT_TYPE_SET_STATE");
		interface->m_state = (IfState)args;
		break;

	case IF_EVENT_TYPE_SET_SCANNING:
		INFO("IF_EVENT_TYPE_SET_SCANNING");
		interface->m_scanning = (bool)args;
		break;

	case IF_EVENT_TYPE_SET_AP_SCAN:
		interface->m_apScan = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE:
		INFO("IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE");
		interface->m_bssExpireAge = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT:
		INFO("IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT");
		interface->m_bssExpireCount = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_COUNTRY:
		INFO("IF_EVENT_TYPE_SET_COUNTRY");
		strcpy(interface->m_country, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_IF_NAME:
		INFO("IF_EVENT_TYPE_SET_IF_NAME");
		strcpy(interface->m_ifName, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_BRIDGE_IF_NAME:
		INFO("IF_EVENT_TYPE_SET_BRIDGE_IF_NAME");
		strcpy(interface->m_bridgeIfName, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_DRIVER:
		INFO("IF_EVENT_TYPE_SET_DRIVER");
		strcpy(interface->m_driver, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_FAST_REAUTH:
		INFO("IF_EVENT_TYPE_SET_FAST_REAUTH");
		interface->m_fastReauth = (bool)args;
		break;

	case IF_EVENT_TYPE_SET_SCAN_INTERVAL:
		INFO("IF_EVENT_TYPE_SET_SCAN_INTERVAL");
		interface->m_scanInterval = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_ADD_NW:
		{
			INFO("IF_EVENT_TYPE_ADD_NW");
			char *network = (char *)args;
			PROGRESS("Interface (%s): Adding Network with Object Path %s",interface->m_ifName, network);
			wpa_supplicantClient_networkMgr_AddNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_REM_NW:
		{
			INFO("IF_EVENT_TYPE_REM_NW");
			char *network = (char *)args;
			PROGRESS("Interface (%s): Removing Network with Object Path %s",interface->m_ifName, network);
			wpa_supplicantClient_networkMgr_RemNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_ADD_BSS:
		{
			INFO("IF_EVENT_TYPE_ADD_BSS");
			char *bss = (char *)args;
			PROGRESS("Interface (%s): Adding BSS with Object Path %s",interface->m_ifName, bss);
			wpa_supplicantClient_bssManager_AddBss(interface->m_bssManager, bss);
		}
		break;

	case IF_EVENT_TYPE_REM_BSS:
		{
			INFO("IF_EVENT_TYPE_REM_BSS");
			char *bss = (char *)args;
			PROGRESS("Interface (%s): Removing BSS with Object Path %s",interface->m_ifName, bss);
			wpa_supplicantClient_bssManager_RemBss(interface->m_bssManager, bss);
		}
		break;


	default:
		ERROR("Invalid/UnSupported IF Event Type %d", type);
		break;
	}

	EXIT();
	return;
}
