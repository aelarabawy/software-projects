/*
 * wpa_supplicant_client_name_watcher.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_dbus_name_watcher.h"

//Prototypes for Private functions
static void onNameUp (GDBusConnection *, const gchar *, const gchar *, gpointer);
static void onNameDown (GDBusConnection *, const gchar *, gpointer);
//End of Prototypes for Private Functions

wpa_supplicant_dbus_nameWatcher * wpa_supplicant_dbus_nameWatcher_Init (char *busName,
		                                                                  void *notifyCb,
																		  void *parent) {
	ENTER_FUNC("busName = %s", busName);


	//Create the Name Watcher Object
	wpa_supplicant_dbus_nameWatcher * nameWatcher = malloc(sizeof(wpa_supplicant_dbus_nameWatcher));
	if (!nameWatcher) {
		ALLOC_FAIL("nameWatcher");
		EXIT_WITH_ERROR();
		return NULL;
	}
	memset (nameWatcher, 0, sizeof(wpa_supplicant_dbus_nameWatcher));

	strcpy(nameWatcher->m_busName, busName);
	nameWatcher->m_notifyCb = notifyCb;
	nameWatcher->m_parent = parent;

	EXIT();
	return nameWatcher;
}

void wpa_supplicant_dbus_nameWatcher_Start (wpa_supplicant_dbus_nameWatcher *nameWatcher) {

	ENTER();

	if (!nameWatcher){
		NULL_POINTER("nameWatcher");
		EXIT_WITH_ERROR();
		return;
	}

    //Start Watching on the Bus
	nameWatcher->m_watchId = g_bus_watch_name (G_BUS_TYPE_SYSTEM,
                                               nameWatcher->m_busName,
			                                   G_BUS_NAME_WATCHER_FLAGS_NONE,
							                   onNameUp,
							                   onNameDown,
			                                   (gpointer) nameWatcher,
			                                   NULL);
	EXIT();
	return;
}

void wpa_supplicant_dbus_nameWatcher_Stop (wpa_supplicant_dbus_nameWatcher *nameWatcher) {
	ENTER();

	if (!nameWatcher){
		NULL_POINTER("nameWatcher");
		EXIT_WITH_ERROR();
		return;
	}

	//Perform an unwatch
	if (nameWatcher->m_watchId){
		g_bus_unwatch_name (nameWatcher->m_watchId);
	}
	nameWatcher->m_watchId = 0;

	EXIT();
	return;
}

void wpa_supplicant_dbus_nameWatcher_Destroy (wpa_supplicant_dbus_nameWatcher *nameWatcher) {
	ENTER();

	if (!nameWatcher){
		ALLOC_FAIL("nameWatcher");
		EXIT_WITH_ERROR();
		return;
	}

	//Finally, free the nameWatcher Object
	free (nameWatcher);

	EXIT();
}


//Private Functions
///////////////////

static void onNameUp (GDBusConnection *connection,
		              const gchar * busName,
		 		   	  const gchar * nameOwner,
					  gpointer userData) {
	ENTER();

    INFO("----- BusName = %s", busName);
	INFO("----- Owner Unique Name = %s", nameOwner);
	INFO("----- Connection = %p", connection);

	//Get the nameWatcher Object
	if (!userData){
		NULL_POINTER("userData");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_nameWatcher *nameWatcher = (wpa_supplicant_dbus_nameWatcher *) userData;

	//Inform the parent of the event
	nameWatcher->m_notifyCb (nameWatcher->m_parent,
			                 NAME_WATCHER_EVENT_NAME_UP,
			                 (void *)connection);

	EXIT();
	return;
}

static void onNameDown (GDBusConnection *connection,
	                    const gchar * busName,
		                gpointer userData) {
	ENTER();
    INFO("----- BusName = %s", busName);
	INFO("----- Connection = %p", connection);

	//Get the nameWatcher Object
	if (!userData){
		NULL_POINTER("userData");
		EXIT_WITH_ERROR();
		return;
	}

	wpa_supplicant_dbus_nameWatcher *nameWatcher = (wpa_supplicant_dbus_nameWatcher *) userData;

	//Inform the parent of the event
	nameWatcher->m_notifyCb (nameWatcher->m_parent,
			                 NAME_WATCHER_EVENT_NAME_DOWN,
			                 (void *)connection);

	EXIT();
	return;
}
