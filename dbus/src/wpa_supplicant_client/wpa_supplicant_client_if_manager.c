/*
 * wpa_supplicant_client_if_manager.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_if_manager.h"


wpa_supplicantClient_ifManager *wpa_supplicantClient_ifManager_Init (char *busName) {

	ENTER();

	wpa_supplicantClient_ifManager *manager = malloc(sizeof(wpa_supplicantClient_ifManager));
    if (!manager) {
    	ALLOC_FAIL("manager");
    	EXIT_WITH_ERROR();
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicantClient_ifManager));

    strcpy(manager->m_busName, busName);

    EXIT();
	return manager;
}

void wpa_supplicantClient_ifManager_Start (wpa_supplicantClient_ifManager *manager,
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

void wpa_supplicantClient_ifManager_Stop (wpa_supplicantClient_ifManager *manager) {

	ENTER();
	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicantClient_ifManager_Destroy (wpa_supplicantClient_ifManager *manager) {
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


void wpa_supplicantClient_ifManager_AddIf(wpa_supplicantClient_ifManager* mgr, char* ifPathName) {

	ENTER();

	interfaceList *ifRec = (interfaceList *)&(mgr->m_interfaceGroup);

	while (ifRec->m_next) {
		ifRec = ifRec->m_next;
	}

	//Create a new Record;
	ifRec->m_next = (interfaceList *)malloc(sizeof(interfaceList));
	if(!ifRec) {
		ALLOC_FAIL("ifRec");
		EXIT_WITH_ERROR();
	    return;
	}
	ifRec = ifRec->m_next;
    memset(ifRec, 0, sizeof(interfaceList));

    //Now initializing the Interface
    ifRec->m_interface = wpa_supplicantClient_if_Init(mgr->m_busName,
    		                                          ifPathName,
													  mgr->m_dbusConnection);
    if (!ifRec->m_interface) {
    	ERROR("Failed to initialize the Interface Record .. exit");
    	EXIT_WITH_ERROR();
    	return;
    }

    //Now starting the interface
    wpa_supplicantClient_if_Start(ifRec->m_interface);

    EXIT();
	return;
}

void wpa_supplicantClient_ifManager_RemIf(wpa_supplicantClient_ifManager* mgr, char* ifPathName) {

	ENTER();

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
		ERROR("Can not find the interface with PathName %s to delete", ifPathName);
		EXIT_WITH_ERROR();
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

	EXIT();
	return;
}



//Private Functions
///////////////////
