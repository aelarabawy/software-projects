/*
 * wpa_supplicant_client_if_manager.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_if_manager.h"


wpa_supplicantClient_ifManager *wpa_supplicantClient_ifManager_Init (char *busName) {
	wpa_supplicantClient_ifManager *manager = malloc(sizeof(wpa_supplicantClient_ifManager));
    if (!manager) {
    	printf("Failed to allocate the Interface Manager Object ... Exiting\n");
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicantClient_ifManager));

    strcpy(manager->m_busName, busName);

	return manager;
}

void wpa_supplicantClient_ifManager_Start (wpa_supplicantClient_ifManager *manager,
		                                   void *connection) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	manager->m_dbusConnection = connection;

	return;
}

void wpa_supplicantClient_ifManager_Stop (wpa_supplicantClient_ifManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	return;
}

void wpa_supplicantClient_ifManager_Destroy (wpa_supplicantClient_ifManager *manager) {

	if (!manager){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Finally free the object
	free(manager);

	return;
}


void wpa_supplicantClient_ifManager_AddIf(wpa_supplicantClient_ifManager* mgr, char* ifPathName) {

	interfaceList *ifRec = (interfaceList *)&(mgr->m_interfaceGroup);

	while (ifRec->m_next) {
		ifRec = ifRec->m_next;
	}

	//Create a new Record;
	ifRec->m_next = (interfaceList *)malloc(sizeof(interfaceList));
	if(!ifRec) {
		printf("Failed to create the interfaceList structure to hold a new interface\n");
	    return;
	}
	ifRec = ifRec->m_next;
    memset(ifRec, 0, sizeof(interfaceList));

    //Now initializing the Interface
    ifRec->m_interface = wpa_supplicantClient_if_Init(mgr->m_busName,
    		                                          ifPathName,
													  mgr->m_dbusConnection);
    if (!ifRec->m_interface) {
    	printf("Failed to initialize the Interface Record .. exit\n");
    	return;
    }

    //Now starting the interface
    wpa_supplicantClient_if_Start(ifRec->m_interface);

	return;
}

void wpa_supplicantClient_ifManager_RemIf(wpa_supplicantClient_ifManager* mgr, char* ifPathName) {

	interfaceList *ifRec = mgr->m_interfaceGroup.m_next;
	interfaceList *prevRec = NULL;

	while (ifRec) {
		if (!strcmp (wpa_supplicantClient_if_GetIfPathName(ifRec->m_interface), ifPathName)) {
			//Found the Interface to delete
			break;
		}
		prevRec = ifRec;
		ifRec = ifRec->m_next;
	}

	if (!ifRec) {
		printf("Can not find the interface with PathName %s to delete\n", ifPathName);
		return;
	}

	//First Stop the Interface
	wpa_supplicantClient_if_Stop(ifRec->m_interface);

	//Then Destroy the Interface
	wpa_supplicantClient_if_Destroy(ifRec->m_interface);

	//Then remove the ifRec from the list
	if (!prevRec) {
		//The removed interface is the first interface in the list
		mgr->m_interfaceGroup.m_next = ifRec->m_next;
	} else {
		prevRec->m_next = ifRec->m_next;
	}

	//Now we can delete the ifRec
	free(ifRec);

	return;
}



//Private Functions
///////////////////
