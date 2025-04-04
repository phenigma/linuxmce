/*
$Id: ow_standard.h,v 1.4 2010/05/02 19:07:07 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
	email: palfille@earthlink.net
	Released under the GPL
	See the header file: ow.h for full attribution
	1wire/iButton system from Dallas Semiconductor
*/

#ifndef OW_STANDARD_H
#define OW_STANDARD_H

#ifndef OWFS_CONFIG_H
#error Please make sure owfs_config.h is included *before* this header file
#endif
#include "ow.h"
#include "ow_connection.h"
#include "ow_codes.h"

/* ------- Prototypes ----------- */
ZERO_OR_ERROR FS_type(struct one_wire_query *owq);
ZERO_OR_ERROR FS_alias(struct one_wire_query *owq);
ZERO_OR_ERROR FS_code(struct one_wire_query *owq);
ZERO_OR_ERROR FS_crc8(struct one_wire_query *owq);
ZERO_OR_ERROR FS_ID(struct one_wire_query *owq);
ZERO_OR_ERROR FS_r_ID(struct one_wire_query *owq);
ZERO_OR_ERROR FS_address(struct one_wire_query *owq);
ZERO_OR_ERROR FS_r_address(struct one_wire_query *owq);
ZERO_OR_ERROR FS_locator(struct one_wire_query *owq);
ZERO_OR_ERROR FS_r_locator(struct one_wire_query *owq);
ZERO_OR_ERROR FS_present(struct one_wire_query *owq);

/* ------- Structures ----------- */

#define F_address  \
{"address"   ,  PROPERTY_LENGTH_ADDRESS,  NON_AGGREGATE, ft_ascii , fc_static  , FS_address  , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_r_address  \
{"r_address" ,  PROPERTY_LENGTH_ADDRESS,  NON_AGGREGATE, ft_ascii , fc_static  , FS_r_address, NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_crc8     \
{"crc8"      ,   2,  NON_AGGREGATE, ft_ascii , fc_static  , FS_crc8     , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_id       \
{"id"        ,  12,  NON_AGGREGATE, ft_ascii , fc_static  , FS_ID       , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_r_id       \
{"r_id"      ,  12,  NON_AGGREGATE, ft_ascii , fc_static  , FS_r_ID     , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_code     \
{"family"    ,   2,  NON_AGGREGATE, ft_ascii , fc_static  , FS_code     , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_present  \
{"present"   ,   PROPERTY_LENGTH_YESNO,  NON_AGGREGATE, ft_yesno , fc_volatile, FS_present  , NO_WRITE_FUNCTION, VISIBILE_PRESENT, {v:NULL}, }
#define F_type     \
{"type"      ,  PROPERTY_LENGTH_TYPE,  NON_AGGREGATE, ft_vascii, fc_static  , FS_type     , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_alias     \
{"alias"      ,PROPERTY_LENGTH_ALIAS,  NON_AGGREGATE, ft_alias, fc_static   , FS_alias    , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_locator  \
{"locator"   ,  PROPERTY_LENGTH_ADDRESS,  NON_AGGREGATE, ft_ascii , fc_directory,FS_locator  , NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }
#define F_r_locator \
{"r_locator" ,  PROPERTY_LENGTH_ADDRESS,  NON_AGGREGATE, ft_ascii , fc_directory,FS_r_locator, NO_WRITE_FUNCTION, VISIBLE, {v:NULL}, }

#define F_STANDARD_NO_TYPE          F_address,F_code,F_crc8,F_id,F_locator,F_present,F_r_address,F_r_id,F_r_locator,F_alias

#define F_STANDARD F_STANDARD_NO_TYPE,F_type

#endif /* OW_STANDARD_H */
