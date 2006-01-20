/* 
 * cx25840 - register map 
 *
 * This file is auto generated do not edit. 
 *
 * There are non obvoius relationships between values.
 *
 * Submit errors in this file to the ivtv development mail list.
 *
 */
#include <asm/types.h>
#include "cx25840-registers.h"

#define DEFINE_REGISTER_INFO_ENTRY(address, type) ((address&0x0fff)|(type<<12))

__u16 cx25840_register_info[] = {
	DEFINE_REGISTER_INFO_ENTRY(0x0000, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0001, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0100, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0101, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0102, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0103, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0104, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0105, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0106, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0107, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0108, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0109, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x010f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0110, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0111, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0112, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0113, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0114, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0115, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0116, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0117, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0118, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0119, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x011c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x011d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x011e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x011f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0120, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0121, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0122, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0123, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0124, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0125, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0126, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0127, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0128, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0129, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x012f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0130, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0131, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0134, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0135, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0136, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x013c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x013d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x013e, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x013f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0140, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0141, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0159, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x015f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0200, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0201, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0204, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0205, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0208, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0209, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x020c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0210, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0214, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0218, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0219, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x023c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x023d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x023e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0400, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0401, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0402, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0403, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0404, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0405, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0406, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0407, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0408, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0409, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x040a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x040b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x040c, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x040d, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x040e, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0410, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0411, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0412, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0413, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0414, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0415, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0416, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0418, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0419, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x041a, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x041b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x041c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x041d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x041e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x041f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0420, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0421, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0422, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0423, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0424, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0425, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0426, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0427, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0428, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0429, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x042f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0430, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0431, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0432, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0433, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0434, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0438, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0439, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x043f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0440, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0441, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0442, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0443, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0444, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0445, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0446, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0447, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0448, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0449, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x044a, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x044b, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x044c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x044d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x044e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x044f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0450, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0451, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0452, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0453, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0454, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0455, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0456, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0457, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0458, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0459, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x045f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0460, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0461, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0462, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0463, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0464, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0465, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0466, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0467, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0468, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0469, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x046f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0470, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0471, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0472, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0473, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0474, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0475, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0476, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0477, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0478, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0479, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x047f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0480, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0481, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0484, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0485, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0487, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0488, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0489, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x048a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x048b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x048c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x048d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x048e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0490, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0491, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0492, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0493, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0494, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0495, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0496, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0497, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0498, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x049c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x049d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x049e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x049f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04a0, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04a1, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04a2, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04a4, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04a5, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x04b4, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x04b8, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0800, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0801, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0802, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0803, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0804, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0805, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x0808, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0809, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x080a, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x080b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0810, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0812, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0813, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0814, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0815, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0816, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0817, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x081b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08cc, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08cd, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ce, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08cf, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d0, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d1, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d2, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d3, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d4, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d5, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d6, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d7, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d8, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08d9, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08da, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08db, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08dc, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08dd, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08de, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08df, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e0, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e1, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e2, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e3, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e4, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e5, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e6, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e7, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e8, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08e9, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ea, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08eb, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ec, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ed, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ee, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ef, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08f1, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08f2, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08f3, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x08f8, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08f9, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08fa, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08fb, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08fc, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08fd, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08fe, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x08ff, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0900, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0901, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0902, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0903, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0904, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0905, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0906, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0907, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0908, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0909, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x090f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0910, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0911, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0912, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0913, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0914, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0915, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0918, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0919, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x091c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x091d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x091e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x091f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0980, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0981, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0982, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0983, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0984, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0985, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0986, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0987, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0988, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0989, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098a, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098b, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x098f, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0990, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0991, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0992, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0994, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0995, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0998, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x0999, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x099c, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x099d, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x099e, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x09a0, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x09a1, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x09a2, RW),
	DEFINE_REGISTER_INFO_ENTRY(0x09a4, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09a5, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09a6, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09a7, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09c0, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09c1, RO),
	DEFINE_REGISTER_INFO_ENTRY(0x09c2, RO)
};

char *cx25840_register_name[] = {
	"Host_Register_1",
	"Host_Register_2",
	"Device_ID_Low_Byte",
	"Device_ID_High_Byte",
	"Configuration",
	"Video_Input_Control",
	"AFE_Control_1",
	"AFE_Control_2",
	"AFE_Control_3",
	"AFE_Control_4",
	"Video_PLL_Integer",
	"Video_PLL_Divider",
	"Aux_PLL_Integer",
	"Aux_PLL_Divider",
	"Video_PLL_Fractional_1",
	"Video_PLL_Fractional_2",
	"Video_PLL_Fractional_3",
	"Video_PLL_Fractional_4",
	"Aux_PLL_Fractional_1",
	"Aux_PLL_Fractional_2",
	"Aux_PLL_Fractional_3",
	"Aux_PLL_Fractional_4",
	"Pin_Control_1",
	"Pin_Control_2",
	"Pin_Control_3",
	"Pin_Control_4",
	"Pin_Control_5",
	"Pin_Control_6",
	"Pin_Configuration_1",
	"Pin_Configuration_2",
	"Pin_Configuration_3",
	"Pin_Configuration_4",
	"Pin_Configuration_5",
	"Pin_Configuration_6",
	"Pin_Configuration_7",
	"Pin_Configuration_8",
	"Pin_Configuration_9",
	"Pin_Configuration_10",
	"Pin_Configuration_11",
	"Pin_Configuration_12",
	"Video_Count_Low",
	"Video_Count_Mid",
	"Video_Count_High",
	"Audio_Lock",
	"Audio_Count_Low",
	"Audio_Count_Mid",
	"Audio_Lock_2",
	"Audio_Lock_3",
	"Power_Control_1",
	"Power_Control_2",
	"AFE_Diagnostic_Control_1",
	"AFE_Diagnostic_Control_2",
	"AFE_Diagnostic_Control_3",
	"AFE_Diagnostic_Control_5",
	"AFE_Diagnostic_Control_6",
	"AFE_Diagnostic_Control_7",
	"AFE_Diagnostic_Control_8",
	"PLL_Diagnostic_Control_1",
	"PLL_Diagnostic_Control_2",
	"DLL1_Diagnostic_Control_2",
	"DLL1_Diagnostic_Control_3",
	"DLL1_Diagnostic_Control_4",
	"DLL2_Diagnostic_Control_1",
	"DLL2_Diagnostic_Control_2",
	"DLL2_Diagnostic_Control_3",
	"DLL2_Diagnostic_Control_4",
	"IR_Control_1",
	"IR_Control_2",
	"IR_TX_Clock_Divider_Low",
	"IR_TX_Clock_Divider_High",
	"IR_RX_Clock_Divider_Low",
	"IR_RX_Clock_Divider_High",
	"IR_TX_Carrier_Duty_Cycle",
	"IR_Status",
	"IR_Interrupt_Enable",
	"IR_Low_Pass_Filter_Low",
	"IR_Low_Pass_Filter_High",
	"IR_FIFO_Low",
	"IR_FIFO_High",
	"IR_FIFO_Level",
	"Video_Mode_Control_1",
	"Video_Mode_Control_2",
	"Video_Mode_Control_3",
	"Video_Mode_Control_4",
	"Video_Out_Control_1",
	"Video_Out_Control_2",
	"Video_Out_Control_3",
	"Video_Out_Control_4",
	"Ancillary_IDID_0",
	"Ancillary_IDID_1",
	"Ancillary_IDID_0_1",
	"Ancillary_Audio_Output_Control",
	"Copy_Protection_Status",
	"General_Status_1",
	"General_Status_2",
	"Interrupt_Status_1",
	"Interrupt_Status_2",
	"Interrupt_Mask_1",
	"Interrupt_Mask_2",
	"Brightness",
	"Contrast",
	"Luma_Control",
	"Horizontal_Scaling_Low",
	"Horizontal_Scaling_Mid",
	"Horizontal_Scaling_High",
	"Horizontal_Scaling_Control",
	"Vertical_Scaling_Low",
	"Vertical_Scaling_High",
	"Vertical_Scaling_Control",
	"Vertical_Line_Control",
	"Saturation_U",
	"Saturation_V",
	"Hue",
	"Chroma_Control",
	"VBI_Line_Control_1",
	"VBI_Line_Control_2",
	"VBI_Line_Control_3",
	"VBI_Line_Control_4",
	"VBI_Line_Control_5",
	"VBI_Line_Control_6",
	"VBI_Line_Control_7",
	"VBI_Line_Control_8",
	"VBI_Line_Control_9",
	"VBI_Line_Control_10",
	"VBI_Line_Control_11",
	"VBI_Line_Control_12",
	"VBI_Line_Control_13",
	"VBI_Line_Control_14",
	"VBI_Line_Control_15",
	"VBI_Line_Control_16",
	"VBI_Line_Control_17",
	"VBI_Frame_Code_Search_Mode",
	"VBI_Alternate_Frame_Code_Type",
	"VBI_Alternate_1_Frame_Code",
	"VBI_Alternate_2_Frame_Code",
	"VBI_Miscellaneous_Config_1",
	"TTX_Packet_Address_1",
	"TTX_Packet_Address_2",
	"TTX_Packet_Address_3",
	"VBI_1_and_2_SDID",
	"VBI_3_SDID",
	"VBI_FIFO_Reset",
	"VBI_Hamming",
	"Closed_Caption_Status",
	"Closed_Caption_Data",
	"GEMSTAR_1x_Status",
	"GEMSTAR_1x_Data",
	"GEMSTAR_2x_Status",
	"GEMSTAR_2x_Data",
	"WSS_Status",
	"WSS_Data",
	"VBI_Custom_1_Horizontal_Delay",
	"VBI_Custom_1_Bit_Increment",
	"VBI_Custom_1_Slice_Distance",
	"VBI_Custom_1_Clock_Run_in_Window",
	"VBI_Custom_1_Frame_Code_Low",
	"VBI_Custom_1_Frame_Code_Mid",
	"VBI_Custom_1_Frame_Code_High",
	"VBI_Custom_1_Frame_Code_Length",
	"VBI_Custom_1_Clock_Run_in_Period",
	"VBI_Custom_1_Clock_Run_in_Margin_and_Length",
	"VBI_Custom_1_Payload_Length",
	"VBI_Custom_1_Miscellaneous",
	"VBI_Custom_2_Horizontal_Delay",
	"VBI_Custom_2_Bit_Increment",
	"VBI_Custom_2_Slice_Distance",
	"VBI_Custom_2_Clock_Run_In_Window",
	"VBI_Custom_2_Frame_Code_Low",
	"VBI_Custom_2_Frame_Code_Mid",
	"VBI_Custom_2_Frame_Code_High",
	"VBI_Custom_2_Frame_Code_Length",
	"VBI_Custom_2_Clock_Run_in_Period",
	"VBI_Custom_2_Clock_Run_in_Margin_and_Length",
	"VBI_Custom_2_Payload_Length",
	"VBI_Custom_2_Miscellaneous",
	"VBI_Custom_3_Horizontal_Delay",
	"VBI_Custom_3_Bit_Increment",
	"VBI_Custom_3_Slice_Distance",
	"VBI_Custom_3_Clock_Run_in_Window",
	"VBI_Custom_3_Frame_Code_Low",
	"VBI_Custom_3_Frame_Code_Mid",
	"VBI_Custom_3_Frame_Code_High",
	"VBI_Custom_3_Frame_Code_Length",
	"VBI_Custom_3_Clock_Run_in_Period",
	"VBI_Custom_3_Clock_Run_in_Margin_and_Length",
	"VBI_Custom_3_Payload_Length",
	"VBI_Custom_3_Miscellaneous",
	"Horizontal_Blanking_Delay_Low",
	"Horizontal_Blanking_Delay_High",
	"Horizontal_Active_High",
	"Burst_Gate_Delay",
	"Vertical_Blanking_Delay_Low",
	"Vertical_Blanking_Delay_High",
	"Vertical_Active_High",
	"Vertical_Blanking_Delay_656",
	"SRC_Decimation_Ratio_Low",
	"SRC_Decimation_Ratio_High",
	"Comb_Filter_Bandwidth_Select",
	"Comb_Filter_Enable",
	"Subcarrier_Step_Size_Low",
	"Subcarrier_Step_Size_Mid",
	"Subcarrier_Step_Size_High",
	"VBI_Offset",
	"Field_Count_Low",
	"Field_Count_High",
	"Temporal_Decimation",
	"Miscellaneous_Timing_Control",
	"Video_Detect_Configuration",
	"VGA_Gain_Control",
	"AGC_Gain_Control_Low",
	"AGC_Gain_Control_High",
	"Digital_Front_End_Control",
	"VGA_Sync_Control",
	"VGA_Track_Range",
	"VGA_Acquire_Range",
	"DFE_Control",
	"Backporch_Loop_Gain",
	"DFT_Threshold",
	"Backporch_Percent",
	"PLL_Offset_Low",
	"PLL_Offset_High",
	"PLL_Indirect_Loop_Gain",
	"PLL_Direct_Loop_Gain",
	"Horizontal_Tracking_Loop_Indirect_Gain",
	"Luma_Comb_Error_Limit_Max",
	"Luma_Comb_Threshold",
	"Chroma_Comb_Error_Limit_Max",
	"Comb_Phase_Limit_Max",
	"White_Crush_Increment",
	"White_Crush_Decrement",
	"White_Crush_Comparison_Point",
	"Soft_Reset_Mask_1",
	"Soft_Reset_Mask_2",
	"Version_ID",
	"Miscellaneous_Diagnostic_Control",
	"Download_Address_Low_Byte",
	"Download_Address_High_Byte",
	"Download_Data_Control",
	"Download_Control",
	"Mode_Detect_Status_0",
	"Mode_Detect_Status_1",
	"Audio_Configuration",
	"Preferred_Decode_Mode",
	"Embedded_Microcontroller_Video_Format",
	"Audio_Format_Control_0",
	"Soft_Reset",
	"Audio_Interrupt_Control",
	"Audio_Interrupt_Status",
	"Analog_AGC_Hysteresis1",
	"Analog_AGC_Hysteresis2",
	"Analog_AGC_Control",
	"AFE_12DB_ENABLE",
	"IF_SRC_Mode",
	"Phase_Fix_Control",
	"Dematrix_Bypass",
	"Dematrix_Select_Control",
	"Digital_Audio_Input_Shifter",
	"Path1_Audio_Select_Control",
	"Path1_AVC_Control1",
	"Path1_AVC_Control2",
	"Mute_Control",
	"Path1_Volume",
	"Path1_Balance",
	"Path1_AVC_Threshold_Low",
	"Path1_AVC_Threshold_High",
	"Path1_EQ_Band_Select",
	"Path1_EQ_Bass_Adjust",
	"Path1_EQ_Midrange_Adjust",
	"Path1_EQ_Treble_Adjust",
	"Path1_Soft_Clip_Control1",
	"Path1_Soft_Clip_Control2",
	"Path1_Soft_Clip_Threshold_Low",
	"Path1_Soft_Clip_Threshold_High",
	"Path2_Audio_Select_Control",
	"Path2_AVC_Control1",
	"Path2_AVC_Control2",
	"Path2_Mute_Control",
	"Path2_Volume",
	"Path2_Balance",
	"Path2_AVC_Threshold_Low",
	"Path2_AVC_Threshold_High",
	"Path2_EQ_Band_Select",
	"Path2_EQ_Bass_Adjust",
	"Path2_EQ_Midrange_Adjust",
	"Path2_EQ_Treble_Adjust",
	"Path2_Soft_Clip_Control1",
	"Path2_Soft_Clip_Control2",
	"Path2_Soft_Clip_Threshold_Low",
	"Path2_Soft_Clip_Threshold_High",
	"Sample_Rate_Converter_Status1",
	"Sample_Rate_Converter_Status2",
	"Sample_Rate_Converter_Status3",
	"SRC1_Phase_Increment_Low",
	"SRC1_Phase_Increment_Mid",
	"SRC1_Phase_Increment_High",
	"SRC1_FIFO_Read_Threshold",
	"SRC2_Phase_Increment_Low",
	"SRC2_Phase_Increment_Mid",
	"SRC2_Phase_Increment_High",
	"SRC2_FIFO_Read_Threshold",
	"SRC3_Phase_Increment_Low",
	"SRC3_Phase_Increment_Mid",
	"SRC3_Phase_Increment_High",
	"SRC3_FIFO_Read_Threshold",
	"SRC4_Phase_Increment_Low",
	"SRC4_Phase_Increment_Mid",
	"SRC4_Phase_Increment_High",
	"SRC4_FIFO_Read_Threshold",
	"SRC5_Phase_Increment_Low",
	"SRC5_Phase_Increment_Mid",
	"SRC5_Phase_Increment_High",
	"SRC5_FIFO_Read_Threshold",
	"SRC6_Phase_Increment_Low",
	"SRC6_Phase_Increment_Mid",
	"SRC6_Phase_Increment_High",
	"SRC6_FIFO_Read_Threshold",
	"Output_SRC_Source_Select",
	"Baseband_Bypass_Control",
	"Input_SRC_Source_Select1",
	"Input_SRC_Source_Select2",
	"Serial_Audio_Input_Control1",
	"Serial_Audio_Input_Control2",
	"Serial_Audio_Output_Control1",
	"Serial_Audio_Output_Control2",
	"AC97_Shutdown",
	"AC97_Wake_Up",
	"AC97_Reset",
	"AC97_Upsample_Bypass",
	"ACL_Control_0",
	"Space_Holder_Dummy",
	"Space_Holder_Dummy_UNIQ1",
	"Space_Holder_Dummy_UNIQ2",
	"ACL_Control_1",
	"Space_Holder_Dummy_UNIQ3",
	"Space_Holder_Dummy_UNIQ4",
	"Space_Holder_Dummy_UNIQ5",
	"ACL_Control_2",
	"Space_Holder_Dummy_UNIQ6",
	"Space_Holder_Dummy_UNIQ7",
	"Space_Holder_Dummy_UNIQ8",
	"ACL_Control_3",
	"Space_Holder_Dummy_UNIQ9",
	"Space_Holder_Dummy_UNIQ10",
	"Space_Holder_Dummy_UNIQ11",
	"ACL_Command",
	"Space_Holder_Dummy_UNIQ12",
	"Space_Holder_Dummy_UNIQ13",
	"ACL_GPIO_Output_Data",
	"Space_Holder_Dummy_UNIQ14",
	"ACL_GPIO_Input_Data",
	"Space_Holder_Dummy_UNIQ15",
	"ACL_Status_Slot_Interface",
	"Space_Holder_Dummy_UNIQ16",
	"Space_Holder_Dummy_UNIQ17",
	"ACL_Warm_Reset",
	"Space_Holder_Dummy_UNIQ18",
	"Space_Holder_Dummy_UNIQ19",
	"ACL_Tag",
	"Space_Holder_Dummy_UNIQ20",
	"Space_Holder_Dummy_UNIQ21",
	"Space_Holder_Dummy_UNIQ22",
	"RDS_I_Low",
	"RDS_I_High",
	"RDS_Q_Low"
};

__u8 cx25840_register_default[] = {
	0x04,
	0x00,
	0x00,
	0x84,
	0x00,
	0x00,
	0x00,
	0xdc,
	0x04,
	0x00,
	0x0f,
	0x04,
	0x0a,
	0x10,
	0xfe,
	0xe2,
	0x2b,
	0x00,
	0x09,
	0xd7,
	0x98,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x01,
	0x10,
	0x89,
	0x38,
	0x00,
	0x00,
	0xf8,
	0x93,
	0x11,
	0xa0,
	0xff,
	0x5f,
	0x00,
	0x11,
	0x00,
	0x00,
	0x02,
	0x18,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x04,
	0x00,
	0x21,
	0xe3,
	0x00,
	0x00,
	0x21,
	0xe3,
	0x00,
	0x00,
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xf0,
	0x08,
	0x00,
	0x31,
	0x25,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
	0x00,
	0xff,
	0x07,
	0x00,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x80,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x06,
	0x00,
	0x70,
	0xff,
	0x29,
	0x03,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x70,
	0x99,
	0x10,
	0x2c,
	0x01,
	0x00,
	0x00,
	0x03,
	0x0d,
	0xc4,
	0x08,
	0x06,
	0x77,
	0x88,
	0x00,
	0x2c,
	0x00,
	0x00,
	0x00,
	0x00,
	0x02,
	0x14,
	0x0a,
	0x14,
	0x6e,
	0xca,
	0x36,
	0x06,
	0xe7,
	0x00,
	0x00,
	0x08,
	0x20,
	0xf6,
	0x84,
	0x02,
	0x7a,
	0x00,
	0x00,
	0x5b,
	0x14,
	0xe0,
	0x1e,
	0x20,
	0x1f,
	0x02,
	0x50,
	0x66,
	0x1f,
	0x7c,
	0x08,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x40,
	0x20,
	0x00,
	0x01,
	0xf8,
	0xdc,
	0x40,
	0x10,
	0x8a,
	0x02,
	0x3f,
	0xcd,
	0x00,
	0x03,
	0x1f,
	0x16,
	0x22,
	0x14,
	0x00,
	0x50,
	0x20,
	0x0f,
	0x00,
	0x0c,
	0x00,
	0x00,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xff,
	0x00,
	0x05,
	0x09,
	0x0e,
	0x20,
	0x00,
	0x09,
	0x00,
	0x00,
	0x00,
	0x90,
	0x98,
	0x08,
	0x01,
	0x24,
	0x00,
	0xff,
	0x7f,
	0x00,
	0x00,
	0x00,
	0x00,
	0x09,
	0x99,
	0xff,
	0x7f,
	0x90,
	0x98,
	0x08,
	0x01,
	0x24,
	0x00,
	0x00,
	0x04,
	0x00,
	0x00,
	0x00,
	0x00,
	0x09,
	0x99,
	0xff,
	0x7f,
	0x00,
	0x00,
	0x00,
	0x7c,
	0x86,
	0x01,
	0x08,
	0x7c,
	0x86,
	0x01,
	0x08,
	0xaa,
	0x4f,
	0x01,
	0x08,
	0xaa,
	0x4f,
	0x01,
	0x08,
	0x53,
	0x04,
	0x01,
	0x08,
	0xaa,
	0x4f,
	0x01,
	0x08,
	0xc9,
	0x00,
	0xb0,
	0x12,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x3f,
	0x00,
	0x3f,
	0x00,
	0x3f,
	0x00,
	0x3f,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};

