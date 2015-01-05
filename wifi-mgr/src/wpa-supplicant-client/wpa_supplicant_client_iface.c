/*
 * wpa_supplicant_client_if.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_iface.h"

//Prototype for the call back function from the D-Bus Handler
static void dbusNotify (void *, IfEventType, void*);


wpa_supplicant_client_iface *wpa_supplicant_client_iface_Init (char *busName,
		                                                       char *pathName,
													           void *connection)  {
	ENTER_FUNC("pathName = %s", pathName);

	wpa_supplicant_client_iface * interface = malloc(sizeof(wpa_supplicant_client_iface));
    if (!interface) {
    	ALLOC_FAIL("interface");
    	goto FAIL_IF;
    }
    memset(interface, 0, sizeof(wpa_supplicant_client_iface));

    strcpy(interface->m_busName, busName);
    strcpy(interface->m_pathName, pathName);

	//Initialize the D-Bus Handler
	interface->m_dbusHandle = wpa_supplicant_dbus_iface_Init(busName,
			                                                 pathName,
															 connection,
			                                                 dbusNotify,
			                                                 (void *)interface);
	if (!interface->m_dbusHandle) {
		ERROR("Failed to initialize the Interface D-Bus Handler ... exiting");

		goto FAIL_DBUS_HANDLER;
	}

	//Initialize the Network Manager
	interface->m_networkMgr = wpa_supplicant_client_networkMgr_Init(busName, connection);
	if (!interface->m_networkMgr) {
		ERROR("Failed to initialize the Interface Network Manager ... exiting");

		goto FAIL_NW_MANAGER;
	}

	//Initialize the BSS Manager
	interface->m_bssManager = wpa_supplicant_client_bssMgr_Init(busName, connection);
	if (!interface->m_bssManager) {
		ERROR("Failed to initialize the Interface BSS Manager ... exiting");

		goto FAIL_BSS_MANAGER;
	}

	//Success
	goto SUCCESS;

FAIL_BSS_MANAGER:
	wpa_supplicant_client_networkMgr_Destroy(interface->m_networkMgr);

FAIL_NW_MANAGER:
	wpa_supplicant_dbus_iface_Destroy(interface->m_dbusHandle);

FAIL_DBUS_HANDLER:
	free(interface);

FAIL_IF:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return interface;
}


void wpa_supplicant_client_iface_Start (wpa_supplicant_client_iface *interface)  {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_iface_Start(interface->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_iface_Stop (wpa_supplicant_client_iface *interface)  {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_iface_Stop(interface->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_iface_Destroy (wpa_supplicant_client_iface *interface) {
	ENTER();

	if (!interface){
		NULL_POINTER("interface");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_iface_Destroy(interface->m_dbusHandle);

	//Finally Free the Interface
	free(interface);

	EXIT();
	return;
}

char* wpa_supplicant_client_iface_GetIfacePathName(wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_pathName;
}


// Get & Set of Properties
//////////////////////////
IfaceState wpa_supplicant_client_if_GetState (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_state;
}

bool wpa_supplicant_client_if_GetIsScanning (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_scanning;
}

unsigned long wpa_supplicant_client_if_GetApScan (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_apScan;
}

void wpa_supplicant_client_if_SetApScan(wpa_supplicant_client_iface *interface, unsigned long scan) {
	ENTER();
	wpa_supplicant_dbus_iface_SetApScan(interface->m_dbusHandle, scan);
	interface->m_apScan = scan;
	EXIT();
}

unsigned long wpa_supplicant_client_if_GetBssExpireAge (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_bssExpireAge;
}

void wpa_supplicant_client_if_SetBssExpireAge (wpa_supplicant_client_iface *interface, unsigned long age) {
	ENTER();
	wpa_supplicant_dbus_iface_SetBssExpireAge(interface->m_dbusHandle, age);
	interface->m_bssExpireAge = age;
	EXIT();
}

unsigned long wpa_supplicant_client_if_GetBssExpireCount (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_bssExpireCount;
}

void wpa_supplicant_client_if_SetBssExpireCount (wpa_supplicant_client_iface *interface, unsigned long count) {
	ENTER();
	wpa_supplicant_dbus_iface_SetBssExpireCount(interface->m_dbusHandle, count);
	interface->m_bssExpireCount = count;
	EXIT();
}

char *wpa_supplicant_client_if_GetCountry (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_country;
}

void wpa_supplicant_client_if_SetCountry (wpa_supplicant_client_iface *interface, char *country) {
	ENTER();
	wpa_supplicant_dbus_iface_SetCountry(interface->m_dbusHandle, country);
	strcpy(interface->m_country, country);
	EXIT();
}

char *wpa_supplicant_client_if_GetIfName (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_ifName;
}

char *wpa_supplicant_client_if_GetBridgeIfName (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_bridgeIfName;
}

char *wpa_supplicant_client_if_GetDriver (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_driver;
}

bool wpa_supplicant_client_if_GetFastReauth (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_fastReauth;
}

void wpa_supplicant_client_if_SetFastReauth (wpa_supplicant_client_iface *interface, bool fastReauth) {
	ENTER();
	wpa_supplicant_dbus_iface_SetFastReauth(interface->m_dbusHandle, fastReauth);
	interface->m_fastReauth = fastReauth;
	EXIT();
}

unsigned long wpa_supplicant_client_if_GetScanInterval (wpa_supplicant_client_iface *interface) {
	ENTER();
	EXIT();
	return interface->m_scanInterval;
}

void wpa_supplicant_client_if_SetScanInterval (wpa_supplicant_client_iface *interface, unsigned long interval) {
	ENTER();
	wpa_supplicant_dbus_iface_SetScanInterval(interface->m_dbusHandle, interval);
	interface->m_scanInterval = interval;
	EXIT();
}


// Private Functions
////////////////////

//Notification function (call back from dbus handler)
static void dbusNotify (void *parent, IfEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args);

	wpa_supplicant_client_iface *interface = (wpa_supplicant_client_iface *)parent;

	switch (type) {
	case IF_EVENT_TYPE_SET_STATE:
		INFO("IF_EVENT_TYPE_SET_STATE");
		interface->m_state = (IfaceState)args;
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
			wpa_supplicant_client_networkMgr_AddNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_REM_NW:
		{
			INFO("IF_EVENT_TYPE_REM_NW");
			char *network = (char *)args;
			PROGRESS("Interface (%s): Removing Network with Object Path %s",interface->m_ifName, network);
			wpa_supplicant_client_networkMgr_RemNetwork(interface->m_networkMgr, network);
		}
		break;

	case IF_EVENT_TYPE_ADD_BSS:
		{
			INFO("IF_EVENT_TYPE_ADD_BSS");
			char *bss = (char *)args;
			PROGRESS("Interface (%s): Adding BSS with Object Path %s",interface->m_ifName, bss);
			wpa_supplicant_client_bssMgr_AddBss(interface->m_bssManager, bss);
		}
		break;

	case IF_EVENT_TYPE_REM_BSS:
		{
			INFO("IF_EVENT_TYPE_REM_BSS");
			char *bss = (char *)args;
			PROGRESS("Interface (%s): Removing BSS with Object Path %s",interface->m_ifName, bss);
			wpa_supplicant_client_bssMgr_RemBss(interface->m_bssManager, bss);
		}
		break;


	default:
		ERROR("Invalid/UnSupported IF Event Type %d", type);
		break;
	}

	EXIT();
	return;
}
