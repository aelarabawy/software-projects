/*****************************
 * intro-dbus.c
 * Author: Ahmed I. ElArabawy
 * Created on: 11/16/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>

//Static and Global variables
static GDBusObjectManagerServer * objectManagerServer = NULL;
static GDBusObjectManagerClient * objectManagerClient = NULL;

enum OperationMode {
	OPER_MODE_INVALID = 0,
	OPER_MODE_CLIENT = 1,
	OPER_MODE_SERVER = 2
};

void dbus_usage (void) {
	//Print the usage of the command line
	//TODO Complete this function
}

void* waitForEvents(void) {
    GMainLoop * loop;
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return (void *)loop;
}

void cleanupEventsLoop(void * eventLoop){
    printf("Entering cleanupEventsLoop() \n");
	g_main_loop_unref ((GMainLoop *)eventLoop);
}

void dbus_create_and_export_objects () {

    char objectPath [100];
    char objectManagerPath  [100];
    strcpy (objectManagerPath, g_dbus_object_manager_get_object_path(G_DBUS_OBJECT_MANAGER(objectManagerServer)));

	//Create and export 10 Objects
  	for (int i = 0; i< 10; i++) {
        sprintf(objectPath,"%s/object/%03d",objectManagerPath,i );
        printf ("Creating the skeleton object : %s\n", objectPath);

        GDBusObjectSkeleton* object = g_dbus_object_skeleton_new (objectPath);
        g_dbus_object_manager_server_export (objectManagerServer, object);
  	}
}

static void dbus_acquiredCallBack (GDBusConnection *connection,
        		                   const gchar *name,
							       gpointer userData) {
	printf("Entering the function dbus_acquiredCallBack() \n");
	printf("---- Bus Name is %s\n",name);
	printf("---- Connection is %p\n", connection);

    //Create an Object Manager with the desired path
	objectManagerServer = g_dbus_object_manager_server_new("/example/server");
	if (!objectManagerServer){
		printf("Could not create the Object Manager Server \n");
		return;
	}

	//Create and Export Objects
	dbus_create_and_export_objects ();

	//Export all objects on the Bus through the manager
	g_dbus_object_manager_server_set_connection (objectManagerServer,
			                                     connection);
}

static void dbus_nameAcquiredCallBack (GDBusConnection *connection,
		                               const gchar *name,
					   		           gpointer userData) {
	printf("Entering the function dbus_nameAcquiredCallBack() \n");
	printf("Name is %s\n",name);
	printf("---- Connection is %p\n", connection);
}

static void dbus_nameLostCallBack (GDBusConnection *connection,
		                           const gchar *name,
							       gpointer userData) {
	printf("Entering the function dbus_nameLostCallBack() \n");
	printf("Name is %s\n",name);
	printf("---- Connection is %p\n", connection);

	//The server will need to de-register the objects here
}

guint dbus_reserveBusName (char* busName){
	printf ("Entering dbus_reserveBusName, busName = %s\n", busName);

	guint ownerId;
	ownerId  = g_bus_own_name (G_BUS_TYPE_SESSION,
			                   busName,
							   G_BUS_NAME_OWNER_FLAGS_NONE,
							   dbus_acquiredCallBack,
							   dbus_nameAcquiredCallBack,
							   dbus_nameLostCallBack,
							   NULL,
							   NULL);
	printf ("dbus_ownName: ownerId = %d \n", ownerId);

	return ownerId;
}

void dbus_releaseBusName (guint ownerId) {
	printf ("Entering the dbus_releaseBusName, ownerId = %d\n", ownerId);
	g_bus_unown_name(ownerId);
}

/*****************************************
 * dbus_watchName ()
 * This function is used by the client to watch for a name on the bus
 * It takes the busName to watch
 * */

void onObjectAdded (GDBusObjectManager *manager,
		            GDBusObject *object,
					gpointer userData) {
	printf("Entering onObjectAdded ....\n");
	printf("----- Object Path: %s \n", g_dbus_object_get_object_path(object));

	return;
}

void onObjectRemoved (GDBusObjectManager *manager,
		              GDBusObject *object,
					  gpointer userData) {
	printf("Entering onObjectRemoved ....\n");
	printf("----- Object Path: %s \n", g_dbus_object_get_object_path(object));

	return;
}

