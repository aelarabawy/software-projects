/*
 * wpa_supplicant_client_proxy_object.c
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#include "wpa_supplicant_client_defines.h"
#include "wpa_supplicant_client_proxy_object.h"

wpa_supplicantClient_ProxyObject * wpa_supplicantClient_proxyObject_Init (void *cb, void *parent) {
	printf("Entering wpa_supplicantClient_proxyObject_Init() \n");
	//First Create the object
	wpa_supplicantClient_ProxyObject * proxy = malloc(sizeof(wpa_supplicantClient_ProxyObject));
	if (!proxy){
		printf ("Can not allocate a wpa_supplicantClient_ProxyObject Object ...Exiting\n");
		return NULL;
	}
	memset(proxy, 0, sizeof(wpa_supplicantClient_ProxyObject));

	proxy->m_notifyCb = cb;
	proxy->m_parent = parent;

	return proxy;
}


void wpa_supplicantClient_proxyObject_Start (wpa_supplicantClient_ProxyObject *proxy) {
	printf("Entering wpa_supplicantClient_ProxyObject_Start() \n");

	if (!proxy) {
		printf("NULL is passed to the function wpa_supplicantClient_ProxyObject_Start() \n");
		return;
	}

	return;
}

void signalNotify (GDBusProxy *dbusIfProxy,
		           gchar *sender,
                   gchar *signal,
                   GVariant *parameter,
                   gpointer userData) {
	printf("Entering Client signalNotify() :\n");
	printf("------ Sender: %s\n", sender);
	printf("------ Signal: %s\n", signal);

	//First Get the proxy
	wpa_supplicantClient_ProxyObject *proxy = (wpa_supplicantClient_ProxyObject *)userData;


	if (!strcmp(signal,"InterfaceAdded")) {
	    GVariantIter *iter;
	    GVariant *v;
	    gchar *str;

		printf("Received InterfaceAdded Signal\n");
	    g_variant_get(parameter, "(oa{sv})", &str, &iter);

	    printf("Added Interface : %s\n", str);

	    /*Note that although the parameter includes a dictionary of the interface properties,
	     * I am not going to parse it here....
	     * Instead, the interface D-Bus controller will query for it
	     * */

	    //Call the call back function
        proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );

#if 0
	    //Now working with the array of key-value pairs

	    while (g_variant_iter_loop(iter, "{sv}", &str, &v)) {
	    	printf("Key is : %s\n", str);
	    }

	    Capabilities
	    Key is : State
	    Key is : Scanning
	    Key is : ApScan
	    Key is : BSSExpireAge
	    Key is : BSSExpireCount
	    Key is : Country
	    Key is : Ifname
	    Key is : Driver
	    Key is : BridgeIfname
	    Key is : CurrentBSS
	    Key is : CurrentNetwork
	    Key is : CurrentAuthMode
	    Key is : Blobs
	    Key is : BSSs
	    Key is : Networks
	    Key is : FastReauth
	    Key is : ScanInterval
	    Key is : PKCS11EnginePath
	    Key is : PKCS11ModulePath
	    Key is : DisconnectReason

#endif

	} else if (!strcmp(signal, "InterfaceRemoved")) {
	    gchar *str;

	    printf("Received InterfaceRemoved Signal\n");
	    g_variant_get(parameter, "(o)", &str);

	    printf("Removed Interface : %s\n", str);

	    //Call the call back function
        proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_REM_IF, (void *)str );

	} else if (!strcmp(signal, "PropertiesChanged")) {
		printf("Received PropertiesChanged Signal");
	} else if (!strcmp(signal, "NetworkRequest")) {
		printf("Received NetworkRequest Signal");
	} else {
		printf("Received an Invalid Signal");
	}
}


void getProp_dbgLvl(wpa_supplicantClient_ProxyObject *proxy) {
    GVariant *v;
    ClientDbgLvl dbgLvl;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (proxy->m_mainIfProxy,
  	                                      "DebugLevel");
    if (!v) {
    	printf("Failed to get the property for DebugLevel\n");
    	return;
    }

    g_variant_get(v, "s",&str);
    printf("Debug Level Property is %s\n",str);

    if (!strcmp(str, "msgdump")) {
    	dbgLvl = CLIENT_DBG_LVL_VERBOSE;
    } else if (!strcmp(str, "debug")) {
    	dbgLvl = CLIENT_DBG_LVL_DEBUG;
    } else if (!strcmp(str, "info")) {
    	dbgLvl = CLIENT_DBG_LVL_INFO;
    } else if (!strcmp(str, "warning")) {
    	dbgLvl = CLIENT_DBG_LVL_WRN;
    } else if (!strcmp(str, "erro")) {
    	dbgLvl = CLIENT_DBG_LVL_ERR;
    } else {
    	printf("Received Invalid Debug Level: %s\n", str);
    }

    g_free(str);

    //Call the call back function
    proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_SET_DBG_LEVEL, (void *)dbgLvl );
}

