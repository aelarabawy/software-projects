/*
 * wpa_supplicant_client_proxy_object_manager.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_proxy_object_manager.h"

wpa_supplicantClient_ProxyObjectManager * wpa_supplicantClient_proxyObjectManager_Init () {
	printf("Entering wpa_supplicantClient_proxyObjectManager_Init() \n");
	//First Create the object
	wpa_supplicantClient_ProxyObjectManager * manager = malloc(sizeof(wpa_supplicantClient_ProxyObjectManager));
	if (!manager){
		printf ("Can not allocate a wpa_supplicantClient_ProxyObjectManager Object ...Exiting\n");
		return NULL;
	}
	memset(manager, 0, sizeof(wpa_supplicantClient_ProxyObjectManager));

	return manager;
}

void onObjectAdded (GDBusObjectManager *manager,
		            GDBusObject *object,
					gpointer userData) {
	printf("Entering onObjectAdded() ....\n");
	printf("----- Object Path: %s \n", g_dbus_object_get_object_path(object));

	return;
}

void onObjectRemoved (GDBusObjectManager *manager,
		              GDBusObject *object,
					  gpointer userData) {
	printf("Entering onObjectRemoved() ....\n");
	printf("----- Object Path: %s \n", g_dbus_object_get_object_path(object));

	return;
}

void onInterfaceAdded (GDBusObject *object,
		               GDBusInterface *interface,
					   gpointer userData) {
	printf ("Entering the onInterfaceAdded()\n");
}

void onInterfaceRemoved (GDBusObject *object,
		                 GDBusInterface *interface,
					     gpointer userData) {
	printf ("Entering the onInterfaceRemoved()\n");
}

void wpa_supplicantClient_proxyObjectManager_Start (wpa_supplicantClient_ProxyObjectManager *manager) {
	printf("Entering wpa_supplicantClient_ProxyObjectManager_Start() \n");

	if (!manager) {
		printf("NULL is passed to the function wpa_supplicantClient_ProxyObjectManager_Start() \n");
		return;
	}

	return;
}




void wpa_supplicantClient_proxyObjectManager_StartFollowing(wpa_supplicantClient_ProxyObjectManager *manager,
		                                                    GDBusConnection *connection) {
	GError * error = NULL;

	manager->mainObject = g_dbus_object_proxy_new (connection,
			                                       WPA_SUPPLICANT_MAIN_OBJECT_PATH);
	if (!manager->mainObject){
		printf ("Failed to create an object proxy for the main object \n");
		return;
	}

	manager->mainInterfaceProxy = g_dbus_proxy_new_sync (connection,
			                                             G_DBUS_PROXY_FLAGS_NONE,
								  						 NULL,
														 WPA_SUPPLICANT_BUS_NAME,
														 WPA_SUPPLICANT_MAIN_OBJECT_PATH,
														 WPA_SUPPLICANT_MAIN_INTERFACE,
														 NULL,
														 &error);
	if (!manager->mainInterfaceProxy){
		printf("Can not create an interface proxy for the main interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the main interface successfully \n");
	}

	manager->interfaceInterfaceProxy = g_dbus_proxy_new_sync (connection,
			                                                  G_DBUS_PROXY_FLAGS_NONE,
								  						      NULL,
														      WPA_SUPPLICANT_BUS_NAME,
														      WPA_SUPPLICANT_MAIN_OBJECT_PATH,
														      WPA_SUPPLICANT_INTERFACE_INTERFACE,
														      NULL,
														      &error);
	if (!manager->interfaceInterfaceProxy){
		printf("Can not create an interface proxy for the Interface interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the Interface interface successfully \n");
	}


    g_signal_connect (G_DBUS_OBJECT(manager->mainObject),
    			      "interface-added",
    				   G_CALLBACK(onInterfaceAdded),
    				   NULL);

    g_signal_connect (G_DBUS_OBJECT(manager->mainObject),
    			      "interface-removed",
    				   G_CALLBACK(onInterfaceRemoved),
    				   NULL);



#if 0
	//DEAD Code
	const gchar *objectPath =  g_dbus_object_get_object_path(G_DBUS_OBJECT(manager->mainObject));
	printf("The Object path is %s \n",objectPath);


    GList *interfaceList = g_dbus_object_get_interfaces(G_DBUS_OBJECT(manager->mainObject));
    if (!interfaceList){
    	printf ("The Object has no Interfaces\n");
    } else {
        while (interfaceList) {
        	GDBusInterface *interface = (GDBusInterface *)interfaceList->data;

        	interfaceList = interfaceList->next;
        }
    }


	manager->m_objectManager =
			g_dbus_object_manager_client_new_sync (connection,
												   G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
													"fi.w1.wpa_supplicant1",
													"/fi/w1/wpa_supplicant1",
													NULL, //Proxies will be built automatically
													NULL,
													NULL,
													NULL,
													&error);

	if (!manager->m_objectManager) {
		printf("Failed to create the Object Proxy Manager with the message\n");
		printf("%s \n", error->message);

		if (g_dbus_error_is_remote_error(error)) {
			printf("This is a remote error\n");
			printf("Error Name: %s \n", g_dbus_error_get_remote_error(error));
		} else {
			printf("This is a local error\n");
		}
	}


	//Now we will list the object proxies
	GList * objectList  = g_dbus_object_manager_get_objects (manager->m_objectManager);

	if (!objectList) {
		printf("The Object Proxy Manager has no objects\n");
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
	g_signal_connect (manager->m_objectManager,
			          "object-added",
					  G_CALLBACK(onObjectAdded),
					  NULL);

	g_signal_connect (manager->m_objectManager,
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
#endif

    return;
}

void wpa_supplicantClient_proxyObjectManager_StopFollowing(wpa_supplicantClient_ProxyObjectManager *manager) {
	return;
}

void wpa_supplicantClient_proxyObjectManager_Stop (wpa_supplicantClient_ProxyObjectManager *manager){
	printf("Entering wpa_supplicantClient_ProxyObjectManager_Stop() \n");

	if (!manager) {
		printf("NULL is passed to the function wpa_supplicantClient_ProxyObjectManager_Start() \n");
		return;
	}

	return;
}

void wpa_supplicantClient_proxyObjectManager_Cleanup(wpa_supplicantClient_ProxyObjectManager *manager) {
	printf("Entering wpa_supplicantClient_ProxyObjectManager_Cleanup() \n");

	if (!manager) {
		printf("NULL is passed to wpa_supplicantClient_ProxyObjectManager_Cleanup() ... Exiting \n");
		return;
	}

	//finally free the object
	free(manager);

	return;
}

