/*
 * wpa_supplicant_client_network_mgr.c
 *
 *  Created on: Dec 7, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_network_mgr.h"

wpa_supplicantClient_networkMgr *wpa_supplicantClient_networkMgr_Init (char *busName, void* connection) {
	wpa_supplicantClient_networkMgr *manager = malloc(sizeof(wpa_supplicantClient_networkMgr));
    if (!manager) {
    	printf("Failed to allocate the Network Manager Object ... Exiting\n");
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicantClient_networkMgr));

    strcpy(manager->m_busName, busName);
    manager->m_dbusConnection = connection;

    return manager;
}

void wpa_supplicantClient_networkMgr_Start (wpa_supplicantClient_networkMgr *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_networkMgr_Stop (wpa_supplicantClient_networkMgr *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_networkMgr_Destroy (wpa_supplicantClient_networkMgr *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Free the manager
	free(manager);

	return;
}


void wpa_supplicantClient_networkMgr_AddNetwork(wpa_supplicantClient_networkMgr* mgr, char* pathName) {

	networkList *nwRec = (networkList *)&(mgr->m_networkGroup);

	while (nwRec->m_next) {
		nwRec = nwRec->m_next;
	}

	//Create a new Record;
	nwRec->m_next = (networkList *)malloc(sizeof(networkList));
	if(!nwRec) {
		printf("Failed to create the networkList structure to hold a new Network\n");
	    return;
	}
	nwRec = nwRec->m_next;
    memset(nwRec, 0, sizeof(networkList));

    //Now initializing the BSS
    nwRec->m_network = wpa_supplicantClient_network_Init(mgr->m_busName,
    		                                             pathName,
										                 mgr->m_dbusConnection);
    if (!nwRec->m_network) {
    	printf("Failed to initialize the Network Record .. exit\n");
    	return;
    }

    //Now starting the Network
    wpa_supplicantClient_network_Start(nwRec->m_network);

	return;
}

void wpa_supplicantClient_networkMgr_RemNetwork(wpa_supplicantClient_networkMgr* mgr, char* pathName) {

	networkList *nwRec = mgr->m_networkGroup.m_next;
	networkList *prevRec = NULL;

	while (nwRec) {
		if (!strcmp (wpa_supplicantClient_network_GetPathName(nwRec->m_network), pathName)) {
			//Found the network to delete
			break;
		}
		prevRec = nwRec;
		nwRec = nwRec->m_next;
	}

	if (!nwRec) {
		printf("Can not find the Network with PathName %s to delete\n", pathName);
		return;
	}

	//First Stop the Network
	wpa_supplicantClient_network_Stop(nwRec->m_network);

	//Then Destroy the BSS
	wpa_supplicantClient_network_Destroy(nwRec->m_network);

	//Then remove the nwRec from the list
	if (!prevRec) {
		//The removed BSS is the first BSS in the list
		mgr->m_networkGroup.m_next = nwRec->m_next;
	} else {
		prevRec->m_next = nwRec->m_next;
	}

	//Now we can delete the nwRec
	free(nwRec);

	return;
}


//Private Functions
///////////////////
