/* $Id: ow_traffic.h,v 1.2 2010/07/23 00:43:36 alfille Exp $
   Debug and error messages
   Meant to be included in ow.h
   Separated just for readability
*/

/* OWFS source code
   1-wire filesystem for linux
   {c} 2006 Paul H Alfille
   License GPL2.0
*/

#ifndef OW_TRAFFIC_H
#define OW_TRAFFIC_H

/* Debugging interface to showing all bus traffic
 * You need to configure compile with
 * ./configure --enable-owtraffic
 * */

/* Show bus traffic in detail (must be configured into the build) */
#if OW_SHOW_TRAFFIC

void TrafficOut( const char * data_type, const BYTE * data, size_t length, const struct connection_in * in );
void TrafficIn( const char * data_type, const BYTE * data, size_t length, const struct connection_in * in );

void TrafficOutFD( const char * data_type, const BYTE * data, size_t length, FILE_DESCRIPTOR_OR_ERROR file_descriptor );
void TrafficInFD( const char * data_type, const BYTE * data, size_t length, FILE_DESCRIPTOR_OR_ERROR file_descriptor );

#else /* OW_SHOW_TRAFFIC */

#define TrafficOut( data_type, data, length, in ) { } while (0)
#define TrafficIn( data_type, data, length,  in ) { } while (0)

#define TrafficOutFD( data_type, data, length, file_descriptor ) { } while (0)
#define TrafficInFD( data_type, data, length,  file_descriptor ) { } while (0)

#endif /* OW_SHOW_TRAFFIC */

#endif							/* OW_TRAFFIC_H */
