/*
 * wpa_supplicant_client_network.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_common.h"
#include "wpa_supplicant_client_network.h"

//Prototype for the call back function from the D-Bus Controller
static void dbusNotify (void *, NetworkEventType, void *);


wpa_supplicant_client_network *wpa_supplicant_client_network_Init (char *busName,
                                                                 char *objectPath,
		                                                         void *connection)  {
	ENTER_FUNC("Path Name = %s",objectPath);

	wpa_supplicant_client_network *network = malloc(sizeof(wpa_supplicant_client_network));
    if (!network) {
    	NULL_POINTER("network");
    	goto FAIL_NETWORK;
    }
    memset(network, 0, sizeof(wpa_supplicant_client_network));

    strcpy(network->m_busName, busName);
    strcpy(network->m_objectPath, objectPath);

	//Initialize the D-Bus Handler
    PROGRESS("Initializing the Network D-Bus Handler");
	network->m_dbusHandle = wpa_supplicant_dbus_network_Init(busName,
			                                                 objectPath,
															 connection,
															 dbusNotify,
			                                                 (void *)network);
	if (!network->m_dbusHandle) {
		ERROR("Failed to initialize the Network D-BUS Handle ... exiting");

		goto FAIL_DBUS_HANDLE;
	}

	//Success
	goto SUCCESS;

FAIL_DBUS_HANDLE:
    free(network);

FAIL_NETWORK:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return network;
}

void wpa_supplicant_client_network_Start (wpa_supplicant_client_network *network)  {

	ENTER();
	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the D-Bus Handler
	PROGRESS("Start the Network D-Bus Handler");
	wpa_supplicant_dbus_network_Start(network->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_network_Stop (wpa_supplicant_client_network *network)  {

	ENTER();

	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Stop the D-Bus Handler
	PROGRESS("Stop the Network D-Bus Handler");
	wpa_supplicant_dbus_network_Stop(network->m_dbusHandle);

	EXIT();
	return;
}

void wpa_supplicant_client_network_Destroy (wpa_supplicant_client_network *network) {

	ENTER();

	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the D-Bus Handler
	PROGRESS("Destroy the Network D-Bus Handler");
	wpa_supplicant_dbus_network_Destroy(network->m_dbusHandle);


	//Free the Network Object
	free(network);

	EXIT();
	return;
}

char* wpa_supplicant_client_network_GetPathName (wpa_supplicant_client_network *network) {
	ENTER();
	EXIT();
	return network->m_objectPath;
}

// Get & Set of Properties
//////////////////////////
bool wpa_supplicant_client_network_GetEnabled (wpa_supplicant_client_network *network) {
	ENTER();
	EXIT();
	return network->m_isEnabled;
}

void wpa_supplicant_client_network_SetEnabled (wpa_supplicant_client_network *network,
		                                      bool isEnabled) {
	ENTER();
	wpa_supplicant_dbus_network_SetEnabled (network->m_dbusHandle, isEnabled);
	network->m_isEnabled = isEnabled;
	EXIT();
}


//Private Functions
///////////////////
//Notification function (call back from dbus handler)
static void dbusNotify (void *parent, NetworkEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args );

	wpa_supplicant_client_network *network = (wpa_supplicant_client_network *)parent;

	switch (type) {
	case NETWORK_EVENT_TYPE_SET_ENABLED:
		INFO("NETWORK_EVENT_TYPE_SET_ENABLED");
		network->m_isEnabled = (bool)args;
		break;

	default:
		ERROR("Invalid/UnSupported NETWORK Event Type %d", type);
		break;
	}

	EXIT();
	return;
}
