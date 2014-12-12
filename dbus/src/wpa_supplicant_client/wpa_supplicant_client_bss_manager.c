/*
 * wpa_supplicant_client_bss_manager.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss_manager.h"


wpa_supplicantClient_bssManager *wpa_supplicantClient_bssManager_Init (char *busName, void* connection) {
	ENTER();

	wpa_supplicantClient_bssManager *manager = malloc(sizeof(wpa_supplicantClient_bssManager));
    if (!manager) {
    	ALLOC_FAIL("manager");
    	EXIT_WITH_ERROR();
    	return NULL;
    }
    memset(manager, 0, sizeof(wpa_supplicantClient_bssManager));

    strcpy(manager->m_busName, busName);
    manager->m_dbusConnection = connection;

    EXIT();
    return manager;
}

void wpa_supplicantClient_bssManager_Start (wpa_supplicantClient_bssManager *manager) {
	ENTER();

	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicantClient_bssManager_Stop (wpa_supplicantClient_bssManager *manager) {

	ENTER();
	if (!manager){
		NULL_POINTER("manager");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicantClient_bssManager_Destroy (wpa_supplicantClient_bssManager *manager) {

	ENTER();
	if (!manager){
		NULL_POINTER("Passing NULL to the function ...Exiting");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the manager
	free(manager);

	EXIT();
	return;
}

void wpa_supplicantClient_bssManager_AddBss(wpa_supplicantClient_bssManager* mgr, char* pathName) {
	ENTER();

	bssList *bssRec = (bssList *)&(mgr->m_bssGroup);

	while (bssRec->m_next) {
		bssRec = bssRec->m_next;
	}

	//Create a new Record;
	bssRec->m_next = (bssList *)malloc(sizeof(bssList));
	if(!bssRec) {
		ALLOC_FAIL("bssRec");
		EXIT_WITH_ERROR();
	    return;
	}
	bssRec = bssRec->m_next;
    memset(bssRec, 0, sizeof(bssList));

    //Now initializing the BSS
    bssRec->m_bss = wpa_supplicantClient_bss_Init(mgr->m_busName,
    		                                      pathName,
												  mgr->m_dbusConnection);
    if (!bssRec->m_bss) {
    	ERROR("Failed to initialize the BSS Record .. exit");
    	EXIT_WITH_ERROR();
    	return;
    }

    //Now starting the BSS
    wpa_supplicantClient_bss_Start(bssRec->m_bss);

    EXIT();
	return;
}

void wpa_supplicantClient_bssManager_RemBss(wpa_supplicantClient_bssManager* mgr, char* pathName) {
	ENTER();

	bssList *bssRec = mgr->m_bssGroup.m_next;
	bssList *prevRec = NULL;

	while (bssRec) {
		if (!strcmp (wpa_supplicantClient_bss_GetPathName(bssRec->m_bss), pathName)) {
			//Found the bss to delete
			break;
		}
		prevRec = bssRec;
		bssRec = bssRec->m_next;
	}

	if (!bssRec) {
		ERROR("Can not find the BSS with PathName %s to delete", pathName);
		EXIT_WITH_ERROR();
		return;
	}

	//First Stop the BSS
	wpa_supplicantClient_bss_Stop(bssRec->m_bss);

	//Then Destroy the BSS
	wpa_supplicantClient_bss_Destroy(bssRec->m_bss);

	//Then remove the bssRec from the list
	if (!prevRec) {
		//The removed BSS is the first BSS in the list
		mgr->m_bssGroup.m_next = bssRec->m_next;
	} else {
		prevRec->m_next = bssRec->m_next;
	}

	//Now we can delete the bssRec
	free(bssRec);

	EXIT();
	return;
}


//Private Functions
///////////////////
