/*
 * wpa_supplicant_client.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client.h"


/**
 * wpa_supplicantClient_Init()
 * This function performs required initializations for the wpa_supplicant Client
 */
wpa_supplicantClient *wpa_supplicantClient_Init() {
	printf("Entering wpa_supplicantClient_Init() \n");

	//Create the Client Object
	wpa_supplicantClient *client = malloc(sizeof (wpa_supplicantClient));
	if (!client){
		printf("Failed to allocate a wpa_supplicantClient Object ... Exiting \n");

		goto FAIL_CLIENT;
	}
	memset (client, 0, sizeof(wpa_supplicantClient));

	//Initialize the D-Bus Controller
	client->m_dbusController = wpa_supplicantClient_dbusController_Init();
	if (!client->m_dbusController) {
		printf("Failed to initialize the Client D-BUS Controller ... exiting\n");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Initialize the Interface Manager
	client->m_ifManager = wpa_supplicantClient_ifManager_Init();
	if (!client->m_ifManager) {
		printf("Failed to initialize the Client Interface Manager ... exiting\n");

		goto FAIL_IF_MANAGER;
	}

	//Success
	goto SUCCESS;

FAIL_IF_MANAGER:
	wpa_supplicantClient_dbusController_Destroy(client->m_dbusController);
FAIL_DBUS_CONTROLLER:
	free(client);
FAIL_CLIENT:
    return NULL;

SUCCESS:
	return client;
}


void wpa_supplicantClient_Start (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Start() \n");

	if (!client){
		printf("NULL is passed for client in wpa_supplicantClient_Start()....Exiting \n");
		return ;
	}

	//Start the D-Bus Controller
	wpa_supplicantClient_dbusController_Start(client->m_dbusController);

	//Start the Interface Manager
	wpa_supplicantClient_ifManager_Start(client->m_ifManager);

	return;
}

void wpa_supplicantClient_Stop (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Stop() \n");

	if (!client){
		printf("NULL is passed for client in wpa_supplicantClient_Stop()....Exiting \n");
		return ;
	}

	//Stop the Interface Manager
	wpa_supplicantClient_ifManager_Stop(client->m_ifManager);

	//Stop the D-Bus Controller
	wpa_supplicantClient_dbusController_Stop(client->m_dbusController);

	return;
}


/**
 * wpa_SupplicantClient_Destroy
 * This function performs required cleanup before exit
 */
void wpa_supplicantClient_Destroy (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Destroy() \n");

	if (!client){
		printf("NULL is passed for client....Exiting \n");
		return ;
	}

	//Destroy the Interface Manager
	wpa_supplicantClient_ifManager_Destroy(client->m_ifManager);

	//Destroy the D-Bus Controller
	wpa_supplicantClient_dbusController_Destroy(client->m_dbusController);

	//Finally, we free the client
	free (client);

	return;
}


