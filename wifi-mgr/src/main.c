/*****************************
 * main.c
 * This program tracks and controls the wpa_supplicant program
 *
 * Author: Ahmed I. ElArabawy
 * Created on: 11/20/2014
 */

#include "logger.h"
#include "wpa_supplicant_mgr_api.h"

//main function
int main (int argc, char* argv[]) {
	ENTER();

	PROGRESS("Create the wpa_supplicant mgr");
	wpaHandle mgrHandle = wpa_supplicant_mgr_Init();
	if (!mgrHandle){
		ALLOC_FAIL("mgrHandle");
		EXIT_WITH_ERROR();
		return -1;
	}

	PROGRESS("Start the Manager Operation");
	wpa_supplicant_mgr_Start(mgrHandle);
    //This function will not return until operation is complete


	//Perform needed Cleanup
	//The program enters here before it exits
	PROGRESS("Cleanup: Stop and Destroy the Mgr");
	wpa_supplicant_mgr_Stop(mgrHandle);
	wpa_supplicant_mgr_Destroy (mgrHandle);

	EXIT();
	return 0;
}
