/*
 * wpa_supplicant_client.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client.h"

//Prototype for the call back function from the D-Bus Controller
void dbusControllerNotify (void *, ClientEventType, void*);

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

	//Setting the state
	client->m_state = CLIENT_STATE_IDLE;

	//Initialize the D-Bus Controller
	client->m_dbusController = wpa_supplicantClient_dbusController_Init(dbusControllerNotify,
			                                                            (void *)client);
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


/**
 * Public Get/Set methods
 * */
ClientDbgLvl getDbgLvl (wpa_supplicantClient *client) {
	return client->m_dbgLvl;
}

void setDbgLvl(wpa_supplicantClient *client, ClientDbgLvl lvl) {
	client->m_dbgLvl = lvl;
}

bool getDbgShowTS (wpa_supplicantClient *client) {
	return client->m_dbgShowTS;
}

void setDbgShowTS (wpa_supplicantClient *client, bool show) {
	client->m_dbgShowTS = show;
}

bool getDbgShowKeys (wpa_supplicantClient *client) {
	return client->m_dbgShowKeys;
}

void setDbgShowKeys (wpa_supplicantClient *client, bool show) {
	client->m_dbgShowKeys = show;
}


int getEapMethodCount (wpa_supplicantClient *client) {
	return client->m_eapMethodCount;
}

EapMethod getDEapMethod (wpa_supplicantClient *client, int index) {
	if (index >= client->m_eapMethodCount) {
		printf("Invalid Index ... exiting");
		return EAP_NONE;
	}
	else {
		return client->m_eapMethods[index];
	}
}


/**
 * Private methods
 */
//Notification function (call back from dbus controller)
void dbusControllerNotify (void *parent, ClientEventType type, void* args) {
    printf("Entered the dbusControllerNotify() Call back function with type = %d and value %d\n", type, (int)args);

	wpa_supplicantClient *client = (wpa_supplicantClient *)parent;

	switch (type) {
	case CLIENT_EVENT_TYPE_READY:
		client->m_state = CLIENT_STATE_READY;
		break;

	case CLIENT_EVENT_TYPE_ADD_IF:
		{
			char *interface = (char *)args;
			printf("Client: Adding Interface with Object Path %s\n", interface);
			wpa_supplicantClient_ifManager_AddIf(client->m_ifManager, interface);
		}
		break;

	case CLIENT_EVENT_TYPE_REM_IF:
		{
			char *interface = (char *)args;
			printf("Client: Removing Interface with Object Path %s\n", interface);
			wpa_supplicantClient_ifManager_RemIf(client->m_ifManager, interface);
		}
		break;

	case CLIENT_EVENT_TYPE_SET_DBG_LEVEL:
		client->m_dbgLvl = (ClientDbgLvl)args;

		break;

	case CLIENT_EVENT_TYPE_SET_SHOW_TS:
		client->m_dbgShowTS = (bool)args;
		break;

	case CLIENT_EVENT_TYPE_SET_SHOW_KEYS:
		client->m_dbgShowKeys = (bool)args;
		break;

	case CLIENT_EVENT_TYPE_ADD_EAP_METHOD:
		client->m_eapMethods[client->m_eapMethodCount++] = (EapMethod)args;
		break;

	default:
		printf("Invalid Client Event Type %d\n", type);
		break;
	}

	return;
}

