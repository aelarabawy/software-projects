/*****************************
 * intro-netlink.c
 * Author: Ahmed I. ElArabawy
 * Created on: 11/26/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Static and Global variables


void netlink_usage (void) {
	//Print the usage of the command line
	//TODO Complete this function
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
