/*
 * wpa_supplicant_client_dbus_controller.c
 *
 *  Created on: Dec 5, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_dbus_controller.h"

//Prototypes for call back functions
void OnNameWatchEventCb (void *, NameWatcherEventType, void *);


wpa_supplicantClient_dbusController *wpa_supplicantClient_dbusController_Init (void *notifyCb,
		                                                                       void *parent) {

    wpa_supplicantClient_dbusController *controller = malloc(sizeof(wpa_supplicantClient_dbusController));
    if (!controller) {
    	printf("Failed to allocate the Client D-Bus Object ... Exiting\n");
    	goto FAIL_CONTROLLER;
    }
	memset (controller, 0, sizeof(wpa_supplicantClient_dbusController));

	//Set the Notification parameters
	controller->m_notifyCb = notifyCb;
	controller->m_parent = parent;


	//Initialize the Name Watcher for the wpa_supplicant name on the bus
	controller->m_nameWatcher = wpa_supplicantClient_nameWatcher_Init(WPA_SUPPLICANT_BUS_NAME,
			                                                          OnNameWatchEventCb,
																      (void *) controller);
	if (!controller->m_nameWatcher) {
		printf("Failed to initialize the Name Watcher ... exiting\n");

		goto FAIL_NAME_WATCHER;
	}

	controller->m_proxyIntrospectable = wpa_supplicantClient_proxyIntrospectable_Init();
	if (!controller->m_proxyIntrospectable) {
		printf("Failed to Initialize the Proxy Introspectable Object... Exiting\n");

		goto FAIL_PROXY_INTROSPECTABLE;
	}

	//Initialize the Object Proxy
	controller->m_proxyObject = wpa_supplicantClient_proxyObject_Init();
	if (!controller->m_proxyObject) {
		printf("Failed to Initialize the Proxy Object  .. Exiting\n");

		goto FAIL_PROXY_OBJECT;
	}

	//Success
	goto SUCCESS;

FAIL_PROXY_OBJECT:
    wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);
FAIL_PROXY_INTROSPECTABLE:
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);
FAIL_NAME_WATCHER:
	free(controller);
FAIL_CONTROLLER:
    return NULL;

SUCCESS:
    return controller;
}

void wpa_supplicantClient_dbusController_Start (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Start the Name Watcher
	wpa_supplicantClient_nameWatcher_Start(controller->m_nameWatcher);

	//Start the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Start(controller->m_proxyIntrospectable);

	//Start the Proxy Object
	wpa_supplicantClient_proxyObject_Start(controller->m_proxyObject);

    //Start the Event Loop
	controller->m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(controller->m_loop);


	return;
}

void wpa_supplicantClient_dbusController_Stop (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Clean-up Event loop
	g_main_loop_unref (controller->m_loop);

	//Stop the Proxy Object
	wpa_supplicantClient_proxyObject_Stop(controller->m_proxyObject);

	//Stop the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Stop(controller->m_proxyIntrospectable);


	//Stop the Name Watcher
	wpa_supplicantClient_nameWatcher_Stop(controller->m_nameWatcher);

	return;
}

void wpa_supplicantClient_dbusController_Destroy (wpa_supplicantClient_dbusController *controller) {

	if (!controller){
		printf("Passing NULL to the function ...Exiting\n");
		return;
	}

	//Destroy the Proxy Object
	wpa_supplicantClient_proxyObject_Destroy(controller->m_proxyObject);

	//Destroy the Proxy Introspectable
	wpa_supplicantClient_proxyIntrospectable_Destroy(controller->m_proxyIntrospectable);


	//Destroy the Name watcher
	wpa_supplicantClient_nameWatcher_Destroy(controller->m_nameWatcher);

	//Finally, we free the controller
	free (controller);

	return;
}


void detectedNameUp(wpa_supplicantClient_dbusController *controller,
                    GDBusConnection *connection) {
	printf ("Entering the detectedNameUp() \n");

	if (controller->m_connection) {
		printf("Something is wrong: Stored Connection should be NULL\n");
		return;
	}

	controller->m_connection = connection;

	//Get the Object Description
	wpa_supplicantClient_proxyIntrospectable_GetXmlDescription(controller->m_proxyIntrospectable,
			                                                   &controller->xmlDescription,
															   connection);
	if (controller->xmlDescription) {
		printf("Interface Description: \n %s \n", controller->xmlDescription);
	} else {
		printf("Could not get Introspect the Interface \n");
	}

	//Now we need to trigger the Object Proxy
	wpa_supplicantClient_proxyObject_StartFollowing (controller->m_proxyObject,
		  	                                         connection);

	return;
}

void detectedNameDown(wpa_supplicantClient_dbusController *controller,
		              GDBusConnection *connection) {
	printf ("Entering detectedNameDown() \n");

	if ((!controller->m_connection)  || (controller->m_connection != connection)) {
		printf("Something is wrong: Stored Connection should be the same as the received value\n");
		return;
	}

	controller->m_connection = NULL;

	free (controller->xmlDescription); //TODO this should be changed to gfree.... check with the api


	//Now we need to disable the Object Proxy
	wpa_supplicantClient_proxyObject_StopFollowing (controller->m_proxyObject);

	return;
}

void OnNameWatchEventCb (void *parent,
		                 NameWatcherEventType type,
						 void *args) {
	GDBusConnection *connection;

	if (!parent) {
		printf ("NULL pointer passed for parent....Exiting\n");
		return;
	}
	wpa_supplicantClient_dbusController *controller = (wpa_supplicantClient_dbusController *)parent;

	switch (type) {
	case  NAME_WATCHER_EVENT_NAME_UP:
		connection = (GDBusConnection *)args;
		detectedNameUp (controller, connection);
		break;

	case NAME_WATCHER_EVENT_NAME_DOWN:
		connection = (GDBusConnection *)args;
		detectedNameDown (controller, connection);
		break;

	default:
		printf("Invalid Name Watch Event\n");
	}

	return;
}
