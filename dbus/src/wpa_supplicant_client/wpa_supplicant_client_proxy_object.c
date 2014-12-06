/*
 * wpa_supplicant_client_proxy_object.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_proxy_object.h"

wpa_supplicantClient_ProxyObject * wpa_supplicantClient_proxyObject_Init () {
	printf("Entering wpa_supplicantClient_proxyObject_Init() \n");
	//First Create the object
	wpa_supplicantClient_ProxyObject * proxy = malloc(sizeof(wpa_supplicantClient_ProxyObject));
	if (!proxy){
		printf ("Can not allocate a wpa_supplicantClient_ProxyObject Object ...Exiting\n");
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicantClient_ProxyObject));

	return proxy;
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


void wpa_supplicantClient_proxyObject_Start (wpa_supplicantClient_ProxyObject *proxy) {
	printf("Entering wpa_supplicantClient_ProxyObject_Start() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_ProxyObject_Start() \n");
		return;
	}

	return;
}

void signalNotify (GDBusProxy *proxy,
		           gchar *sender,
                   gchar *signal,
                   GVariant *parameter,
                   gpointer userData) {
	printf("Entering Client signalNotify() :\n");
	printf("------ Sender: %s\n", sender);
	printf("------ Signal: %s\n", signal);

	if (!strcmp(signal,"InterfaceAdded")) {
		printf("Received InterfaceAdded Signal\n");
	} else if (!strcmp(signal, "InterfaceRemoved")) {
		printf("Received InterfaceRemoved Signal\n");
	} else if (!strcmp(signal, "PropertiesChanged")) {
		printf("Received PropertiesChanged Signal");
	} else if (!strcmp(signal, "NetworkRequest")) {
		printf("Received NetworkRequest Signal");
	} else {
		printf("Received an Invalid Signal");
	}
}


void wpa_supplicantClient_proxyObject_StartFollowing (wpa_supplicantClient_ProxyObject *proxy,
		                                              GDBusConnection *connection) {
	GError * error = NULL;

	proxy->m_objectProxy = g_dbus_object_proxy_new (connection,
			                                        WPA_SUPPLICANT_MAIN_OBJECT_PATH);
	if (!proxy->m_objectProxy) {
		printf ("Failed to create an object proxy for the Client \n");
		return;
	}

	proxy->m_mainIfProxy = g_dbus_proxy_new_sync (connection,
			                                      G_DBUS_PROXY_FLAGS_NONE,
						   						  NULL,
												  WPA_SUPPLICANT_BUS_NAME,
												  WPA_SUPPLICANT_MAIN_OBJECT_PATH,
												  WPA_SUPPLICANT_MAIN_INTERFACE,
												  NULL,
												  &error);
	if (!proxy->m_mainIfProxy) {
		printf("Can not create an interface proxy for the main interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the main interface successfully \n");
	}

	proxy->m_ifIfProxy = g_dbus_proxy_new_sync (connection,
			                                    G_DBUS_PROXY_FLAGS_NONE,
								  				NULL,
												WPA_SUPPLICANT_BUS_NAME,
												WPA_SUPPLICANT_MAIN_OBJECT_PATH,
												WPA_SUPPLICANT_INTERFACE_INTERFACE,
												NULL,
												&error);
	if (!proxy->m_ifIfProxy) {
		printf("Can not create an interface proxy for the Interface interface \n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the Interface interface successfully \n");
	}


    //Register for the signals for Main Interface
    g_signal_connect ((proxy->m_mainIfProxy),
    			      "g-signal",
    				   G_CALLBACK(signalNotify),
    				   NULL);

    //Register for the signals for Interface Interface
    g_signal_connect ((proxy->m_ifIfProxy),
    			      "g-signal",
    				   G_CALLBACK(signalNotify),
    				   NULL);

    //Next step is to collect property values


#if 0 //will use some of it later
    g_signal_connect (G_DBUS_OBJECT(proxy->m_objectProxy),
    			      "interface-added",
    				   G_CALLBACK(onInterfaceAdded),
    				   NULL);

    g_signal_connect (G_DBUS_OBJECT(proxy->m_objectProxy),
    			      "interface-removed",
    				   G_CALLBACK(onInterfaceRemoved),
    				   NULL);



	GDBusProxy *pp = g_dbus_proxy_new_sync (connection,
			                                    G_DBUS_PROXY_FLAGS_NONE,
								  				NULL,
												WPA_SUPPLICANT_BUS_NAME,
												"/fi/w1/wpa_supplicant1/Interfaces/13",
												WPA_SUPPLICANT_INTERFACE_INTERFACE,
												NULL,
												&error);
	if (!pp) {
		printf("Can not create an interface proxy for the interface 13\n");
		printf("Error Message: %s \n", error->message);
	} else {
		printf("Created the proxy for the Interface 13 successfully \n");
	}

    g_signal_connect (pp,
    			      "g-signal",
    				   G_CALLBACK(user_function2),
    				   NULL);

    g_signal_connect (pp,
        			      "g-properties-changed",
        				   G_CALLBACK(user_function3),
        				   NULL);
#endif

#if 0
	//DEAD Code

#if 0
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
#endif

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

void wpa_supplicantClient_proxyObject_StopFollowing(wpa_supplicantClient_ProxyObject *proxy) {

	//TODO I need to add disconnect of signals and release of interfaces and objects
	return;
}

void wpa_supplicantClient_proxyObject_Stop (wpa_supplicantClient_ProxyObject *proxy){
	printf("Entering wpa_supplicantClient_ProxyObject_Stop() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_ProxyObject_Start() \n");
		return;
	}

	return;
}

void wpa_supplicantClient_proxyObject_Destroy(wpa_supplicantClient_ProxyObject *proxy) {
	printf("Entering wpa_supplicantClient_ProxyObject_Destroy() \n");

	if (!proxy) {
		printf("NULL is passed to wpa_supplicantClient_ProxyObject_Destroy() ... Exiting \n");
		return;
	}

	//finally free the object
	free(proxy);

	return;
}

