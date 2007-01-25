 #include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
 
OobsIfaceEthernet* get_external_interface(OobsList *interfaces);
gboolean connect_test(const gchar* sourceIp);

 void start_network_wizard(void) {
        OobsSession      *oobs_session = oobs_session_get();
        OobsIfacesConfig *ifaces_config = OOBS_IFACES_CONFIG(oobs_ifaces_config_get(oobs_session));
        OobsList         *interfaces = oobs_ifaces_config_get_ifaces(ifaces_config ,OOBS_IFACE_TYPE_ETHERNET);
       OobsHostsConfig  *hosts_config = OOBS_HOSTS_CONFIG(oobs_hosts_config_get(oobs_session));
       GList            *nameservers = oobs_hosts_config_get_dns_servers(hosts_config);
 
       setting_netIfaceNo = 2; // oobs_list_get_n_items(interfaces);

       if (setting_netIfaceNo == 0) {
               //displayStep2X();
       } else {
               OobsIfaceEthernet* iface = get_external_interface(interfaces);
               if (iface != NULL) {
                       setting_netExtName    = g_strdup(oobs_iface_get_device_name(OOBS_IFACE(iface)));
                       setting_netExtIP      = g_strdup(oobs_iface_ethernet_get_ip_address(iface));
                       setting_netExtMask    = g_strdup(oobs_iface_ethernet_get_network_mask(iface));
                       setting_netExtGateway = g_strdup(oobs_iface_ethernet_get_gateway_address(iface));
                       setting_netExtDNS1 = g_list_nth_data(nameservers, 0);
                       setting_netExtDNS2 = g_list_nth_data(nameservers, 1);
                       displayStep2A();
               } else {
                       displayStep2C();
               }
        }
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

       printf("- can connect to internet \n");
       return TRUE;
}
