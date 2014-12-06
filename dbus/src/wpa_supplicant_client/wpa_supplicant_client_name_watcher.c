/*
 * wpa_supplicant_client_name_watcher.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "common.h"
#include "wpa_supplicant_client_name_watcher.h"

//Prototypes for call back functions
void wpa_supplicant_nameWatcher_OnNameUp (GDBusConnection *, const gchar *, const gchar *, gpointer);
void wpa_supplicant_nameWatcher_OnNameDown (GDBusConnection *, const gchar *, gpointer);

wpa_supplicantClient_nameWatcher * wpa_supplicantClient_nameWatcher_Init (char *busName,
		                                                                  void *notifyCb,
																		  void *parent) {
	printf ("Entering wpa_supplicantClient_nameWatcher_Init(), busName = %s\n", busName);

	//Create the Name Watcher Object
	wpa_supplicantClient_nameWatcher * nameWatcher = malloc(sizeof(wpa_supplicantClient_nameWatcher));
	if (!nameWatcher) {
		printf ("Failed to allocate a wpa_supplicantClient_nameWatcher Object .. Exiting\n");
		return NULL;
	}
	memset (nameWatcher, 0, sizeof(wpa_supplicantClient_nameWatcher));

	strcpy(nameWatcher->m_busName, busName);
	nameWatcher->m_notifyCb = notifyCb;
	nameWatcher->m_parent = parent;

	return nameWatcher;
}

void wpa_supplicantClient_nameWatcher_Start (wpa_supplicantClient_nameWatcher *nameWatcher) {
	printf ("Entering wpa_supplicantClient_nameWatcher_Start()\n");

	if (!nameWatcher){
		printf ("NULL passed to wpa_supplicantClient_nameWatcher_Start ... Doing nothing \n");
		return;
	}

    //Start Watching on the Bus
	nameWatcher->m_watchId = g_bus_watch_name (G_BUS_TYPE_SYSTEM,
                                               nameWatcher->m_busName,
			                                   G_BUS_NAME_WATCHER_FLAGS_NONE,
							                   wpa_supplicant_nameWatcher_OnNameUp,
							                   wpa_supplicant_nameWatcher_OnNameDown,
			                                   (gpointer) nameWatcher,
			                                   NULL);


	return;
}

void wpa_supplicantClient_nameWatcher_Stop (wpa_supplicantClient_nameWatcher *nameWatcher) {
	printf ("Entering wpa_supplicantClient_nameWatcher_Stop()\n");

	if (!nameWatcher){
		printf ("NULL passed to wpa_supplicantClient_nameWatcher_Stop ... Doing nothing \n");
		return;
	}

	//Perform an unwatch
	if (nameWatcher->m_watchId){
		g_bus_unwatch_name (nameWatcher->m_watchId);
	}
	nameWatcher->m_watchId = 0;

	return;
}

void wpa_supplicantClient_nameWatcher_Destroy (wpa_supplicantClient_nameWatcher *nameWatcher) {
	printf ("Entering wpa_supplicantClient_nameWatcher_Destroy()\n");

	if (!nameWatcher){
		printf ("NULL passed to wpa_supplicantClient_nameWatcher_Destroy ... Doing nothing \n");
		return;
	}

	//Finally, free the nameWatcher Object
	free (nameWatcher);
}

void wpa_supplicant_nameWatcher_OnNameUp (GDBusConnection *connection,
		                                  const gchar * busName,
		 		   		                  const gchar * nameOwner,
						                  gpointer userData) {
	printf ("Entering wpa_supplicant_nameWatcher_OnNameUp()\n");
    printf ("----- BusName = %s \n", busName);
	printf ("----- Owner Unique Name = %s \n", nameOwner);
	printf ("----- Connection = %p \n", connection);

	//Get the nameWatcher Object
	if (!userData){
		printf ("NULL passed to wpa_supplicant_nameWatcher_OnNameUp() ... Doing nothing \n");
		return;
	}

	wpa_supplicantClient_nameWatcher *nameWatcher = (wpa_supplicantClient_nameWatcher *) userData;

	//Inform the parent of the event
	nameWatcher->m_notifyCb (nameWatcher->m_parent,
			                 NAME_WATCHER_EVENT_NAME_UP,
			                 (void *)connection);

	return;
}

void wpa_supplicant_nameWatcher_OnNameDown (GDBusConnection *connection,
	                                        const gchar * busName,
				       	  	                gpointer userData) {
	printf ("Entering wpa_supplicant_nameWatcher_OnNameDown()\n");
    printf ("----- BusName = %s \n", busName);
	printf ("----- Connection = %p \n", connection);

	//Get the nameWatcher Object
	if (!userData){
		printf ("NULL passed to wpa_supplicant_nameWatcher_OnNameDown() ... Doing nothing \n");
		return;
	}

	wpa_supplicantClient_nameWatcher *nameWatcher = (wpa_supplicantClient_nameWatcher *) userData;

	//Inform the parent of the event
	nameWatcher->m_notifyCb (nameWatcher->m_parent,
			                 NAME_WATCHER_EVENT_NAME_DOWN,
			                 (void *)connection);

	return;
}

