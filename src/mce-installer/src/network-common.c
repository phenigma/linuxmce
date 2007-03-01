/*
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
 #include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
 
OobsIfaceEthernet* get_external_interface(OobsList *interfaces);
OobsIfaceEthernet* get_internal_interface(OobsList *interfaces);
gboolean connect_test(const gchar* sourceIp);

void* start_network_wizard_thread(void *next_stepv) {
	int *next_step = (int *)next_stepv;

	OobsSession      *oobs_session = oobs_session_get();
	OobsIfacesConfig *ifaces_config = OOBS_IFACES_CONFIG(oobs_ifaces_config_get(oobs_session));
	OobsList         *interfaces = oobs_ifaces_config_get_ifaces(ifaces_config, OOBS_IFACE_TYPE_ETHERNET);
	OobsHostsConfig  *hosts_config = OOBS_HOSTS_CONFIG(oobs_hosts_config_get(oobs_session));
	GList            *nameservers = oobs_hosts_config_get_dns_servers(hosts_config);

	setting_netIfaceNo = oobs_list_get_n_items(interfaces);

	setting_netExtName=g_strdup("");
	if (setting_netIfaceNo == 0) {
		printf("No Cards!\n");
	       displayStep2F();
	} else {
		OobsIfaceEthernet* iface = get_external_interface(interfaces);
		if (iface != NULL) {
			setting_netExtName    = g_strdup(oobs_iface_get_device_name(OOBS_IFACE(iface)));
			setting_netExtIP      = g_strdup(oobs_iface_ethernet_get_ip_address(iface));
			setting_netExtMask    = g_strdup(oobs_iface_ethernet_get_network_mask(iface));
			setting_netExtGateway = g_strdup(oobs_iface_ethernet_get_gateway_address(iface));
			setting_netExtDNS1    = g_list_nth_data(nameservers, 0);
			setting_netExtDNS2    = g_list_nth_data(nameservers, 1);
			setting_netExtUseDhcp = (oobs_iface_ethernet_get_configuration_method(iface) == OOBS_METHOD_STATIC ? FALSE : TRUE);

			OobsIfaceEthernet *iface_o = get_internal_interface(interfaces);

			if (setting_netIfaceNo > 1 || iface_o) {
				setting_netIntName = g_strdup(oobs_iface_get_device_name(OOBS_IFACE(iface_o)));
			} else {
				setting_netIntName = g_strdup_printf("%s:0", setting_netExtName);
			}
		
			gdk_threads_enter();	
			displayStep2A();
			gdk_threads_leave();

		} else {
			iface = get_internal_interface(interfaces);
			setting_netExtName    = g_strdup(oobs_iface_get_device_name(OOBS_IFACE(iface)));
			setting_netExtIP      = g_strdup(oobs_iface_ethernet_get_ip_address(iface));
			setting_netExtMask    = g_strdup(oobs_iface_ethernet_get_network_mask(iface));
			setting_netExtGateway = g_strdup(oobs_iface_ethernet_get_gateway_address(iface));
			setting_netExtDNS1    = g_list_nth_data(nameservers, 0);
			setting_netExtDNS2    = g_list_nth_data(nameservers, 1);
			setting_netExtUseDhcp = (oobs_iface_ethernet_get_configuration_method(iface) == OOBS_METHOD_STATIC ? FALSE : TRUE);

			OobsIfaceEthernet *iface_o = get_internal_interface(interfaces);
			if (setting_netIfaceNo > 1 || iface_o) {
                                setting_netIntName = g_strdup(oobs_iface_get_device_name(OOBS_IFACE(iface_o)));
                        } else {
                                setting_netIntName = g_strdup_printf("%s:0", setting_netExtName);
                        }

			gdk_threads_enter();	
			displayStep2C();
			gdk_threads_leave();
	       }
	}

	return NULL;
}

void start_network_wizard(void) {
	pthread_t snetwiz_tid;
	int next_step = -1;

	pthread_create(&snetwiz_tid, NULL, start_network_wizard_thread, &next_step);

}

OobsIfaceEthernet* get_external_interface(OobsList *interfaces) {
	OobsListIter iter;
	gboolean valid;
	valid = oobs_list_get_iter_first(interfaces, &iter);

	while (valid) {
		OobsIfaceEthernet *iface = OOBS_IFACE_ETHERNET(oobs_list_get(interfaces, &iter));

		if (oobs_iface_get_configured(OOBS_IFACE(iface)) && oobs_iface_has_gateway(OOBS_IFACE(iface))) {

			const gchar *ip = oobs_iface_ethernet_get_ip_address(iface);

			if (connect_test(ip)) {
			       return iface;
			}
		}

		g_object_unref (iface);
		valid = oobs_list_iter_next(interfaces, &iter);
	}

	return NULL;
}

OobsIfaceEthernet* get_internal_interface(OobsList *interfaces) {
	OobsListIter iter;
	gboolean valid;
	valid = oobs_list_get_iter_first(interfaces, &iter);

	while (valid) {
		OobsIfaceEthernet *iface = OOBS_IFACE_ETHERNET(oobs_list_get(interfaces, &iter));
		if (g_ascii_strcasecmp(oobs_iface_get_device_name(OOBS_IFACE(iface)), setting_netExtName) != 0) {
		       return iface;
		}

		g_object_unref (iface);
		valid = oobs_list_iter_next(interfaces, &iter);
	}

	return NULL;

}

gboolean connect_test(const gchar* sourceIp) {
       int                sockfd;
       struct sockaddr_in serv_addr;
       struct hostent     *server;

       printf("Testing interface : %s\n", sourceIp);

       sockfd = socket(AF_INET, SOCK_STREAM, 0);
       if (sockfd < 0) {
               fprintf(stderr, "WARNING: error opening socket\n");
               return FALSE;
       }

       server = gethostbyname("www.google.com");
       if (server == NULL) {
               printf(" - can't find hostname www.google.com\n");
               return FALSE;
       }

       memset ((char*) &serv_addr, sizeof(serv_addr), 0);
       serv_addr.sin_family = AF_INET;
       serv_addr.sin_port = htons(80);
       bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

       if (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0) {
               printf(" - can't connect to host\n");
               return FALSE;
       }
	   close(sockfd);

       printf("- can connect to internet \n");
       return TRUE;
}
