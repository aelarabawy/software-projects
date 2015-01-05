/*
 * wpa_supplicant_client_iface_mgr.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_iface_mgr.h"


wpa_supplicant_client_ifaceMgr *wpa_supplicant_client_ifaceMgr_Init (char *busName) {

	ENTER();

	wpa_supplicant_client_ifaceMgr *manager = malloc(sizeof(wpa_supplicant_client_ifaceMgr));
    if (!manager) {
    	ALLOC_FAIL("manager");
    	EXIT_WITH_ERROR();
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicant_client_ifaceMgr));

    strcpy(manager->m_busName, busName);

    EXIT();
	return manager;
}

void wpa_supplicant_client_ifaceMgr_Start (wpa_supplicant_client_ifaceMgr *manager,
		                                   void *connection) {
	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	manager->m_dbusConnection = connection;

	EXIT();
	return;
}

void wpa_supplicant_client_ifaceMgr_Stop (wpa_supplicant_client_ifaceMgr *manager) {

	ENTER();
	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicant_client_ifaceMgr_Destroy (wpa_supplicant_client_ifaceMgr *manager) {
	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	//Finally free the object
	free(manager);

	EXIT();
	return;
}


void wpa_supplicant_client_ifaceMgr_AddIface(wpa_supplicant_client_ifaceMgr* mgr, char* ifPathName) {

	ENTER();

	ifaceList *ifRec = (ifaceList *)&(mgr->m_ifaceGroup);

	while (ifRec->m_next) {
		ifRec = ifRec->m_next;
	}

	//Create a new Record;
	ifRec->m_next = (ifaceList *)malloc(sizeof(ifaceList));
	if(!ifRec) {
		ALLOC_FAIL("ifRec");
		EXIT_WITH_ERROR();
	    return;
	}
	ifRec = ifRec->m_next;
    memset(ifRec, 0, sizeof(ifaceList));

    //Now initializing the Interface
    ifRec->m_iface = wpa_supplicant_client_iface_Init(mgr->m_busName,
    		                                          ifPathName,
													  mgr->m_dbusConnection);
    if (!ifRec->m_iface) {
    	ERROR("Failed to initialize the Interface Record .. exit");
    	EXIT_WITH_ERROR();
    	return;
    }

    //Now starting the interface
    wpa_supplicant_client_iface_Start(ifRec->m_iface);

    EXIT();
	return;
}

void wpa_supplicant_client_ifaceMgr_RemIface(wpa_supplicant_client_ifaceMgr* mgr, char* ifPathName) {

	ENTER();

	ifaceList *ifRec = mgr->m_ifaceGroup.m_next;
	ifaceList *prevRec = NULL;

	while (ifRec) {
		if (!strcmp (wpa_supplicant_client_iface_GetIfacePathName(ifRec->m_iface), ifPathName)) {
			//Found the Interface to delete
			break;
		}
		prevRec = ifRec;
		ifRec = ifRec->m_next;
	}

	if (!ifRec) {
		ERROR("Can not find the interface with PathName %s to delete", ifPathName);
		EXIT_WITH_ERROR();
		return;
	}

	//First Stop the Interface
	wpa_supplicant_client_iface_Stop(ifRec->m_iface);

	//Then Destroy the Interface
	wpa_supplicant_client_iface_Destroy(ifRec->m_iface);

	//Then remove the ifRec from the list
	if (!prevRec) {
		//The removed interface is the first interface in the list
		mgr->m_ifaceGroup.m_next = ifRec->m_next;
	} else {
		prevRec->m_next = ifRec->m_next;
	}

	//Now we can delete the ifRec
	free(ifRec);

	EXIT();
	return;
}



//Private Functions
///////////////////
