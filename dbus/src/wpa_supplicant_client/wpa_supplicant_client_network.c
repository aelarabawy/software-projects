/*
 * wpa_supplicant_client_network.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_network.h"

//Prototype for the call back function from the D-Bus Controller
static void dbusControllerNotify (void *, NetworkEventType, void *);


wpa_supplicantClient_network *wpa_supplicantClient_network_Init (char *busName,
                                                                 char *objectPath,
		                                                         void *connection)  {
	ENTER_FUNC("Path Name = %s",objectPath);

	wpa_supplicantClient_network *network = malloc(sizeof(wpa_supplicantClient_network));
    if (!network) {
    	NULL_POINTER("network");
    	goto FAIL_NETWORK;
    }
    memset(network, 0, sizeof(wpa_supplicantClient_network));

    strcpy(network->m_busName, busName);
    strcpy(network->m_objectPath, objectPath);

	//Initialize the D-Bus Controller
    PROGRESS("Initializing the Network D-Bus Controller");
	network->m_dbusController = wpa_supplicantClient_network_dbusController_Init(busName,
			                                                                     objectPath,
																				 connection,
																		         dbusControllerNotify,
			                                                                     (void *)network);
	if (!network->m_dbusController) {
		ERROR("Failed to initialize the Network D-BUS Controller ... exiting");

		goto FAIL_DBUS_CONTROLLER;
	}

	//Success
	goto SUCCESS;

FAIL_DBUS_CONTROLLER:
    free(network);

FAIL_NETWORK:
    EXIT_WITH_ERROR();
    return NULL;

SUCCESS:
	EXIT();
	return network;
}

void wpa_supplicantClient_network_Start (wpa_supplicantClient_network *network)  {

	ENTER();
	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Start the D-Bus Controller
	PROGRESS("Start the Network D-Bus Controller");
	wpa_supplicantClient_network_dbusController_Start(network->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_network_Stop (wpa_supplicantClient_network *network)  {

	ENTER();

	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Stop the D-Bus Controller
	PROGRESS("Stop the Network D-Bus Controller");
	wpa_supplicantClient_network_dbusController_Stop(network->m_dbusController);

	EXIT();
	return;
}

void wpa_supplicantClient_network_Destroy (wpa_supplicantClient_network *network) {

	ENTER();

	if (!network){
		NULL_POINTER("network");
		EXIT_WITH_ERROR();
		return;
	}

	//Destroy the D-Bus Controller
	PROGRESS("Destroy the Network D-Bus Controller");
	wpa_supplicantClient_network_dbusController_Destroy(network->m_dbusController);


	//Free the Network Object
	free(network);

	EXIT();
	return;
}

char* wpa_supplicantClient_network_GetPathName (wpa_supplicantClient_network *network) {
	ENTER();
	EXIT();
	return network->m_objectPath;
}

// Get & Set of Properties
//////////////////////////
bool wpa_supplicantClient_network_GetEnabled (wpa_supplicantClient_network *network) {
	ENTER();
	EXIT();
	return network->m_isEnabled;
}

void wpa_supplicantClient_network_SetEnabled (wpa_supplicantClient_network *network,
		                                      bool isEnabled) {
	ENTER();
	wpa_supplicantClient_network_dbusController_SetEnabled (network->m_dbusController, isEnabled);
	network->m_isEnabled = isEnabled;
	EXIT();
}


//Private Functions
///////////////////
//Notification function (call back from dbus controller)
static void dbusControllerNotify (void *parent, NetworkEventType type, void* args) {
	ENTER_FUNC("type = %d and value %d",type, args );

	wpa_supplicantClient_network *network = (wpa_supplicantClient_network *)parent;

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
