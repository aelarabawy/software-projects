/*****************************
 * intro-dbus.c
 * Author: Ahmed I. ElArabawy
 * Created on: 11/16/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>

void dbus_acquiredCallBack (GDBusConnection *connection,
		                    const gchar *name,
							gpointer userData) {
	printf("Entering the function dbus_acquiredCallBack() \n");
	printf("Name is %s\n",name);
}

void dbus_nameAcquiredCallBack (GDBusConnection *connection,
		                        const gchar *name,
					   		    gpointer userData) {
	printf("Entering the function dbus_nameAcquiredCallBack() \n");
	printf("Name is %s\n",name);
}

void dbus_nameLostCallBack (GDBusConnection *connection,
		                    const gchar *name,
							gpointer userData) {
	printf("Entering the function dbus_nameLostCallBack() \n");
	printf("Name is %s\n",name);
}



guint dbus_ownName (char* busName){
	printf ("Entering dbus_ownName, busName = %s\n", busName);

	guint ownerId;
	ownerId  = g_bus_own_name (G_BUS_TYPE_SESSION,
			                   busName,
							   G_BUS_NAME_OWNER_FLAGS_NONE,
							   dbus_acquiredCallBack,
							   dbus_nameAcquiredCallBack,
							   dbus_nameLostCallBack,
							   NULL,
							   NULL);

	return ownerId;
}

void dbus_unownName (guint ownerId) {
	printf ("Entering the dbus_unownName, ownerId = %d\n", ownerId);
	g_bus_unown_name(ownerId);
}

void dbus_watchName (char* busName){
	printf ("Entering dbus_watchName, busName = %s\n", busName);
	return;
}

//main function
void main (int argc, char** argv) {
	char command [100]="";
	char* token;
	guint ownerId = 0;

	while (strcmp(command,"exit") != 0) {
		printf("> ");
		fgets(command, sizeof (command), stdin);

		//Remove the Terminating EOL
		strtok(command,"\n");

		//Get the command
		token = strtok(command, " ");
		if (strcmp(token,"own_name") == 0) {
			//Get the Name to be owned
			token = strtok(NULL," ");
			if (token != NULL){
				ownerId = dbus_ownName(token);
			} else {
				printf("Invalid Entry, BusName needs to be provided\n");
			}
      } else if (strcmp(token,"unown_name") == 0) {
			if (ownerId != 0){
				dbus_unownName(ownerId);
				ownerId = 0;
			} else {
				printf("There is no Name to release \n");
			}
      } else if (strcmp(token,"watch_name") == 0){
  	  //Get the Name to be watched
			token = strtok(NULL," ");
			if (token != NULL){
				dbus_watchName(token);
			} else {
				printf("Invalid Entry, BusName needs to be provided\n");
			}
      }
	}

	return;
}
