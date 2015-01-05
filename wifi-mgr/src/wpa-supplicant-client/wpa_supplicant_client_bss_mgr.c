/*
 * wpa_supplicant_client_bss_mgr.c
 *
 *  Created on: Dec 4, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_bss_mgr.h"


wpa_supplicant_client_bssMgr *wpa_supplicant_client_bssMgr_Init (char *busName, void* connection) {
	ENTER();

	wpa_supplicant_client_bssMgr *mgr = malloc(sizeof(wpa_supplicant_client_bssMgr));
    if (!mgr) {
    	ALLOC_FAIL("mgr");
    	EXIT_WITH_ERROR();
    	return NULL;
    }
    memset(mgr, 0, sizeof(wpa_supplicant_client_bssMgr));

    strcpy(mgr->m_busName, busName);
    mgr->m_dbusConnection = connection;

    EXIT();
    return mgr;
}

void wpa_supplicant_client_bssMgr_Start (wpa_supplicant_client_bssMgr *mgr) {
	ENTER();

	if (!mgr){
		NULL_POINTER("mgr");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicant_client_bssMgr_Stop (wpa_supplicant_client_bssMgr *mgr) {

	ENTER();

	if (!mgr){
		NULL_POINTER("mgr");
		EXIT_WITH_ERROR();
		return;
	}

	EXIT();
	return;
}

void wpa_supplicant_client_bssMgr_Destroy (wpa_supplicant_client_bssMgr *mgr) {

	ENTER();

	if (!mgr) {
		NULL_POINTER("Passing NULL to the function ...Exiting");
		EXIT_WITH_ERROR();
		return;
	}

	//Free the mgr
	free(mgr);

	EXIT();
	return;
}

void wpa_supplicant_client_bssMgr_AddBss(wpa_supplicant_client_bssMgr *mgr, char* pathName) {
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
    bssRec->m_bss = wpa_supplicant_client_bss_Init(mgr->m_busName,
    		                                   pathName,
						   mgr->m_dbusConnection);
    if (!bssRec->m_bss) {
    	ERROR("Failed to initialize the BSS Record .. exit");
    	EXIT_WITH_ERROR();
    	return;
    }

    //Now starting the BSS
    wpa_supplicant_client_bss_Start(bssRec->m_bss);

    EXIT();
	return;
}

void wpa_supplicant_client_bssMgr_RemBss(wpa_supplicant_client_bssMgr *mgr, char* pathName) {
	ENTER();

	bssList *bssRec = mgr->m_bssGroup.m_next;
	bssList *prevRec = NULL;

	while (bssRec) {
		if (!strcmp (wpa_supplicant_client_bss_GetPathName(bssRec->m_bss), pathName)) {
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
	wpa_supplicant_client_bss_Stop(bssRec->m_bss);

	//Then Destroy the BSS
	wpa_supplicant_client_bss_Destroy(bssRec->m_bss);

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