value_map SLEEP_value_map[] = {
	{0x0000, "Do not power down"},
	{0x0001, "Power down"},
	{0, 0}
};
value_map DIGITAL_PWR_DN_value_map[] = {
	{0x0000, "Do not gate"},
	{0x0001, "Hold clocks in high state"},
	{0, 0}
};
value_map REF_CLK_SEL_value_map[] = {
	{0x0000, "Single-ended input on XTI pin"},
	{0x0001, "Differential signal required on crystal pins"},
	{0, 0}
};
value_map PWR_DN_VID_PLL_value_map[] = {
	{0x0000, "Do not power down"},
	{0x0001, "Power down"},
	{0, 0}
};
value_map PWR_DN_AUX_PLL_value_map[] = {
	{0x0000, "Do not power down"},
	{0x0001, "Power down"},
	{0, 0}
};
value_map AUTO_INC_DIS_value_map[] = {
	{0x0000, "Do the auto-address increment"},
	{0x0001, "Do not increment the address"},
	{0, 0}
};
value_map SLV_SI_DIS_value_map[] = {
	{0x0000, "Glitch filters and slew rate control enabled"},
	{0, 0}
};
value_map FORCE_CHIP_SEL_VIPCLK_value_map[] = {
	{0x0000, "Use the CHIP_SEL/VIPCLK pin for I2C decode"},
	{0x0001, "The I2C chip select signal is forced to a 1 regardless of the"},
	{0, 0}
};
value_map PREFETCH_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map DEVICE_ID_value_map[] = {
	{0x8400, "CX25840-21"},
	{0x8401, "CX25840-22"},
	{0x8403, "CX25840-23"},
	{0x8404, "CX25840-24"},
	{0x8410, "CX25841-21"},
	{0x8411, "CX25841-22"},
	{0x8413, "CX25841-23"},
	{0x8414, "CX25841-24"},
	{0x8420, "CX25842-21"},
	{0x8421, "CX25842-22"},
	{0x8423, "CX25842-23"},
	{0x8424, "CX25842-24"},
	{0x8430, "CX25843-21"},
	{0x8431, "CX25843-22"},
	{0x8433, "CX25843-23"},
	{0x8434, "CX25843-24"},
	{0, 0}
};
value_map SOFT_RST_value_map[] = {
	{0x0000, "Deassert reset"},
	{0x0001, "Assert reset"},
	{0, 0}
};
value_map CH_SEL_ADC2_value_map[] = {
	{0x0000, "Analog input CH{2}"},
	{0x0001, "Analog input CH{3}"},
	{0, 0}
};
value_map DUAL_MODE_ADC2_value_map[] = {
	{0x0000, "Normal mode"},
	{0x0001, "Dual sampling mode"},
	{0, 0}
};
value_map CHIP_ACFG_DIS_value_map[] = {
	{0x0000, "Allow VID_PLL_INT, VID_PLL_FRAC, and AFE control"},
	{0x0001, "Disable autoconfig"},
	{0, 0}
};
value_map CH_1__SOURCE_value_map[] = {
	{0x0000, "CVBS1/Y1"},
	{0x0001, "CVBS2/Y2"},
	{0x0002, "CVBS3/Y3"},
	{0x0003, "CVBS4/Y4/Pb1"},
	{0x0004, "CVBS5/C1/Pb2"},
	{0x0005, "CVBS6/C2"},
	{0x0006, "CVBS7/C3/Pr1"},
	{0x0007, "CVBS8/C4/Pr2"},
	{0, 0}
};
value_map CH_2__SOURCE_value_map[] = {
	{0x0000, "CVBS4/Y4/Pb1"},
	{0x0001, "CVBS5/C1/Pb2"},
	{0x0002, "CVBS6/C2"},
	{0x0003, "None"},
	{0, 0}
};
value_map CH_3__SOURCE_value_map[] = {
	{0x0000, "CVBS7/C3/Pr1"},
	{0x0001, "CVBS8/C4/Pr2"},
	{0x0002, "None"},
	{0x0003, "None"},
	{0, 0}
};
value_map EN_12DB_CH1_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map EN_12DB_CH2_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map EN_12DB_CH3_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map VGA_SEL_CH1_value_map[] = {
	{0x0000, "Video decoder drives VGA gain setting"},
	{0x0001, "Audio decoder drives VGA gain setting"},
	{0, 0}
};
value_map VGA_SEL_CH2_value_map[] = {
	{0x0000, "Video decoder drives VGA gain setting"},
	{0x0001, "Audio decoder drives VGA gain setting"},
	{0, 0}
};
value_map VGA_SEL_CH3_value_map[] = {
	{0x0000, "Video decoder drives VGA gain setting"},
	{0x0001, "Audio decoder drives VGA gain setting"},
	{0, 0}
};
value_map CLAMP_SEL_CH1_value_map[] = {
	{0x0000, "Video decoder drives clamp level"},
	{0x0001, "Clamp level is fixed at 3'b111 (midcode clamp). Use for"},
	{0, 0}
};
value_map CLAMP_SEL_CH2_value_map[] = {
	{0x0000, "Video decoder drives clamp level"},
	{0x0001, "Clamp level is fixed at 3'b111 (midcode clamp). Use for"},
	{0, 0}
};
value_map CLAMP_SEL_CH3_value_map[] = {
	{0x0000, "Video decoder drives clamp level"},
	{0x0001, "Clamp level is fixed at 3'b111 (midcode clamp). Use for"},
	{0, 0}
};
value_map CHROMA_IN_SEL_value_map[] = {
	{0x0000, "ADC 1"},
	{0x0001, "ADC 2"},
	{0, 0}
};
value_map LUMA_IN_SEL_value_map[] = {
	{0x0000, "ADC 1"},
	{0x0001, "ADC 2"},
	{0, 0}
};
value_map AUD_IN_SEL_value_map[] = {
	{0x0000, "ADC1 input"},
	{0x0001, "Miscellaneous Chip Control input"},
	{0, 0}
};
value_map CLAMP_EN_CH1_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable (power-up)"},
	{0, 0}
};
value_map CLAMP_EN_CH2_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CLAMP_EN_CH3_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map GPIO0_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map GPIO1_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CHIP_SEL_VIPCLK_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map IRQN_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map IR_RX_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map IR_TX_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map DVALID_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map FIELD_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map HRESET_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map VRESET_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map VID_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map PIXCLK_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_SDIN_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_BCLKIN_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_WCLKIN_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_SDOUT_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_BCLKOUT_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_WCLKOUT_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map PLL_CLK_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AC_OUT_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AC_IN_OUT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map IRQ_N_PRGM4_value_map[] = {
	{0x0000, "IRQ_N/PRGM4 is active low"},
	{0x0001, "IRQ_N/PRGM4 is active high"},
	{0, 0}
};
value_map VID_IRQ_STAT_value_map[] = {
	{0x0000, "Interrupt is not active"},
	{0x0001, "Interrupt is active"},
	{0, 0}
};
value_map IR_IRQ_STAT_value_map[] = {
	{0x0000, "Interrupt is not active"},
	{0x0001, "Interrupt is active"},
	{0, 0}
};
value_map VID_OUT_SPD_value_map[] = {
	{0x0000, "Medium"},
	{0x0001, "Slow"},
	{0, 0}
};
value_map VID_CTRL_SPD_value_map[] = {
	{0x0000, "Medium"},
	{0x0001, "Slow"},
	{0, 0}
};
value_map AC_OUT_SPD_value_map[] = {
	{0x0000, "Medium"},
	{0x0001, "Slow"},
	{0, 0}
};
value_map SA_OUT_SPD_value_map[] = {
	{0x0000, "Medium"},
	{0x0001, "Slow"},
	{0, 0}
};
value_map GEN_OUT_SPD_value_map[] = {
	{0x0000, "Medium"},
	{0x0001, "Slow"},
	{0, 0}
};
value_map GPIO0_OUT_SEL_value_map[] = {
	{0x0000, "GPO[0]"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map GPIO1_OUT_SEL_value_map[] = {
	{0x0000, "GPO[1]"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map CHIPSEL_OUT_SEL_value_map[] = {
	{0x0000, "GPO[0]"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map IRQN_OUT_SEL_value_map[] = {
	{0x0000, "IRQ_N/PRGM4"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map IR_RX_OUT_SEL_value_map[] = {
	{0x0000, "GPO[2]"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map IR_TX_OUT_SEL_value_map[] = {
	{0x0000, "IR_TX/PRGM6"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map DVALID_PRGM0_OUT_SEL_value_map[] = {
	{0x0000, "DVALID"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map FIELD_PRGM1_OUT_SEL_value_map[] = {
	{0x0000, "FIELD"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map HRESET_PRGM2_OUT_SEL_value_map[] = {
	{0x0000, "HRESET"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map VRESET_HCTL_PRGM3_OUT_SEL_value_map[] = {
	{0x0000, "VRESET/HCTL/PRGM3"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map AC_OUT_SEL_value_map[] = {
	{0x0000, "GPO[3]. AC_SYNC, AC_SDOUT"},
	{0, 0}
};
value_map SA_OUT_OUT_SEL_value_map[] = {
	{0x0000, "SA_WCLK_OUT, SA_BCLK_OUT, GPO[2]"},
	{0x0001, "AC_SYNC, AC_SDOUT, GPO[3]"},
	{0, 0}
};
value_map SA_IN_OUT_SEL_value_map[] = {
	{0x0000, "SA_WCLK_OUT, SA_BCLK_OUT, SA_SDOUT"},
	{0x0001, "AC_SYNC, AC_SDOUT, GPO[3]"},
	{0, 0}
};
value_map AUX_PLL_AOUT_SEL_value_map[] = {
	{0x0001, "Auxiliary PLL"},
	{0x0065, "Video PLL"},
	{0, 0}
};
value_map AUX_PLL_DOUT_SEL_value_map[] = {
	{0x0000, "PLL_CLK/PRGM7"},
	{0x0001, "ACTIVE"},
	{0x0008, "VACTIVE"},
	{0x0009, "CBFLAG"},
	{0x0040, "VID_DATA_EXT[0]"},
	{0x0041, "VID_DATA_EXT[1]"},
	{0x0048, "GPO[0]"},
	{0x0049, "GPO[1]"},
	{0x03e8, "GPO[2]"},
	{0x03e9, "GPO[3]"},
	{0x03f2, "IRQ_N/PRGM4"},
	{0x03f3, "AC_SYNC"},
	{0x044c, "AC_SDOUT"},
	{0x044d, "PLL_CLK/PRGM7"},
	{0x0456, "VRESET/HCTL/PRGM3"},
	{0, 0}
};
value_map AC_BITCLK_IN_SEL_value_map[] = {
	{0x0001, "DVALID/PRGM0"},
	{0, 0}
};
value_map AC_SDIN_IN_SEL_value_map[] = {
	{0x0000, "ground"},
	{0x0001, "DVALID/PRGM0"},
	{0, 0}
};
value_map GPI0_IN_SEL_value_map[] = {
	{0x0000, "DVALID/PRGM0"},
	{0x0001, "FIELD/PRGM1"},
	{0x0008, "HRESET/PRGM2"},
	{0x0009, "VRESET/HCTL/PRGM3"},
	{0x0040, "IRQ_N/PRGM4"},
	{0x0041, "IR_TX/PRGM6"},
	{0x0048, "IR_RX/PRGM5"},
	{0x0049, "GPI[0]"},
	{0x03e8, "GPI[1]"},
	{0x03e9, "SA_SDIN"},
	{0x03f2, "SA_SDOUT"},
	{0x03f3, "PLL_CLK/PRGM7"},
	{0, 0}
};
value_map GPI1_IN_SEL_value_map[] = {
	{0x0000, "DVALID/PRGM0"},
	{0x0001, "FIELD/PRGM1"},
	{0x0008, "HRESET/PRGM2"},
	{0x0009, "VRESET/HCTL/PRGM3"},
	{0x0040, "IRQ_N/PRGM4"},
	{0x0041, "IR_TX/PRGM6"},
	{0x0048, "IR_RX/PRGM5"},
	{0x0049, "GPIO[0]/PRGM8"},
	{0x03e8, "GPIO[1]/PRGM9"},
	{0x03e9, "SA_SDIN"},
	{0x03f2, "SA_SDOUT"},
	{0x03f3, "PLL_CLK/PRGM7"},
	{0, 0}
};
value_map GPI2_IN_SEL_value_map[] = {
	{0x0000, "DVALID/PRGM0"},
	{0x0001, "FIELD/PRGM1"},
	{0x0008, "HRESET/PRGM2"},
	{0x0009, "VRESET/HCTL/PRGM3"},
	{0x0040, "IRQ_N/PRGM4"},
	{0x0041, "IR_TX/PRGM6"},
	{0x0048, "IR_RX/PRGM5"},
	{0x0049, "GPIO[0]/PRGM8"},
	{0x03e8, "GPIO[1]/PRGM9"},
	{0x03e9, "SA_SDIN"},
	{0x03f2, "SA_SDOUT"},
	{0x03f3, "PLL_CLK/PRGM7"},
	{0, 0}
};
value_map GPI3_IN_SEL_value_map[] = {
	{0x0000, "DVALID/PRGM0"},
	{0x0001, "FIELD/PRGM1"},
	{0x0008, "HRESET/PRGM2"},
	{0x0009, "VRESET/HCTL/PRGM3"},
	{0x0040, "IRQ_N/PRGM4"},
	{0x0041, "IR_TX/PRGM6"},
	{0x0048, "IR_RX/PRGM5"},
	{0x0049, "GPIO[0]/PRGM8"},
	{0x03e8, "GPIO[1]/PRGM9"},
	{0x03e9, "SA_SDIN"},
	{0x03f2, "SA_SDOUT"},
	{0x03f3, "PLL_CLK/PRGM7"},
	{0, 0}
};
value_map SA_MCLK_SEL_value_map[] = {
	{0x0000, "Use AUX_PLL_POST value to derive SA_MCLK."},
	{0x0001, "PLL_CLK/PRGM7 is divided by the alternate post-divider"},
	{0, 0}
};
value_map PWR_DN_TUNING_value_map[] = {
	{0x0000, "Do not power down filter tuning"},
	{0x0001, "Power down filter tuning"},
	{0, 0}
};
value_map PWR_DN_DLL1_value_map[] = {
	{0x0000, "Do not power down DLL1"},
	{0x0001, "Power down DLL1"},
	{0, 0}
};
value_map PWR_DN_DLL2_value_map[] = {
	{0x0000, "Do not power down DLL2"},
	{0x0001, "Power down DLL2"},
	{0, 0}
};
value_map PWR_DN_ADC1_value_map[] = {
	{0x0000, "Do not power down ADC1"},
	{0x0001, "Power down ACD1"},
	{0, 0}
};
value_map PWR_DN_ADC2_value_map[] = {
	{0x0000, "Do not power down Miscellaneous Chip Control"},
	{0x0001, "Power down ACD2"},
	{0, 0}
};
value_map PWR_DN_CH1_value_map[] = {
	{0x0000, "Do not power down Channel 1"},
	{0x0001, "Power down Channel 1"},
	{0, 0}
};
value_map PWR_DN_CH2_value_map[] = {
	{0x0000, "Do not power down Channel 2"},
	{0x0001, "Power down Channel 2"},
	{0, 0}
};
value_map PWR_DN_CH3_value_map[] = {
	{0x0000, "Do not power down Channel 3"},
	{0x0001, "Power down Channel 3"},
	{0, 0}
};
value_map SLEEP_ANALOG_MSK_value_map[] = {
	{0x0000, "SLEEP powers down analog subsystem"},
	{0x0001, "Power down of analog subsystem inhibited including DLL"},
	{0, 0}
};
value_map SLEEP_DLL_MSK_value_map[] = {
	{0x0000, "SLEEP powers down DLL (5x clock) circuitry"},
	{0x0001, "Power down inhibited"},
	{0, 0}
};
value_map SLEEP_PLL_MSK_value_map[] = {
	{0x0000, "SLEEP powers down PLLs"},
	{0x0001, "Power down of PLL inhibited"},
	{0, 0}
};
value_map VCLK_GATE_MSK_value_map[] = {
	{0x0000, "DIG_PWR_ON disables high-speed video clock (VCLK)"},
	{0x0001, "Gating of VCLK inhibited"},
	{0, 0}
};
value_map CLK5x_GATE_MSK_value_map[] = {
	{0x0000, "DIG_PWR_ON disables high-speed audio clock (CLK5x)"},
	{0x0001, "Gating of CLK5x inhibited"},
	{0, 0}
};
value_map SCLK_GATE_MSK_value_map[] = {
	{0x0000, "DIG_PWR_ON disables sample clock (SCLK) and SA master"},
	{0x0001, "Gating of SCLK and SA_MCLK inhibited"},
	{0, 0}
};
value_map FOUR_X_CLK_ADC_value_map[] = {
	{0x0000, "5x output"},
	{0x0001, "4x output"},
	{0, 0}
};
value_map VREG_D_1_0__value_map[] = {
	{0x0000, "1.20 V"},
	{0x0001, "1.26 V"},
	{0x000a, "1.32 V"},
	{0x000b, "1.38 V"},
	{0, 0}
};
value_map VREF_CTRL_ADC_value_map[] = {
	{0x0000, "1.60 V"},
	{0x0001, "1.20 V"},
	{0, 0}
};
value_map BIAS_CTRL_ADC_1_0__value_map[] = {
	{0x0000, "50 microA"},
	{0x0001, "37.5 microA"},
	{0x000a, "62.5 microA"},
	{0x000b, "75 microA"},
	{0, 0}
};
value_map BIAS_CTRL_ADC_2__value_map[] = {
	{0x0000, "1.6 V, 0.8 V"},
	{0x0001, "1.5 V, 0.9 V"},
	{0, 0}
};
value_map BIAS_CTRL_ADC_4_3__value_map[] = {
	{0x0000, "12.5 microA"},
	{0x0001, "6.25 microA"},
	{0x000a, "25 microA"},
	{0x000b, "50 microA"},
	{0, 0}
};
value_map BIAS_CTRL_ADC_6_5__value_map[] = {
	{0x0000, "50 microA"},
	{0x0001, "62.5 microA"},
	{0x000a, "37.5 microA"},
	{0x000b, "75 microA"},
	{0, 0}
};
value_map S2DIFF_BIAS_1_0__value_map[] = {
	{0x0000, "0.75"},
	{0x0001, "1"},
	{0x000a, "1.25"},
	{0x000b, "1.5"},
	{0, 0}
};
value_map FILTER_BIAS_1_0__value_map[] = {
	{0x0000, "0.5"},
	{0x0001, "1"},
	{0x000a, "1.5"},
	{0x000b, "2"},
	{0, 0}
};
value_map CH_SEL_ADC1_value_map[] = {
	{0x0000, "Connects ADC1 to output of filter."},
	{0x0001, "Connects ADC1 to the output of first VGA stage."},
	{0, 0}
};
value_map DISCONNECT_CH1_value_map[] = {
	{0x0000, "Enable filter"},
	{0x0001, "Disable filter"},
	{0, 0}
};
value_map TEST_MODE_CH1_value_map[] = {
	{0x0000, "Do not connect"},
	{0x0001, "Connect"},
	{0, 0}
};
value_map FORCE_TUNING_value_map[] = {
	{0x0000, "Do not override"},
	{0x0001, "Forces tuning code to value contained in TUNE_IN field."},
	{0, 0}
};
value_map TUNING_READY_value_map[] = {
	{0x0000, "Not complete"},
	{0x0001, "Filter tuning complete"},
	{0, 0}
};
value_map VID_DUAL_FLAG_POL_value_map[] = {
	{0x0000, "Noninverted polarity"},
	{0x0001, "Inverted polarity"},
	{0, 0}
};
value_map AUD_DUAL_FLAG_POL_value_map[] = {
	{0x0000, "Noninverted polarity"},
	{0x0001, "Inverted polarity"},
	{0, 0}
};
value_map VID_PLL_DDS_value_map[] = {
	{0x0000, "Enable"},
	{0x0001, "Disable"},
	{0, 0}
};
value_map AUX_PLL_DDS_value_map[] = {
	{0x0000, "Enable"},
	{0x0001, "Disable"},
	{0, 0}
};
value_map VID_PLL_RST_value_map[] = {
	{0x0000, "Do not reset"},
	{0x0001, "Reset the auxiliary PLL"},
	{0, 0}
};
value_map AUX_PLL_RST_value_map[] = {
	{0x0000, "Do not reset"},
	{0x0001, "Reset the auxiliary PLL"},
	{0, 0}
};
value_map AUX_PLL_LOCK_value_map[] = {
	{0x0000, "Unlocked"},
	{0x0001, "Locked"},
	{0, 0}
};
value_map VID_PLL_LOCK_value_map[] = {
	{0x0000, "Unlocked"},
	{0x0001, "Locked"},
	{0, 0}
};
value_map AUX_PLL_UNLOCK_value_map[] = {
	{0x0000, "No unlock detected"},
	{0x0001, "Unlock detected"},
	{0, 0}
};
value_map VID_PLL_UNLOCK_value_map[] = {
	{0x0000, "No unlock detected"},
	{0x0001, "Unlock detected"},
	{0, 0}
};
value_map WIN_value_map[] = {
	{0x0000, "Next carrier edge predicted to be 16 RX clocks 3/+3"},
	{0x0001, "Next carrier edge predicted to be 16 RX clocks 4/+3"},
	{0x000a, "Next carrier edge predicted to be 16 RX clocks 3/+4"},
	{0x000b, "Next carrier edge predicted to be 16 RX clocks 4/+4"},
	{0, 0}
};
value_map EDG_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Falling edges trigger RX filter/pulse timer start/stop"},
	{0x000a, "Rising edges trigger RX filter/pulse timer start/stop"},
	{0x000b, "Either edge trigger RX filter/pulse timer start/stop"},
	{0, 0}
};
value_map DMD_value_map[] = {
	{0x0000, "Disable receive carrier demodulation, receive data as simple logic"},
	{0x0001, "Enable receive carrier demodulation, detect a mark as a series of"},
	{0, 0}
};
value_map MOD_value_map[] = {
	{0x0000, "Disable transmit carrier modulation, transmit data as simple logic"},
	{0x0001, "Enable transmit carrier modulation, transmit a mark as a burst of"},
	{0, 0}
};
value_map RFE_value_map[] = {
	{0x0000, "Disable and reset receive FIFO to all 0s"},
	{0x0001, "Enable receive FIFO"},
	{0, 0}
};
value_map TFE_value_map[] = {
	{0x0000, "Disable and reset transmit FIFO to all 0s"},
	{0x0001, "Enable transmit FIFO"},
	{0, 0}
};
value_map RXE_value_map[] = {
	{0x0000, "Disable receiver"},
	{0x0001, "Enable receiver"},
	{0, 0}
};
value_map TXE_value_map[] = {
	{0x0000, "Disable transmitter"},
	{0x0001, "Enable transmitter"},
	{0, 0}
};
value_map RIC_value_map[] = {
	{0x0000, "Receive FIFO service interrupt/DMA request asserted when RX"},
	{0x0001, "Receive FIFO service interrupt/DMA request asserted when RX"},
	{0, 0}
};
value_map TIC_value_map[] = {
	{0x0000, "Transmit FIFO service interrupt/DMA request asserted when TX"},
	{0x0001, "Transmit FIFO service interrupt/DMA request asserted after"},
	{0, 0}
};
value_map CPL_value_map[] = {
	{0x0000, "If mod/demodulation enabled, 1s are transmitted and received as"},
	{0x0001, "If mod/demodulation enabled, 1s transmitted and received as the"},
	{0, 0}
};
value_map LBM_value_map[] = {
	{0x0000, "Transmit and receive operation functions normally through the IR"},
	{0x0001, "The output of the transmit modulation logic is fed into the input of"},
	{0, 0}
};
value_map R_value_map[] = {
	{0x0000, "Load Rx FIFO with 1s and Rx_data_lh level on timer overflow;"},
	{0x0001, "Do not load Rx FIFO on timer overflow."},
	{0, 0}
};
value_map CDC_value_map[] = {
	{0x0000, "1 TX clock high and 15 TX clocks low"},
	{0x0001, "2 TX clocks high and 14 TX clocks low"},
	{0x0008, "3 TX clocks high and 13 TX clocks low"},
	{0x044d, "14 TX clocks high and 2 TX clocks low"},
	{0x0456, "15 TX clocks high and 1 TX clock low"},
	{0x0457, "16 TX clocks high and 0 TX clocks low"},
	{0, 0}
};
value_map RTO_value_map[] = {
	{0x0000, "Receive pulse width counter has not reached its limit"},
	{0x0001, "Receive pulse width counter has reached its limit (contains all 1s),"},
	{0, 0}
};
value_map ROR_value_map[] = {
	{0x0000, "Receive FIFO contains one or more empty entries or is full but has"},
	{0x0001, "Receive FIFO has experienced an overrun, generate an irq request if"},
	{0, 0}
};
value_map TBY_value_map[] = {
	{0x0000, "Receiver is idle"},
	{0x0000, "Transmitter is idle"},
	{0x0001, "Receiver is busy"},
	{0x0001, "Transmitter is busy"},
	{0, 0}
};
value_map RSR_value_map[] = {
	{0x0000, "If RIC=0 in IR_CNTL_REG, receive FIFO is less than half full. If"},
	{0x0001, "IF RIC=0, receive FIFO is half full or more. If RIC=1, receive FIFO is"},
	{0, 0}
};
value_map TSR_value_map[] = {
	{0x0000, "If TIC=0 in IR_CNTL_REG, transmit FIFO is more than half full. If"},
	{0x0001, "IF TIC=0, transmit FIFO is half full or less."},
	{0, 0}
};
value_map ROE_value_map[] = {
	{0x0000, "Receive FIFO overrun interrupt disabled"},
	{0x0000, "Receive pulse width timer time-out interrupt disabled"},
	{0x0001, "Receive FIFO overrun interrupt enabled"},
	{0x0001, "Receive pulse width timer time-out interrupt enabled"},
	{0, 0}
};
value_map RSE_value_map[] = {
	{0x0000, "Receive FIFO interrupt disabled"},
	{0x0001, "Receive FIFO interrupt enabled"},
	{0, 0}
};
value_map TSE_value_map[] = {
	{0x0000, "Transmit FIFO/Idle interrupt disabled"},
	{0x0001, "Transmit FIFO/Idle interrupt enabled"},
	{0, 0}
};
value_map RXNDV_value_map[] = {
	{0x0000, "No more data in the RX FIFO"},
	{0x0001, "One or more entries of valid data remain in RX FIFO"},
	{0, 0}
};
value_map VID_FMT_SEL_value_map[] = {
	{0x0000, "AUTO-DETECT"},
	{0x0001, "NTSC-M"},
	{0x0008, "NTSC-J"},
	{0x0009, "NTSC-4.43"},
	{0x0040, "PAL-BDGHI"},
	{0x0041, "PAL-M"},
	{0x0048, "PAL-N"},
	{0x0049, "PAL-NC"},
	{0x03e8, "PAL-60"},
	{0x044c, "SECAM"},
	{0, 0}
};
value_map ACFG_DIS_value_map[] = {
	{0x0000, "Enable"},
	{0x0001, "Disable"},
	{0, 0}
};
value_map AFD_PAL_SEL_value_map[] = {
	{0x0000, "PAL-BDGHI"},
	{0x0001, "PAL-N"},
	{0, 0}
};
value_map AFD_NTSC_SEL_value_map[] = {
	{0x0000, "NTSC-M"},
	{0x0001, "NTSC-J"},
	{0, 0}
};
value_map AFD_ACQUIRE_value_map[] = {
	{0x0000, "The auto-detect state machine operates normally."},
	{0x0001, "The auto-detect state machine soft reset."},
	{0, 0}
};
value_map INPUT_MODE_value_map[] = {
	{0x0000, "CVBS"},
	{0x0001, "Y/C"},
	{0x000a, "Y/half-rate C"},
	{0x000b, "Y/Pb/Pr"},
	{0, 0}
};
value_map MAN_SC_FAST_LOCK_value_map[] = {
	{0x0001, "Fast lock speed"},
	{0, 0}
};
value_map AUTO_SC_LOCK_value_map[] = {
	{0x0000, "Manual mode. Lock speed is determined by"},
	{0x0001, "Auto Mode"},
	{0, 0}
};
value_map CKILLEN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CAGCEN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map COMB_NOTCH_MODE_value_map[] = {
	{0x0000, "Disable notch filter. Comb only"},
	{0x0001, "Notch data is interpreted as chroma"},
	{0, 0}
};
value_map CKILL_MODE_value_map[] = {
	{0x0000, "Chroma output is forced to 0, and luma output is generated"},
	{0x0001, "Chroma output is forced to 0, entire chroma band is notched"},
	{0, 0}
};
value_map AFD_PALM_SEL_value_map[] = {
	{0x0000, "NTSC will be detected according to the AFD_NTSC_SEL bit."},
	{0x0001, "PAL-M format is chosen when 525 lines and a 3.58 carrier"},
	{0x000a, "Enable algorithm to dynamically detect between NTSC-M"},
	{0, 0}
};
value_map AFD_FORCE_PAL_value_map[] = {
	{0x0000, "The auto-detect algorithm proceeds normally."},
	{0x0001, "PAL-BG/PAL-N is chosen when a 625-line format is detected."},
	{0, 0}
};
value_map AFD_FORCE_PALNC_value_map[] = {
	{0x0000, "The auto-detect algorithm proceeds normally."},
	{0x0001, "PAL-Nc is chosen when a 625-line format is detected"},
	{0, 0}
};
value_map AFD_FORCE_SECAM_value_map[] = {
	{0x0000, "The auto-detect algorithm proceeds normally."},
	{0x0001, "SECAM is chosen when a 625-line format is detected."},
	{0, 0}
};
value_map AFD_PAL60_DIS_value_map[] = {
	{0x0000, "PAL-60 can be detected and discriminate from NTSC-4.43"},
	{0x0001, "Any 525-line 4.43 format is assumed to be NTSC-4.43."},
	{0, 0}
};
value_map OUT_MODE_value_map[] = {
	{0x0000, "ITU-R BT.601 coded video Synchronous Pixel Interface"},
	{0x0001, "ITU-R BT.656 control codes"},
	{0x000a, "VIP 1.1 control codes"},
	{0x000b, "VIP 2 control codes"},
	{0, 0}
};
value_map MODE10B_value_map[] = {
	{0x0000, "Luma and Chroma Output are rounded to 8 bits"},
	{0x0001, "Luma and Chroma Output have 10 bits of resolution"},
	{0, 0}
};
value_map VBIHACTRAW_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map ANC_DATA_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map BLUE_FIELD_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CLAMPRAW_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SWAPRAW_value_map[] = {
	{0x0000, "Even samples on chroma; odd samples on luma"},
	{0x0001, "Odd samples on chroma; even samples on luma"},
	{0, 0}
};
value_map ACTFMT_value_map[] = {
	{0x0000, "Active is composite active"},
	{0x0001, "Active is horizontal active"},
	{0, 0}
};
value_map VALIDFMT_value_map[] = {
	{0x0000, "Valid indicates nonscaled pixels"},
	{0x0001, "Valid is logical AND of nominal VALID and ACTIVE, where"},
	{0, 0}
};
value_map HSFMT_value_map[] = {
	{0x0000, "Nominal width"},
	{0x0001, "One pixel clock (VOF_PIXCLK) pulse wide"},
	{0, 0}
};
value_map CLK_GATING_value_map[] = {
	{0x000a, "Gate with VALID output"},
	{0x000b, "Gate with logical AND of VALID and ACTIVE outputs"},
	{0, 0}
};
value_map DCMODE_value_map[] = {
	{0x0000, "Data Count is number of blocks of 4 UDWs, with padding"},
	{0x0001, "Data Count is number UDWs"},
	{0, 0}
};
value_map IDID0_SOURCE_value_map[] = {
	{0x0000, "IDID0 register"},
	{0x0001, "Line Count from VBI Slicer"},
	{0, 0}
};
value_map VIP_OPT_AL_value_map[] = {
	{0x0000, "VBLANK"},
	{0x0001, "V656BLANK"},
	{0, 0}
};
value_map VIPBLANK_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map VIPCLAMP_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AUD_ANC_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SAMPLE_RATE_value_map[] = {
	{0x0000, "48 kHz"},
	{0x0001, "44.1 kHz"},
	{0x000a, "32 kHz"},
	{0x000b, "Reserved"},
	{0, 0}
};
value_map AUD_GRP_value_map[] = {
	{0x0000, "Audio Group1"},
	{0x0001, "Audio Group2"},
	{0x000a, "Audio Group3"},
	{0x000b, "Audio Group4"},
	{0, 0}
};
value_map AFD_FMT_STAT_value_map[] = {
	{0x0001, "NTSC-M"},
	{0x0002, "NTSC-J"},
	{0x0003, "NTSC-4.43"},
	{0x0004, "PAL-BDGHI"},
	{0x0005, "PAL-M"},
	{0x0006, "PAL-N"},
	{0x0007, "PAL-NC"},
	{0x0008, "PAL-60"},
	{0x000c, "SECAM"},
	{0, 0}
};
value_map PEAK_SEL_value_map[] = {
	{0x0000, "+2.0 dB response @ center freq"},
	{0x0001, "+3.5 dB response @ center freq"},
	{0x000a, "+5.0 dB response @ center freq"},
	{0x000b, "+6.0 dB response @ center freq"},
	{0, 0}
};
value_map PEAK_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map RANGE_value_map[] = {
	{0x0000, "Range: 641016 Nominal 656 range with excursions"},
	{0x0001, "Range: 41016 Nominal 656 range with excursions allowed"},
	{0, 0}
};
value_map LUMA_CORE_SEL_value_map[] = {
	{0x0000, "No coring"},
	{0x0001, "Coring threshold set to + 16"},
	{0x000a, "Coring threshold set to + 32"},
	{0x000b, "Coring threshold set to + 64"},
	{0, 0}
};
value_map HFILT_value_map[] = {
	{0x0000, "Auto Mode"},
	{0x0001, "CIF"},
	{0x000a, "QCIF"},
	{0x000b, "ICON"},
	{0, 0}
};
value_map VFILT_value_map[] = {
	{0x0000, "2-tap interpolation (available at all resolutions)"},
	{0x0001, "3-tap interpolation (available if scaling to less than 385"},
	{0x0008, "4-tap interpolation (available if scaling to less than 193"},
	{0x0009, "5-tap interpolation (available if scaling to less than 193"},
	{0, 0}
};
value_map VS_INTRLACE_value_map[] = {
	{0x0000, "Noninterlace VS"},
	{0x0001, "Interlace VS"},
	{0, 0}
};
value_map LINE_AVG_DIS_value_map[] = {
	{0x0000, "PAL line averaging is enabled. Adjacent lines are averaged"},
	{0x0001, "PAL line averaging is disabled."},
	{0, 0}
};
value_map C_CORE_SEL_value_map[] = {
	{0x0000, "No coring"},
	{0x0001, "+7"},
	{0x000a, "+15"},
	{0x000b, "+31"},
	{0, 0}
};
value_map CHR_DELAY_value_map[] = {
	{0x0000, "0"},
	{0x0001, "+1"},
	{0x0008, "+2"},
	{0x006e, "2"},
	{0x006f, "1"},
	{0, 0}
};
value_map VBI_MD_LINE1_value_map[] = {
	{0x0000, "No VBI data slicing"},
	{0x0000, "No VBI data slicing"},
	{0x0001, "WST525-B"},
	{0x0001, "WST625-B"},
	{0x0008, "WST525-C (NABTS)"},
	{0x0008, "WST625-A"},
	{0x0009, "WST525-D (Moji)"},
	{0x0040, "WSS525"},
	{0x0040, "WSS625"},
	{0x0041, "VITC525"},
	{0x0041, "VITC625"},
	{0x0048, "CC525"},
	{0x0066, "CC525"},
	{0x0048, "CC625"},
	{0x0049, "Gemstar 1x"},
	{0x0049, "VPS"},
	{0x03e8, "Gemstar 2x"},
	{0x03e9, "Custom VBI1"},
	{0x03e9, "Custom VBI1"},
	{0x03f2, "Custom VBI2"},
	{0x03f2, "Custom VBI2"},
	{0x03f3, "Custom VBI3"},
	{0x03f3, "Custom VBI3"},
	{0, 0}
};
value_map FC_SEARCH_MODE_value_map[] = {
	{0x0000, "Frame code match is declared only if frame code (start code,"},
	{0x0001, "Frame code match is declared upon discovering the first bit"},
	{0, 0}
};
value_map ADAPT_SLICE_DIS_value_map[] = {
	{0x0000, "Slice level comes from averaging points in the clock run-in."},
	{0x0001, "Slice level is set to predetermined level based on mode."},
	{0, 0}
};
value_map EDGE_RESYNC_EN_value_map[] = {
	{0x0000, "Sample point timing is determined by initial edge"},
	{0x0001, "Sample point timing is resynchronized upon detecting any"},
	{0, 0}
};
value_map CRI_MARG_SCALE_value_map[] = {
	{0x0000, "Divide default timing margin by 2"},
	{0x0001, "Use default timing margin"},
	{0x000a, "Multiply default timing margin by 2"},
	{0x000b, "Multiply default timing margin by 4"},
	{0, 0}
};
value_map VPS_DEC_DIS_value_map[] = {
	{0x0000, "VPS formats are decoded based on a two-bit biphase pattern."},
	{0x0001, "Raw slice bits are transmitted"},
	{0, 0}
};
value_map MOJI_PACK_DIS_value_map[] = {
	{0x0000, "WST525, system D formats cause the decoder to extract the"},
	{0x0001, "The WST 525, system D bit stream is packed into byte"},
	{0, 0}
};
value_map VBI1_FORMAT_TYPE_value_map[] = {
	{0x0009, "Enable Moji style byte alignment: First six bits go into"},
	{0x0040, "Wide-screen signaling: Assume signal includes single"},
	{0x0040, "Wide-screen signaling: Implement WSS625 style"},
	{0x0041, "VITC: Assume sync pulses every 8 bits"},
	{0x03e9, "VPS: Implement VPS style biphase decoding"},
	{0, 0}
};
value_map VBI1_FIFO_MODE_value_map[] = {
	{0x0000, "Do not load to payload FIFO"},
	{0x0001, "Load to Gemstar 1x FIFO"},
	{0x0008, "Load to CC FIFO"},
	{0x0009, "Load to WSS FIFO"},
	{0x0064, "Load to Gemstar 2x FIFO"},
	{0, 0}
};
value_map VBI2_FORMAT_TYPE_value_map[] = {
	{0x0009, "Enable Moji style byte alignment: First six bits go into"},
	{0x0040, "Wide-screen signaling: Assume signal includes single"},
	{0x0040, "Wide-screen signaling: Implement WSS625 style"},
	{0x0041, "VITC: Assume sync pulses every 8 bits"},
	{0x03e9, "VPS: Implement VPS style biphase decoding"},
	{0, 0}
};
value_map VBI2_FIFO_MODE_value_map[] = {
	{0x0000, "Do not load to payload FIFO"},
	{0x0001, "Load to Gemstar 1x FIFO"},
	{0x0008, "Load to CC FIFO"},
	{0x0009, "Load to WSS FIFO"},
	{0x0064, "Load to Gemstar 2x FIFO"},
	{0, 0}
};
value_map VBI3_FORMAT_TYPE_value_map[] = {
	{0x0009, "Enable Moji style byte alignment: First six bits go into"},
	{0x0040, "Wide-screen signaling: Assume signal includes single"},
	{0x0040, "Wide-screen signaling: Implement WSS625 style"},
	{0x0041, "VITC: Assume sync pulses every 8 bits"},
	{0x03e9, "VPS: Implement VPS-style biphase decoding"},
	{0, 0}
};
value_map VBI3_FIFO_MODE_value_map[] = {
	{0x0000, "Do not load to payload FIFO"},
	{0x0001, "Load to Gemstar 1x FIFO"},
	{0x0008, "Load to CC FIFO"},
	{0x0009, "Load to WSS FIFO"},
	{0x0064, "Load to Gemstar 2x FIFO"},
	{0, 0}
};
value_map UV_LPF_SEL_value_map[] = {
	{0x0000, "Low (~600 kHz)"},
	{0x0001, "Medium (~1 MHz)"},
	{0x000a, "High (~1.5 MHz)"},
	{0, 0}
};
value_map LUMA_LPF_SEL_value_map[] = {
	{0x0000, "Low (~600 kHz)"},
	{0x0001, "Medium (~1 MHz)"},
	{0x000a, "High (~1.5 MHz)"},
	{0, 0}
};
value_map LCOMB_2LN_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map LCOMB_3LN_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CCOMB_2LN_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CCOMB_3LN_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map TDALGN_value_map[] = {
	{0x0000, "Start on odd field"},
	{0x0001, "Start on even field"},
	{0, 0}
};
value_map HR32_value_map[] = {
	{0x0000, "HRESET is 64 clocks wide"},
	{0x0001, "HRESET is 32 clocks wide"},
	{0, 0}
};
value_map VPRES_VERT_EN_value_map[] = {
	{0x0000, "VPRES reflects the horizontal locking only."},
	{0x0001, "VPRES status bit reflects when the video is both locked"},
	{0, 0}
};
value_map VT_LINE_CNT_HYST_value_map[] = {
	{0x0000, "2 consecutive fields with approximately 525/2 or 625/2"},
	{0x0001, "4 consecutive fields with approximately 525/2 or 625/2"},
	{0x000a, "8 consecutive fields with approximately 525/2 or 625/2"},
	{0x000b, "16 consecutive fields with approximately 525/2 or 625/2"},
	{0, 0}
};
value_map DEBOUNCE_COUNT_value_map[] = {
	{0x0000, "2 consecutive fields of stability"},
	{0x0001, "4 consecutive fields of stability"},
	{0x000a, "8 consecutive fields of stability"},
	{0x000b, "16 consecutive fields of stability"},
	{0, 0}
};
value_map VGA_AUTO_EN_value_map[] = {
	{0x0000, "Freeze/manual"},
	{0x0001, "Auto mode"},
	{0, 0}
};
value_map AGC_AUTO_EN_value_map[] = {
	{0x0000, "Freeze/manual"},
	{0x0001, "Auto mode"},
	{0, 0}
};
value_map CLAMP_AUTO_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map MAJ_SEL_value_map[] = {
	{0x0000, " maximum luma"},
	{0x0001, " maximum luma"},
	{0x000a, " maximum luma"},
	{0x000b, "Automatic"},
	{0, 0}
};
value_map MAJ_SEL_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map CRUSH_FREQ_value_map[] = {
	{0x0000, "Field rate"},
	{0x0001, "Frame rate"},
	{0, 0}
};
value_map WTW_EN_value_map[] = {
	{0x0000, "100 IRE peak threshold"},
	{0x0001, "110 IRD peak threshold"},
	{0, 0}
};
value_map APL_DETECT_ENA_value_map[] = {
	{0x0000, "This special case is not detected. The white line will be"},
	{0x0001, "Forces the Y/C separation algorithm into notch mode"},
	{0, 0}
};
value_map DL_AUTO_INC_value_map[] = {
	{0x0000, "Auto increment address on write"},
	{0x0001, "Auto increment address on read"},
	{0, 0}
};
value_map DL_ENABLE_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map START_MICROCNTL_value_map[] = {
	{0x0001, "Embedded microcontroller is running"},
	{0, 0}
};
value_map PREF_MODE_value_map[] = {
	{0x0000, "Mono/Language A: If mono, play mono; if dual or NICAM"},
	{0x0001, "Language B: If dual or NICAM TRIVOX, play language B; if"},
	{0, 0}
};
value_map MUTE_NO_PREF_MODE_value_map[] = {
	{0x0001, "Mute output if preferred mode is not available."},
	{0, 0}
};
value_map DE_EMPHASIS_TIME_value_map[] = {
	{0x0001, "50 micro seconds"},
	{0, 0}
};
value_map FM_DEVIATION_value_map[] = {
	{0x0000, "Normal FM"},
	{0x0001, "High deviation FMup to 360 kHz"},
	{0x000a, "Very high deviation FMup to 540 kHz"},
	{0, 0}
};
value_map FORMAT_45MHZ_value_map[] = {
	{0x0000, "Chroma"},
	{0x0001, "BTSC"},
	{0x0008, "EIAJ"},
	{0x0009, "A2-M"},
	{0x0064, "Auto-detect"},
	{0, 0}
};
value_map FORMAT_65MHZ_value_map[] = {
	{0x0000, "System DK"},
	{0x0001, "System L"},
	{0x000a, "Auto-detect"},
	{0, 0}
};
value_map TUNER_OUTPUT_FORMAT_value_map[] = {
	{0x0000, "2nd IF"},
	{0x0001, "AF"},
	{0, 0}
};
value_map AC97_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map AMC_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map FC_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map FDL_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map IFL_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map NLL_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map NBER_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map RDS_INT_DIS_value_map[] = {
	{0x0001, "Disable"},
	{0, 0}
};
value_map AAGC_GAIN_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AAGC_DEFAULT_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AFE_12DB_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map IF_SRC_MODE_value_map[] = {
	{0x0000, "TV audio input"},
	{0x0001, "FM radio input"},
	{0, 0}
};
value_map PH_CH_SEL_value_map[] = {
	{0x0000, "Channel 1"},
	{0x0001, "Channel 2"},
	{0, 0}
};
value_map DEMATRIX_MODE_value_map[] = {
	{0x0001, "Dematrix Sum/R"},
	{0, 0}
};
value_map DMTRX_BYPASS_value_map[] = {
	{0x0000, "System decide dematrix mode"},
	{0x0001, "Force Dematrix block use mode set above"},
	{0, 0}
};
value_map PATH1_AVC_CR_value_map[] = {
	{0x0000, "2:1"},
	{0x0001, "4:1"},
	{0x0008, "8:1"},
	{0x0009, "16:1"},
	{0x0064, "32:1"},
	{0, 0}
};
value_map PATH1_AVC_STEREO_value_map[] = {
	{0x0001, "Stereo Mode"},
	{0, 0}
};
value_map PATH1_AVC_AT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH1_AVC_RT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1s"},
	{0x0048, "2s"},
	{0x0049, "4s"},
	{0x03e8, "8s"},
	{0, 0}
};
value_map PATH1_AVC_CG_value_map[] = {
	{0x0000, "1"},
	{0x0001, "1.5"},
	{0x000a, "2"},
	{0x000b, "4"},
	{0, 0}
};
value_map SOFT1_MUTE_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SRC1_MUTE_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map SA_MUTE_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map PAR_MUTE_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map AC97_MUTE_EN_value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "Enable"},
	{0, 0}
};
value_map PATH1_EQ_BAND_SEL_value_map[] = {
	{0x0000, "500 Hz, 1 kHz, 2 kHz"},
	{0x0001, "450 Hz, 1.5 kHz, 5 kHz"},
	{0, 0}
};
value_map PATH1_SC_RMS_CON_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x0457, "8 s"},
	{0, 0}
};
value_map PATH1_SC_CR_value_map[] = {
	{0x0000, "2:1"},
	{0x0001, "4:1"},
	{0x0008, "8:1"},
	{0x0009, "16:1"},
	{0x0064, "32:1"},
	{0, 0}
};
value_map PATH1_SC_STEREO_value_map[] = {
	{0x0001, "Stereo Mode"},
	{0, 0}
};
value_map PATH1_SC_AT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH1_SC_RT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH2_AVC_CR_value_map[] = {
	{0x0000, "2:1"},
	{0x0001, "4:1"},
	{0x0008, "8:1"},
	{0x0009, "16:1"},
	{0x0064, "32:1"},
	{0, 0}
};
value_map PATH2_AVC_STEREO_value_map[] = {
	{0x0001, "Stereo Mode"},
	{0, 0}
};
value_map PATH2_AVC_AT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH2_AVC_RT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH2_AVC_CG_value_map[] = {
	{0x0000, "1"},
	{0x0001, "1.5"},
	{0x000a, "2"},
	{0x000b, "4"},
	{0, 0}
};
value_map PATH2_EQ_BAND_SEL_value_map[] = {
	{0x0000, "500 Hz, 1 kHz, 2 kHz"},
	{0x0001, "450 Hz, 1.5 kHz, 5 kHz"},
	{0, 0}
};
value_map PATH2_SC_RMS_CON_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x0457, "8 s"},
	{0, 0}
};
value_map PATH2_SC_CR_value_map[] = {
	{0x0000, "2:1"},
	{0x0001, "4:1"},
	{0x0008, "8:1"},
	{0x0009, "16:1"},
	{0x0064, "32:1"},
	{0, 0}
};
value_map PATH2_SC_STEREO_value_map[] = {
	{0x0001, "Stereo Mode"},
	{0, 0}
};
value_map PATH2_SC_AT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PATH2_SC_RT_value_map[] = {
	{0x0000, "0"},
	{0x0001, "10 ms"},
	{0x0008, "20 ms"},
	{0x0009, "100 ms"},
	{0x0040, "500 ms"},
	{0x0041, "1 s"},
	{0x0048, "2 s"},
	{0x0049, "4 s"},
	{0x03e8, "8 s"},
	{0, 0}
};
value_map PARALLEL1_SRC_SEL_value_map[] = {
	{0x0000, "Select from SRC 3"},
	{0x0001, "Select from SRC 4"},
	{0x000a, "Select from SRC 5"},
	{0x000b, "Select from SRC 6"},
	{0, 0}
};
value_map PARALLEL2_SRC_SEL_value_map[] = {
	{0x0000, "Select from SRC 3"},
	{0x0001, "Select from SRC 4"},
	{0x000a, "Select from SRC 5"},
	{0x000b, "Select from SRC 6"},
	{0, 0}
};
value_map SERIAL_AUDIO_SRC_SEL_value_map[] = {
	{0x0000, "Select from SRC 3"},
	{0x0001, "Select from SRC 4"},
	{0x000a, "Select from SRC 5"},
	{0x000b, "Select from SRC 6"},
	{0, 0}
};
value_map AC97_SRC_SEL_value_map[] = {
	{0x0000, "Select from SRC 3"},
	{0x0001, "Select from SRC 4"},
	{0x000a, "Select from SRC 5"},
	{0x000b, "Select from SRC 6"},
	{0, 0}
};
value_map BASEBAND_BYPASS_CTL_value_map[] = {
	{0x0000, "Normal operation"},
	{0x0000, "Path1 automatic volume control"},
	{0x0001, "Path1 equalizer"},
	{0x0001, "The block is bypassed"},
	{0, 0}
};
value_map SRC3_CLK_SEL_value_map[] = {
	{0x0000, "Select from serial audio WS"},
	{0x0001, "Select from AC97 RD (48 kHz)"},
	{0x000a, "Select from NICAM ENABLE (64 kHz)"},
	{0x000b, "Select from SRC3 FIFO write"},
	{0, 0}
};
value_map SRC3_IN_SEL_value_map[] = {
	{0x0000, "Select from path 1 output"},
	{0x0001, "Select from path 2 output"},
	{0x000a, "Select from dematrix stereo"},
	{0x000b, "Select from mono output"},
	{0, 0}
};
value_map SRC4_CLK_SEL_value_map[] = {
	{0x0000, "Select from serial audio WS"},
	{0x0001, "Select from AC97 RD (48 kHz)"},
	{0x000a, "Select from NICAM ENABLE (64 kHz)"},
	{0x000b, "Select from SRC4 FIFO write"},
	{0, 0}
};
value_map SRC4_IN_SEL_value_map[] = {
	{0x0000, "Select from path 1 output"},
	{0x0001, "Select from path 2 output"},
	{0x000a, "Select from dematrix stereo"},
	{0x000b, "Select from mono output"},
	{0, 0}
};
value_map SRC5_CLK_SEL_value_map[] = {
	{0x0000, "Select from serial audio WS"},
	{0x0001, "Select from AC97 RD (48 kHz)"},
	{0x000a, "Select from NICAM ENABLE (64 kHz)"},
	{0x000b, "Select from SRC5 FIFO write"},
	{0, 0}
};
value_map SRC5_IN_SEL_value_map[] = {
	{0x0000, "Select from analog demod outa, outc"},
	{0x0001, "Select from analog demod RDS output"},
	{0x000a, "Select from dematrix stereo"},
	{0x000b, "Select from mono output"},
	{0, 0}
};
value_map SRC6_CLK_SEL_value_map[] = {
	{0x0000, "Select from serial audio WS"},
	{0x0001, "Select from AC97 RD (48 kHz)"},
	{0x000a, "Select from NICAM ENABLE (64 kHz)"},
	{0x000b, "Select from SRC6 FIFO write"},
	{0, 0}
};
value_map SRC6_IN_SEL_value_map[] = {
	{0x0000, "Select from path 1 output"},
	{0x0001, "Select from path 2 output"},
	{0x000a, "Select from dematrix stereo"},
	{0x000b, "Select from mono output"},
	{0, 0}
};
value_map SA_IN_WS_SEL_value_map[] = {
	{0x0000, "Left sample on SA_WCLK = 0"},
	{0x0001, "Left sample on SA_WCLK = 1"},
	{0, 0}
};
value_map SA_IN_RIGHT_JUST_value_map[] = {
	{0x0001, "Right justified serial data"},
	{0, 0}
};
value_map SA_IN_SONY_MODE_value_map[] = {
	{0x0001, "Sony mode: 1st SA_BCLK_IN rising edge after"},
	{0, 0}
};
value_map SA_IN_MASTER_MODE_value_map[] = {
	{0x0001, "Master: SA_BCLK_IN and SA_WCLK_IN are"},
	{0, 0}
};
value_map SA_OUT_WS_SEL_value_map[] = {
	{0x0000, "Left sample on SA_WCLK = 0"},
	{0x0001, "Left sample on SA_WCLK = 1"},
	{0, 0}
};
value_map SA_OUT_RIGHT_JUST_value_map[] = {
	{0x0001, "Right justified serial data"},
	{0, 0}
};
value_map SA_OUT_SONY_MODE_value_map[] = {
	{0x0001, "Sony mode: 1st SA_BCLK_OUT rising edge after"},
	{0, 0}
};
value_map SA_OUT_MASTER_MODE_value_map[] = {
	{0x0001, "Master: SA_BCLK_OUT, and SA_WCLK_OUT are"},
	{0, 0}
};
value_map AC97_UP2X_BYPASS_value_map[] = {
	{0x0001, "Up sample by 2 block is bypassed. Use for input"},
	{0, 0}
};
value_map ACLMSTR_BIT1_value_map[] = {
	{0x0000, "AC_LINK interface disabled"},
	{0x0001, "AC_LINK interface enabled"},
	{0, 0}
};
value_map ACLMSTR_4TO2_3_2__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "interrupt on any valid GPIO input slot"},
	{0x000a, "interrupt on valid GPIO input slot if data changed"},
	{0, 0}
};
value_map ACLMSTR_4TO2_4__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "interrupt after transfer"},
	{0, 0}
};
value_map ACLMSTR_12TO8_8___value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "interrupt on any valid stat slot"},
	{0x000a, "interrupt on valid status slot if data changed"},
	{0, 0}
};
value_map ACLMSTR_12TO8_9__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "interrupt after transfer"},
	{0, 0}
};
value_map ACLMSTR_12TO8_10__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "interrupt at tag arrival"},
	{0, 0}
};
value_map ACLCTRL01_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0x000b, "16-bit stereo"},
	{0, 0}
};
value_map ACLCTRL02_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0, 0}
};
value_map ACLCTRL03_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0, 0}
};
value_map ACLCTRL04_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0, 0}
};
value_map ACLCTRL06_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0x000b, "16-bit stereo"},
	{0, 0}
};
value_map ACLCTRL07_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0, 0}
};
value_map ACLCTRL08_1_0__value_map[] = {
	{0x0000, "Disable"},
	{0x0001, "16-bit mono/packed"},
	{0x000a, "16-bit mono"},
	{0, 0}
};

