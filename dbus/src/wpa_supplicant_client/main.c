/*****************************
 * main.c
 * This program tracks and controls the wpa_supplicant program
 *
 * Author: Ahmed I. ElArabawy
 * Created on: 11/20/2014
 */

#include "wpa_supplicant_client.h"

//main function
int main (int argc, char* argv[]) {
	ENTER();

	PROGRESS("Starting the wpa_supplicant Client Program");

	PROGRESS("Initialize the Client");
	wpa_supplicantClient * client = wpa_supplicantClient_Init("fi.w1.wpa_supplicant1",  //Bus Name
			                                                  "/fi/w1/wpa_supplicant1");//Object Path
	if (!client){
		ALLOC_FAIL("client");
		EXIT_WITH_ERROR();
		return -1;
	}

	//Starting Operation
	PROGRESS("Starting the Client");
	wpa_supplicantClient_Start(client);


	//Perform needed Cleanup
	//The program enters here before it exits
	PROGRESS("Cleanup: Stop and Destroy the Client");
	wpa_supplicantClient_Stop(client);
	wpa_supplicantClient_Destroy (client);

	EXIT();
	return 0;
}
