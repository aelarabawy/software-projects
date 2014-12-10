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
	printf("Entered wpa_supplicantClient_if_Init() with pathName = %s \n", pathName);

	wpa_supplicantClient_if * interface = malloc(sizeof(wpa_supplicantClient_if));
    if (!interface) {
    	printf("Failed to allocate the Interface Object ... Exiting\n");
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
		printf("Failed to initialize the Interface D-BUS Controller ... exiting\n");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Initialize the Network Manager
	interface->m_networkMgr = wpa_supplicantClient_networkMgr_Init(busName, connection);
	if (!interface->m_networkMgr) {
		printf("Failed to initialize the Interface Network Manager ... exiting\n");

		goto FAIL_NW_MANAGER;
	}

	//Initialize the BSS Manager
	interface->m_bssManager = wpa_supplicantClient_bssManager_Init(busName, connection);
	if (!interface->m_bssManager) {
		printf("Failed to initialize the Interface BSS Manager ... exiting\n");

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
    return NULL;
SUCCESS:
	return interface;
}

void wpa_supplicantClient_if_Start (wpa_supplicantClient_if *interface)  {
	printf("Entered wpa_supplicantClient_if_Start() \n");
	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	wpa_supplicantClient_if_dbusController_Start(interface->m_dbusController);

	return;
}

void wpa_supplicantClient_if_Stop (wpa_supplicantClient_if *interface)  {
	printf("Entered wpa_supplicantClient_if_Stop() \n");

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	wpa_supplicantClient_if_dbusController_Stop(interface->m_dbusController);

	return;
}

void wpa_supplicantClient_if_Destroy (wpa_supplicantClient_if *interface) {
	printf("Entered wpa_supplicantClient_if_Destroy()\n");

	if (!interface){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	wpa_supplicantClient_if_dbusController_Destroy(interface->m_dbusController);

	//Finally Free the Interface
	free(interface);

	return;
}

char* wpa_supplicantClient_if_GetIfPathName(wpa_supplicantClient_if *interface) {
	return interface->m_pathName;
}


// Get & Set of Properties
//////////////////////////
IfState wpa_supplicantClient_if_GetState (wpa_supplicantClient_if *interface) {
	return interface->m_state;
}

bool wpa_supplicantClient_if_GetIsScanning (wpa_supplicantClient_if *interface) {
	return interface->m_scanning;
}

unsigned long wpa_supplicantClient_if_GetApScan (wpa_supplicantClient_if *interface) {
	return interface->m_apScan;
}

void wpa_supplicantClient_if_SetApScan(wpa_supplicantClient_if *interface, unsigned long scan) {
	wpa_supplicantClient_if_dbusController_SetApScan(interface->m_dbusController, scan);
	interface->m_apScan = scan;
}

unsigned long wpa_supplicantClient_if_GetBssExpireAge (wpa_supplicantClient_if *interface) {
	return interface->m_bssExpireAge;
}

void wpa_supplicantClient_if_SetBssExpireAge (wpa_supplicantClient_if *interface, unsigned long age) {
	wpa_supplicantClient_if_dbusController_SetBssExpireAge(interface->m_dbusController, age);
	interface->m_bssExpireAge = age;
}

unsigned long wpa_supplicantClient_if_GetBssExpireCount (wpa_supplicantClient_if *interface) {
	return interface->m_bssExpireCount;
}

void wpa_supplicantClient_if_SetBssExpireCount (wpa_supplicantClient_if *interface, unsigned long count) {
	wpa_supplicantClient_if_dbusController_SetBssExpireCount(interface->m_dbusController, count);
	interface->m_bssExpireCount = count;
}

char *wpa_supplicantClient_if_GetCountry (wpa_supplicantClient_if *interface) {
	return interface->m_country;
}

void wpa_supplicantClient_if_SetCountry (wpa_supplicantClient_if *interface, char *country) {
	wpa_supplicantClient_if_dbusController_SetCountry(interface->m_dbusController, country);
	strcpy(interface->m_country, country);
}

char *wpa_supplicantClient_if_GetIfName (wpa_supplicantClient_if *interface) {
	return interface->m_ifName;
}

char *wpa_supplicantClient_if_GetBridgeIfName (wpa_supplicantClient_if *interface) {
	return interface->m_bridgeIfName;
}

char *wpa_supplicantClient_if_GetDriver (wpa_supplicantClient_if *interface) {
	return interface->m_driver;
}

bool wpa_supplicantClient_if_GetFastReauth (wpa_supplicantClient_if *interface) {
	return interface->m_fastReauth;
}

void wpa_supplicantClient_if_SetFastReauth (wpa_supplicantClient_if *interface, bool fastReauth) {
	wpa_supplicantClient_if_dbusController_SetFastReauth(interface->m_dbusController, fastReauth);
	interface->m_fastReauth = fastReauth;
}

unsigned long wpa_supplicantClient_if_GetScanInterval (wpa_supplicantClient_if *interface) {
	return interface->m_scanInterval;
}

void wpa_supplicantClient_if_SetScanInterval (wpa_supplicantClient_if *interface, unsigned long interval) {
	wpa_supplicantClient_if_dbusController_SetScanInterval(interface->m_dbusController, interval);
	interface->m_scanInterval = interval;
}


// Private Functions
////////////////////

//Notification function (call back from dbus controller)
static void dbusControllerNotify (void *parent, IfEventType type, void* args) {
    printf("Entered the Interface dbusControllerNotify() Call back function with type = %d and value %d\n", type, (int)args);

	wpa_supplicantClient_if *interface = (wpa_supplicantClient_if *)parent;

	switch (type) {
	case IF_EVENT_TYPE_SET_STATE:
		interface->m_state = (IfState)args;
		break;

	case IF_EVENT_TYPE_SET_SCANNING:
		interface->m_scanning = (bool)args;
		break;

	case IF_EVENT_TYPE_SET_AP_SCAN:
		interface->m_apScan = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_BSS_EXPIRE_AGE:
		interface->m_bssExpireAge = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_BSS_EXPIRE_COUNT:
		interface->m_bssExpireCount = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_SET_COUNTRY:
		strcpy(interface->m_country, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_IF_NAME:
		strcpy(interface->m_ifName, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_BRIDGE_IF_NAME:
		strcpy(interface->m_bridgeIfName, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_DRIVER:
		strcpy(interface->m_driver, (char *)args);
		break;

	case IF_EVENT_TYPE_SET_FAST_REAUTH:
		interface->m_fastReauth = (bool)args;
		break;

	case IF_EVENT_TYPE_SET_SCAN_INTERVAL:
		interface->m_scanInterval = (unsigned long)args;
		break;

	case IF_EVENT_TYPE_ADD_NW:
		{
			char *network = (char *)args;
			printf("Interface (%s): Adding Network with Object Path %s\n",interface->m_ifName, network);
			wpa_supplicantClient_networkMgr_AddNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_REM_NW:
		{
			char *network = (char *)args;
			printf("Interface (%s): Removing Network with Object Path %s\n",interface->m_ifName, network);
			wpa_supplicantClient_networkMgr_RemNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_ADD_BSS:
		{
			char *bss = (char *)args;
			printf("Interface (%s): Adding BSS with Object Path %s\n",interface->m_ifName, bss);
			wpa_supplicantClient_bssManager_AddBss(interface->m_bssManager, bss);
		}
		break;

	case IF_EVENT_TYPE_REM_BSS:
		{
			char *bss = (char *)args;
			printf("Interface (%s): Removing BSS with Object Path %s\n",interface->m_ifName, bss);
			wpa_supplicantClient_bssManager_RemBss(interface->m_bssManager, bss);
		}
		break;


	default:
		printf("Invalid/UnSupported IF Event Type %d\n", type);
		break;
	}

	return;
}