#define DEFINE_SETTING_ENTRY(name, rname, sbit, bitl, type, default, vptr, des) { (((rname)&0x1ff)<<0)|(((sbit)&0x7)<<9)|(((bitl)&0x1f)<<12)|(((type)&0xf)<<17), default, name, vptr, des }

cx25840_setting_entry cx25840_settings[] = {
	DEFINE_SETTING_ENTRY("SLEEP", Host_Register_1, 0, 1, RW, 0x0000, SLEEP_value_map, "Put the chip in sleep mode. Gate the digital clocks and power"),
	DEFINE_SETTING_ENTRY("DIGITAL_PWR_DN", Host_Register_1, 1, 1, RW, 0x0000, DIGITAL_PWR_DN_value_map, "Gate digital clocks (sclk, vclk, clkx5). This bit can be configured"),
	DEFINE_SETTING_ENTRY("REF_CLK_SEL", Host_Register_1, 2, 1, RW, 0x0001, REF_CLK_SEL_value_map, "Crystal clock input type select."),
	DEFINE_SETTING_ENTRY("PWR_DN_VID_PLL", Host_Register_1, 3, 1, RW, 0x0000, PWR_DN_VID_PLL_value_map, "Power down the Video Clock PLL."),
	DEFINE_SETTING_ENTRY("PWR_DN_AUX_PLL", Host_Register_1, 4, 1, RW, 0x0000, PWR_DN_AUX_PLL_value_map, "Power down the Auxiliary PLL."),
	DEFINE_SETTING_ENTRY("AUTO_INC_DIS", Host_Register_1, 5, 1, RW, 0x0000, AUTO_INC_DIS_value_map, "Control auto-address increment after each byte transfer."),
	DEFINE_SETTING_ENTRY("SLV_SI_DIS", Host_Register_1, 6, 1, RW, 0x0000, SLV_SI_DIS_value_map, "Control the glitch filters and slew rate control in the 12C pads to"),
	DEFINE_SETTING_ENTRY("FORCE_CHIP_SEL_VIPCLK", Host_Register_1, 7, 1, RW, 0x0000, FORCE_CHIP_SEL_VIPCLK_value_map, "Override the CHIP_SEL/VIPCLK pin for 12C address decode."),
	DEFINE_SETTING_ENTRY("PREFETCH_EN", Host_Register_2, 0, 1, RW, 0x0000, PREFETCH_EN_value_map, "Enables prefetch on reads. Prefetch is unnecessary unless the"),
	DEFINE_SETTING_ENTRY("PWR_DN_PLL_REG1", Host_Register_2, 1, 1, RW, 0x0000, 0, "Powers down the regulator for the video clock PLL."),
	DEFINE_SETTING_ENTRY("PWR_DN_PLL_REG2", Host_Register_2, 2, 1, RW, 0x0000, 0, "Powers down the regulator for the auxiliary clock PLL."),
	DEFINE_SETTING_ENTRY("DEVICE_ID", Device_ID_Low_Byte, 0, 16, RO, 0x8400, DEVICE_ID_value_map, "0x1x,"),
	DEFINE_SETTING_ENTRY("SOFT_RST", Configuration, 0, 1, RW, 0x0000, SOFT_RST_value_map, ""),
	DEFINE_SETTING_ENTRY("CH_SEL_ADC2", Configuration, 1, 1, RW, 0x0000, CH_SEL_ADC2_value_map, "Miscellaneous Chip Control input select. This bit has no"),
	DEFINE_SETTING_ENTRY("DUAL_MODE_ADC2", Configuration, 2, 1, RW, 0x0000, DUAL_MODE_ADC2_value_map, "Sets Miscellaneous Chip Control to dual sampling mode"),
	DEFINE_SETTING_ENTRY("CHIP_ACFG_DIS", Configuration, 4, 1, RW, 0x0000, CHIP_ACFG_DIS_value_map, "Auto-config disable of the following registers:"),
	DEFINE_SETTING_ENTRY("CH_1__SOURCE", Video_Input_Control, 0, 3, RW, 0x0000, CH_1__SOURCE_value_map, "Selects input pin to CH{1} ADC"),
	DEFINE_SETTING_ENTRY("CH_2__SOURCE", Video_Input_Control, 4, 2, RW, 0x0000, CH_2__SOURCE_value_map, "Selects input pin to CH{2} ADC"),
	DEFINE_SETTING_ENTRY("CH_3__SOURCE", Video_Input_Control, 6, 2, RW, 0x0000, CH_3__SOURCE_value_map, "Selects input pin to CH{3} ADC"),
	DEFINE_SETTING_ENTRY("EN_12DB_CH1", AFE_Control_1, 0, 1, RW, 0x0000, EN_12DB_CH1_value_map, "Enables CH1 extra 12 dB gain"),
	DEFINE_SETTING_ENTRY("EN_12DB_CH2", AFE_Control_1, 1, 1, RW, 0x0000, EN_12DB_CH2_value_map, "Enables CH2 extra 12 dB gain"),
	DEFINE_SETTING_ENTRY("EN_12DB_CH3", AFE_Control_1, 2, 1, RW, 0x0000, EN_12DB_CH3_value_map, "Enables CH3 extra 12 dB gain"),
	DEFINE_SETTING_ENTRY("HALF_BW_CH1", AFE_Control_1, 3, 1, RW, 0x0000, 0, "When set, the CH1 anti-alias filter bandwidth is reduced to half."),
	DEFINE_SETTING_ENTRY("HALF_BW_CH2", AFE_Control_1, 4, 1, RW, 0x0000, 0, "When set, the CH2 anti-alias filter bandwidth is reduced to half."),
	DEFINE_SETTING_ENTRY("HALF_BW_CH3", AFE_Control_1, 5, 1, RW, 0x0000, 0, "When set, the CH3 anti-alias filter bandwidth is reduced to half."),
	DEFINE_SETTING_ENTRY("VGA_SEL_CH1", AFE_Control_1, 6, 1, RW, 0x0000, VGA_SEL_CH1_value_map, "VGA gain select for CH1"),
	DEFINE_SETTING_ENTRY("VGA_SEL_CH2", AFE_Control_1, 7, 1, RW, 0x0000, VGA_SEL_CH2_value_map, "VGA gain select for CH2"),
	DEFINE_SETTING_ENTRY("VGA_SEL_CH3", AFE_Control_2, 0, 1, RW, 0x0000, VGA_SEL_CH3_value_map, "VGA gain select for CH3"),
	DEFINE_SETTING_ENTRY("CLAMP_SEL_CH1", AFE_Control_2, 1, 1, RW, 0x0000, CLAMP_SEL_CH1_value_map, "Clamp level select for CH1"),
	DEFINE_SETTING_ENTRY("CLAMP_SEL_CH2", AFE_Control_2, 2, 1, RW, 0x0001, CLAMP_SEL_CH2_value_map, "Clamp level select for CH2"),
	DEFINE_SETTING_ENTRY("CLAMP_SEL_CH3", AFE_Control_2, 3, 1, RW, 0x0001, CLAMP_SEL_CH3_value_map, "Clamp level select for CH3"),
	DEFINE_SETTING_ENTRY("CHROMA_IN_SEL", AFE_Control_2, 4, 1, RW, 0x0001, CHROMA_IN_SEL_value_map, "ADC input select for chroma input path"),
	DEFINE_SETTING_ENTRY("LUMA_IN_SEL", AFE_Control_2, 5, 1, RW, 0x0000, LUMA_IN_SEL_value_map, "ADC input select for luma input path"),
	DEFINE_SETTING_ENTRY("AUD_IN_SEL", AFE_Control_2, 6, 1, RW, 0x0001, AUD_IN_SEL_value_map, "ADC input select for audio input paths:"),
	DEFINE_SETTING_ENTRY("CLAMP_EN_CH1", AFE_Control_2, 7, 1, RW, 0x0001, CLAMP_EN_CH1_value_map, "Channel 1 clamping"),
	DEFINE_SETTING_ENTRY("CLAMP_EN_CH2", AFE_Control_3, 0, 1, RW, 0x0000, CLAMP_EN_CH2_value_map, "Enables (powers up) clamping for CH2"),
	DEFINE_SETTING_ENTRY("CLAMP_EN_CH3", AFE_Control_3, 1, 1, RW, 0x0000, CLAMP_EN_CH3_value_map, "Enables (powers up) clamping for CH3"),
	DEFINE_SETTING_ENTRY("DROOP_COMP_CH1", AFE_Control_3, 2, 1, RW, 0x0001, 0, "Enables resistance boosting in CH1 input"),
	DEFINE_SETTING_ENTRY("DROOP_COMP_CH2", AFE_Control_3, 3, 1, RW, 0x0000, 0, "Enables resistance boosting in CH2 input"),
	DEFINE_SETTING_ENTRY("DROOP_COMP_CH3", AFE_Control_3, 4, 1, RW, 0x0000, 0, "Enables resistance boosting in CH3 input"),
	DEFINE_SETTING_ENTRY("BYPASS_CH1", AFE_Control_3, 5, 1, RW, 0x0000, 0, "Bypass CH1 anti-alias filter when set."),
	DEFINE_SETTING_ENTRY("BYPASS_CH2", AFE_Control_3, 6, 1, RW, 0x0000, 0, "Bypass CH2 anti-alias filter when set."),
	DEFINE_SETTING_ENTRY("BYPASS_CH3", AFE_Control_3, 7, 1, RW, 0x0000, 0, "Bypass CH3 anti-alias filter when set."),
	DEFINE_SETTING_ENTRY("IREF_SEL", AFE_Control_4, 3, 1, RW, 0x0000, 0, "Selects between on-chip (1) and external (0) resistor to generate"),
	DEFINE_SETTING_ENTRY("VID_PLL_INT", Video_PLL_Integer, 0, 6, RW, 0x000f, 0, "Video PLL integer coefficient"),
	DEFINE_SETTING_ENTRY("VID_PLL_POST", Video_PLL_Divider, 0, 6, RW, 0x0004, 0, "Video PLL post divide"),
	DEFINE_SETTING_ENTRY("AUX_PLL_INT", Aux_PLL_Integer, 0, 6, RW, 0x000a, 0, "Auxiliary PLL integer coefficient"),
	DEFINE_SETTING_ENTRY("AUX_PLL_POST", Aux_PLL_Divider, 0, 6, RW, 0x0010, 0, "Auxiliary PLL post divide"),
	DEFINE_SETTING_ENTRY("VID_PLL_FRAC1", Video_PLL_Fractional_1, 0, 8, RW, 0x00fe, 0, "Lowest byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("VID_PLL_FRAC2", Video_PLL_Fractional_2, 0, 8, RW, 0x00e2, 0, "Middle byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("VID_PLL_FRAC3", Video_PLL_Fractional_3, 0, 8, RW, 0x002b, 0, "Highest byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("VID_PLL_FRAC4", Video_PLL_Fractional_4, 0, 1, RW, 0x0000, 0, "Highest bit of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("AUX_PLL_FRAC1", Aux_PLL_Fractional_1, 0, 8, RW, 0x0009, 0, "Lowest byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("AUX_PLL_FRAC2", Aux_PLL_Fractional_2, 0, 8, RW, 0x00d7, 0, "Middle byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("AUX_PLL_FRAC3", Aux_PLL_Fractional_3, 0, 8, RW, 0x0098, 0, "Highest byte of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("AUX_PLL_FRAC4", Aux_PLL_Fractional_4, 0, 1, RW, 0x0000, 0, "Highest bit of the 25-bit fractional portion of the PLL multiplier"),
	DEFINE_SETTING_ENTRY("GPIO0_OUT_EN", Pin_Control_1, 0, 1, RW, 0x0000, GPIO0_OUT_EN_value_map, "Output enable control for GPIO0 pin (2584x only)"),
	DEFINE_SETTING_ENTRY("GPIO1_OUT_EN", Pin_Control_1, 1, 1, RW, 0x0000, GPIO1_OUT_EN_value_map, "Output enable control for GPIO1 pin (2584x only)"),
	DEFINE_SETTING_ENTRY("CHIP_SEL_VIPCLK_OUT_EN", Pin_Control_1, 2, 1, RW, 0x0000, CHIP_SEL_VIPCLK_OUT_EN_value_map, "Output enable for CHIP_SEL/VIPCLK_OUT pin"),
	DEFINE_SETTING_ENTRY("IRQN_OUT_EN", Pin_Control_1, 3, 1, RW, 0x0000, IRQN_OUT_EN_value_map, "Output enable for IRQN_OUT pin"),
	DEFINE_SETTING_ENTRY("IR_RX_OUT_EN", Pin_Control_1, 4, 1, RW, 0x0000, IR_RX_OUT_EN_value_map, "Output enable for IR_RX_OUT pin"),
	DEFINE_SETTING_ENTRY("IR_TX_OUT_EN", Pin_Control_1, 5, 1, RW, 0x0000, IR_TX_OUT_EN_value_map, "Output enable for IR_TX_OUT pin"),
	DEFINE_SETTING_ENTRY("DVALID_OUT_EN", Pin_Control_1, 6, 1, RW, 0x0000, DVALID_OUT_EN_value_map, "Output enable for DVALID/PRGM0 pin"),
	DEFINE_SETTING_ENTRY("FIELD_OUT_EN", Pin_Control_1, 7, 1, RW, 0x0000, FIELD_OUT_EN_value_map, "Output enable for FIELD/PRGM1 pin"),
	DEFINE_SETTING_ENTRY("HRESET_OUT_EN", Pin_Control_2, 0, 1, RW, 0x0000, HRESET_OUT_EN_value_map, "Output enable for HRESET/PRGM2 pin"),
	DEFINE_SETTING_ENTRY("VRESET_OUT_EN", Pin_Control_2, 1, 1, RW, 0x0000, VRESET_OUT_EN_value_map, "Output enable for VRESET/HCTL/PRGM3 pin"),
	DEFINE_SETTING_ENTRY("VID_OUT_EN", Pin_Control_2, 2, 1, RW, 0x0000, VID_OUT_EN_value_map, "Output enable for VID_DATA [7:0] pin"),
	DEFINE_SETTING_ENTRY("PIXCLK_OUT_EN", Pin_Control_2, 3, 1, RW, 0x0000, PIXCLK_OUT_EN_value_map, "Output enable for PIXCLK pin"),
	DEFINE_SETTING_ENTRY("SA_SDIN_OUT_EN", Pin_Control_2, 4, 1, RW, 0x0000, SA_SDIN_OUT_EN_value_map, "Output enable for SA_SDIN pin"),
	DEFINE_SETTING_ENTRY("SA_BCLKIN_OUT_EN", Pin_Control_2, 5, 1, RW, 0x0000, SA_BCLKIN_OUT_EN_value_map, "Output enable for SA_WCLKIN pin"),
	DEFINE_SETTING_ENTRY("SA_WCLKIN_OUT_EN", Pin_Control_2, 6, 1, RW, 0x0000, SA_WCLKIN_OUT_EN_value_map, "Output enable for SA_WCLKIN pin"),
	DEFINE_SETTING_ENTRY("SA_SDOUT_OUT_EN", Pin_Control_2, 7, 1, RW, 0x0000, SA_SDOUT_OUT_EN_value_map, "Output enable for SA_SDOUT pin"),
	DEFINE_SETTING_ENTRY("SA_BCLKOUT_OUT_EN", Pin_Control_3, 0, 1, RW, 0x0000, SA_BCLKOUT_OUT_EN_value_map, "Output enable for SA_BCLKOUT pin (2584X only)."),
	DEFINE_SETTING_ENTRY("SA_WCLKOUT_OUT_EN", Pin_Control_3, 1, 1, RW, 0x0000, SA_WCLKOUT_OUT_EN_value_map, "Output enable for SA_WCLKOUT pin (2584X only)."),
	DEFINE_SETTING_ENTRY("PLL_CLK_OUT_EN", Pin_Control_3, 2, 1, RW, 0x0000, PLL_CLK_OUT_EN_value_map, "Output enable for PLL_CLK/PRGM7 pin"),
	DEFINE_SETTING_ENTRY("AC_OUT_OUT_EN", Pin_Control_3, 3, 1, RW, 0x0000, AC_OUT_OUT_EN_value_map, "Output enable control for AC_SDOUT, ADC_SYNC, and"),
	DEFINE_SETTING_ENTRY("AC_IN_OUT_EN", Pin_Control_3, 4, 1, RW, 0x0000, AC_IN_OUT_EN_value_map, "Output enable control for AC_SDIN and AC_BITCLK pads"),
	DEFINE_SETTING_ENTRY("IRQ_N_PRGM4", Pin_Control_4, 1, 1, RW, 0x0000, IRQ_N_PRGM4_value_map, "_POLAR Polarity of IRQ_N/PRGM4 output"),
	DEFINE_SETTING_ENTRY("VID_IRQ_STAT", Pin_Control_4, 2, 1, RO, 0x0000, VID_IRQ_STAT_value_map, "Video Interrupt status"),
	DEFINE_SETTING_ENTRY("IR_IRQ_STAT", Pin_Control_4, 4, 1, RO, 0x0000, IR_IRQ_STAT_value_map, "Infrared Remote Interrupt status"),
	DEFINE_SETTING_ENTRY("VID_OUT_SPD", Pin_Control_5, 0, 2, RW, 0x0000, VID_OUT_SPD_value_map, "Controls drive strength of PIXCLK and VID_DATA pads."),
	DEFINE_SETTING_ENTRY("VID_CTRL_SPD", Pin_Control_5, 2, 2, RW, 0x0000, VID_CTRL_SPD_value_map, "Controls drive strength of PRGM{0:2} pads."),
	DEFINE_SETTING_ENTRY("AC_OUT_SPD", Pin_Control_5, 4, 2, RW, 0x0000, AC_OUT_SPD_value_map, "Controls drive strength of AC_SYNC, AC_RST_N, AC_BITCLK,"),
	DEFINE_SETTING_ENTRY("SA_OUT_SPD", Pin_Control_5, 6, 2, RW, 0x0000, SA_OUT_SPD_value_map, "Controls drive strength of SA_SDOUT, SA_WCLK_OUT, and"),
	DEFINE_SETTING_ENTRY("GEN_OUT_SPD", Pin_Control_6, 0, 2, RW, 0x0000, GEN_OUT_SPD_value_map, "Controls drive strength of IR_TX/PRGM6, IR_RX/PRGM5,"),
	DEFINE_SETTING_ENTRY("GPIO0_OUT_SEL", Pin_Configuration_1, 0, 4, RW, 0x0000, GPIO0_OUT_SEL_value_map, "Selects which internal state is output on the GPIO[0]/PRGM8 pin."),
	DEFINE_SETTING_ENTRY("GPIO1_OUT_SEL", Pin_Configuration_1, 4, 4, RW, 0x0000, GPIO1_OUT_SEL_value_map, "Selects which internal state is output on the GPIO[1]/PRGM9 pin."),
	DEFINE_SETTING_ENTRY("CHIPSEL_OUT_SEL", Pin_Configuration_2, 0, 4, RW, 0x0000, CHIPSEL_OUT_SEL_value_map, "Selects which internal state is output on the CHIP_SEL/"),
	DEFINE_SETTING_ENTRY("IRQN_OUT_SEL", Pin_Configuration_2, 4, 4, RW, 0x0000, IRQN_OUT_SEL_value_map, "Selects which signal to output on the IRQ_N/PRGM4 pin."),
	DEFINE_SETTING_ENTRY("IR_RX_OUT_SEL", Pin_Configuration_3, 0, 4, RW, 0x0000, IR_RX_OUT_SEL_value_map, "Selects which signal to output on the IR_RX/PRGM5 pin if this"),
	DEFINE_SETTING_ENTRY("IR_TX_OUT_SEL", Pin_Configuration_3, 4, 4, RW, 0x0000, IR_TX_OUT_SEL_value_map, "Selects which signal to output on the IR_TX/PRGM6 pin."),
	DEFINE_SETTING_ENTRY("DVALID_PRGM0_OUT_SEL", Pin_Configuration_4, 0, 4, RW, 0x0000, DVALID_PRGM0_OUT_SEL_value_map, "Selects which signal to output on the PRGM0 pin."),
	DEFINE_SETTING_ENTRY("FIELD_PRGM1_OUT_SEL", Pin_Configuration_4, 4, 4, RW, 0x0000, FIELD_PRGM1_OUT_SEL_value_map, "Selects which signal to output on the PRGM1 pin."),
	DEFINE_SETTING_ENTRY("HRESET_PRGM2_OUT_SEL", Pin_Configuration_5, 0, 4, RW, 0x0000, HRESET_PRGM2_OUT_SEL_value_map, "Selects which signal to output on the PRGM2 pin."),
	DEFINE_SETTING_ENTRY("VRESET_HCTL_PRGM3_OUT_SEL", Pin_Configuration_5, 4, 4, RW, 0x0000, VRESET_HCTL_PRGM3_OUT_SEL_value_map, "Selects which signal to output on the PRGM3 pin."),
	DEFINE_SETTING_ENTRY("VID_DATA_OUT_SEL", Pin_Configuration_6, 0, 1, RW, 0x0000, 0, "Should only be written with a 0"),
	DEFINE_SETTING_ENTRY("PIXCLK_OUT_SEL", Pin_Configuration_6, 1, 1, RW, 0x0000, 0, "Should only be written with a 0"),
	DEFINE_SETTING_ENTRY("AC_OUT_SEL", Pin_Configuration_6, 2, 1, RW, 0x0000, AC_OUT_SEL_value_map, "Selects which data is output on AC†¢97 output pins of AC_RSTN,"),
	DEFINE_SETTING_ENTRY("SA_OUT_OUT_SEL", Pin_Configuration_6, 4, 2, RW, 0x0000, SA_OUT_OUT_SEL_value_map, "Selects which data is output on the SA_WCLK_IN SA_BCLK_IN, and"),
	DEFINE_SETTING_ENTRY("SA_IN_OUT_SEL", Pin_Configuration_6, 6, 2, RW, 0x0000, SA_IN_OUT_SEL_value_map, "Selects which data is output on the SA_WCLK_OUT, SA_BCLK_OUT,"),
	DEFINE_SETTING_ENTRY("AUX_PLL_AOUT_SEL", Pin_Configuration_7, 0, 3, RW, 0x0001, AUX_PLL_AOUT_SEL_value_map, "Selects which signal to output on the PLL_CLK/PRGM7 pin when AUX"),
	DEFINE_SETTING_ENTRY("AUX_PLL_DOUT_SEL", Pin_Configuration_7, 4, 4, RW, 0x0000, AUX_PLL_DOUT_SEL_value_map, "Selects which signal to output on the PLL _CLK pin."),
	DEFINE_SETTING_ENTRY("AC_BITCLK_IN_SEL", Pin_Configuration_8, 0, 4, RW, 0x0000, AC_BITCLK_IN_SEL_value_map, "Selects the pin that is muxed to the AC_BITCLK input of the"),
	DEFINE_SETTING_ENTRY("AC_SDIN_IN_SEL", Pin_Configuration_8, 4, 4, RW, 0x0001, AC_SDIN_IN_SEL_value_map, "Selects the pin that is muxed to the AC_/sdin input of the"),
	DEFINE_SETTING_ENTRY("GPI0_IN_SEL", Pin_Configuration_9, 0, 4, RW, 0x0049, GPI0_IN_SEL_value_map, "Selects the pin that is muxed to the internal GPI0 flop."),
	DEFINE_SETTING_ENTRY("GPI1_IN_SEL", Pin_Configuration_9, 4, 4, RW, 0x03e8, GPI1_IN_SEL_value_map, "Selects the pin that is muxed to the internal GPI1 flop."),
	DEFINE_SETTING_ENTRY("GPI2_IN_SEL", Pin_Configuration_10, 0, 4, RW, 0x0048, GPI2_IN_SEL_value_map, "Selects the pin that is muxed to the internal GPI2 flop."),
	DEFINE_SETTING_ENTRY("GPI3_IN_SEL", Pin_Configuration_10, 4, 4, RW, 0x03f3, GPI3_IN_SEL_value_map, "Selects the pin that is muxed to the internal GPI3 flop."),
	DEFINE_SETTING_ENTRY("GPO_OUT", Pin_Configuration_11, 0, 4, RW, 0x0000, 0, "Registers that hold data that may get sent to the pins selected by"),
	DEFINE_SETTING_ENTRY("GPI_IN", Pin_Configuration_11, 4, 4, RO, 0x0000, 0, "Data from pins selected by the GPIOx_IN_SEL register settings."),
	DEFINE_SETTING_ENTRY("SA_MCLK_DIV", Pin_Configuration_12, 0, 6, RW, 0x0000, 0, "Controls the generation of alternate serial audio master clock"),
	DEFINE_SETTING_ENTRY("SA_MCLK_SEL", Pin_Configuration_12, 6, 1, RW, 0x0000, SA_MCLK_SEL_value_map, "Selects alternate post-divider for internal SA_MCLK. The serial"),
	DEFINE_SETTING_ENTRY("VID_COUNT", Video_Count_Low, 0, 24, RW, 0x1193f8, 0, "Least significant byte of the expected count (minus 1 and times"),
	DEFINE_SETTING_ENTRY("EN_AV_LOCK", Audio_Lock, 0, 1, RW, 0x0000, 0, "Enable locking of auxiliary PLL to video pixel rate."),
	DEFINE_SETTING_ENTRY("AUD_LOCK_KD_SHIFT", Audio_Lock, 4, 2, RW, 0x000a, 0, "The gain applied to the direct error path of the loop filter through"),
	DEFINE_SETTING_ENTRY("AUD_LOCK_KI_SHIFT", Audio_Lock, 6, 2, RW, 0x000a, 0, "The gain applied to the indirect error path of the loop filter"),
	DEFINE_SETTING_ENTRY("AUD_COUNT", Audio_Count_Low, 0, 20, RW, 0x005fff, 0, "Least significant byte of the number of audio sample-rate clocks,"),
	DEFINE_SETTING_ENTRY("AUD_LOCK_FREQ_SHIFT", Audio_Lock_2, 4, 2, RW, 0x0000, 0, "The gain applied to the frequency error (difference in actual vs."),
	DEFINE_SETTING_ENTRY("AUD_LOCK_KD_MULT", Audio_Lock_3, 0, 4, RW, 0x0001, 0, ""),
	DEFINE_SETTING_ENTRY("AUD_LOCK_KI_MULT", Audio_Lock_3, 4, 4, RW, 0x0001, 0, "The gain applied to the indirect error path of the loop filter"),
	DEFINE_SETTING_ENTRY("PWR_DN_TUNING", Power_Control_1, 0, 1, RW, 0x0000, PWR_DN_TUNING_value_map, "Powers down filter tuning circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_DLL1", Power_Control_1, 1, 1, RW, 0x0000, PWR_DN_DLL1_value_map, "Powers down DLL1 (5x clock) circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_DLL2", Power_Control_1, 2, 1, RW, 0x0000, PWR_DN_DLL2_value_map, "Powers down DLL2 (5x clock) circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_ADC1", Power_Control_1, 3, 1, RW, 0x0000, PWR_DN_ADC1_value_map, "Powers down ADC1 circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_ADC2", Power_Control_1, 4, 1, RW, 0x0000, PWR_DN_ADC2_value_map, "Powers down Miscellaneous Chip Control circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_CH1", Power_Control_1, 5, 1, RW, 0x0000, PWR_DN_CH1_value_map, "Powers down Channel 1 VGA/filter circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_CH2", Power_Control_1, 6, 1, RW, 0x0000, PWR_DN_CH2_value_map, "Powers down Channel 2 VGA/filter circuitry."),
	DEFINE_SETTING_ENTRY("PWR_DN_CH3", Power_Control_1, 7, 1, RW, 0x0000, PWR_DN_CH3_value_map, "Powers down Channel 3 VGA/filter circuitry."),
	DEFINE_SETTING_ENTRY("SLEEP_ANALOG_MSK", Power_Control_2, 0, 1, RW, 0x0000, SLEEP_ANALOG_MSK_value_map, "Analog subsystem sleep mask"),
	DEFINE_SETTING_ENTRY("SLEEP_DLL_MSK", Power_Control_2, 1, 1, RW, 0x0000, SLEEP_DLL_MSK_value_map, "DLL sleep mask"),
	DEFINE_SETTING_ENTRY("SLEEP_PLL_MSK", Power_Control_2, 2, 1, RW, 0x0000, SLEEP_PLL_MSK_value_map, "PLL sleep mask"),
	DEFINE_SETTING_ENTRY("VCLK_GATE_MSK", Power_Control_2, 3, 1, RW, 0x0000, VCLK_GATE_MSK_value_map, "Video clock gate mask"),
	DEFINE_SETTING_ENTRY("CLK5x_GATE_MSK", Power_Control_2, 4, 1, RW, 0x0000, CLK5x_GATE_MSK_value_map, "CLK5x gate mask"),
	DEFINE_SETTING_ENTRY("SCLK_GATE_MSK", Power_Control_2, 5, 1, RW, 0x0000, SCLK_GATE_MSK_value_map, "SCLK and SA_MCLK gate mask"),
	DEFINE_SETTING_ENTRY("FOUR_X_CLK_ADC", AFE_Diagnostic_Control_1, 0, 1, RW, 0x0000, FOUR_X_CLK_ADC_value_map, "Chooses 4x or 5x DLL output"),
	DEFINE_SETTING_ENTRY("VREG_D_1_0_", AFE_Diagnostic_Control_1, 1, 2, RW, 0x0001, VREG_D_1_0__value_map, "Voltage Regulator Select. Controls what voltage the regulator"),
	DEFINE_SETTING_ENTRY("VREF_CTRL_ADC", AFE_Diagnostic_Control_1, 3, 1, RW, 0x0000, VREF_CTRL_ADC_value_map, "Digital control for ADC reference voltage."),
	DEFINE_SETTING_ENTRY("BIAS_CTRL_ADC_1_0_", AFE_Diagnostic_Control_2, 0, 2, RW, 0x0000, BIAS_CTRL_ADC_1_0__value_map, "Digital control for ADC bias current."),
	DEFINE_SETTING_ENTRY("BIAS_CTRL_ADC_2_", AFE_Diagnostic_Control_2, 2, 1, RW, 0x0000, BIAS_CTRL_ADC_2__value_map, "Digital control for refp and refm"),
	DEFINE_SETTING_ENTRY("BIAS_CTRL_ADC_4_3_", AFE_Diagnostic_Control_2, 3, 2, RW, 0x000b, BIAS_CTRL_ADC_4_3__value_map, "MSB comparator current."),
	DEFINE_SETTING_ENTRY("BIAS_CTRL_ADC_6_5_", AFE_Diagnostic_Control_2, 5, 2, RW, 0x0000, BIAS_CTRL_ADC_6_5__value_map, "Digital control for the reference buffer bias."),
	DEFINE_SETTING_ENTRY("S2DIFF_BIAS_1_0_", AFE_Diagnostic_Control_3, 0, 2, RW, 0x0001, S2DIFF_BIAS_1_0__value_map, "Digital control for the single-ended to differential converter bias"),
	DEFINE_SETTING_ENTRY("FILTER_BIAS_1_0_", AFE_Diagnostic_Control_3, 2, 2, RW, 0x0001, FILTER_BIAS_1_0__value_map, "Digital control for bias current multiplier."),
	DEFINE_SETTING_ENTRY("TUNE_FIL_RST", AFE_Diagnostic_Control_5, 0, 1, RW, 0x0000, 0, "Reset filter tuning logic. When 1, filter tuning is reset. When set"),
	DEFINE_SETTING_ENTRY("CH_SEL_ADC1", AFE_Diagnostic_Control_5, 1, 1, RW, 0x0000, CH_SEL_ADC1_value_map, "ADC1 test mode"),
	DEFINE_SETTING_ENTRY("DISCONNECT_CH1", AFE_Diagnostic_Control_5, 2, 1, RW, 0x0000, DISCONNECT_CH1_value_map, "Disables the filter in CH1 test mode."),
	DEFINE_SETTING_ENTRY("TEST_MODE_CH1", AFE_Diagnostic_Control_5, 3, 1, RW, 0x0000, TEST_MODE_CH1_value_map, "Connects the differential analog signal at ADC1 input to pins"),
	DEFINE_SETTING_ENTRY("TUNE_IN_4_0_", AFE_Diagnostic_Control_6, 0, 5, RW, 0x0000, 0, "Tuning code to be used when FORCE_TUNING is set. Overrides"),
	DEFINE_SETTING_ENTRY("FORCE_TUNING", AFE_Diagnostic_Control_6, 7, 1, RW, 0x0000, FORCE_TUNING_value_map, "Auto tuning code override"),
	DEFINE_SETTING_ENTRY("TUNE_OUT", AFE_Diagnostic_Control_7, 0, 5, RO, 0x0000, 0, "The tuning code selected by the auto-tune algorithm."),
	DEFINE_SETTING_ENTRY("TUNING_READY", AFE_Diagnostic_Control_7, 5, 1, RO, 0x0000, TUNING_READY_value_map, "Filter auto tuning status"),
	DEFINE_SETTING_ENTRY("VID_DUAL_FLAG_POL", AFE_Diagnostic_Control_8, 0, 1, RW, 0x0000, VID_DUAL_FLAG_POL_value_map, "Video Decoder Dual Flag Polarity"),
	DEFINE_SETTING_ENTRY("AUD_DUAL_FLAG_POL", AFE_Diagnostic_Control_8, 1, 1, RW, 0x0000, AUD_DUAL_FLAG_POL_value_map, "Audio Decoder Dual Flag Polarity"),
	DEFINE_SETTING_ENTRY("PLL_SPMP", PLL_Diagnostic_Control_1, 0, 6, RW, 0x0004, 0, "PLL charge pump current"),
	DEFINE_SETTING_ENTRY("VID_PLL_DDS", PLL_Diagnostic_Control_2, 0, 1, RW, 0x0000, VID_PLL_DDS_value_map, "Video PLL delta signal fractional divide"),
	DEFINE_SETTING_ENTRY("AUX_PLL_DDS", PLL_Diagnostic_Control_2, 1, 1, RW, 0x0000, AUX_PLL_DDS_value_map, "Auxiliary PLL delta sigma fractional divide"),
	DEFINE_SETTING_ENTRY("VID_PLL_RST", PLL_Diagnostic_Control_2, 2, 1, RW, 0x0000, VID_PLL_RST_value_map, "Auxiliary PLL reset"),
	DEFINE_SETTING_ENTRY("AUX_PLL_RST", PLL_Diagnostic_Control_2, 3, 1, RW, 0x0000, AUX_PLL_RST_value_map, "Video PLL reset"),
	DEFINE_SETTING_ENTRY("AUX_PLL_LOCK", PLL_Diagnostic_Control_2, 4, 1, RO, 0x0000, AUX_PLL_LOCK_value_map, "Auxiliary PLL lock status"),
	DEFINE_SETTING_ENTRY("VID_PLL_LOCK", PLL_Diagnostic_Control_2, 5, 1, RO, 0x0000, VID_PLL_LOCK_value_map, "Video PLL lock status"),
	DEFINE_SETTING_ENTRY("AUX_PLL_UNLOCK", PLL_Diagnostic_Control_2, 6, 1, RR, 0x0000, AUX_PLL_UNLOCK_value_map, "Auxiliary PLL unlock detection"),
	DEFINE_SETTING_ENTRY("VID_PLL_UNLOCK", PLL_Diagnostic_Control_2, 7, 1, RR, 0x0000, VID_PLL_UNLOCK_value_map, "Video PLL unlock detection"),
	DEFINE_SETTING_ENTRY("DLL1_FLD", DLL1_Diagnostic_Control_2, 0, 1, RW, 0x0001, 0, "False Lock Detect mode"),
	DEFINE_SETTING_ENTRY("DLL1_UP_OVRD", DLL1_Diagnostic_Control_2, 1, 1, RW, 0x0000, 0, "Overrides the up command to PFD."),
	DEFINE_SETTING_ENTRY("DLL1_DOWN_OVRD", DLL1_Diagnostic_Control_2, 2, 1, RW, 0x0000, 0, "Overrides the down command to PFD."),
	DEFINE_SETTING_ENTRY("DLL1_CHPREF", DLL1_Diagnostic_Control_2, 3, 3, RW, 0x0064, 0, "Charge pump current."),
	DEFINE_SETTING_ENTRY("DLL1_COMP_GT", DLL1_Diagnostic_Control_2, 6, 3, RW, 0x0004, 0, "Lower 2 bits of COMP_GT"),
	DEFINE_SETTING_ENTRY("DLL1_COMP_LT", DLL1_Diagnostic_Control_3, 1, 3, RW, 0x0009, 0, "Used in False Lock Detect: counts this many pulses in half period"),
	DEFINE_SETTING_ENTRY("DLL1_DEPTH", DLL1_Diagnostic_Control_3, 4, 3, RW, 0x006e, 0, "Used in False Lock detect: increases stability of decisions in FLD."),
	DEFINE_SETTING_ENTRY("DLL1_DLYS", DLL1_Diagnostic_Control_3, 7, 2, RW, 0x0001, 0, "Low bit of DLYS"),
	DEFINE_SETTING_ENTRY("DLL1_CURRSET", DLL1_Diagnostic_Control_4, 1, 4, RW, 0x0000, 0, "Changes current used in DLL."),
	DEFINE_SETTING_ENTRY("DLL1_BYPASS", DLL1_Diagnostic_Control_4, 7, 1, RW, 0x0000, 0, "Clock reference for DLL1 (ADC DLL) input from clock pad rather"),
	DEFINE_SETTING_ENTRY("DLL2_Diagnostic_Control_1_REG", DLL2_Diagnostic_Control_1, 0, 8, RW, 0x0000, 0, "Missing bit spec for reg"),
	DEFINE_SETTING_ENTRY("DLL2_FLD", DLL2_Diagnostic_Control_2, 0, 1, RW, 0x0001, 0, "False Lock Detect mode."),
	DEFINE_SETTING_ENTRY("DLL2_UP_OVRD", DLL2_Diagnostic_Control_2, 1, 1, RW, 0x0000, 0, "Overrides the up command to PFD."),
	DEFINE_SETTING_ENTRY("DLL2_DOWN_OVRD", DLL2_Diagnostic_Control_2, 2, 1, RW, 0x0000, 0, "Overrides the down command to PFD."),
	DEFINE_SETTING_ENTRY("DLL2_CHPREF", DLL2_Diagnostic_Control_2, 3, 3, RW, 0x0064, 0, "Charge pump current."),
	DEFINE_SETTING_ENTRY("DLL2_COMP_GT", DLL2_Diagnostic_Control_2, 6, 3, RW, 0x0004, 0, "Used in False Lock Detect: counts this many pulses in half period"),
	DEFINE_SETTING_ENTRY("DLL2_COMP_LT", DLL2_Diagnostic_Control_3, 1, 3, RW, 0x0009, 0, "Used in False Lock Detect: counts this many pulses in half period"),
	DEFINE_SETTING_ENTRY("DLL2_DEPTH", DLL2_Diagnostic_Control_3, 4, 3, RW, 0x006e, 0, "Used in False Lock detect: increases stability of decisions in FLD."),
	DEFINE_SETTING_ENTRY("DLL2_DLYS", DLL2_Diagnostic_Control_3, 7, 2, RW, 0x0001, 0, "Low bit of DLYS"),
	DEFINE_SETTING_ENTRY("DLL2_CURRSET", DLL2_Diagnostic_Control_4, 1, 4, RW, 0x0000, 0, "Changes current used in DLL"),
	DEFINE_SETTING_ENTRY("DLL2_BYPASS", DLL2_Diagnostic_Control_4, 6, 2, RW, 0x0000, 0, "Clock reference for DLL2 (Audio Decoder DLL) input from clock"),
	DEFINE_SETTING_ENTRY("WIN", IR_Control_1, 0, 2, RW, 0x0000, WIN_value_map, "Next Predicted Receive Carrier Edge Window Limits"),
	DEFINE_SETTING_ENTRY("EDG", IR_Control_1, 2, 2, RW, 0x0000, EDG_value_map, "Receive Edge Detect Control"),
	DEFINE_SETTING_ENTRY("DMD", IR_Control_1, 4, 1, RW, 0x0000, DMD_value_map, "Receive Demodulation Enable"),
	DEFINE_SETTING_ENTRY("MOD", IR_Control_1, 5, 1, RW, 0x0000, MOD_value_map, "Transmit Modulation Enable"),
	DEFINE_SETTING_ENTRY("RFE", IR_Control_1, 6, 1, RW, 0x0000, RFE_value_map, "Receive FIFO Enable"),
	DEFINE_SETTING_ENTRY("TFE", IR_Control_1, 7, 1, RW, 0x0000, TFE_value_map, "Transmit FIFO Enable"),
	DEFINE_SETTING_ENTRY("RXE", IR_Control_2, 0, 1, RW, 0x0000, RXE_value_map, "Receiver Enable"),
	DEFINE_SETTING_ENTRY("TXE", IR_Control_2, 1, 1, RW, 0x0000, TXE_value_map, "Transmitter Enable"),
	DEFINE_SETTING_ENTRY("RIC", IR_Control_2, 2, 1, RW, 0x0000, RIC_value_map, "Receiver Interrupt Control"),
	DEFINE_SETTING_ENTRY("TIC", IR_Control_2, 3, 1, RW, 0x0000, TIC_value_map, "Transmitter Interrupt Control"),
	DEFINE_SETTING_ENTRY("CPL", IR_Control_2, 4, 1, RW, 0x0000, CPL_value_map, "Carrier Polarity (Transmitter Only)"),
	DEFINE_SETTING_ENTRY("LBM", IR_Control_2, 5, 1, RW, 0x0000, LBM_value_map, "Loop Back Mode"),
	DEFINE_SETTING_ENTRY("R", IR_Control_2, 6, 1, RW, 0x0000, R_value_map, "Receive FIFO load on Timer Overflow Disable"),
	DEFINE_SETTING_ENTRY("TCD", IR_TX_Clock_Divider_Low, 0, 16, RW, 0xffff, 0, "Transmit Clock Divider"),
	DEFINE_SETTING_ENTRY("RCD", IR_RX_Clock_Divider_Low, 0, 16, RW, 0xffff, 0, "Receive Clock Divider"),
	DEFINE_SETTING_ENTRY("CDC", IR_TX_Carrier_Duty_Cycle, 0, 4, RW, 0x0000, CDC_value_map, "Transmit Carrier Duty Cycle"),
	DEFINE_SETTING_ENTRY("RTO", IR_Status, 0, 1, RO, 0x0000, RTO_value_map, "Receive Pulse Width Timer Time-out"),
	DEFINE_SETTING_ENTRY("ROR", IR_Status, 1, 1, RO, 0x0000, ROR_value_map, "Receive FIFO Overrun"),
	DEFINE_SETTING_ENTRY("TBY", IR_Status, 3, 1, RO, 0x0000, TBY_value_map, "Transmitter Busy (noninterruptible)"),
	DEFINE_SETTING_ENTRY("RSR", IR_Status, 4, 1, RO, 0x0000, RSR_value_map, "Receive FIFO Service Request"),
	DEFINE_SETTING_ENTRY("TSR", IR_Status, 5, 1, RO, 0x0000, TSR_value_map, "Transmit FIFO Service Request"),
	DEFINE_SETTING_ENTRY("ROE", IR_Interrupt_Enable, 1, 1, RW, 0x0000, ROE_value_map, "Receive FIFO Overrun Interrupt Enable"),
	DEFINE_SETTING_ENTRY("RSE", IR_Interrupt_Enable, 4, 1, RW, 0x0000, RSE_value_map, "Receive FIFO Service Request Interrupt Enable"),
	DEFINE_SETTING_ENTRY("TSE", IR_Interrupt_Enable, 5, 1, RW, 0x0000, TSE_value_map, "Transmit FIFO Service Request Interrupt Enable"),
	DEFINE_SETTING_ENTRY("LPF", IR_Low_Pass_Filter_Low, 0, 16, RW, 0x0000, 0, "Low-Pass Filter Modulus"),
	DEFINE_SETTING_ENTRY("RX_TX_FIFO", IR_FIFO_Low, 0, 16, RW, 0x0000, 0, "Transmit/Receive"),
	DEFINE_SETTING_ENTRY("RX_TX_LVL", IR_FIFO_Level, 0, 1, RW, 0x0000, 0, "Transmit/Receive Pin Level"),
	DEFINE_SETTING_ENTRY("RXNDV", IR_FIFO_Level, 1, 1, RO, 0x0000, RXNDV_value_map, "Receive Next Data Valid (read only)"),
	DEFINE_SETTING_ENTRY("VID_FMT_SEL", Video_Mode_Control_1, 0, 4, RW, 0x0000, VID_FMT_SEL_value_map, "Manual video format select value. This value is used to force the"),
	DEFINE_SETTING_ENTRY("SQ_PIXEL", Video_Mode_Control_1, 4, 1, RW, 0x0000, 0, "Square-pixel mode"),
	DEFINE_SETTING_ENTRY("ACFG_DIS", Video_Mode_Control_1, 5, 1, RW, 0x0000, ACFG_DIS_value_map, "Disable autoconfig of registers addressed 0x470 to 0x47F based"),
	DEFINE_SETTING_ENTRY("AFD_PAL_SEL", Video_Mode_Control_1, 6, 1, RW, 0x0000, AFD_PAL_SEL_value_map, "This bit is used by the Auto Format Detect block to differentiate"),
	DEFINE_SETTING_ENTRY("AFD_NTSC_SEL", Video_Mode_Control_1, 7, 1, RW, 0x0000, AFD_NTSC_SEL_value_map, "This bit is used by the Auto Format Detect block to differentiate"),
	DEFINE_SETTING_ENTRY("AFD_ACQUIRE", Video_Mode_Control_2, 0, 1, RW, 0x0000, AFD_ACQUIRE_value_map, "By setting to 1 and then 0, forces the auto-format-detect state"),
	DEFINE_SETTING_ENTRY("INPUT_MODE", Video_Mode_Control_2, 1, 2, RW, 0x0000, INPUT_MODE_value_map, "Signal Input Format"),
	DEFINE_SETTING_ENTRY("MAN_SC_FAST_LOCK", Video_Mode_Control_2, 3, 1, RW, 0x0000, MAN_SC_FAST_LOCK_value_map, "Manual chroma subcarrier lock speed select."),
	DEFINE_SETTING_ENTRY("AUTO_SC_LOCK", Video_Mode_Control_2, 4, 1, RW, 0x0001, AUTO_SC_LOCK_value_map, "Auto chroma subcarrier lock speed select"),
	DEFINE_SETTING_ENTRY("CKILLEN", Video_Mode_Control_2, 5, 1, RW, 0x0001, CKILLEN_value_map, "Chroma killer enable"),
	DEFINE_SETTING_ENTRY("CAGCEN", Video_Mode_Control_2, 6, 1, RW, 0x0001, CAGCEN_value_map, "Chroma AGC enable"),
	DEFINE_SETTING_ENTRY("WCEN", Video_Mode_Control_2, 7, 1, RW, 0x0001, 0, "White crush enable"),
	DEFINE_SETTING_ENTRY("FAST_LOCK_MD", Video_Mode_Control_3, 0, 1, RW, 0x0000, 0, "Active-high fast lock algorithm select. This register selects"),
	DEFINE_SETTING_ENTRY("CLR_LOCK_STAT", Video_Mode_Control_3, 1, 1, SC, 0x0000, 0, "Clear HLOCK, VLOCK, and clock status bits"),
	DEFINE_SETTING_ENTRY("COMB_NOTCH_MODE", Video_Mode_Control_3, 2, 2, RW, 0x000a, COMB_NOTCH_MODE_value_map, "Controls behavior Y/C separation when adaptive comb filter"),
	DEFINE_SETTING_ENTRY("CKILL_MODE", Video_Mode_Control_3, 4, 2, RW, 0x0000, CKILL_MODE_value_map, "Color Kill Mode. Defines how luma output is generated when"),
	DEFINE_SETTING_ENTRY("AFD_PALM_SEL", Video_Mode_Control_4, 0, 2, RW, 0x0000, AFD_PALM_SEL_value_map, "Select PAL-M format when 525-lines and 3.58 carrier is"),
	DEFINE_SETTING_ENTRY("AFD_FORCE_PAL", Video_Mode_Control_4, 2, 1, RW, 0x0000, AFD_FORCE_PAL_value_map, "Force PAL_BG format when 625 lines are detected."),
	DEFINE_SETTING_ENTRY("AFD_FORCE_PALNC", Video_Mode_Control_4, 3, 1, RW, 0x0000, AFD_FORCE_PALNC_value_map, "Force PAL_Nc format when 625 lines are detected."),
	DEFINE_SETTING_ENTRY("AFD_FORCE_SECAM", Video_Mode_Control_4, 4, 1, RW, 0x0000, AFD_FORCE_SECAM_value_map, "Force SECAM format when 625 lines are detected."),
	DEFINE_SETTING_ENTRY("AFD_PAL60_DIS", Video_Mode_Control_4, 5, 1, RW, 0x0000, AFD_PAL60_DIS_value_map, "Disable the auto detection of PAL-60 formats."),
	DEFINE_SETTING_ENTRY("OUT_MODE", Video_Out_Control_1, 0, 2, RW, 0x0001, OUT_MODE_value_map, "Selects video output format"),
	DEFINE_SETTING_ENTRY("MODE10B", Video_Out_Control_1, 2, 1, RW, 0x0000, MODE10B_value_map, "Selects either 8-bit or 10-bit output for 4:2:2 Luma and Chroma"),
	DEFINE_SETTING_ENTRY("VBIHACTRAW_EN", Video_Out_Control_1, 3, 1, RW, 0x0000, VBIHACTRAW_EN_value_map, "Enables raw data output during the horizontal active region of the"),
	DEFINE_SETTING_ENTRY("ANC_DATA_EN", Video_Out_Control_1, 4, 1, RW, 0x0001, ANC_DATA_EN_value_map, "Enable Ancillary Data Insertion for BT.656 or VIP modes."),
	DEFINE_SETTING_ENTRY("TASKBIT_VAL", Video_Out_Control_1, 5, 1, RW, 0x0001, 0, "Task bit value in VIP 2 mode."),
	DEFINE_SETTING_ENTRY("BLUE_FIELD_ACT", Video_Out_Control_1, 6, 1, RW, 0x0000, 0, "Activate blue field on output regardless of BLUE_FIELD_EN"),
	DEFINE_SETTING_ENTRY("BLUE_FIELD_EN", Video_Out_Control_1, 7, 1, RW, 0x0000, BLUE_FIELD_EN_value_map, "Enable generation of blue field on output when decoder loses"),
	DEFINE_SETTING_ENTRY("CLAMPRAW_EN", Video_Out_Control_2, 0, 1, RW, 0x0001, CLAMPRAW_EN_value_map, "Enable clamping of raw ADC samples to 1254.75 when video"),
	DEFINE_SETTING_ENTRY("SWAPRAW", Video_Out_Control_2, 1, 1, RW, 0x0000, SWAPRAW_value_map, "Switch the positioning of the raw samples between the luma and"),
	DEFINE_SETTING_ENTRY("ACTFMT", Video_Out_Control_2, 2, 1, RW, 0x0001, ACTFMT_value_map, "Active signal format"),
	DEFINE_SETTING_ENTRY("VALIDFMT", Video_Out_Control_2, 3, 1, RW, 0x0000, VALIDFMT_value_map, "VALID signal format"),
	DEFINE_SETTING_ENTRY("HSFMT", Video_Out_Control_2, 4, 1, RW, 0x0000, HSFMT_value_map, "Selects width of HRESET_N"),
	DEFINE_SETTING_ENTRY("CLK_INVERT", Video_Out_Control_2, 5, 1, RW, 0x0001, 0, "When set, the pixel clock output is inverted."),
	DEFINE_SETTING_ENTRY("CLK_GATING", Video_Out_Control_2, 6, 2, RW, 0x0000, CLK_GATING_value_map, "Select pixel clock gating scheme"),
	DEFINE_SETTING_ENTRY("DCMODE", Video_Out_Control_3, 0, 1, RW, 0x0000, DCMODE_value_map, "Determines the format of the data count field in ancillary data in"),
	DEFINE_SETTING_ENTRY("IDID0_SOURCE", Video_Out_Control_3, 1, 1, RW, 0x0000, IDID0_SOURCE_value_map, "Source of IDID0 byte in VIP ancillary data"),
	DEFINE_SETTING_ENTRY("VIP_OPT_AL", Video_Out_Control_3, 2, 1, RW, 0x0000, VIP_OPT_AL_value_map, "VIP optional active line enable. In VIP modes, the transition of"),
	DEFINE_SETTING_ENTRY("VIPBLANK_EN", Video_Out_Control_3, 3, 1, RW, 0x0000, VIPBLANK_EN_value_map, "Enable substitution of blanking data during horizontal and"),
	DEFINE_SETTING_ENTRY("VIPCLAMP_EN", Video_Out_Control_3, 4, 1, RW, 0x0000, VIPCLAMP_EN_value_map, "Clamp luma and chroma data in VIP modes (i.e., when"),
	DEFINE_SETTING_ENTRY("POLAR", Video_Out_Control_4, 0, 7, RW, 0x0000, 0, "When bit is set, invert polarity of output"),
	DEFINE_SETTING_ENTRY("IDID0_9_2__LOW", Ancillary_IDID_0, 0, 8, RW, 0x0000, 0, "Value for IDID0 byte in VIP ancillary data. ([9:2] are used in 8-bit"),
	DEFINE_SETTING_ENTRY("IDID1_9_2__LOW", Ancillary_IDID_1, 0, 8, RW, 0x0000, 0, "Value for IDID0 byte in VIP ancillary data. ([9:2] are used in 8-bit"),
	DEFINE_SETTING_ENTRY("IDID0_1_0__HIGH", Ancillary_IDID_0_1, 0, 2, RW, 0x0000, 0, "Value for IDID0 byte in VIP ancillary data. ([1:0] are used in"),
	DEFINE_SETTING_ENTRY("IDID1_1_0__HIGH", Ancillary_IDID_0_1, 2, 2, RW, 0x0000, 0, "Value for IDID1 byte in VIP ancillary data. ([1:0] are used in"),
	DEFINE_SETTING_ENTRY("EN_A", Ancillary_Audio_Output_Control, 0, 1, RW, 0x0000, 0, "This bit enables data insertion from the FIFO_A in the VOF into"),
	DEFINE_SETTING_ENTRY("EN_B", Ancillary_Audio_Output_Control, 1, 1, RW, 0x0000, 0, "This bit enables data insertion from the FIFO_B in the VOF into"),
	DEFINE_SETTING_ENTRY("EN_C", Ancillary_Audio_Output_Control, 2, 1, RW, 0x0000, 0, "This bit enables data insertion from the FIFO_C in the VOF into"),
	DEFINE_SETTING_ENTRY("AUD_ANC_EN", Ancillary_Audio_Output_Control, 3, 1, RW, 0x0000, AUD_ANC_EN_value_map, "This bit enables audio ancillary data insertion. This bit, along"),
	DEFINE_SETTING_ENTRY("SAMPLE_RATE", Ancillary_Audio_Output_Control, 4, 2, RW, 0x0000, SAMPLE_RATE_value_map, "This indicates the sampling rate of the audio data available for"),
	DEFINE_SETTING_ENTRY("AUD_GRP", Ancillary_Audio_Output_Control, 6, 2, RW, 0x0000, AUD_GRP_value_map, "This field indicates the audio group that particular data flow"),
	DEFINE_SETTING_ENTRY("MV_CDAT", Copy_Protection_Status, 0, 2, RO, 0x0000, 0, "Macrovision Copy Control Bits as described in the MacroVision"),
	DEFINE_SETTING_ENTRY("MV_PSP", Copy_Protection_Status, 4, 1, RO, 0x0000, 0, "Macrovision Pseudo Sync Pulses detected"),
	DEFINE_SETTING_ENTRY("MV_CS", Copy_Protection_Status, 5, 1, RO, 0x0000, 0, "Macrovision Color Striping Detected"),
	DEFINE_SETTING_ENTRY("MV_T3CS", Copy_Protection_Status, 6, 1, RO, 0x0000, 0, "A 1 indicates the presence of type 3 of the color stripe process. A"),
	DEFINE_SETTING_ENTRY("MV_TYPE2_PAIR", Copy_Protection_Status, 7, 1, RO, 0x0000, 0, "Macrovision Type 2 pair detected"),
	DEFINE_SETTING_ENTRY("AFD_FMT_STAT", General_Status_1, 0, 4, RO, 0x0001, AFD_FMT_STAT_value_map, "Currently detected Format"),
	DEFINE_SETTING_ENTRY("FIELD", General_Status_1, 4, 1, RO, 0x0000, 0, "Field status (even/odd)"),
	DEFINE_SETTING_ENTRY("SRC_FIFO_OFLOW", General_Status_1, 5, 1, RO, 0x0000, 0, "Sample Rate Converter FIFO Overflow"),
	DEFINE_SETTING_ENTRY("SRC_FIFO_UFLOW", General_Status_1, 6, 1, RO, 0x0000, 0, "Sample Rate Converter FIFO Underflow"),
	DEFINE_SETTING_ENTRY("VSYNC", General_Status_1, 7, 1, RO, 0x0000, 0, "Vertical sync"),
	DEFINE_SETTING_ENTRY("HLOCK", General_Status_2, 0, 1, RO, 0x0000, 0, "Horizontal lock status"),
	DEFINE_SETTING_ENTRY("SRC_LOCK", General_Status_2, 1, 1, RO, 0x0000, 0, "Sample Rate Converter lock Status"),
	DEFINE_SETTING_ENTRY("VLOCK", General_Status_2, 2, 1, RO, 0x0000, 0, "Vertical lock status"),
	DEFINE_SETTING_ENTRY("CSC_LOCK", General_Status_2, 3, 1, RO, 0x0000, 0, "Color Subcarrier lock status"),
	DEFINE_SETTING_ENTRY("AGC_LOCK", General_Status_2, 4, 1, RO, 0x0000, 0, "VGA lock status"),
	DEFINE_SETTING_ENTRY("VPRES", General_Status_2, 5, 1, RO, 0x0000, 0, "Active-high video present. Indication of the presence of a"),
	DEFINE_SETTING_ENTRY("SPECIAL_PLAY_N", General_Status_2, 6, 1, RO, 0x0000, 0, "Active-low special play mode (fast forward, rewind, pulse, or slow"),
	DEFINE_SETTING_ENTRY("SRC_FIFO_OFLOW_STAT", Interrupt_Status_1, 0, 1, RR, 0x0000, 0, "The detection of a SRC FIFO Overflow sets this bit.Production Data Sheet5-60 Conexant 102284A"),
	DEFINE_SETTING_ENTRY("SRC_FIFO_UFLOW_STAT", Interrupt_Status_1, 1, 1, RR, 0x0000, 0, "The detection of a SRC FIFO Underflow sets this bit."),
	DEFINE_SETTING_ENTRY("CSC_LOCK_CHANGE_STAT", Interrupt_Status_1, 2, 1, RR, 0x0000, 0, "A change in the Color Subcarrier Lock status sets this bit."),
	DEFINE_SETTING_ENTRY("VLOCK_CHANGE_STAT", Interrupt_Status_1, 3, 1, RR, 0x0000, 0, "A change in the vertical lock status sets this bit."),
	DEFINE_SETTING_ENTRY("HLOCK_CHANGE_STAT", Interrupt_Status_1, 4, 1, RR, 0x0000, 0, "A change in the horizontal lock status sets this bit."),
	DEFINE_SETTING_ENTRY("VSYNC_TRAIL_STAT", Interrupt_Status_1, 5, 1, RR, 0x0000, 0, "The falling edge of the detected VSYNC sets this bit."),
	DEFINE_SETTING_ENTRY("FMT_CHANGE_STAT", Interrupt_Status_1, 6, 1, RR, 0x0000, 0, "A change in the detected video format sets this bit."),
	DEFINE_SETTING_ENTRY("END_VBI_ODD_STAT", Interrupt_Status_1, 7, 1, RR, 0x0000, 0, "The end of the VBI region of an odd field sets this bit."),
	DEFINE_SETTING_ENTRY("END_VBI_EVEN_STAT", Interrupt_Status_2, 0, 1, RR, 0x0000, 0, "The end of the VBI region of an even field sets this bit."),
	DEFINE_SETTING_ENTRY("MV_CHANGE_STAT", Interrupt_Status_2, 1, 1, RR, 0x0000, 0, "A change in the MV_CDAT field sets this bit."),
	DEFINE_SETTING_ENTRY("VPRES_CHANGE_STAT", Interrupt_Status_2, 2, 1, RR, 0x0000, 0, "A change in the VPRES (video present) status bit sets this bit."),
	DEFINE_SETTING_ENTRY("CC_DAT_AVAIL_STAT", Interrupt_Status_2, 3, 1, RR, 0x0000, 0, "VBI FIFO for Closed Caption has data (not empty)."),
	DEFINE_SETTING_ENTRY("GS1_DAT_AVAIL_STAT", Interrupt_Status_2, 4, 1, RR, 0x0000, 0, "VBI FIFO for Gemstar 1X has data (not empty)."),
	DEFINE_SETTING_ENTRY("GS2_DAT_AVAIL_STAT", Interrupt_Status_2, 5, 1, RR, 0x0000, 0, "VBI FIFO for Gemstar 2X has data (not empty)."),
	DEFINE_SETTING_ENTRY("WSS_DAT_AVAIL_STAT", Interrupt_Status_2, 6, 1, RR, 0x0000, 0, "VBI FIFO for Wide-Screen-Signaling has data (not empty)."),
	DEFINE_SETTING_ENTRY("SRC_FIFO_OFLOW_MSK", Interrupt_Mask_1, 0, 1, RW, 0x0001, 0, "When set, SRC_FIFO_OFLOW_STAT is masked from generating"),
	DEFINE_SETTING_ENTRY("SRC_FIFO_UFLOW_MSK", Interrupt_Mask_1, 1, 1, RW, 0x0001, 0, "When set, SRC_FIFO_UFLOW_STAT is masked from generating"),
	DEFINE_SETTING_ENTRY("CSC_LOCK_CHANGE_MSK", Interrupt_Mask_1, 2, 1, RW, 0x0001, 0, "When set, CSC_LOCK_CHANGE_STAT is masked from"),
	DEFINE_SETTING_ENTRY("VLOCK_CHANGE_MSK", Interrupt_Mask_1, 3, 1, RW, 0x0001, 0, "When set, VLOCK_CHANGE_STAT is masked from generating"),
	DEFINE_SETTING_ENTRY("HLOCK_CHANGE_MSK", Interrupt_Mask_1, 4, 1, RW, 0x0001, 0, "When set, HLOCK_CHANGE_STAT is masked from generating"),
	DEFINE_SETTING_ENTRY("VSYNC_TRAIL_MSK", Interrupt_Mask_1, 5, 1, RW, 0x0001, 0, "When set, VSYNC_TRAIL_STAT is masked from generating an"),
	DEFINE_SETTING_ENTRY("FMT_CHANGE_MSK", Interrupt_Mask_1, 6, 1, RW, 0x0001, 0, "When set, FMT_CHANGE_STAT is masked from generating an"),
	DEFINE_SETTING_ENTRY("END_VBI_ODD_MSK", Interrupt_Mask_1, 7, 1, RW, 0x0001, 0, "When set, END_VBI_ODD_STAT is masked from generating an"),
	DEFINE_SETTING_ENTRY("END_VBI_EVEN_MSK", Interrupt_Mask_2, 0, 1, RW, 0x0001, 0, "When set, END_VBI_EVEN_STAT is masked from"),
	DEFINE_SETTING_ENTRY("MV_CHANGE_MSK", Interrupt_Mask_2, 1, 1, RW, 0x0001, 0, "When set, MV_CHANGE_STAT is masked from"),
	DEFINE_SETTING_ENTRY("VPRES_CHANGE_MSK", Interrupt_Mask_2, 2, 1, RW, 0x0001, 0, "When set, VPRES_CHANGE_STAT is masked"),
	DEFINE_SETTING_ENTRY("CC_DAT_AVAIL_MSK", Interrupt_Mask_2, 3, 1, RR, 0x0000, 0, "When set, CC_DAT_AVAIL_STAT from generating"),
	DEFINE_SETTING_ENTRY("GS1_DAT_AVAIL_MSK", Interrupt_Mask_2, 4, 1, RR, 0x0000, 0, "When set, GS1_DAT_AVAIL_STAT from"),
	DEFINE_SETTING_ENTRY("GS2_DAT_AVAIL_MSK", Interrupt_Mask_2, 5, 1, RR, 0x0000, 0, "When set, GS2_DAT_AVAIL_STAT from"),
	DEFINE_SETTING_ENTRY("WSS_DAT_AVAIL_MSK", Interrupt_Mask_2, 6, 1, RR, 0x0000, 0, "When set, WSS_DAT_AVAIL_STAT is masked"),
	DEFINE_SETTING_ENTRY("BRIGHT", Brightness, 0, 8, RW, 0x0000, 0, "Brightness offset. This value is effectively an offset that is added"),
	DEFINE_SETTING_ENTRY("CNTRST", Contrast, 0, 8, RW, 0x0080, 0, "Contrast multiply value. This value is a 1.7 number that is"),
	DEFINE_SETTING_ENTRY("PEAK_SEL", Luma_Control, 0, 2, RW, 0x0000, PEAK_SEL_value_map, "Select for peaking filter response. This value selects from the four"),
	DEFINE_SETTING_ENTRY("PEAK_EN", Luma_Control, 2, 1, RW, 0x0000, PEAK_EN_value_map, "Peaking enable"),
	DEFINE_SETTING_ENTRY("RANGE", Luma_Control, 4, 2, RW, 0x0000, RANGE_value_map, "Selects the allowed luma output range. This allows the user to"),
	DEFINE_SETTING_ENTRY("LUMA_CORE_SEL", Luma_Control, 6, 2, RW, 0x0000, LUMA_CORE_SEL_value_map, "Luma coring threshold select. This value determines the cutoff"),
	DEFINE_SETTING_ENTRY("HSCALE", Horizontal_Scaling_Low, 0, 24, RW, 0x0000, 0, "Least significant byte of Horizontal Scaling Ratio (hscale ="),
	DEFINE_SETTING_ENTRY("HFILT", Horizontal_Scaling_Control, 0, 2, RW, 0x0000, HFILT_value_map, "Low-pass filter select. This is used in the luma low-pass filter"),
	DEFINE_SETTING_ENTRY("VSCALE", Vertical_Scaling_Low, 0, 13, RW, 0x0000, 0, "Least significant byte of Vertical Scaling Ratio (vsf = 216"),
	DEFINE_SETTING_ENTRY("VFILT", Vertical_Scaling_Control, 0, 3, RW, 0x0000, VFILT_value_map, "These bits control the number of taps in the Vertical Scaling"),
	DEFINE_SETTING_ENTRY("VS_INTRLACE", Vertical_Scaling_Control, 3, 1, RW, 0x0000, VS_INTRLACE_value_map, "VS Interlace Format. The initial output phase must alternate if the"),
	DEFINE_SETTING_ENTRY("LINE_AVG_DIS", Vertical_Line_Control, 0, 1, RW, 0x0000, LINE_AVG_DIS_value_map, "PAL line average disable"),
	DEFINE_SETTING_ENTRY("USAT", Saturation_U, 0, 8, RW, 0x0080, 0, "Saturation adjust for U chroma"),
	DEFINE_SETTING_ENTRY("VSAT", Saturation_V, 0, 8, RW, 0x0080, 0, "Saturation adjust for V chroma"),
	DEFINE_SETTING_ENTRY("HUE", Hue, 0, 8, RW, 0x0000, 0, "Hue adjust. A 1-bit change causes the subcarrier phase to rotate"),
	DEFINE_SETTING_ENTRY("C_CORE_SEL", Chroma_Control, 0, 2, RW, 0x0000, C_CORE_SEL_value_map, "Chroma coring select."),
	DEFINE_SETTING_ENTRY("CHR_DELAY", Chroma_Control, 2, 3, RW, 0x0000, CHR_DELAY_value_map, "Chroma delay. A signed number representing the number of"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE1", VBI_Line_Control_1, 0, 8, RW, 0x0000, VBI_MD_LINE1_value_map, "1st VBI line data type"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE2", VBI_Line_Control_2, 0, 8, RW, 0x0000, 0, "2nd VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE3", VBI_Line_Control_3, 0, 8, RW, 0x0000, 0, "3rd VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE4", VBI_Line_Control_4, 0, 8, RW, 0x0000, 0, "4th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE5", VBI_Line_Control_5, 0, 8, RW, 0x0000, 0, "5th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE6", VBI_Line_Control_6, 0, 8, RW, 0x0000, 0, "6th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE7", VBI_Line_Control_7, 0, 8, RW, 0x0000, 0, "7th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE8", VBI_Line_Control_8, 0, 8, RW, 0x0000, 0, "8th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE9", VBI_Line_Control_9, 0, 8, RW, 0x0000, 0, "9th VBI line data type. Valid programmed values are the same as"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE10", VBI_Line_Control_10, 0, 8, RW, 0x0000, 0, "10th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE11", VBI_Line_Control_11, 0, 8, RW, 0x0000, 0, "11th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE12", VBI_Line_Control_12, 0, 8, RW, 0x0000, 0, "12th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE13", VBI_Line_Control_13, 0, 8, RW, 0x0000, 0, "13th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE14", VBI_Line_Control_14, 0, 8, RW, 0x0000, 0, "14th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE15", VBI_Line_Control_15, 0, 8, RW, 0x0000, 0, "15th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE16", VBI_Line_Control_16, 0, 8, RW, 0x0000, 0, "16th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("VBI_MD_LINE17", VBI_Line_Control_17, 0, 8, RW, 0x0000, 0, "17th VBI line data type. Valid programmed values are the same"),
	DEFINE_SETTING_ENTRY("FC_SEARCH_MODE", VBI_Frame_Code_Search_Mode, 0, 1, RW, 0x0000, FC_SEARCH_MODE_value_map, "Frame code search mode. Allows dynamic search from frame"),
	DEFINE_SETTING_ENTRY("FC_ALT1_TYPE", VBI_Alternate_Frame_Code_Type, 0, 4, RW, 0x0000, 0, "When this field matches the VBIMODE setting for a particular"),
	DEFINE_SETTING_ENTRY("FC_ALT2_TYPE", VBI_Alternate_Frame_Code_Type, 4, 4, RW, 0x0000, 0, "When this field matches the VBIMODE setting for a particular"),
	DEFINE_SETTING_ENTRY("FC_ALT1", VBI_Alternate_1_Frame_Code, 0, 8, RW, 0x0000, 0, "Alternate frame code used when VBIMODE matches"),
	DEFINE_SETTING_ENTRY("FC_ALT2", VBI_Alternate_2_Frame_Code, 0, 8, RW, 0x0000, 0, "Alternate frame code used when VBIMODE matches"),
	DEFINE_SETTING_ENTRY("ADAPT_SLICE_DIS", VBI_Miscellaneous_Config_1, 0, 1, RW, 0x0000, ADAPT_SLICE_DIS_value_map, "Disable adaptive slice level"),
	DEFINE_SETTING_ENTRY("EDGE_RESYNC_EN", VBI_Miscellaneous_Config_1, 1, 1, RW, 0x0001, EDGE_RESYNC_EN_value_map, "Enable dynamic timing resynchronization based on edge"),
	DEFINE_SETTING_ENTRY("CRI_MARG_SCALE", VBI_Miscellaneous_Config_1, 2, 2, RW, 0x0001, CRI_MARG_SCALE_value_map, "Clock run-in margin scale. Used to loosen or tighten lock criteria"),
	DEFINE_SETTING_ENTRY("VPS_DEC_DIS", VBI_Miscellaneous_Config_1, 4, 1, RW, 0x0000, VPS_DEC_DIS_value_map, "VPS biphase decode disable."),
	DEFINE_SETTING_ENTRY("MOJI_PACK_DIS", VBI_Miscellaneous_Config_1, 5, 1, RW, 0x0000, MOJI_PACK_DIS_value_map, "Moji packing disable."),
	DEFINE_SETTING_ENTRY("TTX_PKTADRL_LB", TTX_Packet_Address_1, 0, 8, RW, 0x0000, 0, "Low byte of Teletext packet address lower limit for packet"),
	DEFINE_SETTING_ENTRY("TTX_PKTADRL_HN", TTX_Packet_Address_2, 0, 4, RW, 0x0000, 0, "High nibble of Teletext packet address lower limit for packet"),
	DEFINE_SETTING_ENTRY("TTX_PKTADRU_LN", TTX_Packet_Address_2, 4, 4, RW, 0x0457, 0, "Low nibble of Teletext packet address upper limit for packet"),
	DEFINE_SETTING_ENTRY("TTX_PKTADRU_HB", TTX_Packet_Address_3, 0, 8, RW, 0x00ff, 0, "High byte of Teletext packet address upper limit for packet"),
	DEFINE_SETTING_ENTRY("VBI1_SDID", VBI_1_and_2_SDID, 0, 4, RW, 0x03e9, 0, "SDID to use when VBI_CUST1 data type is selected."),
	DEFINE_SETTING_ENTRY("VBI2_SDID", VBI_1_and_2_SDID, 4, 4, RW, 0x03f2, 0, "SDID to use when VBI_CUST2 data type is selected."),
	DEFINE_SETTING_ENTRY("VBI3_SDID", VBI_3_SDID, 0, 4, RW, 0x03f3, 0, "SDID to use when VBI_CUST3 data type is selected."),
	DEFINE_SETTING_ENTRY("CC_FIFO_RST", VBI_FIFO_Reset, 0, 1, RW, 0x0000, 0, "When = 1, reset Closed Caption/XDS payload FIFO"),
	DEFINE_SETTING_ENTRY("GS1_FIFO_RST", VBI_FIFO_Reset, 1, 1, RW, 0x0000, 0, "When = 1, reset Gemstar1x payload FIFO"),
	DEFINE_SETTING_ENTRY("GS2_FIFO_RST", VBI_FIFO_Reset, 2, 1, RW, 0x0000, 0, "When = 1, reset Gemstar2x payload FIFO"),
	DEFINE_SETTING_ENTRY("WSS_FIFO_RST", VBI_FIFO_Reset, 3, 1, RW, 0x0000, 0, "When = 1, reset WSS payload FIFO"),
	DEFINE_SETTING_ENTRY("HAMMING_TYPE", VBI_Hamming, 0, 4, RW, 0x0000, 0, "When this field matches the VBIMODE setting for a"),
	DEFINE_SETTING_ENTRY("CC_STAT", Closed_Caption_Status, 0, 8, RO, 0x0000, 0, "Generic payload status format:"),
	DEFINE_SETTING_ENTRY("CC_FIFO_DAT", Closed_Caption_Data, 0, 8, RO, 0x0000, 0, "0x00 CC_FIFO_DAT CC/XDS payload data. Data pointer advanced after reading"),
	DEFINE_SETTING_ENTRY("GS1_STAT", GEMSTAR_1x_Status, 0, 8, RO, 0x0000, 0, "See description of CC_STAT byte."),
	DEFINE_SETTING_ENTRY("GS1_FIFO_DAT", GEMSTAR_1x_Data, 0, 8, RO, 0x0000, 0, "0x00 GS1_FIFO_DAT Gemstar 1x payload data. Data pointer advanced after"),
	DEFINE_SETTING_ENTRY("GS2_STAT", GEMSTAR_2x_Status, 0, 8, RO, 0x0000, 0, "See description of CC_STAT byte"),
	DEFINE_SETTING_ENTRY("GS2_FIFO_DAT", GEMSTAR_2x_Data, 0, 8, RO, 0x0000, 0, "0x00 GS2_FIFO_DAT Gemstar 2x payload data. Data pointer advanced after"),
	DEFINE_SETTING_ENTRY("WSS_STAT", WSS_Status, 0, 8, RO, 0x0000, 0, "See description of CC_STAT byte (PARERR will be held at"),
	DEFINE_SETTING_ENTRY("WSS_FIFO_DAT", WSS_Data, 0, 8, RO, 0x0000, 0, "0x00 WSS_FIFO_DAT Wide Screen Signaling payload data. Data pointer advanced"),
	DEFINE_SETTING_ENTRY("VBI1_HDELAY", VBI_Custom_1_Horizontal_Delay, 0, 8, RW, 0x0070, 0, "Delay from internal hreset signal to start of clock run-in in"),
	DEFINE_SETTING_ENTRY("VBI1_BITINC", VBI_Custom_1_Bit_Increment, 0, 12, RW, 0x0099, 0, "Least significant byte of the value used to increment a 14-bit"),
	DEFINE_SETTING_ENTRY("VBI1_SLICE_DIST", VBI_Custom_1_Slice_Distance, 4, 4, RW, 0x0001, 0, "Slice level sample distance. Controls how often samples are"),
	DEFINE_SETTING_ENTRY("VBI1_CRWIN", VBI_Custom_1_Clock_Run_in_Window, 0, 7, RW, 0x002c, 0, "Specifies the time window during which edge detection (positive"),
	DEFINE_SETTING_ENTRY("VBI1_FRAME_CODE", VBI_Custom_1_Frame_Code_Low, 0, 24, RW, 0x000001, 0, "Least significant byte of start code bit pattern in transmission"),
	DEFINE_SETTING_ENTRY("VBI1_FC_LENGTH", VBI_Custom_1_Frame_Code_Length, 0, 5, RW, 0x0003, 0, "Number of start bits (or frame code bits)"),
	DEFINE_SETTING_ENTRY("VBI1_CRI_TIME", VBI_Custom_1_Clock_Run_in_Period, 0, 8, RW, 0x000d, 0, "Expected time period of clock run-in period in terms of halves of"),
	DEFINE_SETTING_ENTRY("VBI1_CRI_MARGIN", VBI_Custom_1_Clock_Run_in_Margin_and_Length, 0, 4, RW, 0x0004, 0, "This field specifies the margin around VBI1_CRI_TIME in which"),
	DEFINE_SETTING_ENTRY("VBI1_CRI_LENGTH", VBI_Custom_1_Clock_Run_in_Margin_and_Length, 4, 4, RW, 0x000c, 0, "Number of clock run-in edges expected in the CRI period. This"),
	DEFINE_SETTING_ENTRY("VBI1_PAYLD_LENGTH", VBI_Custom_1_Payload_Length, 0, 8, RW, 0x0008, 0, "Number of data bits to be captured, divided by two. If N is the"),
	DEFINE_SETTING_ENTRY("VBI1_FORMAT_TYPE", VBI_Custom_1_Miscellaneous, 0, 4, RW, 0x0006, VBI1_FORMAT_TYPE_value_map, "Specifies basic VBI decoding model. Find the standard format"),
	DEFINE_SETTING_ENTRY("VBI1_FIFO_MODE", VBI_Custom_1_Miscellaneous, 4, 3, RW, 0x0008, VBI1_FIFO_MODE_value_map, "Determines which payload FIFO is loaded."),
	DEFINE_SETTING_ENTRY("VBI1_HAM_EN", VBI_Custom_1_Miscellaneous, 7, 1, RW, 0x0000, 0, "Enable hamming comparison for framing comparison when"),
	DEFINE_SETTING_ENTRY("VBI2_HDELAY", VBI_Custom_2_Horizontal_Delay, 0, 8, RW, 0x0077, 0, "Delay from internal hreset signal to start of clock run-in in"),
	DEFINE_SETTING_ENTRY("VBI2_BITINC", VBI_Custom_2_Bit_Increment, 0, 12, RW, 0x0088, 0, "Least significant byte of the value used to increment a 14-bit"),
	DEFINE_SETTING_ENTRY("VBI2_SLICE_DIST", VBI_Custom_2_Slice_Distance, 4, 4, RW, 0x0000, 0, "Slice level sample distance. Controls how often samples are"),
	DEFINE_SETTING_ENTRY("VBI1_CRIWIN", VBI_Custom_2_Clock_Run_In_Window, 0, 7, RW, 0x002c, 0, "Specifies the time window during which edge detection"),
	DEFINE_SETTING_ENTRY("VBI2_FRAME_CODE", VBI_Custom_2_Frame_Code_Low, 0, 24, RW, 0x000000, 0, "Least significant byte of start code bit pattern in transmission"),
	DEFINE_SETTING_ENTRY("VBI2_FC_LENGTH", VBI_Custom_2_Frame_Code_Length, 0, 5, RW, 0x0000, 0, "Number of start bits (or frame code bits)"),
	DEFINE_SETTING_ENTRY("VBI2_CRI_TIME", VBI_Custom_2_Clock_Run_in_Period, 0, 8, RW, 0x0002, 0, "Expected time period of clock run-in period in terms of"),
	DEFINE_SETTING_ENTRY("VBI2_CRI_MARGIN", VBI_Custom_2_Clock_Run_in_Margin_and_Length, 0, 4, RW, 0x0004, 0, "This field specifies the margin around VBI1_CRI_TIME in"),
	DEFINE_SETTING_ENTRY("VBI2_CRI_LENGTH", VBI_Custom_2_Clock_Run_in_Margin_and_Length, 4, 4, RW, 0x0001, 0, "Number of clock run-in edges expected in the CRI period."),
	DEFINE_SETTING_ENTRY("VBI2_PAYLD_LENGTH", VBI_Custom_2_Payload_Length, 0, 8, RW, 0x000a, 0, "Number of data bits to be captured, divided by two. If N is the"),
	DEFINE_SETTING_ENTRY("VBI2_FORMAT_TYPE", VBI_Custom_2_Miscellaneous, 0, 4, RW, 0x0004, VBI2_FORMAT_TYPE_value_map, "Specifies basic VBI decoding model. Find the standard format"),
	DEFINE_SETTING_ENTRY("VBI2_FIFO_MODE", VBI_Custom_2_Miscellaneous, 4, 3, RW, 0x0009, VBI2_FIFO_MODE_value_map, "Determines which payload FIFO is loaded."),
	DEFINE_SETTING_ENTRY("VBI2_HAM_EN", VBI_Custom_2_Miscellaneous, 7, 1, RW, 0x0000, 0, "Enable hamming comparison for framing comparison when"),
	DEFINE_SETTING_ENTRY("VBI3_HDELAY", VBI_Custom_3_Horizontal_Delay, 0, 8, RW, 0x006e, 0, "Delay from internal hreset signal to start of clock run-in in"),
	DEFINE_SETTING_ENTRY("VBI3_BITINC", VBI_Custom_3_Bit_Increment, 0, 12, RW, 0x06ca, 0, "Least significant byte of the value used to increment a 14-bit"),
	DEFINE_SETTING_ENTRY("VBI3_SLICE_DIST", VBI_Custom_3_Slice_Distance, 4, 4, RW, 0x0003, 0, "Slice level sample distance. Controls how often samples are"),
	DEFINE_SETTING_ENTRY("VBI3_CRWIN", VBI_Custom_3_Clock_Run_in_Window, 0, 7, RW, 0x0006, 0, "Specifies the time window during which edge detection"),
	DEFINE_SETTING_ENTRY("VBI3_FRAME_CODE", VBI_Custom_3_Frame_Code_Low, 0, 24, RW, 0x00e7, 0, "Least significant byte of start code bit pattern in transmission"),
	DEFINE_SETTING_ENTRY("VBI3_FC_LENGTH", VBI_Custom_3_Frame_Code_Length, 0, 5, RW, 0x0008, 0, "Number of start bits (or frame code bits)"),
	DEFINE_SETTING_ENTRY("VBI3_CRI_TIME", VBI_Custom_3_Clock_Run_in_Period, 0, 8, RW, 0x0020, 0, "Expected time period of clock run-in period in terms of halves"),
	DEFINE_SETTING_ENTRY("VBI3_CRI_MARGIN", VBI_Custom_3_Clock_Run_in_Margin_and_Length, 0, 4, RW, 0x0006, 0, "This field specifies the margin around VBI3_CRI_TIME in"),
	DEFINE_SETTING_ENTRY("VBI3_CRI_LENGTH", VBI_Custom_3_Clock_Run_in_Margin_and_Length, 4, 4, RW, 0x000f, 0, "Number of clock run-in edges expected in the CRI period. This"),
	DEFINE_SETTING_ENTRY("VBI3_PAYLD_LENGTH", VBI_Custom_3_Payload_Length, 0, 8, RW, 0x0084, 0, "Number of data bits to be captured, divided by two. If N is the"),
	DEFINE_SETTING_ENTRY("VBI3_FORMAT_TYPE", VBI_Custom_3_Miscellaneous, 0, 4, RW, 0x0002, VBI3_FORMAT_TYPE_value_map, "Specifies basic VBI decoding model. Find the standard format"),
	DEFINE_SETTING_ENTRY("VBI3_FIFO_MODE", VBI_Custom_3_Miscellaneous, 4, 3, RW, 0x0000, VBI3_FIFO_MODE_value_map, "Determines which payload FIFO is loaded."),
	DEFINE_SETTING_ENTRY("VBI3_HAM_EN", VBI_Custom_3_Miscellaneous, 7, 1, RW, 0x0000, 0, "Enable hamming comparison for framing comparison when"),
	DEFINE_SETTING_ENTRY("HBLANK_CNT", Horizontal_Blanking_Delay_Low, 0, 10, VO, 0x007a, 0, "Lower 8-bits of horizontal blanking delay. It is number of"),
	DEFINE_SETTING_ENTRY("HACTIVE_CNT", Horizontal_Blanking_Delay_High, 4, 10, VO, 0x0000, 0, "Lower nibble of horizontal active region duration. It is the"),
	DEFINE_SETTING_ENTRY("BGDEL_CNT", Burst_Gate_Delay, 0, 8, VO, 0x005b, 0, "Burst gate delay. This value is used to generate the window"),
	DEFINE_SETTING_ENTRY("VBLANK_CNT", Vertical_Blanking_Delay_Low, 0, 10, VO, 0x0014, 0, "Lower 8-bits of vertical blanking delay. It is the number of"),
	DEFINE_SETTING_ENTRY("VACTIVE_CNT", Vertical_Blanking_Delay_High, 4, 10, VO, 0x00f7, 0, "Lower nibble of vertical active region duration. It is the"),
	DEFINE_SETTING_ENTRY("V656BLANK_CNT", Vertical_Blanking_Delay_656, 0, 8, VO, 0x0020, 0, "Vertical blanking for 656 output. Determines the timing of"),
	DEFINE_SETTING_ENTRY("SRC_DECIM_RATIO", SRC_Decimation_Ratio_Low, 0, 10, VO, 0x021f, 0, "Lower byte of sample rate converter decimation ratio."),
	DEFINE_SETTING_ENTRY("UV_LPF_SEL", Comb_Filter_Bandwidth_Select, 4, 2, VO, 0x0001, UV_LPF_SEL_value_map, "Selects U/V low-pass filter bandwidth:"),
	DEFINE_SETTING_ENTRY("LUMA_LPF_SEL", Comb_Filter_Bandwidth_Select, 6, 2, VO, 0x0001, LUMA_LPF_SEL_value_map, "Selects luma low-pass filter bandwidth:"),
	DEFINE_SETTING_ENTRY("LCOMB_2LN_EN", Comb_Filter_Enable, 1, 1, VO, 0x0001, LCOMB_2LN_EN_value_map, "Enables the adaptation algorithm to choose the 2-line luma"),
	DEFINE_SETTING_ENTRY("LCOMB_3LN_EN", Comb_Filter_Enable, 2, 1, VO, 0x0001, LCOMB_3LN_EN_value_map, "Enables the adaptation algorithm to choose the 3-line luma"),
	DEFINE_SETTING_ENTRY("CCOMB_2LN_EN", Comb_Filter_Enable, 5, 1, VO, 0x0001, CCOMB_2LN_EN_value_map, "Enables the adaptation algorithm to choose the 2-line chroma"),
	DEFINE_SETTING_ENTRY("CCOMB_3LN_EN", Comb_Filter_Enable, 6, 1, VO, 0x0001, CCOMB_3LN_EN_value_map, "Enables the adaptation algorithm to choose the 3-line chroma"),
	DEFINE_SETTING_ENTRY("SC_STEP", Subcarrier_Step_Size_Low, 0, 24, VO, 0x087c1f, 0, "Lower byte of the chroma subcarrier DTO step size value."),
	DEFINE_SETTING_ENTRY("VBI_OFFSET", VBI_Offset, 0, 5, VO, 0x0000, 0, "The offset in lines from VRESET/HCTL/PRGM3 to enable the"),
	DEFINE_SETTING_ENTRY("FIELD_COUNT", Field_Count_Low, 0, 10, VO, 0x0000, 0, "Lower byte of the count value. Counts fields continuously,"),
	DEFINE_SETTING_ENTRY("TEMPDEC", Temporal_Decimation, 0, 6, RW, 0x0000, 0, "This signal is the control for the temporal decimation logic."),
	DEFINE_SETTING_ENTRY("TDFIELD", Miscellaneous_Timing_Control, 1, 1, RW, 0x0000, 0, "This signal is an indication of whether the temporal"),
	DEFINE_SETTING_ENTRY("TDALGN", Miscellaneous_Timing_Control, 2, 1, RW, 0x0000, TDALGN_value_map, "Aligns start of decimation with even or odd field."),
	DEFINE_SETTING_ENTRY("HR32", Miscellaneous_Timing_Control, 3, 1, RW, 0x0000, HR32_value_map, "This bit controls the width of the HRESET output."),
	DEFINE_SETTING_ENTRY("VPRES_VERT_EN", Miscellaneous_Timing_Control, 7, 1, RW, 0x0000, VPRES_VERT_EN_value_map, "Enable for the vertical portion of the video present logic."),
	DEFINE_SETTING_ENTRY("VT_LINE_CNT_HYST", Video_Detect_Configuration, 4, 2, RW, 0x0000, VT_LINE_CNT_HYST_value_map, "Number of consecutive fields with approximately 525/2 or"),
	DEFINE_SETTING_ENTRY("DEBOUNCE_COUNT", Video_Detect_Configuration, 6, 2, RW, 0x0001, DEBOUNCE_COUNT_value_map, "Number of consecutive fields of detected video format stability"),
	DEFINE_SETTING_ENTRY("VGA_GAIN", VGA_Gain_Control, 0, 6, RW, 0x0020, 0, ""),
	DEFINE_SETTING_ENTRY("AGC_GAIN", AGC_Gain_Control_Low, 0, 12, RW, 0x0100, 0, ""),
	DEFINE_SETTING_ENTRY("CLAMP_LEVEL", Digital_Front_End_Control, 0, 3, RW, 0x0000, 0, "Analog clamp setting (if CLAMP_AUTO_EN = 0)"),
	DEFINE_SETTING_ENTRY("VBI_GATE_EN", Digital_Front_End_Control, 3, 1, RW, 0x0001, 0, "Enable gating of back porch updates during vertical blanking"),
	DEFINE_SETTING_ENTRY("VGA_AUTO_EN", Digital_Front_End_Control, 4, 1, RW, 0x0001, VGA_AUTO_EN_value_map, "VGA enable"),
	DEFINE_SETTING_ENTRY("VGA_CRUSH_EN", Digital_Front_End_Control, 5, 1, RW, 0x0001, 0, "ADC overflow protection enable (decreases VGA_SYNC if ADC"),
	DEFINE_SETTING_ENTRY("AGC_AUTO_EN", Digital_Front_End_Control, 6, 1, RW, 0x0001, AGC_AUTO_EN_value_map, "AGC enable"),
	DEFINE_SETTING_ENTRY("CLAMP_AUTO_EN", Digital_Front_End_Control, 7, 1, RW, 0x0001, CLAMP_AUTO_EN_value_map, "Analog clamp setting is tied to VGA gain"),
	DEFINE_SETTING_ENTRY("VGA_SYNC", VGA_Sync_Control, 0, 8, RW, 0x00dc, 0, "Sync pulse height out of ADC"),
	DEFINE_SETTING_ENTRY("VGA_TRACK_RANGE", VGA_Track_Range, 0, 8, RW, 0x0040, 0, "Minimum error of sync height before losing VGA lock"),
	DEFINE_SETTING_ENTRY("VGA_ACQUIRE_RANGE", VGA_Acquire_Range, 0, 8, RW, 0x0010, 0, "Maximum error of sync height before declaring VGA lock."),
	DEFINE_SETTING_ENTRY("DCC_LOOP_GAIN", DFE_Control, 0, 2, RW, 0x000a, 0, "Backporch clamp control loop gain = 2n/4"),
	DEFINE_SETTING_ENTRY("AGC_LOOP_GAIN", DFE_Control, 2, 2, RW, 0x000a, 0, "AGC control loop gain = 2n/4"),
	DEFINE_SETTING_ENTRY("SYNC_LOOP_GAIN", DFE_Control, 6, 2, RW, 0x000a, 0, "Sync level detect control loop gain = 2n/4"),
	DEFINE_SETTING_ENTRY("BP_LOOP_GAIN", Backporch_Loop_Gain, 0, 2, RW, 0x000a, 0, "Backporch level detect control loop gain = 2n/4"),
	DEFINE_SETTING_ENTRY("DFT_THRESHOLD", DFT_Threshold, 0, 8, RW, 0x003f, 0, "Correlator threshold for SC detect (threshold = 256 x setting)."),
	DEFINE_SETTING_ENTRY("BP_PERCENT", Backporch_Percent, 0, 8, RW, 0x00cd, 0, "Percent of line expected to be used above or equal to"),
	DEFINE_SETTING_ENTRY("PLL_MAX_OFFSET", PLL_Offset_Low, 0, 16, RW, 0x0300, 0, "Least significant byte of video PLL maximum adjustment"),
	DEFINE_SETTING_ENTRY("PLL_KI", PLL_Indirect_Loop_Gain, 0, 8, RW, 0x001f, 0, "PLL control loop indirect gain = 1/2(n+11)"),
	DEFINE_SETTING_ENTRY("PLL_KD", PLL_Direct_Loop_Gain, 0, 8, RW, 0x0016, 0, "PLL control loop indirect gain = 1/2n"),
	DEFINE_SETTING_ENTRY("HTL_KI", Horizontal_Tracking_Loop_Indirect_Gain, 0, 2, RW, 0x000a, 0, "Horizontal tracking loop indirect gain = 1/2n"),
	DEFINE_SETTING_ENTRY("HTL_KD", Horizontal_Tracking_Loop_Indirect_Gain, 4, 2, RW, 0x000a, 0, "Horizontal tracking loop indirect gain = 1/2n"),
	DEFINE_SETTING_ENTRY("LCOMB_ERR_LIMIT", Luma_Comb_Error_Limit_Max, 0, 8, RW, 0x0014, 0, "Maximum comb error before falling back to complementary"),
	DEFINE_SETTING_ENTRY("LUMA_THRESHOLD", Luma_Comb_Threshold, 0, 8, RW, 0x0000, 0, "Minimum chroma amplitude before using luma comb filter."),
	DEFINE_SETTING_ENTRY("CCOMB_ERR_LIMIT", Chroma_Comb_Error_Limit_Max, 0, 8, RW, 0x0050, 0, "Maximum comb error before falling back to notch filter mode."),
	DEFINE_SETTING_ENTRY("COMB_PHASE_LIMIT", Comb_Phase_Limit_Max, 0, 8, RW, 0x0020, 0, "Comb filter is enabled when the burst phase difference"),
	DEFINE_SETTING_ENTRY("SYNC_TIP_INC", White_Crush_Increment, 0, 6, RW, 0x000f, 0, "White crush increment value. This value is the step amount"),
	DEFINE_SETTING_ENTRY("SYNC_TIP_REDUCE", White_Crush_Decrement, 1, 6, RW, 0x0000, 0, "White crush decrement value. This value is the step amount"),
	DEFINE_SETTING_ENTRY("MAJ_SEL", White_Crush_Comparison_Point, 2, 2, RW, 0x000b, MAJ_SEL_value_map, "White crush majority comparison point select bits. This"),
	DEFINE_SETTING_ENTRY("MAJ_SEL_EN", White_Crush_Comparison_Point, 4, 1, RW, 0x0000, MAJ_SEL_EN_value_map, "Enables adaptive majority select logic."),
	DEFINE_SETTING_ENTRY("CRUSH_FREQ", White_Crush_Comparison_Point, 5, 1, RW, 0x0000, CRUSH_FREQ_value_map, "White crush adjust frequency. This bit is used to determine"),
	DEFINE_SETTING_ENTRY("WTW_EN", White_Crush_Comparison_Point, 6, 1, RW, 0x0000, WTW_EN_value_map, "Active-high enable for the white crush whiter-than-white"),
	DEFINE_SETTING_ENTRY("DFE_RST_MSK", Soft_Reset_Mask_1, 1, 1, RW, 0x0000, 0, "Masks soft reset for the Digital Front End module"),
	DEFINE_SETTING_ENTRY("SRC_RST_MSK", Soft_Reset_Mask_1, 2, 1, RW, 0x0000, 0, "Masks soft reset for the Sample Rate Converter module"),
	DEFINE_SETTING_ENTRY("YCSEP_RST_MSK", Soft_Reset_Mask_1, 3, 1, RW, 0x0000, 0, "Masks soft reset for the Y/C separation module"),
	DEFINE_SETTING_ENTRY("VTG_RST_MSK", Soft_Reset_Mask_1, 4, 1, RW, 0x0000, 0, "Masks soft reset for the Video Timing Generator module"),
	DEFINE_SETTING_ENTRY("LUMA_RST_MSK", Soft_Reset_Mask_1, 5, 1, RW, 0x0000, 0, "Masks soft reset for the Luma Datapath module"),
	DEFINE_SETTING_ENTRY("CHROMA_RST_MSK", Soft_Reset_Mask_1, 6, 1, RW, 0x0000, 0, "Masks soft reset for the Chroma Datapath module"),
	DEFINE_SETTING_ENTRY("SCALE_RST_MSK", Soft_Reset_Mask_1, 7, 1, RW, 0x0000, 0, "Masks soft reset for the Scaling module"),
	DEFINE_SETTING_ENTRY("VBI_RST_MSK", Soft_Reset_Mask_2, 0, 1, RW, 0x0000, 0, "Masks soft reset for the VBI slicer module"),
	DEFINE_SETTING_ENTRY("MVDET_RST_MSK", Soft_Reset_Mask_2, 1, 1, RW, 0x0000, 0, "Masks soft reset for the Macrovision Detect module"),
	DEFINE_SETTING_ENTRY("VOF_RST_MSK", Soft_Reset_Mask_2, 2, 1, RW, 0x0000, 0, "Masks soft reset for the Video Output Formatter module"),
	DEFINE_SETTING_ENTRY("REG_RST_MSK", Soft_Reset_Mask_2, 3, 1, RW, 0x0000, 0, "Masks soft reset for the Register module"),
	DEFINE_SETTING_ENTRY("VD_SOFT_RST", Soft_Reset_Mask_2, 7, 1, RW, 0x0000, 0, "Video decoder soft reset. Resets video decoder core per the"),
	DEFINE_SETTING_ENTRY("REV_ID", Version_ID, 0, 8, RO, 0x0002, 0, "Revision ID. The initial value is set to 0x01. This refers to"),
	DEFINE_SETTING_ENTRY("APL_DETECT_ENA", Miscellaneous_Diagnostic_Control, 0, 1, RW, 0x000a, APL_DETECT_ENA_value_map, "Enables detection of video pattern with one white line"),
	DEFINE_SETTING_ENTRY("DL_ADDR_LB", Download_Address_Low_Byte, 0, 8, RW, 0x0000, 0, "Address lower byte for embedded"),
	DEFINE_SETTING_ENTRY("DL_ADDR_HB", Download_Address_High_Byte, 0, 8, RW, 0x0000, 0, "Address higher byte for embedded"),
	DEFINE_SETTING_ENTRY("DL_DATA_CTL", Download_Data_Control, 0, 8, RW, 0x0000, 0, "Control data port for embedded microcontroller"),
	DEFINE_SETTING_ENTRY("DL_MAP", Download_Control, 0, 2, RW, 0x0000, 0, "Memory map mode"),
	DEFINE_SETTING_ENTRY("DL_AUTO_INC", Download_Control, 2, 1, RW, 0x0000, DL_AUTO_INC_value_map, "Increment Mode"),
	DEFINE_SETTING_ENTRY("DL_ENABLE", Download_Control, 3, 1, RW, 0x0000, DL_ENABLE_value_map, "Download Enable"),
	DEFINE_SETTING_ENTRY("START_MICROCNTL", Download_Control, 4, 1, RW, 0x0000, START_MICROCNTL_value_map, "+++VALUE 0 Embedded microcontroller is stopped"),
	DEFINE_SETTING_ENTRY("MOD_DET_STATUS0", Mode_Detect_Status_0, 0, 8, RO, 0x0000, 0, "Detected audio mode"),
	DEFINE_SETTING_ENTRY("MOD_DET_STATUS1", Mode_Detect_Status_1, 0, 8, RO, 0x0000, 0, "Detected audio standard"),
	DEFINE_SETTING_ENTRY("AUD_MODE_AUD_SYSTEM", Audio_Configuration, 0, 4, RW, 0x0000, 0, "Force audio mode:"),
	DEFINE_SETTING_ENTRY("AUD_STANDARD", Audio_Configuration, 4, 4, RW, 0x0000, 0, "Audio standard"),
	DEFINE_SETTING_ENTRY("PREF_MODE", Preferred_Decode_Mode, 0, 4, RW, 0x0000, PREF_MODE_value_map, "Note: Not all settings apply to all standards. If PREF_MODE is set to"),
	DEFINE_SETTING_ENTRY("MUTE_NO_PREF_MODE", Preferred_Decode_Mode, 4, 1, RW, 0x0000, MUTE_NO_PREF_MODE_value_map, "+++VALUE 0 Do not mute output if the preferred mode is not available."),
	DEFINE_SETTING_ENTRY("DE_EMPHASIS_TIME", Preferred_Decode_Mode, 5, 1, RW, 0x0000, DE_EMPHASIS_TIME_value_map, "+++VALUE 0 75 µs"),
	DEFINE_SETTING_ENTRY("FM_DEVIATION", Preferred_Decode_Mode, 6, 2, RW, 0x0000, FM_DEVIATION_value_map, "Only applicable for AUD_STANDARD (0x808) = 0x5 or when A2-DK"),
	DEFINE_SETTING_ENTRY("MICROCNTL_VIDEO_FORMAT", Embedded_Microcontroller_Video_Format, 0, 4, RO, 0x0000, 0, "Detected video format"),
	DEFINE_SETTING_ENTRY("VIDEO_PRESENT", Embedded_Microcontroller_Video_Format, 4, 1, RO, 0x0000, 0, "Video present signal"),
	DEFINE_SETTING_ENTRY("FORMAT_45MHZ", Audio_Format_Control_0, 0, 3, RW, 0x0000, FORMAT_45MHZ_value_map, "Only applicable when AUD_MODE/AUD_SYSTEM (0x808) = 0xF"),
	DEFINE_SETTING_ENTRY("FORMAT_65MHZ", Audio_Format_Control_0, 3, 2, RW, 0x0000, FORMAT_65MHZ_value_map, "Only applicable when AUD_MODE/AUD_SYSTEM (0x808) = 0xF"),
	DEFINE_SETTING_ENTRY("TUNER_OUTPUT_FORMAT", Audio_Format_Control_0, 5, 1, RW, 0x0000, TUNER_OUTPUT_FORMAT_value_map, "Only applicable when AUD_STANDARD (0x808) = 0x01 or 0xE"),
	DEFINE_SETTING_ENTRY("SOFT_RESET", Soft_Reset, 0, 1, RW, 0x0000, 0, "Soft reset to ensure smooth transition of operation modes."),
	DEFINE_SETTING_ENTRY("AC97_INT_DIS", Audio_Interrupt_Control, 0, 1, RW, 0x0001, AC97_INT_DIS_value_map, "0= AC97 interrupt enabled"),
	DEFINE_SETTING_ENTRY("AMC_INT_DIS", Audio_Interrupt_Control, 1, 1, RW, 0x0001, AMC_INT_DIS_value_map, "+++VALUE 0 Audio mode change interrupt enabled (i.e., mono to stereo, stereo to"),
	DEFINE_SETTING_ENTRY("FC_INT_DIS", Audio_Interrupt_Control, 2, 1, RW, 0x0001, FC_INT_DIS_value_map, "+++VALUE 0 Audio format change interrupt enabled"),
	DEFINE_SETTING_ENTRY("FDL_INT_DIS", Audio_Interrupt_Control, 3, 1, RW, 0x0001, FDL_INT_DIS_value_map, "+++VALUE 0 Format detection loop complete interrupt enabled"),
	DEFINE_SETTING_ENTRY("IFL_INT_DIS", Audio_Interrupt_Control, 4, 1, RW, 0x0001, IFL_INT_DIS_value_map, "+++VALUE 0 IF signal lost interrupt enabled"),
	DEFINE_SETTING_ENTRY("NLL_INT_DIS", Audio_Interrupt_Control, 5, 1, RW, 0x0001, NLL_INT_DIS_value_map, "+++VALUE 0 NICAM lost lock interrupt enabled"),
	DEFINE_SETTING_ENTRY("NBER_INT_DIS", Audio_Interrupt_Control, 6, 1, RW, 0x0001, NBER_INT_DIS_value_map, "+++VALUE 0 NICAM bit error rate too high interrupt enabled"),
	DEFINE_SETTING_ENTRY("RDS_INT_DIS", Audio_Interrupt_Control, 7, 1, RW, 0x0001, RDS_INT_DIS_value_map, "+++VALUE 0 RDS data ready interrupt enabled"),
	DEFINE_SETTING_ENTRY("AC97_INT", Audio_Interrupt_Status, 0, 1, RO, 0x0000, 0, "+++VALUE 1 AC97 interrupt asserted"),
	DEFINE_SETTING_ENTRY("AMC_INT", Audio_Interrupt_Status, 1, 1, RO, 0x0000, 0, "+++VALUE 1 Audio mode change interrupt asserted"),
	DEFINE_SETTING_ENTRY("AFC_INT", Audio_Interrupt_Status, 2, 1, RO, 0x0000, 0, "+++VALUE 1 Audio format change interrupt asserted"),
	DEFINE_SETTING_ENTRY("FDL_INT", Audio_Interrupt_Status, 3, 1, RO, 0x0000, 0, "+++VALUE 1 Format detection loop complete interrupt asserted"),
	DEFINE_SETTING_ENTRY("IFL_INT", Audio_Interrupt_Status, 4, 1, RW, 0x0000, 0, "+++VALUE 1 IF signal lost interrupt asserted"),
	DEFINE_SETTING_ENTRY("NLL_INT", Audio_Interrupt_Status, 5, 1, RO, 0x0000, 0, "+++VALUE 1 NICAM lost lock interrupt asserted"),
	DEFINE_SETTING_ENTRY("NBER_INT", Audio_Interrupt_Status, 6, 1, RO, 0x0000, 0, "+++VALUE 1 NICAM bit error rate too high interrupt asserted"),
	DEFINE_SETTING_ENTRY("RDS_INT", Audio_Interrupt_Status, 7, 1, RO, 0x0000, 0, "+++VALUE 1 RDS interrupt asserted"),
	DEFINE_SETTING_ENTRY("AAGC_HYST1", Analog_AGC_Hysteresis1, 0, 6, RW, 0x0005, 0, "Controls the hysteresis of the AGC so that the value to the VGA"),
	DEFINE_SETTING_ENTRY("AAGC_HYST2", Analog_AGC_Hysteresis2, 0, 6, RW, 0x0009, 0, "Controls the hysteresis of the AGC so that the value to the VGA"),
	DEFINE_SETTING_ENTRY("AAGC_TH", Analog_AGC_Control, 0, 6, RW, 0x000e, 0, "Bypass value for the AGC when AAGC_DEFAULT_EN is"),
	DEFINE_SETTING_ENTRY("AAGC_GAIN_EN", Analog_AGC_Control, 6, 1, RW, 0x0000, AAGC_GAIN_EN_value_map, "Controls the gain of the AGC feedback."),
	DEFINE_SETTING_ENTRY("AAGC_DEFAULT", AFE_12DB_ENABLE, 0, 6, RW, 0x0020, 0, "Bypass value for the AGC when AAGC_DEFAULT_EN is asserted."),
	DEFINE_SETTING_ENTRY("AAGC_DEFAULT_EN", AFE_12DB_ENABLE, 6, 1, RW, 0x0000, AAGC_DEFAULT_EN_value_map, "Enables bypass of the AGC."),
	DEFINE_SETTING_ENTRY("AFE_12DB_EN", AFE_12DB_ENABLE, 7, 1, RW, 0x0000, AFE_12DB_EN_value_map, "Enables a +12 dB gain in the analog front end (AFE). When using"),
	DEFINE_SETTING_ENTRY("IF_SRC_MODE", IF_SRC_Mode, 0, 1, RW, 0x0000, IF_SRC_MODE_value_map, "Controls IF SRC Decimation filter for different input"),
	DEFINE_SETTING_ENTRY("PHASE_FIX", Phase_Fix_Control, 0, 4, RW, 0x03e9, 0, "Phase fix delay measured in samples of delay at the"),
	DEFINE_SETTING_ENTRY("PH_CH_SEL", Phase_Fix_Control, 4, 1, RW, 0x0000, PH_CH_SEL_value_map, "Selects channel to be phase delayed"),
	DEFINE_SETTING_ENTRY("PH_DBX_SEL", Phase_Fix_Control, 5, 1, RW, 0x0000, 0, "Selects DBX input to be channel 2 when asserted."),
	DEFINE_SETTING_ENTRY("DEMATRIX_MODE", Dematrix_Bypass, 0, 2, RW, 0x0000, DEMATRIX_MODE_value_map, "+++VALUE 0 Dematrix Sum/Diff"),
	DEFINE_SETTING_ENTRY("DMTRX_BYPASS", Dematrix_Bypass, 2, 1, RW, 0x0000, DMTRX_BYPASS_value_map, "Dematrix bypass enable bit"),
	DEFINE_SETTING_ENTRY("DEMATRIX_SEL_CTL", Dematrix_Select_Control, 0, 8, RW, 0x0000, 0, "0x08 = BTSC force mono"),
	DEFINE_SETTING_ENTRY("SA_IN_SHIFT", Digital_Audio_Input_Shifter, 0, 4, RW, 0x0000, 0, "Serial audio input shift. Signed 2s complement format."),
	DEFINE_SETTING_ENTRY("AC97_IN_SHIFT", Digital_Audio_Input_Shifter, 4, 4, RW, 0x0000, 0, "AC97 input shift. Signed 2s complement format. Shift"),
	DEFINE_SETTING_ENTRY("PATH1_SEL_CTL", Path1_Audio_Select_Control, 0, 4, RW, 0x0000, 0, "Source select for the first baseband audio processing"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_RMS_CON", Path1_Audio_Select_Control, 4, 4, RW, 0x0049, 0, "Define AVC time gain for level detection"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_CR", Path1_AVC_Control1, 0, 3, RW, 0x0000, PATH1_AVC_CR_value_map, "Define path 1 AVC compression ratio"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_STEREO", Path1_AVC_Control1, 3, 1, RW, 0x0001, PATH1_AVC_STEREO_value_map, "+++VALUE 0 Independence Mode"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_AT", Path1_AVC_Control1, 4, 4, RW, 0x0009, PATH1_AVC_AT_value_map, "Define AVC attack time"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_RT", Path1_AVC_Control2, 0, 4, RW, 0x0048, PATH1_AVC_RT_value_map, "Define AVC release time"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_CG", Path1_AVC_Control2, 4, 2, RW, 0x0000, PATH1_AVC_CG_value_map, "Define AVC compression gain"),
	DEFINE_SETTING_ENTRY("SOFT1_MUTE_EN", Mute_Control, 0, 1, RW, 0x0001, SOFT1_MUTE_EN_value_map, "Soft mute enable"),
	DEFINE_SETTING_ENTRY("SRC1_MUTE_EN", Mute_Control, 1, 1, RW, 0x0000, SRC1_MUTE_EN_value_map, "Source mute enable"),
	DEFINE_SETTING_ENTRY("SA_MUTE_EN", Mute_Control, 2, 1, RW, 0x0000, SA_MUTE_EN_value_map, "Serial audio mute enable"),
	DEFINE_SETTING_ENTRY("PAR_MUTE_EN", Mute_Control, 3, 1, RW, 0x0000, PAR_MUTE_EN_value_map, "Ancillary Audio mute enable"),
	DEFINE_SETTING_ENTRY("AC97_MUTE_EN", Mute_Control, 4, 1, RW, 0x0000, AC97_MUTE_EN_value_map, "AC97 mute"),
	DEFINE_SETTING_ENTRY("PATH1_VOLUME", Path1_Volume, 0, 8, RW, 0x0024, 0, "Volume control in dB steps. +18 dB to 96 dB in ½ dB"),
	DEFINE_SETTING_ENTRY("PATH1_BAL_LEVEL", Path1_Balance, 0, 7, RW, 0x0000, 0, "Attenuation to be provided to the selected channel in"),
	DEFINE_SETTING_ENTRY("PATH1_BAL_LEFT", Path1_Balance, 7, 1, RW, 0x0000, 0, "Select left channel for balance control if 1, select right"),
	DEFINE_SETTING_ENTRY("PATH1_AVC_THRESH", Path1_AVC_Threshold_Low, 0, 16, RW, 0x7fff, 0, "Define AVC Threshold"),
	DEFINE_SETTING_ENTRY("PATH1_EQ_BAND_SEL", Path1_EQ_Band_Select, 0, 1, RW, 0x0000, PATH1_EQ_BAND_SEL_value_map, "Equalizer band selection"),
	DEFINE_SETTING_ENTRY("PATH1_EQ_BASS_VOL", Path1_EQ_Bass_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Bass Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH1_EQ_MID_VOL", Path1_EQ_Midrange_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Mid-tone Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH1_EQ_TREBLE_VOL", Path1_EQ_Treble_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Treble Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH1_SC_RMS_CON", Path1_Soft_Clip_Control1, 0, 4, RW, 0x0009, PATH1_SC_RMS_CON_value_map, "Define SC time gain for level detection"),
	DEFINE_SETTING_ENTRY("PATH1_SC_CR", Path1_Soft_Clip_Control1, 4, 3, RW, 0x0008, PATH1_SC_CR_value_map, "Define SC compression ratio"),
	DEFINE_SETTING_ENTRY("PATH1_SC_STEREO", Path1_Soft_Clip_Control1, 7, 1, RW, 0x0000, PATH1_SC_STEREO_value_map, "+++VALUE 0 Independence Mode"),
	DEFINE_SETTING_ENTRY("PATH1_SC_AT", Path1_Soft_Clip_Control2, 0, 4, RW, 0x0009, PATH1_SC_AT_value_map, "Define SC attack time"),
	DEFINE_SETTING_ENTRY("PATH1_SC_RT", Path1_Soft_Clip_Control2, 4, 4, RW, 0x0009, PATH1_SC_RT_value_map, "Define SC release time"),
	DEFINE_SETTING_ENTRY("PATH1_SC_THRESH", Path1_Soft_Clip_Threshold_Low, 0, 16, RW, 0x7fff, 0, "Define Soft Clip Threshold Low Byte"),
	DEFINE_SETTING_ENTRY("PATH2_SEL_CTL", Path2_Audio_Select_Control, 0, 4, RW, 0x0000, 0, "Source select for the first baseband audio"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_RMS_CON", Path2_Audio_Select_Control, 4, 4, RW, 0x0049, 0, "Define AVC time gain for level detection"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_CR", Path2_AVC_Control1, 0, 3, RW, 0x0000, PATH2_AVC_CR_value_map, "Define path 2 AVC compression ratio"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_STEREO", Path2_AVC_Control1, 3, 1, RW, 0x0001, PATH2_AVC_STEREO_value_map, "+++VALUE 0 Independence Mode"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_AT", Path2_AVC_Control1, 4, 4, RW, 0x0009, PATH2_AVC_AT_value_map, "Define AVC attack time"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_RT", Path2_AVC_Control2, 0, 4, RW, 0x0048, PATH2_AVC_RT_value_map, "Define AVC release time"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_CG", Path2_AVC_Control2, 4, 2, RW, 0x0000, PATH2_AVC_CG_value_map, "Define AVC compression gain"),
	DEFINE_SETTING_ENTRY("SOFT2_MUTE_EN", Path2_Mute_Control, 0, 1, RW, 0x0001, 0, "Soft mute enable"),
	DEFINE_SETTING_ENTRY("SRC2_MUTE_EN", Path2_Mute_Control, 1, 1, RW, 0x0000, 0, "Source mute enable"),
	DEFINE_SETTING_ENTRY("PATH2_VOLUME", Path2_Volume, 0, 8, RW, 0x0024, 0, "Volume control in dB steps. +18 dB to 96 dB in ½ dB steps"),
	DEFINE_SETTING_ENTRY("PATH2_BAL_LEVEL", Path2_Balance, 0, 7, RW, 0x0000, 0, "Attenuation to be provided to the selected channel in dB."),
	DEFINE_SETTING_ENTRY("PATH2_BAL_LEFT", Path2_Balance, 7, 1, RW, 0x0000, 0, "Select left channel for balance control if 1, select right"),
	DEFINE_SETTING_ENTRY("PATH2_AVC_THRESH", Path2_AVC_Threshold_Low, 0, 16, RW, 0x0400, 0, "Define AVC Threshold"),
	DEFINE_SETTING_ENTRY("PATH2_EQ_BAND_SEL", Path2_EQ_Band_Select, 0, 1, RW, 0x0000, PATH2_EQ_BAND_SEL_value_map, "Equalizer band selection"),
	DEFINE_SETTING_ENTRY("PATH2_EQ_BASS_VOL", Path2_EQ_Bass_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Bass Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH2_EQ_MID_VOL", Path2_EQ_Midrange_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Mid-tone Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH2_EQ_TREBLE_VOL", Path2_EQ_Treble_Adjust, 0, 6, RW, 0x0000, 0, "Equalizer Treble Volume control in dB steps."),
	DEFINE_SETTING_ENTRY("PATH2_SC_RMS_CON", Path2_Soft_Clip_Control1, 0, 4, RW, 0x0009, PATH2_SC_RMS_CON_value_map, "Define SC time gain for level detection"),
	DEFINE_SETTING_ENTRY("PATH2_SC_CR", Path2_Soft_Clip_Control1, 4, 3, RW, 0x0008, PATH2_SC_CR_value_map, "Define SC compression ratio"),
	DEFINE_SETTING_ENTRY("PATH2_SC_STEREO", Path2_Soft_Clip_Control1, 7, 1, RW, 0x0000, PATH2_SC_STEREO_value_map, "+++VALUE 0 Independence Mode"),
	DEFINE_SETTING_ENTRY("PATH2_SC_AT", Path2_Soft_Clip_Control2, 0, 4, RW, 0x0009, PATH2_SC_AT_value_map, "Define SC attack time"),
	DEFINE_SETTING_ENTRY("PATH2_SC_RT", Path2_Soft_Clip_Control2, 4, 4, RW, 0x0009, PATH2_SC_RT_value_map, "Define SC release time"),
	DEFINE_SETTING_ENTRY("PATH2_SC_THRESH", Path2_Soft_Clip_Threshold_Low, 0, 16, RW, 0x7fff, 0, "Define Soft Clip Threshold Low Byte"),
	DEFINE_SETTING_ENTRY("Sample_Rate_Converter_Status1_REG", Sample_Rate_Converter_Status1, 0, 8, RW, 0x0000, 0, "Missing bit spec for reg"),
	DEFINE_SETTING_ENTRY("Sample_Rate_Converter_Status2_REG", Sample_Rate_Converter_Status2, 0, 8, RW, 0x0000, 0, "Missing bit spec for reg"),
	DEFINE_SETTING_ENTRY("SRC1_PHASE_INC", SRC1_Phase_Increment_Low, 0, 18, RW, 0x01867c, 0, "Low byte of SRC 1 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC1_FIFO_RD_TH", SRC1_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 1 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("SRC2_PHASE_INC", SRC2_Phase_Increment_Low, 0, 18, RW, 0x01867c, 0, "Low byte of SRC 2 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC2_FIFO_RD_TH", SRC2_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 2 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("SRC3_PHASE_INC", SRC3_Phase_Increment_Low, 0, 18, RW, 0x014faa, 0, "Low byte of SRC 3 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC3_FIFO_RD_TH", SRC3_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 3 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("SRC4_PHASE_INC", SRC4_Phase_Increment_Low, 0, 18, RW, 0x014faa, 0, "Low byte of SRC 4 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC4_FIFO_RD_TH", SRC4_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 4 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("SRC5_PHASE_INC", SRC5_Phase_Increment_Low, 0, 18, RW, 0x010453, 0, "Low byte of SRC 5 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC5_FIFO_RD_TH", SRC5_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 5 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("SRC6_PHASE_INC", SRC6_Phase_Increment_Low, 0, 18, RW, 0x014faa, 0, "Low byte of SRC 6 phase increment value. Calculated by"),
	DEFINE_SETTING_ENTRY("SRC6_FIFO_RD_TH", SRC6_FIFO_Read_Threshold, 0, 4, RW, 0x0008, 0, "SRC 6 FIFO threshold for Read Enable. Apply to both left"),
	DEFINE_SETTING_ENTRY("PARALLEL1_SRC_SEL", Output_SRC_Source_Select, 0, 2, RW, 0x0001, PARALLEL1_SRC_SEL_value_map, "Output source select."),
	DEFINE_SETTING_ENTRY("PARALLEL2_SRC_SEL", Output_SRC_Source_Select, 2, 2, RW, 0x000a, PARALLEL2_SRC_SEL_value_map, "Output source select."),
	DEFINE_SETTING_ENTRY("SERIAL_AUDIO_SRC_SEL", Output_SRC_Source_Select, 4, 2, RW, 0x0000, SERIAL_AUDIO_SRC_SEL_value_map, "Output source select."),
	DEFINE_SETTING_ENTRY("AC97_SRC_SEL", Output_SRC_Source_Select, 6, 2, RW, 0x000b, AC97_SRC_SEL_value_map, "Output source select."),
	DEFINE_SETTING_ENTRY("BASEBAND_BYPASS_CTL", Baseband_Bypass_Control, 0, 8, RW, 0x0000, BASEBAND_BYPASS_CTL_value_map, "Baseband block bypass control"),
	DEFINE_SETTING_ENTRY("SRC3_CLK_SEL", Input_SRC_Source_Select1, 0, 2, RW, 0x0000, SRC3_CLK_SEL_value_map, "SRC output FIFO read select"),
	DEFINE_SETTING_ENTRY("SRC3_IN_SEL", Input_SRC_Source_Select1, 2, 2, RW, 0x0000, SRC3_IN_SEL_value_map, "SRC input source select."),
	DEFINE_SETTING_ENTRY("SRC4_CLK_SEL", Input_SRC_Source_Select1, 4, 2, RW, 0x000b, SRC4_CLK_SEL_value_map, "SRC output FIFO read select"),
	DEFINE_SETTING_ENTRY("SRC4_IN_SEL", Input_SRC_Source_Select1, 6, 2, RW, 0x000a, SRC4_IN_SEL_value_map, "SRC input source select."),
	DEFINE_SETTING_ENTRY("SRC5_CLK_SEL", Input_SRC_Source_Select2, 0, 2, RW, 0x000a, SRC5_CLK_SEL_value_map, "SRC output fifo read select"),
	DEFINE_SETTING_ENTRY("SRC5_IN_SEL", Input_SRC_Source_Select2, 2, 2, RW, 0x0000, SRC5_IN_SEL_value_map, "SRC input source select"),
	DEFINE_SETTING_ENTRY("SRC6_CLK_SEL", Input_SRC_Source_Select2, 4, 2, RW, 0x0001, SRC6_CLK_SEL_value_map, "SRC output fifo read select"),
	DEFINE_SETTING_ENTRY("SRC6_IN_SEL", Input_SRC_Source_Select2, 6, 2, RW, 0x0000, SRC6_IN_SEL_value_map, "SRC input source select."),
	DEFINE_SETTING_ENTRY("SA_IN_BCN_DEL", Serial_Audio_Input_Control1, 0, 5, RW, 0x0000, 0, "Controls a number of SA_BCLK_IN cycles delay for"),
	DEFINE_SETTING_ENTRY("SA_IN_WS_SEL", Serial_Audio_Input_Control1, 5, 1, RW, 0x0000, SA_IN_WS_SEL_value_map, "Control for Word Select (SA_WCLK_IN) polarity."),
	DEFINE_SETTING_ENTRY("SA_IN_RIGHT_JUST", Serial_Audio_Input_Control1, 6, 1, RW, 0x0000, SA_IN_RIGHT_JUST_value_map, "+++VALUE 0 Left justified serial data"),
	DEFINE_SETTING_ENTRY("SA_IN_SONY_MODE", Serial_Audio_Input_Control1, 7, 1, RW, 0x0000, SA_IN_SONY_MODE_value_map, "+++VALUE 0 Philips mode: 2nd SA_BCLK_IN rising edge after"),
	DEFINE_SETTING_ENTRY("SA_IN_MASTER_MODE", Serial_Audio_Input_Control2, 0, 1, RW, 0x0000, SA_IN_MASTER_MODE_value_map, "+++VALUE 0 Slave operation"),
	DEFINE_SETTING_ENTRY("SA_UP2X_BYPASS", Serial_Audio_Input_Control2, 1, 1, RW, 0x0000, 0, "Enable bypass mode for greater than 48 kHz sample rate"),
	DEFINE_SETTING_ENTRY("SA_OUT_BCNT_DEL", Serial_Audio_Output_Control1, 0, 5, RW, 0x0000, 0, "Controls a number of SA_BCLK_OUT cycles delay for"),
	DEFINE_SETTING_ENTRY("SA_OUT_WS_SEL", Serial_Audio_Output_Control1, 5, 1, RW, 0x0000, SA_OUT_WS_SEL_value_map, "Control for Word Select (SA_WCLK_OUT) polarity."),
	DEFINE_SETTING_ENTRY("SA_OUT_RIGHT_JUST", Serial_Audio_Output_Control1, 6, 1, RW, 0x0000, SA_OUT_RIGHT_JUST_value_map, "+++VALUE 0 Left justified serial data"),
	DEFINE_SETTING_ENTRY("SA_OUT_SONY_MODE", Serial_Audio_Output_Control1, 7, 1, RW, 0x0000, SA_OUT_SONY_MODE_value_map, "+++VALUE 0 Philips mode: 2nd SA_BCLK_OUT rising edge after"),
	DEFINE_SETTING_ENTRY("SA_OUT_MASTER_MODE", Serial_Audio_Output_Control2, 0, 1, RW, 0x0000, SA_OUT_MASTER_MODE_value_map, "+++VALUE 0 Slave operation"),
	DEFINE_SETTING_ENTRY("AC97_SHUTDOWN", AC97_Shutdown, 0, 1, RW, 0x0000, 0, "Disable the AC97 interface"),
	DEFINE_SETTING_ENTRY("AC97_WAKE_UP_SYNC", AC97_Wake_Up, 0, 1, RW, 0x0000, 0, "Wake up the AC97 interface from sleep mode"),
	DEFINE_SETTING_ENTRY("AC97_RST_ACL", AC97_Reset, 0, 1, RW, 0x0000, 0, "Reset the AC97 interface"),
	DEFINE_SETTING_ENTRY("AC97_UP2X_BYPASS", AC97_Upsample_Bypass, 0, 1, RW, 0x0000, AC97_UP2X_BYPASS_value_map, "+++VALUE 0 Normal operation"),
	DEFINE_SETTING_ENTRY("ACLMSTR_BIT1", ACL_Control_0, 1, 1, RW, 0x0000, ACLMSTR_BIT1_value_map, "AC_LINK Mode:"),
	DEFINE_SETTING_ENTRY("ACLMSTR_4TO2_3_2_", ACL_Control_0, 2, 2, RW, 0x0000, ACLMSTR_4TO2_3_2__value_map, "GPIO Input Interrupt Control:"),
	DEFINE_SETTING_ENTRY("ACLMSTR_4TO2_4_", ACL_Control_0, 4, 1, RW, 0x0000, ACLMSTR_4TO2_4__value_map, "GPIO Output Interrupt Control:"),
	DEFINE_SETTING_ENTRY("ACLMSTR_12TO8_8__", ACL_Control_0, 7, 2, RW, 0x0000, ACLMSTR_12TO8_8___value_map, ""),
	DEFINE_SETTING_ENTRY("ACLMSTR_12TO8_9_", ACL_Control_0, 9, 1, RW, 0x0000, ACLMSTR_12TO8_9__value_map, "Command Output Interrupt Control:"),
	DEFINE_SETTING_ENTRY("ACLMSTR_12TO8_10_", ACL_Control_0, 10, 1, RW, 0x0000, ACLMSTR_12TO8_10__value_map, "Tag Arrival Interrupt Control:"),
	DEFINE_SETTING_ENTRY("ACLMSTR_12TO8_11_", ACL_Control_0, 16, 1, RC, 0x0000, 0, "GPIO Input Interrupt Status"),
	DEFINE_SETTING_ENTRY("ACLMSTR_12TO8_12_", ACL_Control_0, 17, 1, RC, 0x0000, 0, "GPIO Output Interrupt Status"),
	DEFINE_SETTING_ENTRY("ACLMSTR_16TO14_14_", ACL_Control_0, 19, 1, RC, 0x0000, 0, "Slot 1 / 2 Status Input Interrupt Status"),
	DEFINE_SETTING_ENTRY("ACLMSTR_16TO14_15_", ACL_Control_0, 20, 1, RC, 0x0000, 0, "Command Output Interrupt Status"),
	DEFINE_SETTING_ENTRY("ACLMSTR_16TO14_16_", ACL_Control_0, 21, 1, RC, 0x0000, 0, "Tag Arrival Interrupt Status"),
	DEFINE_SETTING_ENTRY("ACLCTRL00", ACL_Control_0, 24, 1, RW, 0x0000, 0, "LBUS Source Target ACL00 (Primary Audio Record)"),
	DEFINE_SETTING_ENTRY("ACLCTRL01_1_0_", ACL_Control_1, 0, 2, RW, 0x0000, ACLCTRL01_1_0__value_map, "ACL01 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL02_1_0_", ACL_Control_1, 8, 2, RW, 0x0000, ACLCTRL02_1_0__value_map, "ACL02 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL03_1_0_", ACL_Control_1, 16, 2, RW, 0x0000, ACLCTRL03_1_0__value_map, "ACL03 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL04_1_0_", ACL_Control_1, 24, 2, RW, 0x0000, ACLCTRL04_1_0__value_map, "ACL04 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL05_5_0_", ACL_Control_2, 8, 6, RW, 0x003f, 0, "ACL05 Target ID: 0x3F = Disable"),
	DEFINE_SETTING_ENTRY("ACLCTRL06_1_0_", ACL_Control_2, 16, 2, RW, 0x0000, ACLCTRL06_1_0__value_map, "ACL06 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL06_7_2_", ACL_Control_2, 24, 6, RW, 0x003f, 0, "ACL06 Target ID: 0x3F = Disable"),
	DEFINE_SETTING_ENTRY("ACLCTRL07_1_0_", ACL_Control_3, 0, 2, RW, 0x0000, ACLCTRL07_1_0__value_map, "ACL07 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL07_7_2_", ACL_Control_3, 8, 6, RW, 0x003f, 0, "ACL07 Target ID: 0x3F = Disable"),
	DEFINE_SETTING_ENTRY("ACLCTRL08_1_0_", ACL_Control_3, 16, 2, RW, 0x0000, ACLCTRL08_1_0__value_map, "ACL08 Format/Enable:"),
	DEFINE_SETTING_ENTRY("ACLCTRL08_7_2_", ACL_Control_3, 24, 6, RW, 0x003f, 0, "ACL08 Target ID: 0x3f = Disable"),
	DEFINE_SETTING_ENTRY("ACLCMD_15_0_", ACL_Command, 0, 16, RW, 0x0000, 0, "Command Data"),
	DEFINE_SETTING_ENTRY("ACLCMD_23_16_", ACL_Command, 16, 8, RW, 0x0000, 0, "Command Address"),
	DEFINE_SETTING_ENTRY("ACLGPOUT_15_0_", ACL_GPIO_Output_Data, 0, 16, RW, 0x0000, 0, "GPIO Output Data"),
	DEFINE_SETTING_ENTRY("ACLGPIN_15_0_", ACL_GPIO_Input_Data, 0, 16, RW, 0x0000, 0, "GPIO Input Data"),
	DEFINE_SETTING_ENTRY("ACLSTAT0_15_0_", ACL_Status_Slot_Interface, 0, 16, RW, 0x0000, 0, "Status DataThis register contains the most recent"),
	DEFINE_SETTING_ENTRY("ACLSTAT0_23_16_", ACL_Status_Slot_Interface, 16, 8, RW, 0x0000, 0, "Status Address"),
	DEFINE_SETTING_ENTRY("LOWPOWER_0_", ACL_Warm_Reset, 0, 1, RW, 0x0000, 0, "Clock Source Selector: Clock source for Reset Counter"),
	DEFINE_SETTING_ENTRY("LOWPOWER_7_1_", ACL_Warm_Reset, 1, 7, RW, 0x0000, 0, "Sync Pulse Counter: Number of cycles for which Sync is"),
	DEFINE_SETTING_ENTRY("LOWPOWER_20_8_", ACL_Warm_Reset, 8, 13, RW, 0x0000, 0, "Reset Counter: Number of cycles for which AC_LINK"),
	DEFINE_SETTING_ENTRY("ACL_TAG", ACL_Tag, 3, 12, RO, 0x0000, 0, "Missing bit spec for reg"),
	DEFINE_SETTING_ENTRY("ACL_LINK_UPDATE", ACL_Tag, 31, 1, RO, 0x0000, 0, "Missing bit spec for reg"),
	DEFINE_SETTING_ENTRY("RDS_I", RDS_I_Low, 0, 16, RO, 0x0000, 0, "Low byte of RDS_I. RDS I output.")
};