void dbus_objectManagerClient_init (GDBusConnection *connection,
		                            const gchar *busName) {
	//Instantiate the Client Manager
	GError * error = NULL;

	GDBusObjectManager* objectManager;
	objectManager = g_dbus_object_manager_client_new_sync (connection,
			                                                     G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
																 busName,
																 "/example/server",
																 NULL,  //Proxies will be built automatically
																 NULL,
																 NULL,
																 NULL,
																 &error);
	if (!objectManager){
		printf("Failed to create the ObjectManagerClient \n");
		return;
	}

	//Now we will list the object proxies
	GList * objectList = g_dbus_object_manager_get_objects (objectManager);

	if (!objectList) {
		printf("The ObjectManagerClient has no objects\n");
	} else {
		printf("-----------Remote Objects -----------\n");
		while (objectList) {
            GDBusObject * object = (GDBusObject *)objectList->data;
            printf ("%s\n", g_dbus_object_get_object_path(object));
			objectList = objectList->next;
		}
		printf("-------------------------\n");
	}

	//Now connecting to the manager signals
	g_signal_connect (objectManager,
			          "object-added",
					  G_CALLBACK(onObjectAdded),
					  NULL);

	g_signal_connect (objectManager,
			          "object-removed",
					  G_CALLBACK(onObjectRemoved),
					  NULL);
	/*
	g_signal_connect (objectManagerClient,
			          "interface-added",
					  G_CALLBACK(onInterfaceAdded),
					  NULL);

	g_signal_connect (objectManagerClient,
			          "interface-removed",
					  G_CALLBACK(onInterfaceRemoved),
					  NULL);
					  */

}

static void dbus_nameUpCallBack (GDBusConnection *connection,
		                         const gchar * busName,
		 		   		         const gchar * nameOwner,
						         gpointer userData) {
	printf ("Entering dbus_nameUpCallBack function\n");
    printf ("----- BusName = %s \n", busName);
	printf ("----- Owner Unique Name = %s \n", nameOwner);
	printf ("----- Connection = %p \n", connection);

	dbus_objectManagerClient_init(connection,
			                      busName);
	return;
}

static void dbus_nameDownCallBack (GDBusConnection *connection,
		                           const gchar * busName,
				       	  	       gpointer userData) {
	printf ("Entering dbus_nameDownCallBack function\n");
    printf ("----- BusName = %s \n", busName);
	printf ("----- Connection = %p \n", connection);

	//The Client should destroy object proxies here

	return;
}


guint dbus_watchBusName (char* busName){
	printf ("Entering dbus_watchName, busName = %s\n", busName);

	guint retVal;
	retVal = g_bus_watch_name (G_BUS_TYPE_SESSION,
                               busName,
			                   G_BUS_NAME_WATCHER_FLAGS_NONE,
							   dbus_nameUpCallBack,
							   dbus_nameDownCallBack,
			                   NULL,
			                   NULL);
	return retVal;
}

void dbus_stopWatchingBusName (guint busNameId){
	printf ("Entering dbus_stopWatchingBusName, busNameId = %d\n", busNameId);

	g_bus_unwatch_name (busNameId);
}

//main function
int main (int argc, char* argv[]) {

	//First Read the passed Command Line Arguments
	//TODO need to use a more systematic approach to parse passed arguments
	//     Check with the file gdbus-example-own-name.c and the use of GOptionEntry
	//     After you fix it, add it to the garden roses
    //TODO I need to build a logger, and use it instead of the printfs
	//     It should have severity level, function name, line number, ...
	//TODO I need to separate functionality in different files
	unsigned int operationMode;
	char busName [100];
	guint busNameId = 0;

	if (argc < 2){
		printf ("Invalid Entry ...\n");
		dbus_usage();
		return -1;
	} else {
		if (strcmp(argv[1], "--server") == 0){
			operationMode = OPER_MODE_SERVER;
			strcpy (busName, argv[2]);
		} else if (strcmp(argv[1], "--client") == 0) {
			operationMode = OPER_MODE_CLIENT;
			strcpy (busName, argv[2]);
		} else {
			operationMode = OPER_MODE_INVALID;
			printf ("Invalid Operation Mode\n");
			dbus_usage();
			return -1;
		}
	}

	//Now let us start operation
	void *eventLoop;
	switch (operationMode) {
	case OPER_MODE_CLIENT:
		busNameId = dbus_watchBusName (busName);
		eventLoop = waitForEvents();
		cleanupEventsLoop(eventLoop); //TODO should I move this to to inside waitForEvents () before it exits
		break;

	case OPER_MODE_SERVER:
		printf("Trying to own the name : %s \n", busName);

        busNameId = dbus_reserveBusName(busName);
		eventLoop = waitForEvents();
		dbus_releaseBusName(busNameId);
		cleanupEventsLoop(eventLoop); //TODO should I move this to to inside waitForEvents () before it exits
		break;
	}

	return 0;
}
