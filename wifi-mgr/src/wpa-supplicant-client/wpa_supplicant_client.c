/*
 * wpa_supplicant_client.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client.h"

//Prototype for the call back function from the D-Bus Handler
static void dbusNotify (void *, ClientEventType, void*);

/**
 * wpa_supplicant_client_Init()
 * This function performs required initializations for the wpa_supplicant Client
 */
wpa_supplicant_client *wpa_supplicant_client_Init(char *busName,
		                                          char *objectPath) {
	ENTER();

	//Create the Client Object
	PROGRESS("Allocating the Client");
	wpa_supplicant_client *client = malloc(sizeof(wpa_supplicant_client));
	if (!client){
		ALLOC_FAIL("Failed to allocate a wpa_supplicant_client Object ... Exiting");

		goto FAIL_CLIENT;
	}
	memset (client, 0, sizeof(wpa_supplicant_client));

	//Setting the state
	client->m_state = CLIENT_STATE_IDLE;
	strcpy(client->m_busName, busName);
	strcpy(client->m_objectPath, objectPath);

	//Initialize the D-Bus Handler
	PROGRESS("Initialize the Client D-Bus Handler");
	client->m_dbusHandle = wpa_supplicant_dbus_client_Init(busName,
			                                               objectPath,
														   dbusNotify,
			                                               (void *)client);
	if (!client->m_dbusHandle) {
		ERROR("Failed to initialize the Client D-Bus Handler ... exiting");

		goto FAIL_DBUS_HANDLER;
	}

	//Initialize the Interface Manager
	PROGRESS("Initialize the Interface Manager");
	client->m_ifaceMgr = wpa_supplicant_client_ifaceMgr_Init(busName);
	if (!client->m_ifaceMgr) {
		ERROR("Failed to initialize the Client Interface Manager ... exiting");

		goto FAIL_IF_MANAGER;
	}

	//Success
	goto SUCCESS;

FAIL_IF_MANAGER:
	wpa_supplicant_dbus_client_Destroy(client->m_dbusHandle);
FAIL_DBUS_HANDLER:
	free(client);

FAIL_CLIENT:
	EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return client;
}


void wpa_supplicant_client_Start (wpa_supplicant_client *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Start the D-Bus Handler
	PROGRESS("Start the Client D-Bus Handler");
	wpa_supplicant_dbus_client_Start(client->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_Stop (wpa_supplicant_client *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Stop the Interface Manager
	wpa_supplicant_client_ifaceMgr_Stop(client->m_ifaceMgr);

	//Stop the D-Bus Handler
	wpa_supplicant_dbus_client_Stop(client->m_dbusHandle);

	EXIT();
	return;
}


/**
 * wpa_SupplicantClient_Destroy
 * This function performs required cleanup before exit
 */
void wpa_supplicant_client_Destroy (wpa_supplicant_client *client) {
	ENTER();

	if (!client){
		NULL_POINTER("The Client");
		EXIT_WITH_ERROR();
		return ;
	}

	//Destroy the Interface Manager
	wpa_supplicant_client_ifaceMgr_Destroy(client->m_ifaceMgr);

	//Destroy the D-Bus Handler
	wpa_supplicant_dbus_client_Destroy(client->m_dbusHandle);

	//Finally, we free the client
	free (client);

	EXIT();
	return;
}


/**
 * Public Get/Set methods
 * */
ClientDbgLvl wpa_supplicant_client_GetDbgLvl (wpa_supplicant_client *client) {
	ENTER();
	EXIT();
	return client->m_dbgLvl;
}

void wpa_supplicant_client_SetDbgLvl(wpa_supplicant_client *client, ClientDbgLvl lvl) {
	ENTER();
	wpa_supplicant_dbus_client_SetDbgLvl(client->m_dbusHandle, lvl);
	client->m_dbgLvl = lvl;
	EXIT();
}

bool wpa_supplicant_client_GetDbgShowTS (wpa_supplicant_client *client) {
	ENTER();
	EXIT();
	return client->m_dbgShowTS;
}

void wpa_supplicant_client_SetDbgShowTS (wpa_supplicant_client *client, bool show) {
	ENTER();
	wpa_supplicant_dbus_client_SetShowTS(client->m_dbusHandle, show);
	client->m_dbgShowTS = show;
	EXIT();
}

bool wpa_supplicant_client_GetDbgShowKeys (wpa_supplicant_client *client) {
	ENTER();
	EXIT();
	return client->m_dbgShowKeys;
}

void wpa_supplicant_client_SetDbgShowKeys (wpa_supplicant_client *client, bool show) {
	ENTER();
	wpa_supplicant_dbus_client_SetShowKeys(client->m_dbusHandle, show);
	client->m_dbgShowKeys = show;
	EXIT();
}


int wpa_supplicant_client_GetEapMethodCount (wpa_supplicant_client *client) {
	ENTER();
	EXIT();
	return client->m_eapMethodCount;
}

EapMethod wpa_supplicant_client_GetDEapMethod (wpa_supplicant_client *client, int index) {
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
static void resumeStart(wpa_supplicant_client *client,
		                void *connection) {
	ENTER();

	//Start the Interface Manager
	PROGRESS("Start the Interface Manager");
	wpa_supplicant_client_ifaceMgr_Start(client->m_ifaceMgr,
			                             connection);

	EXIT();
	return;
}

//Notification function (call back from dbus handler)
static void dbusNotify (void *parent, ClientEventType type, void* args) {
	ENTER_FUNC("Type = %d , Value = %d",type, args);

	wpa_supplicant_client *client = (wpa_supplicant_client *)parent;

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
			wpa_supplicant_client_ifaceMgr_AddIface(client->m_ifaceMgr,
												 interface);
		}
		break;

	case CLIENT_EVENT_TYPE_REM_IF:
		{
			INFO("CLIENT_EVENT_TYPE_REM_IF");
			char *interface = (char *)args;
			PROGRESS("Client: Removing Interface with Object Path %s", interface);
			wpa_supplicant_client_ifaceMgr_RemIface(client->m_ifaceMgr,
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

