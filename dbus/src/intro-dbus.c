/*****************************
 * intro-dbus.c
 * Author: Ahmed I. ElArabawy
 * Created on: 11/16/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>


enum OperationMode {
	OPER_MODE_INVALID = 0,
	OPER_MODE_CLIENT = 1,
	OPER_MODE_SERVER = 2
};

void dbus_usage (void) {
	//Print the usage of the command line
	//TODO Complete this function
}

void waitForEvents(void) {
    GMainLoop * loop;
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}

static void dbus_acquiredCallBack (GDBusConnection *connection,
        		                   const gchar *name,
							       gpointer userData) {
	printf("Entering the function dbus_acquiredCallBack() \n");
	printf("---- Name is %s\n",name);
	printf("---- Connection is %p\n", connection);

	//The server will need to export the objects here (g_dbus_connection_register_object)
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

static void dbus_nameUpCallBack (GDBusConnection *connection,
		                         const gchar * busName,
		 		   		         const gchar * nameOwner,
						         gpointer userData) {
	printf ("Entering dbus_nameUpCallBack function\n");
    printf ("----- BusName = %s \n", busName);
	printf ("----- Owner Unique Name = %s \n", nameOwner);
	printf ("----- Connection = %p \n", connection);

	//The Client should create object proxies here
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
	switch (operationMode) {
	case OPER_MODE_CLIENT:
		busNameId = dbus_watchBusName (busName);
		waitForEvents();
		break;

	case OPER_MODE_SERVER:
		printf("Trying to own the name : %s \n", busName);
        busNameId = dbus_reserveBusName(busName);
		waitForEvents();
		break;
	}

	return 0;
}
