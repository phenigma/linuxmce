/* 
 * upd640xx - upd64031A / upd64083 common definition
 *
 * 2005 knabe.
 */

#ifndef _UPD640XX_H_
#define _UPD640XX_H_

/* control ID */
#define UPD640XX_SET_VALUE V4L2_CID_PRIVATE_BASE
//#define UPD640XX_CID_CARD_TYPE V4L2_CID_PRIVATE_BASE //tadachi

#define UPD640XX_INPUT_TYPE_MAX        3

// bit mask, etc
#define UPD640XX_SET_COMMAND_SHIFT    24
#define UPD640XX_SET_COMMAND_MASK     0xff000000
#define UPD640XX_SET_VALUE_MASK       0x00ffffff

// ********* upd64031a stuff ***************************
// ---------- set command --------------
#define UPD64031A_COMMAND_MAX          2
// ---------- commands -----------------
#define UPD64031A_DUMMY                0
#define UPD64031A_CHANGE_DEF_SET       1
#define UPD64031A_SET_PARAM1           2

// ********* for SET_PARAM1 ************
// GR MODE
#define UPD64031A_TUN_GRON             0x00000001
#define UPD64031A_TUN_GROFF            0x00000002
#define UPD64031A_TUN_GR_THROUGH       0x00000004
#define UPD64031A_LIN_GRON             0x00000100
#define UPD64031A_LIN_GROFF            0x00000200
#define UPD64031A_LIN_GR_THROUGH       0x00000400

// DIRECT 3DYCS CONNECTION
#define UPD64031A_TUN_3DYCS_DISABLE    0x00000008
#define UPD64031A_TUN_3DYCS_COMPOSITE  0x00000010
#define UPD64031A_TUN_3DYCS_SVIDEO     0x00000020
#define UPD64031A_LIN_3DYCS_DISABLE    0x00000800
#define UPD64031A_LIN_3DYCS_COMPOSITE  0x00001000
#define UPD64031A_LIN_3DYCS_SVIDEO     0x00002000

// COMPOSITE SYNC DIGITAL SPEAPARATION CIRCUIT
#define UPD64031A_COMPOSITE_EXTERNAL   0x00000040

// VERTICAL SYNC DIGITAL SPEAPARATION CIRCUIT
#define UPD64031A_VERTICAL_EXTERNAL    0x00008000

// *******************************************************

// ********* upd64083 stuff ***************************
// ---------- set command --------------
#define UPD64083_COMMAND_MAX          2
// ---------- commands -----------------
#define UPD64083_DUMMY                0
#define UPD64083_CHANGE_DEF_SET       1
#define UPD64083_SET_PARAM1           2

// ********* for SET_PARAM1 ************
// 
#define UPD64083_SET_TUN_YCSMODE             0x00010000
#define UPD64083_SET_COMP_YCSMODE            0x00020000
#define UPD64083_SET_SVID_YCSMODE            0x00040000

#define UPD64083_SET_TUN_YCSMODE_MASK        0x00000003
#define UPD64083_SET_COMP_YCSMODE_MASK       0x0000000c
#define UPD64083_SET_SVID_YCSMODE_MASK       0x00000030

#define UPD64083_TUN_USE_INT_Y_ADC           0x00000040
#define UPD64083_TUN_USE_EXT_Y_ADC           0x00000080
#define UPD64083_COMP_USE_INT_Y_ADC          0x00000100
#define UPD64083_COMP_USE_EXT_Y_ADC          0x00000200
#define UPD64083_SVID_USE_INT_Y_ADC          0x00000400
#define UPD64083_SVID_USE_EXT_Y_ADC          0x00000800
// *******************************************************

#endif

