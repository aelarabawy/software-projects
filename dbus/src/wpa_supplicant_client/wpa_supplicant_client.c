/*
 * wpa_supplicant_client.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client.h"

//Prototype for the call back function from the D-Bus Controller
static void dbusControllerNotify (void *, ClientEventType, void*);

/**
 * wpa_supplicantClient_Init()
 * This function performs required initializations for the wpa_supplicant Client
 */
wpa_supplicantClient *wpa_supplicantClient_Init(char *busName,
		                                        char *objectPath) {
	ENTER();

	//Create the Client Object
	PROGRESS("Allocating the Client");
	wpa_supplicantClient *client = malloc(sizeof(wpa_supplicantClient));
	if (!client){
		ALLOC_FAIL("Failed to allocate a wpa_supplicantClient Object ... Exiting");

		goto FAIL_CLIENT;
	}
	memset (client, 0, sizeof(wpa_supplicantClient));

	//Setting the state
	client->m_state = CLIENT_STATE_IDLE;
	strcpy(client->m_busName, busName);
	strcpy(client->m_objectPath, objectPath);

	//Initialize the D-Bus Controller
	PROGRESS("Initialize the Client D-Bus Controller");
	client->m_dbusController = wpa_supplicantClient_dbusController_Init(busName,
			                                                            objectPath,
																		dbusControllerNotify,
			                                                            (void *)client);
	if (!client->m_dbusController) {
		ERROR("Failed to initialize the Client D-BUS Controller ... exiting");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Initialize the Interface Manager
	PROGRESS("Initialize the Interface Manager");
	client->m_ifManager = wpa_supplicantClient_ifManager_Init(busName);
	if (!client->m_ifManager) {
		ERROR("Failed to initialize the Client Interface Manager ... exiting");

		goto FAIL_IF_MANAGER;
	}

	//Success
	goto SUCCESS;

FAIL_IF_MANAGER:
	wpa_supplicantClient_dbusController_Destroy(client->m_dbusController);
FAIL_DBUS_CONTROLLER:
	free(client);

FAIL_CLIENT:
	EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return client;
}


void wpa_supplicantClient_Start (wpa_supplicantClient *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Start the D-Bus Controller
	PROGRESS("Start the Client D-Bus Controller");
	wpa_supplicantClient_dbusController_Start(client->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_Stop (wpa_supplicantClient *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Stop the Interface Manager
	wpa_supplicantClient_ifManager_Stop(client->m_ifManager);

	//Stop the D-Bus Controller
	wpa_supplicantClient_dbusController_Stop(client->m_dbusController);

	EXIT();
	return;
}


/**
 * wpa_SupplicantClient_Destroy
 * This function performs required cleanup before exit
 */
void wpa_supplicantClient_Destroy (wpa_supplicantClient *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Destroy the Interface Manager
	wpa_supplicantClient_ifManager_Destroy(client->m_ifManager);

	//Destroy the D-Bus Controller
	wpa_supplicantClient_dbusController_Destroy(client->m_dbusController);

	//Finally, we free the client
	free (client);

	EXIT();
	return;
}


/**
 * Public Get/Set methods
 * */
ClientDbgLvl wpa_supplicantClient_GetDbgLvl (wpa_supplicantClient *client) {
	ENTER();
	EXIT();
	return client->m_dbgLvl;
}

void wpa_supplicantClient_SetDbgLvl(wpa_supplicantClient *client, ClientDbgLvl lvl) {
	ENTER();
	wpa_supplicantClient_dbusController_SetDbgLvl(client->m_dbusController, lvl);
	client->m_dbgLvl = lvl;
	EXIT();
}

bool wpa_supplicantClient_GetDbgShowTS (wpa_supplicantClient *client) {
	ENTER();
	EXIT();
	return client->m_dbgShowTS;
}

void wpa_supplicantClient_SetDbgShowTS (wpa_supplicantClient *client, bool show) {
	ENTER();
	wpa_supplicantClient_dbusController_SetShowTS(client->m_dbusController, show);
	client->m_dbgShowTS = show;
	EXIT();
}

bool wpa_supplicantClient_GetDbgShowKeys (wpa_supplicantClient *client) {
	ENTER();
	EXIT();
	return client->m_dbgShowKeys;
}

void wpa_supplicantClient_SetDbgShowKeys (wpa_supplicantClient *client, bool show) {
	ENTER();
	wpa_supplicantClient_dbusController_SetShowKeys(client->m_dbusController, show);
	client->m_dbgShowKeys = show;
	EXIT();
}


int wpa_supplicantClient_GetEapMethodCount (wpa_supplicantClient *client) {
	ENTER();
	EXIT();
	return client->m_eapMethodCount;
}

EapMethod wpa_supplicantClient_GetDEapMethod (wpa_supplicantClient *client, int index) {
	ENTER();
	if (index >= client->m_eapMethodCount) {
		ERROR("Invalid Index ... exiting");

		EXIT_WITH_ERROR();
		return EAP_NONE;
	}

	EXIT();
	return client->m_eapMethods[index];
}



//Private Functions
///////////////////
static void resumeStart(wpa_supplicantClient *client,
		                void *connection) {
	ENTER();

	//Start the Interface Manager
	PROGRESS("Start the Interface Manager");
	wpa_supplicantClient_ifManager_Start(client->m_ifManager,
			                             connection);

	EXIT();
	return;
}

//Notification function (call back from dbus controller)
static void dbusControllerNotify (void *parent, ClientEventType type, void* args) {
	ENTER_FUNC("Type = %d , Value = %d",type, args);

	wpa_supplicantClient *client = (wpa_supplicantClient *)parent;

	switch (type) {
	case CLIENT_EVENT_TYPE_READY:
		INFO("CLIENT_EVENT_TYPE_READY");
		client->m_state = CLIENT_STATE_READY;
		resumeStart(client, args); //args = D-Bus Connection
		break;

	case CLIENT_EVENT_TYPE_ADD_IF:
		{
			INFO("CLIENT_EVENT_TYPE_ADD_IF");
			char *interface = (char *)args;
			PROGRESS("Client: Adding Interface with Object Path %s", interface);
			wpa_supplicantClient_ifManager_AddIf(client->m_ifManager,
												 interface);
		}
		break;

	case CLIENT_EVENT_TYPE_REM_IF:
		{
			INFO("CLIENT_EVENT_TYPE_REM_IF");
			char *interface = (char *)args;
			PROGRESS("Client: Removing Interface with Object Path %s", interface);
			wpa_supplicantClient_ifManager_RemIf(client->m_ifManager,
					                             interface);
		}
		break;

	case CLIENT_EVENT_TYPE_SET_DBG_LEVEL:
		INFO("CLIENT_EVENT_TYPE_SET_DBG_LEVEL");
		client->m_dbgLvl = (ClientDbgLvl)args;
		break;

	case CLIENT_EVENT_TYPE_SET_SHOW_TS:
		INFO("CLIENT_EVENT_TYPE_SET_SHOW_TS");
		client->m_dbgShowTS = (bool)args;
		break;

	case CLIENT_EVENT_TYPE_SET_SHOW_KEYS:
		INFO("CLIENT_EVENT_TYPE_SET_SHOW_KEYS");
		client->m_dbgShowKeys = (bool)args;
		break;

	case CLIENT_EVENT_TYPE_ADD_EAP_METHOD:
		INFO("CLIENT_EVENT_TYPE_ADD_EAP_METHOD");
		client->m_eapMethods[client->m_eapMethodCount++] = (EapMethod)args;
		break;

	default:
		ERROR("Invalid Client Event Type %d", type);
		break;
	}

	EXIT();
	return;
}

