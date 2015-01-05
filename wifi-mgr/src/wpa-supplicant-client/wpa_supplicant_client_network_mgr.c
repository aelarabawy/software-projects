/*
 * wpa_supplicant_client_network_mgr.c
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_network_mgr.h"

wpa_supplicant_client_networkMgr *wpa_supplicant_client_networkMgr_Init (char *busName, void* connection) {

	ENTER();

	wpa_supplicant_client_networkMgr *manager = malloc(sizeof(wpa_supplicant_client_networkMgr));
    if (!manager) {
    	ALLOC_FAIL("manager");
    	EXIT_WITH_ERROR();
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicant_client_networkMgr));

    strcpy(manager->m_busName, busName);
    manager->m_dbusConnection = connection;

    EXIT();
    return manager;
}

void wpa_supplicant_client_networkMgr_Start (wpa_supplicant_client_networkMgr *manager) {
	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicant_client_networkMgr_Stop (wpa_supplicant_client_networkMgr *manager) {
	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicant_client_networkMgr_Destroy (wpa_supplicant_client_networkMgr *manager) {

	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the manager
	free(manager);

	EXIT();
	return;
}


void wpa_supplicant_client_networkMgr_AddNetwork(wpa_supplicant_client_networkMgr* mgr, char* pathName) {

	ENTER();

	networkList *nwRec = (networkList *)&(mgr->m_networkGroup);

	while (nwRec->m_next) {
		nwRec = nwRec->m_next;
	}

	//Create a new Record;
	nwRec->m_next = (networkList *)malloc(sizeof(networkList));
	if(!nwRec) {
		ALLOC_FAIL("nwRec");
		EXIT_WITH_ERROR();
	    return;
	}

	nwRec = nwRec->m_next;
    memset(nwRec, 0, sizeof(networkList));

    //Now initializing the BSS
    nwRec->m_network = wpa_supplicant_client_network_Init(mgr->m_busName,
    		                                             pathName,
										                 mgr->m_dbusConnection);
    if (!nwRec->m_network) {
    	ERROR("Failed to initialize the Network Record .. exit");
    	EXIT_WITH_ERROR();
    	return;
    }

    //Now starting the Network
    PROGRESS("Starting the Network");
    wpa_supplicant_client_network_Start(nwRec->m_network);

    EXIT();
	return;
}

void wpa_supplicant_client_networkMgr_RemNetwork(wpa_supplicant_client_networkMgr* mgr, char* pathName) {

	ENTER();

	networkList *nwRec = mgr->m_networkGroup.m_next;
	networkList *prevRec = NULL;

	while (nwRec) {
		if (!strcmp (wpa_supplicant_client_network_GetPathName(nwRec->m_network), pathName)) {
			//Found the network to delete
			break;
		}
		prevRec = nwRec;
		nwRec = nwRec->m_next;
	}

	if (!nwRec) {
		ERROR("Can not find the Network with PathName %s to delete", pathName);
		EXIT_WITH_ERROR();
		return;
	}

	//First Stop the Network
	PROGRESS("Stop the Network");
	wpa_supplicant_client_network_Stop(nwRec->m_network);

	//Then Destroy the BSS
	PROGRESS("Destroy the Network");
	wpa_supplicant_client_network_Destroy(nwRec->m_network);

	//Then remove the nwRec from the list
	if (!prevRec) {
		//The removed BSS is the first BSS in the list
		mgr->m_networkGroup.m_next = nwRec->m_next;
	} else {
		prevRec->m_next = nwRec->m_next;
	}

	//Now we can delete the nwRec
	free(nwRec);

	EXIT();
	return;
}


//Private Functions
///////////////////