void getProp_dbgShowTS(wpa_supplicantClient_ProxyObject *proxy) {
	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (proxy->m_mainIfProxy,
	  	                                  "DebugTimestamp");
    if (!v) {
    	printf("Failed to get the property for DebugTimestamp\n");
    	return;
    }

    g_variant_get(v, "b", &show);
    printf("DebugTimestamp Property is %d\n",(int)show);

    //Call the call back function
    proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_TS , (void *)show );
}

void getProp_dbgShowKeys(wpa_supplicantClient_ProxyObject *proxy) {
	GVariant *v;
    bool show;

    v = g_dbus_proxy_get_cached_property (proxy->m_mainIfProxy,
	  	                                  "DebugShowKeys");
    if (!v) {
    	printf("Failed to get the property for DebugShowKeys\n");
    	return;
    }

    g_variant_get(v, "b", &show);
    printf("DebugShowKeys Property is %d\n",(int)show);

    //Call the call back function
    proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_SET_SHOW_KEYS, (void *)show );
}

char *eapMethodList[] = {"none", "MD5", "TLS", "MSCHAPV2", "PEAP", "TTLS", "GTC", "OTP", "SIM", "LEAP", "PSK",
		                 "AKA", "AKA'", "FAST", "PAX", "SAKE", "GPSK", "WSC", "IKEV2", "TNC", "PWD"};


void getProp_eapMethods(wpa_supplicantClient_ProxyObject *proxy) {
    GVariant *v;
    GVariantIter *iter;
    gchar *str;
    EapMethod method;

    v = g_dbus_proxy_get_cached_property (proxy->m_mainIfProxy,
  	                                      "EapMethods");
    if (!v) {
    	printf("Failed to get the property for EapMethods\n");
    	return;
    }

    g_variant_get(v, "as", &iter);
    while (g_variant_iter_loop(iter, "s", &str)) {
    	printf("EAP Method : %s\n", str);

    	int i;
    	for (i = 1; i < (int)EAP_LAST ; i++) {
    		if (!strcmp(str,eapMethodList[i])) {
    			method = (EapMethod)i;
    			break;
    		}
    	}
    	if (i == (int)EAP_LAST) {
    		printf("Found Invalid EAP Method: %s\n", str);
    	} else {
        	//Call the call back function
            proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_ADD_EAP_METHOD, (void *)method );
    	}
    }
}

void getProp_interfaces (wpa_supplicantClient_ProxyObject *proxy) {
    GVariant *v;
    GVariantIter *iter;
    gchar *str;

    v = g_dbus_proxy_get_cached_property (proxy->m_mainIfProxy,
  	                                      "Interfaces");
    if (!v) {
    	printf("Failed to get the property for Interfaces\n");
    	return;
    }

    g_variant_get(v, "ao", &iter);
    while (g_variant_iter_loop(iter, "o", &str)) {
    	printf("Interface : %s\n", str);

    	//Call the call back function
        proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_ADD_IF, (void *)str );

#if 0
    	int i;
    	for (i = 1; i < (int)EAP_LAST ; i++) {
    		if (!strcmp(str,eapMethodList[i])) {
    			method = (EapMethod)i;
    			break;
    		}
    	}
    	if (i == (int)EAP_LAST) {
    		printf("Found Invalid EAP Method: %s\n", str);
    	} else {
        	//Call the call back function
            proxy->m_notifyCb(proxy->m_parent, CLIENT_EVENT_TYPE_ADD_EAP_METHOD, (void *)method );
    	}
#endif
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
    				   (void *)proxy);

    //Register for the signals for Interface Interface
    g_signal_connect ((proxy->m_ifIfProxy),
    			      "g-signal",
    				   G_CALLBACK(signalNotify),
    				   (void *)proxy);

    //Next step is to collect property values
    getProp_dbgLvl(proxy);
    getProp_dbgShowTS(proxy);
    getProp_dbgShowKeys(proxy);
    getProp_eapMethods(proxy);
    getProp_interfaces(proxy);

    #if 0 //will use some of it later

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

