#include "common.h"

void start_network_wizard(void) {
	OobsSession	 *oobs_session = oobs_session_get();
	OobsIfacesConfig *ifaces_config = OOBS_IFACES_CONFIG(oobs_ifaces_config_get(oobs_session));
	OobsList         *interfaces = oobs_ifaces_config_get_ifaces(ifaces_config ,OOBS_IFACE_TYPE_ETHERNET);

	// If we have 2 interfaces
	if (oobs_list_get_n_items(interfaces) == 2) {
		displayStep2A();
	} else if (oobs_list_get_n_items(interfaces) == 1) {
		displayStep2A();
	} else if (oobs_list_get_n_items(interfaces) == 0) {
		displayStep2A();
	}
}
