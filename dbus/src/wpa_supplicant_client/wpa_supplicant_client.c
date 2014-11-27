/*
 * wpa_supplicant_client.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */
#include "wpa_supplicant_client.h"

//Prototype for call back function
static void OnNameWatchDetectedNameUp(void *, GDBusConnection *);
static void OnNameWatchDetectedNameDown(void *, GDBusConnection *);
/**
 * wpa_supplicantClient_Init()
 * This function performs required initializations for the wpa_supplicant Client
 */
wpa_supplicantClient *wpa_supplicantClient_Init() {
	printf("Entering wpa_supplicantClient_Init() \n");

	//Create the Client Object
	wpa_supplicantClient *client = malloc(sizeof (wpa_supplicantClient));
	if (!client){
		printf ("Failed to allocate a wpa_supplicantClient Object ... Exiting \n");
		return NULL;
	}
	memset (client, 0, sizeof(wpa_supplicantClient));

	//Initialize the Name Watcher for the wpa_supplicant name on the bus
	client->m_nameWatcher = wpa_supplicantClient_nameWatcher_Init("fi.w1.wpa_supplicant1",
			                                                      OnNameWatchDetectedNameUp,
																  OnNameWatchDetectedNameDown,
																  (void *) client);
	if (!client->m_nameWatcher){
		printf("Failed to initialize the Name Watcher ... exiting\n");

		free (client);
		return NULL;
	}

	//Initialize the Object Proxy Manager
	client->m_proxyObjectManager = wpa_supplicantClient_ProxyObjectManager_Init();
	if (!client->m_proxyObjectManager) {
		printf ("Failed to Initialize the Proxy Object Manager .. Exiting\n");

		wpa_supplicantClient_nameWatcher_Cleanup(client->m_nameWatcher);
		free(client);
		return NULL;
	}

	return client;
}


void wpa_supplicantClient_Start (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Start() \n");

	if (!client){
		printf("NULL is passed for client in wpa_supplicantClient_Start()....Exiting \n");
		return ;
	}

	//Start the Name Watcher
	wpa_supplicantClient_nameWatcher_Start(client->m_nameWatcher);

    //Start the Event Loop
	client->m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(client->m_loop);


	return;
}

void wpa_supplicantClient_Stop (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Stop() \n");

	if (!client){
		printf("NULL is passed for client in wpa_supplicantClient_Stop()....Exiting \n");
		return ;
	}

	//Clean-up Event loop
	g_main_loop_unref (client->m_loop);

	//Stop the Name Watcher
	wpa_supplicantClient_nameWatcher_Stop(client->m_nameWatcher);

	return;
}


/**
 * wpa_SupplicantClient_Cleanup
 * This function performs required cleanup before exit
 */
void wpa_supplicantClient_Cleanup (wpa_supplicantClient *client) {
	printf("Entering wpa_supplicantClient_Cleanup() \n");

	if (!client){
		printf("NULL is passed for client....Exiting \n");
		return ;
	}

	//Cleanup the Name watcher
	wpa_supplicantClient_nameWatcher_Cleanup(client->m_nameWatcher);

	//Finally, we free the client
	free (client);

	return;
}


//Implementation of the Name Detected Call back function
void OnNameWatchDetectedNameUp(void* parent,
		                       GDBusConnection *connection) {
	printf ("Entering the OnNameWatchDetectedNameUp() \n");

	if (!parent){
		printf ("NULL pointer passed for parent....Exiting\n");
		return;
	}

	wpa_supplicantClient *client = (wpa_supplicantClient *)parent;

	if (client->m_connection) {
		printf("Something is wrong: Stored Connection should be NULL\n");
		return;
	}

	client->m_connection = connection;

	//Now we need to trigger the Object Proxy Manager
	wpa_supplicantClient_ProxyObjectManager_Start (client->m_proxyObjectManager,
			                                       connection);

	return;
}

void OnNameWatchDetectedNameDown(void* parent,
		                         GDBusConnection *connection) {
	printf ("Entering the OnNameWatchDetectedNameDown() \n");

	if (!parent){
		printf ("NULL pointer passed for parent....Exiting\n");
		return;
	}

	wpa_supplicantClient *client = (wpa_supplicantClient *)parent;

	if ((!client->m_connection)  || (client->m_connection != connection)) {
		printf("Something is wrong: Stored Connection should be the same as the received value\n");
		return;
	}

	client->m_connection = NULL;


	//Now we need to disable the Object Proxy Manager
	wpa_supplicantClient_ProxyObjectManager_Stop (client->m_proxyObjectManager);

	return;
}
