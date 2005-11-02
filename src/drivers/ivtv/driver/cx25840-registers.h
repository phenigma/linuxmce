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
#ifndef __CX25840_REGISTERS_H
#define __CX25840_REGISTERS_H
#include <asm/types.h>
#include "cx25840-settings.h"
typedef enum { RC, RO, RR, RW, SC, VO } cx25840_setting_type;
extern __u16 cx25840_register_info[];
extern char *cx25840_register_name[];
extern __u8 cx25840_register_default[];
typedef struct {
	unsigned int value;
	char *name;
} value_map;
typedef struct {
	__u32 location;		// bit0-8 reg 9-11 start 12-16 len 17-20 type
	__u32 default_value;
	char *name;
	value_map *mapping;
	char *description;
} cx25840_setting_entry;
extern cx25840_setting_entry cx25840_settings[];

#define CX25840_REGISTER_ADDRESS(i) (cx25840_register_info[(i)]&0x0fff)
#define CX25840_REGISTER_TYPE(i)    (cx25840_register_info[(i)]>>12)
#define CX25840_REGISTER_NAME(i)    (cx25840_register_name[(i)])
#define CX25840_REGISTER_DEFAULT(i)    (cx25840_register_default[(i)])

#define CX25840_SETTING_REGISTER_INDEX(i) ((cx25840_settings[(i)].location>>0)&0x01ff)
#define CX25840_SETTING_START_BIT(i)      ((cx25840_settings[(i)].location>>9)&0x0007)
#define CX25840_SETTING_BIT_LENGTH(i)     ((cx25840_settings[(i)].location>>12)&0x001f)
#define CX25840_SETTING_TYPE(i)           ((cx25840_settings[(i)].location>>17)&0x000f)
#define CX25840_SETTING_DEFAULT_VALUE(i)  (cx25840_settings[(i)].default_value)
#define CX25840_SETTING_NAME(i)           (cx25840_settings[(i)].name)
#define CX25840_SETTING_MAPPING(i)        (cx25840_settings[(i)].mapping)
#define CX25840_SETTING_DESCRIPTION(i)    (cx25840_settings[(i)].description)
typedef enum {
	Host_Register_1,
	Host_Register_2,
	Device_ID_Low_Byte,
	Device_ID_High_Byte,
	Configuration,
	Video_Input_Control,
	AFE_Control_1,
	AFE_Control_2,
	AFE_Control_3,
	AFE_Control_4,
	Video_PLL_Integer,
	Video_PLL_Divider,
	Aux_PLL_Integer,
	Aux_PLL_Divider,
	Video_PLL_Fractional_1,
	Video_PLL_Fractional_2,
	Video_PLL_Fractional_3,
	Video_PLL_Fractional_4,
	Aux_PLL_Fractional_1,
	Aux_PLL_Fractional_2,
	Aux_PLL_Fractional_3,
	Aux_PLL_Fractional_4,
	Pin_Control_1,
	Pin_Control_2,
	Pin_Control_3,
	Pin_Control_4,
	Pin_Control_5,
	Pin_Control_6,
	Pin_Configuration_1,
	Pin_Configuration_2,
	Pin_Configuration_3,
	Pin_Configuration_4,
	Pin_Configuration_5,
	Pin_Configuration_6,
	Pin_Configuration_7,
	Pin_Configuration_8,
	Pin_Configuration_9,
	Pin_Configuration_10,
	Pin_Configuration_11,
	Pin_Configuration_12,
	Video_Count_Low,
	Video_Count_Mid,
	Video_Count_High,
	Audio_Lock,
	Audio_Count_Low,
	Audio_Count_Mid,
	Audio_Lock_2,
	Audio_Lock_3,
	Power_Control_1,
	Power_Control_2,
	AFE_Diagnostic_Control_1,
	AFE_Diagnostic_Control_2,
	AFE_Diagnostic_Control_3,
	AFE_Diagnostic_Control_5,
	AFE_Diagnostic_Control_6,
	AFE_Diagnostic_Control_7,
	AFE_Diagnostic_Control_8,
	PLL_Diagnostic_Control_1,
	PLL_Diagnostic_Control_2,
	DLL1_Diagnostic_Control_2,
	DLL1_Diagnostic_Control_3,
	DLL1_Diagnostic_Control_4,
	DLL2_Diagnostic_Control_1,
	DLL2_Diagnostic_Control_2,
	DLL2_Diagnostic_Control_3,
	DLL2_Diagnostic_Control_4,
	IR_Control_1,
	IR_Control_2,
	IR_TX_Clock_Divider_Low,
	IR_TX_Clock_Divider_High,
	IR_RX_Clock_Divider_Low,
	IR_RX_Clock_Divider_High,
	IR_TX_Carrier_Duty_Cycle,
	IR_Status,
	IR_Interrupt_Enable,
	IR_Low_Pass_Filter_Low,
	IR_Low_Pass_Filter_High,
	IR_FIFO_Low,
	IR_FIFO_High,
	IR_FIFO_Level,
	Video_Mode_Control_1,
	Video_Mode_Control_2,
	Video_Mode_Control_3,
	Video_Mode_Control_4,
	Video_Out_Control_1,
	Video_Out_Control_2,
	Video_Out_Control_3,
	Video_Out_Control_4,
	Ancillary_IDID_0,
	Ancillary_IDID_1,
	Ancillary_IDID_0_1,
	Ancillary_Audio_Output_Control,
	Copy_Protection_Status,
	General_Status_1,
	General_Status_2,
	Interrupt_Status_1,
	Interrupt_Status_2,
	Interrupt_Mask_1,
	Interrupt_Mask_2,
	Brightness,
	Contrast,
	Luma_Control,
	Horizontal_Scaling_Low,
	Horizontal_Scaling_Mid,
	Horizontal_Scaling_High,
	Horizontal_Scaling_Control,
	Vertical_Scaling_Low,
	Vertical_Scaling_High,
	Vertical_Scaling_Control,
	Vertical_Line_Control,
	Saturation_U,
	Saturation_V,
	Hue,
	Chroma_Control,
	VBI_Line_Control_1,
	VBI_Line_Control_2,
	VBI_Line_Control_3,
	VBI_Line_Control_4,
	VBI_Line_Control_5,
	VBI_Line_Control_6,
	VBI_Line_Control_7,
	VBI_Line_Control_8,
	VBI_Line_Control_9,
	VBI_Line_Control_10,
	VBI_Line_Control_11,
	VBI_Line_Control_12,
	VBI_Line_Control_13,
	VBI_Line_Control_14,
	VBI_Line_Control_15,
	VBI_Line_Control_16,
	VBI_Line_Control_17,
	VBI_Frame_Code_Search_Mode,
	VBI_Alternate_Frame_Code_Type,
	VBI_Alternate_1_Frame_Code,
	VBI_Alternate_2_Frame_Code,
	VBI_Miscellaneous_Config_1,
	TTX_Packet_Address_1,
	TTX_Packet_Address_2,
	TTX_Packet_Address_3,
	VBI_1_and_2_SDID,
	VBI_3_SDID,
	VBI_FIFO_Reset,
	VBI_Hamming,
	Closed_Caption_Status,
	Closed_Caption_Data,
	GEMSTAR_1x_Status,
	GEMSTAR_1x_Data,
	GEMSTAR_2x_Status,
	GEMSTAR_2x_Data,
	WSS_Status,
	WSS_Data,
	VBI_Custom_1_Horizontal_Delay,
	VBI_Custom_1_Bit_Increment,
	VBI_Custom_1_Slice_Distance,
	VBI_Custom_1_Clock_Run_in_Window,
	VBI_Custom_1_Frame_Code_Low,
	VBI_Custom_1_Frame_Code_Mid,
	VBI_Custom_1_Frame_Code_High,
	VBI_Custom_1_Frame_Code_Length,
	VBI_Custom_1_Clock_Run_in_Period,
	VBI_Custom_1_Clock_Run_in_Margin_and_Length,
	VBI_Custom_1_Payload_Length,
	VBI_Custom_1_Miscellaneous,
	VBI_Custom_2_Horizontal_Delay,
	VBI_Custom_2_Bit_Increment,
	VBI_Custom_2_Slice_Distance,
	VBI_Custom_2_Clock_Run_In_Window,
	VBI_Custom_2_Frame_Code_Low,
	VBI_Custom_2_Frame_Code_Mid,
	VBI_Custom_2_Frame_Code_High,
	VBI_Custom_2_Frame_Code_Length,
	VBI_Custom_2_Clock_Run_in_Period,
	VBI_Custom_2_Clock_Run_in_Margin_and_Length,
	VBI_Custom_2_Payload_Length,
	VBI_Custom_2_Miscellaneous,
	VBI_Custom_3_Horizontal_Delay,
	VBI_Custom_3_Bit_Increment,
	VBI_Custom_3_Slice_Distance,
	VBI_Custom_3_Clock_Run_in_Window,
	VBI_Custom_3_Frame_Code_Low,
	VBI_Custom_3_Frame_Code_Mid,
	VBI_Custom_3_Frame_Code_High,
	VBI_Custom_3_Frame_Code_Length,
	VBI_Custom_3_Clock_Run_in_Period,
	VBI_Custom_3_Clock_Run_in_Margin_and_Length,
	VBI_Custom_3_Payload_Length,
	VBI_Custom_3_Miscellaneous,
	Horizontal_Blanking_Delay_Low,
	Horizontal_Blanking_Delay_High,
	Horizontal_Active_High,
	Burst_Gate_Delay,
	Vertical_Blanking_Delay_Low,
	Vertical_Blanking_Delay_High,
	Vertical_Active_High,
	Vertical_Blanking_Delay_656,
	SRC_Decimation_Ratio_Low,
	SRC_Decimation_Ratio_High,
	Comb_Filter_Bandwidth_Select,
	Comb_Filter_Enable,
	Subcarrier_Step_Size_Low,
	Subcarrier_Step_Size_Mid,
	Subcarrier_Step_Size_High,
	VBI_Offset,
	Field_Count_Low,
	Field_Count_High,
	Temporal_Decimation,
	Miscellaneous_Timing_Control,
	Video_Detect_Configuration,
	VGA_Gain_Control,
	AGC_Gain_Control_Low,
	AGC_Gain_Control_High,
	Digital_Front_End_Control,
	VGA_Sync_Control,
	VGA_Track_Range,
	VGA_Acquire_Range,
	DFE_Control,
	Backporch_Loop_Gain,
	DFT_Threshold,
	Backporch_Percent,
	PLL_Offset_Low,
	PLL_Offset_High,
	PLL_Indirect_Loop_Gain,
	PLL_Direct_Loop_Gain,
	Horizontal_Tracking_Loop_Indirect_Gain,
	Luma_Comb_Error_Limit_Max,
	Luma_Comb_Threshold,
	Chroma_Comb_Error_Limit_Max,
	Comb_Phase_Limit_Max,
	White_Crush_Increment,
	White_Crush_Decrement,
	White_Crush_Comparison_Point,
	Soft_Reset_Mask_1,
	Soft_Reset_Mask_2,
	Version_ID,
	Miscellaneous_Diagnostic_Control,
	Download_Address_Low_Byte,
	Download_Address_High_Byte,
	Download_Data_Control,
	Download_Control,
	Mode_Detect_Status_0,
	Mode_Detect_Status_1,
	Audio_Configuration,
	Preferred_Decode_Mode,
	Embedded_Microcontroller_Video_Format,
	Audio_Format_Control_0,
	Soft_Reset,
	Audio_Interrupt_Control,
	Audio_Interrupt_Status,
	Analog_AGC_Hysteresis1,
	Analog_AGC_Hysteresis2,
	Analog_AGC_Control,
	AFE_12DB_ENABLE,
	IF_SRC_Mode,
	Phase_Fix_Control,
	Dematrix_Bypass,
	Dematrix_Select_Control,
	Digital_Audio_Input_Shifter,
	Path1_Audio_Select_Control,
	Path1_AVC_Control1,
	Path1_AVC_Control2,
	Mute_Control,
	Path1_Volume,
	Path1_Balance,
	Path1_AVC_Threshold_Low,
	Path1_AVC_Threshold_High,
	Path1_EQ_Band_Select,
	Path1_EQ_Bass_Adjust,
	Path1_EQ_Midrange_Adjust,
	Path1_EQ_Treble_Adjust,
	Path1_Soft_Clip_Control1,
	Path1_Soft_Clip_Control2,
	Path1_Soft_Clip_Threshold_Low,
	Path1_Soft_Clip_Threshold_High,
	Path2_Audio_Select_Control,
	Path2_AVC_Control1,
	Path2_AVC_Control2,
	Path2_Mute_Control,
	Path2_Volume,
	Path2_Balance,
	Path2_AVC_Threshold_Low,
	Path2_AVC_Threshold_High,
	Path2_EQ_Band_Select,
	Path2_EQ_Bass_Adjust,
	Path2_EQ_Midrange_Adjust,
	Path2_EQ_Treble_Adjust,
	Path2_Soft_Clip_Control1,
	Path2_Soft_Clip_Control2,
	Path2_Soft_Clip_Threshold_Low,
	Path2_Soft_Clip_Threshold_High,
	Sample_Rate_Converter_Status1,
	Sample_Rate_Converter_Status2,
	Sample_Rate_Converter_Status3,
	SRC1_Phase_Increment_Low,
	SRC1_Phase_Increment_Mid,
	SRC1_Phase_Increment_High,
	SRC1_FIFO_Read_Threshold,
	SRC2_Phase_Increment_Low,
	SRC2_Phase_Increment_Mid,
	SRC2_Phase_Increment_High,
	SRC2_FIFO_Read_Threshold,
	SRC3_Phase_Increment_Low,
	SRC3_Phase_Increment_Mid,
	SRC3_Phase_Increment_High,
	SRC3_FIFO_Read_Threshold,
	SRC4_Phase_Increment_Low,
	SRC4_Phase_Increment_Mid,
	SRC4_Phase_Increment_High,
	SRC4_FIFO_Read_Threshold,
	SRC5_Phase_Increment_Low,
	SRC5_Phase_Increment_Mid,
	SRC5_Phase_Increment_High,
	SRC5_FIFO_Read_Threshold,
	SRC6_Phase_Increment_Low,
	SRC6_Phase_Increment_Mid,
	SRC6_Phase_Increment_High,
	SRC6_FIFO_Read_Threshold,
	Output_SRC_Source_Select,
	Baseband_Bypass_Control,
	Input_SRC_Source_Select1,
	Input_SRC_Source_Select2,
	Serial_Audio_Input_Control1,
	Serial_Audio_Input_Control2,
	Serial_Audio_Output_Control1,
	Serial_Audio_Output_Control2,
	AC97_Shutdown,
	AC97_Wake_Up,
	AC97_Reset,
	AC97_Upsample_Bypass,
	ACL_Control_0,
	Space_Holder_Dummy,
	Space_Holder_Dummy_UNIQ1,
	Space_Holder_Dummy_UNIQ2,
	ACL_Control_1,
	Space_Holder_Dummy_UNIQ3,
	Space_Holder_Dummy_UNIQ4,
	Space_Holder_Dummy_UNIQ5,
	ACL_Control_2,
	Space_Holder_Dummy_UNIQ6,
	Space_Holder_Dummy_UNIQ7,
	Space_Holder_Dummy_UNIQ8,
	ACL_Control_3,
	Space_Holder_Dummy_UNIQ9,
	Space_Holder_Dummy_UNIQ10,
	Space_Holder_Dummy_UNIQ11,
	ACL_Command,
	Space_Holder_Dummy_UNIQ12,
	Space_Holder_Dummy_UNIQ13,
	ACL_GPIO_Output_Data,
	Space_Holder_Dummy_UNIQ14,
	ACL_GPIO_Input_Data,
	Space_Holder_Dummy_UNIQ15,
	ACL_Status_Slot_Interface,
	Space_Holder_Dummy_UNIQ16,
	Space_Holder_Dummy_UNIQ17,
	ACL_Warm_Reset,
	Space_Holder_Dummy_UNIQ18,
	Space_Holder_Dummy_UNIQ19,
	ACL_Tag,
	Space_Holder_Dummy_UNIQ20,
	Space_Holder_Dummy_UNIQ21,
	Space_Holder_Dummy_UNIQ22,
	RDS_I_Low,
	RDS_I_High,
	RDS_Q_Low,
	number_of_registers
} cx25840_register;

#define CX25840__SEQUENCE_6BIT(set, value)     cx25840_write_setting(client,set,value)
#define CX25840__SEQUENCE_14BIT(set, value)    cx25840_write_setting(client,set,value)
#define CX25840__SEQUENCE_22BIT(set, value)    cx25840_write_setting(client,set,value)
#define CX25840__SEQUENCE_30BIT(set, value)    cx25840_write_setting(client,set,value)
#define CX25840__SEQUENCE_32BIT(set, value)    cx25840_write_setting(client,set,value)

#define CX25840_SET_SLEEP(value) CX25840__SEQUENCE_6BIT(SLEEP,  value)
#define CX25840_SET_DIGITAL_PWR_DN(value) CX25840__SEQUENCE_6BIT(DIGITAL_PWR_DN,  value)
#define CX25840_SET_REF_CLK_SEL(value) CX25840__SEQUENCE_6BIT(REF_CLK_SEL,  value)
#define CX25840_SET_PWR_DN_VID_PLL(value) CX25840__SEQUENCE_6BIT(PWR_DN_VID_PLL,  value)
#define CX25840_SET_PWR_DN_AUX_PLL(value) CX25840__SEQUENCE_6BIT(PWR_DN_AUX_PLL,  value)
#define CX25840_SET_AUTO_INC_DIS(value) CX25840__SEQUENCE_6BIT(AUTO_INC_DIS,  value)
#define CX25840_SET_SLV_SI_DIS(value) CX25840__SEQUENCE_6BIT(SLV_SI_DIS,  value)
#define CX25840_SET_FORCE_CHIP_SEL_VIPCLK(value) CX25840__SEQUENCE_6BIT(FORCE_CHIP_SEL_VIPCLK,  value)
#define CX25840_SET_PREFETCH_EN(value) CX25840__SEQUENCE_6BIT(PREFETCH_EN,  value)
#define CX25840_SET_PWR_DN_PLL_REG1(value) CX25840__SEQUENCE_6BIT(PWR_DN_PLL_REG1,  value)
#define CX25840_SET_PWR_DN_PLL_REG2(value) CX25840__SEQUENCE_6BIT(PWR_DN_PLL_REG2,  value)
#define CX25840_SET_DEVICE_ID(value) CX25840__SEQUENCE_22BIT(DEVICE_ID,  value)
#define CX25840_SET_SOFT_RST(value) CX25840__SEQUENCE_6BIT(SOFT_RST,  value)
#define CX25840_SET_CH_SEL_ADC2(value) CX25840__SEQUENCE_6BIT(CH_SEL_ADC2,  value)
#define CX25840_SET_DUAL_MODE_ADC2(value) CX25840__SEQUENCE_6BIT(DUAL_MODE_ADC2,  value)
#define CX25840_SET_CHIP_ACFG_DIS(value) CX25840__SEQUENCE_6BIT(CHIP_ACFG_DIS,  value)
#define CX25840_SET_CH_1__SOURCE(value) CX25840__SEQUENCE_6BIT(CH_1__SOURCE,  value)
#define CX25840_SET_CH_2__SOURCE(value) CX25840__SEQUENCE_6BIT(CH_2__SOURCE,  value)
#define CX25840_SET_CH_3__SOURCE(value) CX25840__SEQUENCE_6BIT(CH_3__SOURCE,  value)
#define CX25840_SET_EN_12DB_CH1(value) CX25840__SEQUENCE_6BIT(EN_12DB_CH1,  value)
#define CX25840_SET_EN_12DB_CH2(value) CX25840__SEQUENCE_6BIT(EN_12DB_CH2,  value)
#define CX25840_SET_EN_12DB_CH3(value) CX25840__SEQUENCE_6BIT(EN_12DB_CH3,  value)
#define CX25840_SET_HALF_BW_CH1(value) CX25840__SEQUENCE_6BIT(HALF_BW_CH1,  value)
#define CX25840_SET_HALF_BW_CH2(value) CX25840__SEQUENCE_6BIT(HALF_BW_CH2,  value)
#define CX25840_SET_HALF_BW_CH3(value) CX25840__SEQUENCE_6BIT(HALF_BW_CH3,  value)
#define CX25840_SET_VGA_SEL_CH1(value) CX25840__SEQUENCE_6BIT(VGA_SEL_CH1,  value)
#define CX25840_SET_VGA_SEL_CH2(value) CX25840__SEQUENCE_6BIT(VGA_SEL_CH2,  value)
#define CX25840_SET_VGA_SEL_CH3(value) CX25840__SEQUENCE_6BIT(VGA_SEL_CH3,  value)
#define CX25840_SET_CLAMP_SEL_CH1(value) CX25840__SEQUENCE_6BIT(CLAMP_SEL_CH1,  value)
#define CX25840_SET_CLAMP_SEL_CH2(value) CX25840__SEQUENCE_6BIT(CLAMP_SEL_CH2,  value)
#define CX25840_SET_CLAMP_SEL_CH3(value) CX25840__SEQUENCE_6BIT(CLAMP_SEL_CH3,  value)
#define CX25840_SET_CHROMA_IN_SEL(value) CX25840__SEQUENCE_6BIT(CHROMA_IN_SEL,  value)
#define CX25840_SET_LUMA_IN_SEL(value) CX25840__SEQUENCE_6BIT(LUMA_IN_SEL,  value)
#define CX25840_SET_AUD_IN_SEL(value) CX25840__SEQUENCE_6BIT(AUD_IN_SEL,  value)
#define CX25840_SET_CLAMP_EN_CH1(value) CX25840__SEQUENCE_6BIT(CLAMP_EN_CH1,  value)
#define CX25840_SET_CLAMP_EN_CH2(value) CX25840__SEQUENCE_6BIT(CLAMP_EN_CH2,  value)
#define CX25840_SET_CLAMP_EN_CH3(value) CX25840__SEQUENCE_6BIT(CLAMP_EN_CH3,  value)
#define CX25840_SET_DROOP_COMP_CH1(value) CX25840__SEQUENCE_6BIT(DROOP_COMP_CH1,  value)
#define CX25840_SET_DROOP_COMP_CH2(value) CX25840__SEQUENCE_6BIT(DROOP_COMP_CH2,  value)
#define CX25840_SET_DROOP_COMP_CH3(value) CX25840__SEQUENCE_6BIT(DROOP_COMP_CH3,  value)
#define CX25840_SET_BYPASS_CH1(value) CX25840__SEQUENCE_6BIT(BYPASS_CH1,  value)
#define CX25840_SET_BYPASS_CH2(value) CX25840__SEQUENCE_6BIT(BYPASS_CH2,  value)
#define CX25840_SET_BYPASS_CH3(value) CX25840__SEQUENCE_6BIT(BYPASS_CH3,  value)
#define CX25840_SET_IREF_SEL(value) CX25840__SEQUENCE_6BIT(IREF_SEL,  value)
#define CX25840_SET_VID_PLL_INT(value) CX25840__SEQUENCE_6BIT(VID_PLL_INT,  value)
#define CX25840_SET_VID_PLL_POST(value) CX25840__SEQUENCE_6BIT(VID_PLL_POST,  value)
#define CX25840_SET_AUX_PLL_INT(value) CX25840__SEQUENCE_6BIT(AUX_PLL_INT,  value)
#define CX25840_SET_AUX_PLL_POST(value) CX25840__SEQUENCE_6BIT(AUX_PLL_POST,  value)
#define CX25840_SET_VID_PLL_FRAC1(value) CX25840__SEQUENCE_14BIT(VID_PLL_FRAC1,  value)
#define CX25840_SET_VID_PLL_FRAC2(value) CX25840__SEQUENCE_14BIT(VID_PLL_FRAC2,  value)
#define CX25840_SET_VID_PLL_FRAC3(value) CX25840__SEQUENCE_14BIT(VID_PLL_FRAC3,  value)
#define CX25840_SET_VID_PLL_FRAC4(value) CX25840__SEQUENCE_6BIT(VID_PLL_FRAC4,  value)
#define CX25840_SET_AUX_PLL_FRAC1(value) CX25840__SEQUENCE_14BIT(AUX_PLL_FRAC1,  value)
#define CX25840_SET_AUX_PLL_FRAC2(value) CX25840__SEQUENCE_14BIT(AUX_PLL_FRAC2,  value)
#define CX25840_SET_AUX_PLL_FRAC3(value) CX25840__SEQUENCE_14BIT(AUX_PLL_FRAC3,  value)
#define CX25840_SET_AUX_PLL_FRAC4(value) CX25840__SEQUENCE_6BIT(AUX_PLL_FRAC4,  value)
#define CX25840_SET_GPIO0_OUT_EN(value) CX25840__SEQUENCE_6BIT(GPIO0_OUT_EN,  value)
#define CX25840_SET_GPIO1_OUT_EN(value) CX25840__SEQUENCE_6BIT(GPIO1_OUT_EN,  value)
#define CX25840_SET_CHIP_SEL_VIPCLK_OUT_EN(value) CX25840__SEQUENCE_6BIT(CHIP_SEL_VIPCLK_OUT_EN,  value)
#define CX25840_SET_IRQN_OUT_EN(value) CX25840__SEQUENCE_6BIT(IRQN_OUT_EN,  value)
#define CX25840_SET_IR_RX_OUT_EN(value) CX25840__SEQUENCE_6BIT(IR_RX_OUT_EN,  value)
#define CX25840_SET_IR_TX_OUT_EN(value) CX25840__SEQUENCE_6BIT(IR_TX_OUT_EN,  value)
#define CX25840_SET_DVALID_OUT_EN(value) CX25840__SEQUENCE_6BIT(DVALID_OUT_EN,  value)
#define CX25840_SET_FIELD_OUT_EN(value) CX25840__SEQUENCE_6BIT(FIELD_OUT_EN,  value)
#define CX25840_SET_HRESET_OUT_EN(value) CX25840__SEQUENCE_6BIT(HRESET_OUT_EN,  value)
#define CX25840_SET_VRESET_OUT_EN(value) CX25840__SEQUENCE_6BIT(VRESET_OUT_EN,  value)
#define CX25840_SET_VID_OUT_EN(value) CX25840__SEQUENCE_6BIT(VID_OUT_EN,  value)
#define CX25840_SET_PIXCLK_OUT_EN(value) CX25840__SEQUENCE_6BIT(PIXCLK_OUT_EN,  value)
#define CX25840_SET_SA_SDIN_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_SDIN_OUT_EN,  value)
#define CX25840_SET_SA_BCLKIN_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_BCLKIN_OUT_EN,  value)
#define CX25840_SET_SA_WCLKIN_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_WCLKIN_OUT_EN,  value)
#define CX25840_SET_SA_SDOUT_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_SDOUT_OUT_EN,  value)
#define CX25840_SET_SA_BCLKOUT_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_BCLKOUT_OUT_EN,  value)
#define CX25840_SET_SA_WCLKOUT_OUT_EN(value) CX25840__SEQUENCE_6BIT(SA_WCLKOUT_OUT_EN,  value)
#define CX25840_SET_PLL_CLK_OUT_EN(value) CX25840__SEQUENCE_6BIT(PLL_CLK_OUT_EN,  value)
#define CX25840_SET_AC_OUT_OUT_EN(value) CX25840__SEQUENCE_6BIT(AC_OUT_OUT_EN,  value)
#define CX25840_SET_AC_IN_OUT_EN(value) CX25840__SEQUENCE_6BIT(AC_IN_OUT_EN,  value)
#define CX25840_SET_IRQ_N_PRGM4(value) CX25840__SEQUENCE_6BIT(IRQ_N_PRGM4,  value)
#define CX25840_SET_VID_IRQ_STAT(value) CX25840__SEQUENCE_6BIT(VID_IRQ_STAT,  value)
#define CX25840_SET_IR_IRQ_STAT(value) CX25840__SEQUENCE_6BIT(IR_IRQ_STAT,  value)
#define CX25840_SET_VID_OUT_SPD(value) CX25840__SEQUENCE_6BIT(VID_OUT_SPD,  value)
#define CX25840_SET_VID_CTRL_SPD(value) CX25840__SEQUENCE_6BIT(VID_CTRL_SPD,  value)
#define CX25840_SET_AC_OUT_SPD(value) CX25840__SEQUENCE_6BIT(AC_OUT_SPD,  value)
#define CX25840_SET_SA_OUT_SPD(value) CX25840__SEQUENCE_6BIT(SA_OUT_SPD,  value)
#define CX25840_SET_GEN_OUT_SPD(value) CX25840__SEQUENCE_6BIT(GEN_OUT_SPD,  value)
#define CX25840_SET_GPIO0_OUT_SEL(value) CX25840__SEQUENCE_6BIT(GPIO0_OUT_SEL,  value)
#define CX25840_SET_GPIO1_OUT_SEL(value) CX25840__SEQUENCE_6BIT(GPIO1_OUT_SEL,  value)
#define CX25840_SET_CHIPSEL_OUT_SEL(value) CX25840__SEQUENCE_6BIT(CHIPSEL_OUT_SEL,  value)
#define CX25840_SET_IRQN_OUT_SEL(value) CX25840__SEQUENCE_6BIT(IRQN_OUT_SEL,  value)
#define CX25840_SET_IR_RX_OUT_SEL(value) CX25840__SEQUENCE_6BIT(IR_RX_OUT_SEL,  value)
#define CX25840_SET_IR_TX_OUT_SEL(value) CX25840__SEQUENCE_6BIT(IR_TX_OUT_SEL,  value)
#define CX25840_SET_DVALID_PRGM0_OUT_SEL(value) CX25840__SEQUENCE_6BIT(DVALID_PRGM0_OUT_SEL,  value)
#define CX25840_SET_FIELD_PRGM1_OUT_SEL(value) CX25840__SEQUENCE_6BIT(FIELD_PRGM1_OUT_SEL,  value)
#define CX25840_SET_HRESET_PRGM2_OUT_SEL(value) CX25840__SEQUENCE_6BIT(HRESET_PRGM2_OUT_SEL,  value)
#define CX25840_SET_VRESET_HCTL_PRGM3_OUT_SEL(value) CX25840__SEQUENCE_6BIT(VRESET_HCTL_PRGM3_OUT_SEL,  value)
#define CX25840_SET_VID_DATA_OUT_SEL(value) CX25840__SEQUENCE_6BIT(VID_DATA_OUT_SEL,  value)
#define CX25840_SET_PIXCLK_OUT_SEL(value) CX25840__SEQUENCE_6BIT(PIXCLK_OUT_SEL,  value)
#define CX25840_SET_AC_OUT_SEL(value) CX25840__SEQUENCE_6BIT(AC_OUT_SEL,  value)
#define CX25840_SET_SA_OUT_OUT_SEL(value) CX25840__SEQUENCE_6BIT(SA_OUT_OUT_SEL,  value)
#define CX25840_SET_SA_IN_OUT_SEL(value) CX25840__SEQUENCE_6BIT(SA_IN_OUT_SEL,  value)
#define CX25840_SET_AUX_PLL_AOUT_SEL(value) CX25840__SEQUENCE_6BIT(AUX_PLL_AOUT_SEL,  value)
#define CX25840_SET_AUX_PLL_DOUT_SEL(value) CX25840__SEQUENCE_6BIT(AUX_PLL_DOUT_SEL,  value)
#define CX25840_SET_AC_BITCLK_IN_SEL(value) CX25840__SEQUENCE_6BIT(AC_BITCLK_IN_SEL,  value)
#define CX25840_SET_AC_SDIN_IN_SEL(value) CX25840__SEQUENCE_6BIT(AC_SDIN_IN_SEL,  value)
#define CX25840_SET_GPI0_IN_SEL(value) CX25840__SEQUENCE_6BIT(GPI0_IN_SEL,  value)
#define CX25840_SET_GPI1_IN_SEL(value) CX25840__SEQUENCE_6BIT(GPI1_IN_SEL,  value)
#define CX25840_SET_GPI2_IN_SEL(value) CX25840__SEQUENCE_6BIT(GPI2_IN_SEL,  value)
#define CX25840_SET_GPI3_IN_SEL(value) CX25840__SEQUENCE_6BIT(GPI3_IN_SEL,  value)
#define CX25840_SET_GPO_OUT(value) CX25840__SEQUENCE_6BIT(GPO_OUT,  value)
#define CX25840_SET_GPI_IN(value) CX25840__SEQUENCE_6BIT(GPI_IN,  value)
#define CX25840_SET_SA_MCLK_DIV(value) CX25840__SEQUENCE_6BIT(SA_MCLK_DIV,  value)
#define CX25840_SET_SA_MCLK_SEL(value) CX25840__SEQUENCE_6BIT(SA_MCLK_SEL,  value)
#define CX25840_SET_VID_COUNT(value) CX25840__SEQUENCE_30BIT(VID_COUNT,  value)
#define CX25840_SET_EN_AV_LOCK(value) CX25840__SEQUENCE_6BIT(EN_AV_LOCK,  value)
#define CX25840_SET_AUD_LOCK_KD_SHIFT(value) CX25840__SEQUENCE_6BIT(AUD_LOCK_KD_SHIFT,  value)
#define CX25840_SET_AUD_LOCK_KI_SHIFT(value) CX25840__SEQUENCE_6BIT(AUD_LOCK_KI_SHIFT,  value)
#define CX25840_SET_AUD_COUNT(value) CX25840__SEQUENCE_22BIT(AUD_COUNT,  value)
#define CX25840_SET_AUD_LOCK_FREQ_SHIFT(value) CX25840__SEQUENCE_6BIT(AUD_LOCK_FREQ_SHIFT,  value)
#define CX25840_SET_AUD_LOCK_KD_MULT(value) CX25840__SEQUENCE_6BIT(AUD_LOCK_KD_MULT,  value)
#define CX25840_SET_AUD_LOCK_KI_MULT(value) CX25840__SEQUENCE_6BIT(AUD_LOCK_KI_MULT,  value)
#define CX25840_SET_PWR_DN_TUNING(value) CX25840__SEQUENCE_6BIT(PWR_DN_TUNING,  value)
#define CX25840_SET_PWR_DN_DLL1(value) CX25840__SEQUENCE_6BIT(PWR_DN_DLL1,  value)
#define CX25840_SET_PWR_DN_DLL2(value) CX25840__SEQUENCE_6BIT(PWR_DN_DLL2,  value)
#define CX25840_SET_PWR_DN_ADC1(value) CX25840__SEQUENCE_6BIT(PWR_DN_ADC1,  value)
#define CX25840_SET_PWR_DN_ADC2(value) CX25840__SEQUENCE_6BIT(PWR_DN_ADC2,  value)
#define CX25840_SET_PWR_DN_CH1(value) CX25840__SEQUENCE_6BIT(PWR_DN_CH1,  value)
#define CX25840_SET_PWR_DN_CH2(value) CX25840__SEQUENCE_6BIT(PWR_DN_CH2,  value)
#define CX25840_SET_PWR_DN_CH3(value) CX25840__SEQUENCE_6BIT(PWR_DN_CH3,  value)
#define CX25840_SET_SLEEP_ANALOG_MSK(value) CX25840__SEQUENCE_6BIT(SLEEP_ANALOG_MSK,  value)
#define CX25840_SET_SLEEP_DLL_MSK(value) CX25840__SEQUENCE_6BIT(SLEEP_DLL_MSK,  value)
#define CX25840_SET_SLEEP_PLL_MSK(value) CX25840__SEQUENCE_6BIT(SLEEP_PLL_MSK,  value)
#define CX25840_SET_VCLK_GATE_MSK(value) CX25840__SEQUENCE_6BIT(VCLK_GATE_MSK,  value)
#define CX25840_SET_CLK5x_GATE_MSK(value) CX25840__SEQUENCE_6BIT(CLK5x_GATE_MSK,  value)
#define CX25840_SET_SCLK_GATE_MSK(value) CX25840__SEQUENCE_6BIT(SCLK_GATE_MSK,  value)
#define CX25840_SET_FOUR_X_CLK_ADC(value) CX25840__SEQUENCE_6BIT(FOUR_X_CLK_ADC,  value)
#define CX25840_SET_VREG_D_1_0_(value) CX25840__SEQUENCE_6BIT(VREG_D_1_0_,  value)
#define CX25840_SET_VREF_CTRL_ADC(value) CX25840__SEQUENCE_6BIT(VREF_CTRL_ADC,  value)
#define CX25840_SET_BIAS_CTRL_ADC_1_0_(value) CX25840__SEQUENCE_6BIT(BIAS_CTRL_ADC_1_0_,  value)
#define CX25840_SET_BIAS_CTRL_ADC_2_(value) CX25840__SEQUENCE_6BIT(BIAS_CTRL_ADC_2_,  value)
#define CX25840_SET_BIAS_CTRL_ADC_4_3_(value) CX25840__SEQUENCE_6BIT(BIAS_CTRL_ADC_4_3_,  value)
#define CX25840_SET_BIAS_CTRL_ADC_6_5_(value) CX25840__SEQUENCE_6BIT(BIAS_CTRL_ADC_6_5_,  value)
#define CX25840_SET_S2DIFF_BIAS_1_0_(value) CX25840__SEQUENCE_6BIT(S2DIFF_BIAS_1_0_,  value)
#define CX25840_SET_FILTER_BIAS_1_0_(value) CX25840__SEQUENCE_6BIT(FILTER_BIAS_1_0_,  value)
#define CX25840_SET_TUNE_FIL_RST(value) CX25840__SEQUENCE_6BIT(TUNE_FIL_RST,  value)
#define CX25840_SET_CH_SEL_ADC1(value) CX25840__SEQUENCE_6BIT(CH_SEL_ADC1,  value)
#define CX25840_SET_DISCONNECT_CH1(value) CX25840__SEQUENCE_6BIT(DISCONNECT_CH1,  value)
#define CX25840_SET_TEST_MODE_CH1(value) CX25840__SEQUENCE_6BIT(TEST_MODE_CH1,  value)
#define CX25840_SET_TUNE_IN_4_0_(value) CX25840__SEQUENCE_6BIT(TUNE_IN_4_0_,  value)
#define CX25840_SET_FORCE_TUNING(value) CX25840__SEQUENCE_6BIT(FORCE_TUNING,  value)
#define CX25840_SET_TUNE_OUT(value) CX25840__SEQUENCE_6BIT(TUNE_OUT,  value)
#define CX25840_SET_TUNING_READY(value) CX25840__SEQUENCE_6BIT(TUNING_READY,  value)
#define CX25840_SET_VID_DUAL_FLAG_POL(value) CX25840__SEQUENCE_6BIT(VID_DUAL_FLAG_POL,  value)
#define CX25840_SET_AUD_DUAL_FLAG_POL(value) CX25840__SEQUENCE_6BIT(AUD_DUAL_FLAG_POL,  value)
#define CX25840_SET_PLL_SPMP(value) CX25840__SEQUENCE_6BIT(PLL_SPMP,  value)
#define CX25840_SET_VID_PLL_DDS(value) CX25840__SEQUENCE_6BIT(VID_PLL_DDS,  value)
#define CX25840_SET_AUX_PLL_DDS(value) CX25840__SEQUENCE_6BIT(AUX_PLL_DDS,  value)
#define CX25840_SET_VID_PLL_RST(value) CX25840__SEQUENCE_6BIT(VID_PLL_RST,  value)
#define CX25840_SET_AUX_PLL_RST(value) CX25840__SEQUENCE_6BIT(AUX_PLL_RST,  value)
#define CX25840_SET_AUX_PLL_LOCK(value) CX25840__SEQUENCE_6BIT(AUX_PLL_LOCK,  value)
#define CX25840_SET_VID_PLL_LOCK(value) CX25840__SEQUENCE_6BIT(VID_PLL_LOCK,  value)
#define CX25840_SET_AUX_PLL_UNLOCK(value) CX25840__SEQUENCE_6BIT(AUX_PLL_UNLOCK,  value)
#define CX25840_SET_VID_PLL_UNLOCK(value) CX25840__SEQUENCE_6BIT(VID_PLL_UNLOCK,  value)
#define CX25840_SET_DLL1_FLD(value) CX25840__SEQUENCE_6BIT(DLL1_FLD,  value)
#define CX25840_SET_DLL1_UP_OVRD(value) CX25840__SEQUENCE_6BIT(DLL1_UP_OVRD,  value)
#define CX25840_SET_DLL1_DOWN_OVRD(value) CX25840__SEQUENCE_6BIT(DLL1_DOWN_OVRD,  value)
#define CX25840_SET_DLL1_CHPREF(value) CX25840__SEQUENCE_6BIT(DLL1_CHPREF,  value)
#define CX25840_SET_DLL1_COMP_GT(value) CX25840__SEQUENCE_6BIT(DLL1_COMP_GT,  value)
#define CX25840_SET_DLL1_COMP_LT(value) CX25840__SEQUENCE_6BIT(DLL1_COMP_LT,  value)
#define CX25840_SET_DLL1_DEPTH(value) CX25840__SEQUENCE_6BIT(DLL1_DEPTH,  value)
#define CX25840_SET_DLL1_DLYS(value) CX25840__SEQUENCE_6BIT(DLL1_DLYS,  value)
#define CX25840_SET_DLL1_CURRSET(value) CX25840__SEQUENCE_6BIT(DLL1_CURRSET,  value)
#define CX25840_SET_DLL1_BYPASS(value) CX25840__SEQUENCE_6BIT(DLL1_BYPASS,  value)
#define CX25840_SET_DLL2_Diagnostic_Control_1_REG(value) CX25840__SEQUENCE_14BIT(DLL2_Diagnostic_Control_1_REG,  value)
#define CX25840_SET_DLL2_FLD(value) CX25840__SEQUENCE_6BIT(DLL2_FLD,  value)
#define CX25840_SET_DLL2_UP_OVRD(value) CX25840__SEQUENCE_6BIT(DLL2_UP_OVRD,  value)
#define CX25840_SET_DLL2_DOWN_OVRD(value) CX25840__SEQUENCE_6BIT(DLL2_DOWN_OVRD,  value)
#define CX25840_SET_DLL2_CHPREF(value) CX25840__SEQUENCE_6BIT(DLL2_CHPREF,  value)
#define CX25840_SET_DLL2_COMP_GT(value) CX25840__SEQUENCE_6BIT(DLL2_COMP_GT,  value)
#define CX25840_SET_DLL2_COMP_LT(value) CX25840__SEQUENCE_6BIT(DLL2_COMP_LT,  value)
#define CX25840_SET_DLL2_DEPTH(value) CX25840__SEQUENCE_6BIT(DLL2_DEPTH,  value)
#define CX25840_SET_DLL2_DLYS(value) CX25840__SEQUENCE_6BIT(DLL2_DLYS,  value)
#define CX25840_SET_DLL2_CURRSET(value) CX25840__SEQUENCE_6BIT(DLL2_CURRSET,  value)
#define CX25840_SET_DLL2_BYPASS(value) CX25840__SEQUENCE_6BIT(DLL2_BYPASS,  value)
#define CX25840_SET_WIN(value) CX25840__SEQUENCE_6BIT(WIN,  value)
#define CX25840_SET_EDG(value) CX25840__SEQUENCE_6BIT(EDG,  value)
#define CX25840_SET_DMD(value) CX25840__SEQUENCE_6BIT(DMD,  value)
#define CX25840_SET_MOD(value) CX25840__SEQUENCE_6BIT(MOD,  value)
#define CX25840_SET_RFE(value) CX25840__SEQUENCE_6BIT(RFE,  value)
#define CX25840_SET_TFE(value) CX25840__SEQUENCE_6BIT(TFE,  value)
#define CX25840_SET_RXE(value) CX25840__SEQUENCE_6BIT(RXE,  value)
#define CX25840_SET_TXE(value) CX25840__SEQUENCE_6BIT(TXE,  value)
#define CX25840_SET_RIC(value) CX25840__SEQUENCE_6BIT(RIC,  value)
#define CX25840_SET_TIC(value) CX25840__SEQUENCE_6BIT(TIC,  value)
#define CX25840_SET_CPL(value) CX25840__SEQUENCE_6BIT(CPL,  value)
#define CX25840_SET_LBM(value) CX25840__SEQUENCE_6BIT(LBM,  value)
#define CX25840_SET_R(value) CX25840__SEQUENCE_6BIT(R,  value)
#define CX25840_SET_TCD(value) CX25840__SEQUENCE_22BIT(TCD,  value)
#define CX25840_SET_RCD(value) CX25840__SEQUENCE_22BIT(RCD,  value)
#define CX25840_SET_CDC(value) CX25840__SEQUENCE_6BIT(CDC,  value)
#define CX25840_SET_RTO(value) CX25840__SEQUENCE_6BIT(RTO,  value)
#define CX25840_SET_ROR(value) CX25840__SEQUENCE_6BIT(ROR,  value)
#define CX25840_SET_TBY(value) CX25840__SEQUENCE_6BIT(TBY,  value)
#define CX25840_SET_RSR(value) CX25840__SEQUENCE_6BIT(RSR,  value)
#define CX25840_SET_TSR(value) CX25840__SEQUENCE_6BIT(TSR,  value)
#define CX25840_SET_ROE(value) CX25840__SEQUENCE_6BIT(ROE,  value)
#define CX25840_SET_RSE(value) CX25840__SEQUENCE_6BIT(RSE,  value)
#define CX25840_SET_TSE(value) CX25840__SEQUENCE_6BIT(TSE,  value)
#define CX25840_SET_LPF(value) CX25840__SEQUENCE_22BIT(LPF,  value)
#define CX25840_SET_RX_TX_FIFO(value) CX25840__SEQUENCE_22BIT(RX_TX_FIFO,  value)
#define CX25840_SET_RX_TX_LVL(value) CX25840__SEQUENCE_6BIT(RX_TX_LVL,  value)
#define CX25840_SET_RXNDV(value) CX25840__SEQUENCE_6BIT(RXNDV,  value)
#define CX25840_SET_VID_FMT_SEL(value) CX25840__SEQUENCE_6BIT(VID_FMT_SEL,  value)
#define CX25840_SET_SQ_PIXEL(value) CX25840__SEQUENCE_6BIT(SQ_PIXEL,  value)
#define CX25840_SET_ACFG_DIS(value) CX25840__SEQUENCE_6BIT(ACFG_DIS,  value)
#define CX25840_SET_AFD_PAL_SEL(value) CX25840__SEQUENCE_6BIT(AFD_PAL_SEL,  value)
#define CX25840_SET_AFD_NTSC_SEL(value) CX25840__SEQUENCE_6BIT(AFD_NTSC_SEL,  value)
#define CX25840_SET_AFD_ACQUIRE(value) CX25840__SEQUENCE_6BIT(AFD_ACQUIRE,  value)
#define CX25840_SET_INPUT_MODE(value) CX25840__SEQUENCE_6BIT(INPUT_MODE,  value)
#define CX25840_SET_MAN_SC_FAST_LOCK(value) CX25840__SEQUENCE_6BIT(MAN_SC_FAST_LOCK,  value)
#define CX25840_SET_AUTO_SC_LOCK(value) CX25840__SEQUENCE_6BIT(AUTO_SC_LOCK,  value)
#define CX25840_SET_CKILLEN(value) CX25840__SEQUENCE_6BIT(CKILLEN,  value)
#define CX25840_SET_CAGCEN(value) CX25840__SEQUENCE_6BIT(CAGCEN,  value)
#define CX25840_SET_WCEN(value) CX25840__SEQUENCE_6BIT(WCEN,  value)
#define CX25840_SET_FAST_LOCK_MD(value) CX25840__SEQUENCE_6BIT(FAST_LOCK_MD,  value)
#define CX25840_SET_CLR_LOCK_STAT(value) CX25840__SEQUENCE_6BIT(CLR_LOCK_STAT,  value)
#define CX25840_SET_COMB_NOTCH_MODE(value) CX25840__SEQUENCE_6BIT(COMB_NOTCH_MODE,  value)
#define CX25840_SET_CKILL_MODE(value) CX25840__SEQUENCE_6BIT(CKILL_MODE,  value)
#define CX25840_SET_AFD_PALM_SEL(value) CX25840__SEQUENCE_6BIT(AFD_PALM_SEL,  value)
#define CX25840_SET_AFD_FORCE_PAL(value) CX25840__SEQUENCE_6BIT(AFD_FORCE_PAL,  value)
#define CX25840_SET_AFD_FORCE_PALNC(value) CX25840__SEQUENCE_6BIT(AFD_FORCE_PALNC,  value)
#define CX25840_SET_AFD_FORCE_SECAM(value) CX25840__SEQUENCE_6BIT(AFD_FORCE_SECAM,  value)
#define CX25840_SET_AFD_PAL60_DIS(value) CX25840__SEQUENCE_6BIT(AFD_PAL60_DIS,  value)
#define CX25840_SET_OUT_MODE(value) CX25840__SEQUENCE_6BIT(OUT_MODE,  value)
#define CX25840_SET_MODE10B(value) CX25840__SEQUENCE_6BIT(MODE10B,  value)
#define CX25840_SET_VBIHACTRAW_EN(value) CX25840__SEQUENCE_6BIT(VBIHACTRAW_EN,  value)
#define CX25840_SET_ANC_DATA_EN(value) CX25840__SEQUENCE_6BIT(ANC_DATA_EN,  value)
#define CX25840_SET_TASKBIT_VAL(value) CX25840__SEQUENCE_6BIT(TASKBIT_VAL,  value)
#define CX25840_SET_BLUE_FIELD_ACT(value) CX25840__SEQUENCE_6BIT(BLUE_FIELD_ACT,  value)
#define CX25840_SET_BLUE_FIELD_EN(value) CX25840__SEQUENCE_6BIT(BLUE_FIELD_EN,  value)
#define CX25840_SET_CLAMPRAW_EN(value) CX25840__SEQUENCE_6BIT(CLAMPRAW_EN,  value)
#define CX25840_SET_SWAPRAW(value) CX25840__SEQUENCE_6BIT(SWAPRAW,  value)
#define CX25840_SET_ACTFMT(value) CX25840__SEQUENCE_6BIT(ACTFMT,  value)
#define CX25840_SET_VALIDFMT(value) CX25840__SEQUENCE_6BIT(VALIDFMT,  value)
#define CX25840_SET_HSFMT(value) CX25840__SEQUENCE_6BIT(HSFMT,  value)
#define CX25840_SET_CLK_INVERT(value) CX25840__SEQUENCE_6BIT(CLK_INVERT,  value)
#define CX25840_SET_CLK_GATING(value) CX25840__SEQUENCE_6BIT(CLK_GATING,  value)
#define CX25840_SET_DCMODE(value) CX25840__SEQUENCE_6BIT(DCMODE,  value)
#define CX25840_SET_IDID0_SOURCE(value) CX25840__SEQUENCE_6BIT(IDID0_SOURCE,  value)
#define CX25840_SET_VIP_OPT_AL(value) CX25840__SEQUENCE_6BIT(VIP_OPT_AL,  value)
#define CX25840_SET_VIPBLANK_EN(value) CX25840__SEQUENCE_6BIT(VIPBLANK_EN,  value)
#define CX25840_SET_VIPCLAMP_EN(value) CX25840__SEQUENCE_6BIT(VIPCLAMP_EN,  value)
#define CX25840_SET_POLAR(value) CX25840__SEQUENCE_14BIT(POLAR,  value)
#define CX25840_SET_IDID0_9_2__LOW(value) CX25840__SEQUENCE_14BIT(IDID0_9_2__LOW,  value)
#define CX25840_SET_IDID1_9_2__LOW(value) CX25840__SEQUENCE_14BIT(IDID1_9_2__LOW,  value)
#define CX25840_SET_IDID0_1_0__HIGH(value) CX25840__SEQUENCE_6BIT(IDID0_1_0__HIGH,  value)
#define CX25840_SET_IDID1_1_0__HIGH(value) CX25840__SEQUENCE_6BIT(IDID1_1_0__HIGH,  value)
#define CX25840_SET_EN_A(value) CX25840__SEQUENCE_6BIT(EN_A,  value)
#define CX25840_SET_EN_B(value) CX25840__SEQUENCE_6BIT(EN_B,  value)
#define CX25840_SET_EN_C(value) CX25840__SEQUENCE_6BIT(EN_C,  value)
#define CX25840_SET_AUD_ANC_EN(value) CX25840__SEQUENCE_6BIT(AUD_ANC_EN,  value)
#define CX25840_SET_SAMPLE_RATE(value) CX25840__SEQUENCE_6BIT(SAMPLE_RATE,  value)
#define CX25840_SET_AUD_GRP(value) CX25840__SEQUENCE_6BIT(AUD_GRP,  value)
#define CX25840_SET_MV_CDAT(value) CX25840__SEQUENCE_6BIT(MV_CDAT,  value)
#define CX25840_SET_MV_PSP(value) CX25840__SEQUENCE_6BIT(MV_PSP,  value)
#define CX25840_SET_MV_CS(value) CX25840__SEQUENCE_6BIT(MV_CS,  value)
#define CX25840_SET_MV_T3CS(value) CX25840__SEQUENCE_6BIT(MV_T3CS,  value)
#define CX25840_SET_MV_TYPE2_PAIR(value) CX25840__SEQUENCE_6BIT(MV_TYPE2_PAIR,  value)
#define CX25840_SET_AFD_FMT_STAT(value) CX25840__SEQUENCE_6BIT(AFD_FMT_STAT,  value)
#define CX25840_SET_FIELD(value) CX25840__SEQUENCE_6BIT(FIELD,  value)
#define CX25840_SET_SRC_FIFO_OFLOW(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_OFLOW,  value)
#define CX25840_SET_SRC_FIFO_UFLOW(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_UFLOW,  value)
#define CX25840_SET_VSYNC(value) CX25840__SEQUENCE_6BIT(VSYNC,  value)
#define CX25840_SET_HLOCK(value) CX25840__SEQUENCE_6BIT(HLOCK,  value)
#define CX25840_SET_SRC_LOCK(value) CX25840__SEQUENCE_6BIT(SRC_LOCK,  value)
#define CX25840_SET_VLOCK(value) CX25840__SEQUENCE_6BIT(VLOCK,  value)
#define CX25840_SET_CSC_LOCK(value) CX25840__SEQUENCE_6BIT(CSC_LOCK,  value)
#define CX25840_SET_AGC_LOCK(value) CX25840__SEQUENCE_6BIT(AGC_LOCK,  value)
#define CX25840_SET_VPRES(value) CX25840__SEQUENCE_6BIT(VPRES,  value)
#define CX25840_SET_SPECIAL_PLAY_N(value) CX25840__SEQUENCE_6BIT(SPECIAL_PLAY_N,  value)
#define CX25840_SET_SRC_FIFO_OFLOW_STAT(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_OFLOW_STAT,  value)
#define CX25840_SET_SRC_FIFO_UFLOW_STAT(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_UFLOW_STAT,  value)
#define CX25840_SET_CSC_LOCK_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(CSC_LOCK_CHANGE_STAT,  value)
#define CX25840_SET_VLOCK_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(VLOCK_CHANGE_STAT,  value)
#define CX25840_SET_HLOCK_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(HLOCK_CHANGE_STAT,  value)
#define CX25840_SET_VSYNC_TRAIL_STAT(value) CX25840__SEQUENCE_6BIT(VSYNC_TRAIL_STAT,  value)
#define CX25840_SET_FMT_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(FMT_CHANGE_STAT,  value)
#define CX25840_SET_END_VBI_ODD_STAT(value) CX25840__SEQUENCE_6BIT(END_VBI_ODD_STAT,  value)
#define CX25840_SET_END_VBI_EVEN_STAT(value) CX25840__SEQUENCE_6BIT(END_VBI_EVEN_STAT,  value)
#define CX25840_SET_MV_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(MV_CHANGE_STAT,  value)
#define CX25840_SET_VPRES_CHANGE_STAT(value) CX25840__SEQUENCE_6BIT(VPRES_CHANGE_STAT,  value)
#define CX25840_SET_CC_DAT_AVAIL_STAT(value) CX25840__SEQUENCE_6BIT(CC_DAT_AVAIL_STAT,  value)
#define CX25840_SET_GS1_DAT_AVAIL_STAT(value) CX25840__SEQUENCE_6BIT(GS1_DAT_AVAIL_STAT,  value)
#define CX25840_SET_GS2_DAT_AVAIL_STAT(value) CX25840__SEQUENCE_6BIT(GS2_DAT_AVAIL_STAT,  value)
#define CX25840_SET_WSS_DAT_AVAIL_STAT(value) CX25840__SEQUENCE_6BIT(WSS_DAT_AVAIL_STAT,  value)
#define CX25840_SET_SRC_FIFO_OFLOW_MSK(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_OFLOW_MSK,  value)
#define CX25840_SET_SRC_FIFO_UFLOW_MSK(value) CX25840__SEQUENCE_6BIT(SRC_FIFO_UFLOW_MSK,  value)
#define CX25840_SET_CSC_LOCK_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(CSC_LOCK_CHANGE_MSK,  value)
#define CX25840_SET_VLOCK_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(VLOCK_CHANGE_MSK,  value)
#define CX25840_SET_HLOCK_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(HLOCK_CHANGE_MSK,  value)
#define CX25840_SET_VSYNC_TRAIL_MSK(value) CX25840__SEQUENCE_6BIT(VSYNC_TRAIL_MSK,  value)
#define CX25840_SET_FMT_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(FMT_CHANGE_MSK,  value)
#define CX25840_SET_END_VBI_ODD_MSK(value) CX25840__SEQUENCE_6BIT(END_VBI_ODD_MSK,  value)
#define CX25840_SET_END_VBI_EVEN_MSK(value) CX25840__SEQUENCE_6BIT(END_VBI_EVEN_MSK,  value)
#define CX25840_SET_MV_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(MV_CHANGE_MSK,  value)
#define CX25840_SET_VPRES_CHANGE_MSK(value) CX25840__SEQUENCE_6BIT(VPRES_CHANGE_MSK,  value)
#define CX25840_SET_CC_DAT_AVAIL_MSK(value) CX25840__SEQUENCE_6BIT(CC_DAT_AVAIL_MSK,  value)
#define CX25840_SET_GS1_DAT_AVAIL_MSK(value) CX25840__SEQUENCE_6BIT(GS1_DAT_AVAIL_MSK,  value)
#define CX25840_SET_GS2_DAT_AVAIL_MSK(value) CX25840__SEQUENCE_6BIT(GS2_DAT_AVAIL_MSK,  value)
#define CX25840_SET_WSS_DAT_AVAIL_MSK(value) CX25840__SEQUENCE_6BIT(WSS_DAT_AVAIL_MSK,  value)
#define CX25840_SET_BRIGHT(value) CX25840__SEQUENCE_14BIT(BRIGHT,  value)
#define CX25840_SET_CNTRST(value) CX25840__SEQUENCE_14BIT(CNTRST,  value)
#define CX25840_SET_PEAK_SEL(value) CX25840__SEQUENCE_6BIT(PEAK_SEL,  value)
#define CX25840_SET_PEAK_EN(value) CX25840__SEQUENCE_6BIT(PEAK_EN,  value)
#define CX25840_SET_RANGE(value) CX25840__SEQUENCE_6BIT(RANGE,  value)
#define CX25840_SET_LUMA_CORE_SEL(value) CX25840__SEQUENCE_6BIT(LUMA_CORE_SEL,  value)
#define CX25840_SET_HSCALE(value) CX25840__SEQUENCE_30BIT(HSCALE,  value)
#define CX25840_SET_HFILT(value) CX25840__SEQUENCE_6BIT(HFILT,  value)
#define CX25840_SET_VSCALE(value) CX25840__SEQUENCE_14BIT(VSCALE,  value)
#define CX25840_SET_VFILT(value) CX25840__SEQUENCE_6BIT(VFILT,  value)
#define CX25840_SET_VS_INTRLACE(value) CX25840__SEQUENCE_6BIT(VS_INTRLACE,  value)
#define CX25840_SET_LINE_AVG_DIS(value) CX25840__SEQUENCE_6BIT(LINE_AVG_DIS,  value)
#define CX25840_SET_USAT(value) CX25840__SEQUENCE_14BIT(USAT,  value)
#define CX25840_SET_VSAT(value) CX25840__SEQUENCE_14BIT(VSAT,  value)
#define CX25840_SET_HUE(value) CX25840__SEQUENCE_14BIT(HUE,  value)
#define CX25840_SET_C_CORE_SEL(value) CX25840__SEQUENCE_6BIT(C_CORE_SEL,  value)
#define CX25840_SET_CHR_DELAY(value) CX25840__SEQUENCE_6BIT(CHR_DELAY,  value)
#define CX25840_SET_VBI_MD_LINE1(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE1,  value)
#define CX25840_SET_VBI_MD_LINE2(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE2,  value)
#define CX25840_SET_VBI_MD_LINE3(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE3,  value)
#define CX25840_SET_VBI_MD_LINE4(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE4,  value)
#define CX25840_SET_VBI_MD_LINE5(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE5,  value)
#define CX25840_SET_VBI_MD_LINE6(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE6,  value)
#define CX25840_SET_VBI_MD_LINE7(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE7,  value)
#define CX25840_SET_VBI_MD_LINE8(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE8,  value)
#define CX25840_SET_VBI_MD_LINE9(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE9,  value)
#define CX25840_SET_VBI_MD_LINE10(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE10,  value)
#define CX25840_SET_VBI_MD_LINE11(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE11,  value)
#define CX25840_SET_VBI_MD_LINE12(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE12,  value)
#define CX25840_SET_VBI_MD_LINE13(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE13,  value)
#define CX25840_SET_VBI_MD_LINE14(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE14,  value)
#define CX25840_SET_VBI_MD_LINE15(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE15,  value)
#define CX25840_SET_VBI_MD_LINE16(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE16,  value)
#define CX25840_SET_VBI_MD_LINE17(value) CX25840__SEQUENCE_14BIT(VBI_MD_LINE17,  value)
#define CX25840_SET_FC_SEARCH_MODE(value) CX25840__SEQUENCE_6BIT(FC_SEARCH_MODE,  value)
#define CX25840_SET_FC_ALT1_TYPE(value) CX25840__SEQUENCE_6BIT(FC_ALT1_TYPE,  value)
#define CX25840_SET_FC_ALT2_TYPE(value) CX25840__SEQUENCE_6BIT(FC_ALT2_TYPE,  value)
#define CX25840_SET_FC_ALT1(value) CX25840__SEQUENCE_14BIT(FC_ALT1,  value)
#define CX25840_SET_FC_ALT2(value) CX25840__SEQUENCE_14BIT(FC_ALT2,  value)
#define CX25840_SET_ADAPT_SLICE_DIS(value) CX25840__SEQUENCE_6BIT(ADAPT_SLICE_DIS,  value)
#define CX25840_SET_EDGE_RESYNC_EN(value) CX25840__SEQUENCE_6BIT(EDGE_RESYNC_EN,  value)
#define CX25840_SET_CRI_MARG_SCALE(value) CX25840__SEQUENCE_6BIT(CRI_MARG_SCALE,  value)
#define CX25840_SET_VPS_DEC_DIS(value) CX25840__SEQUENCE_6BIT(VPS_DEC_DIS,  value)
#define CX25840_SET_MOJI_PACK_DIS(value) CX25840__SEQUENCE_6BIT(MOJI_PACK_DIS,  value)
#define CX25840_SET_TTX_PKTADRL_LB(value) CX25840__SEQUENCE_14BIT(TTX_PKTADRL_LB,  value)
#define CX25840_SET_TTX_PKTADRL_HN(value) CX25840__SEQUENCE_6BIT(TTX_PKTADRL_HN,  value)
#define CX25840_SET_TTX_PKTADRU_LN(value) CX25840__SEQUENCE_6BIT(TTX_PKTADRU_LN,  value)
#define CX25840_SET_TTX_PKTADRU_HB(value) CX25840__SEQUENCE_14BIT(TTX_PKTADRU_HB,  value)
#define CX25840_SET_VBI1_SDID(value) CX25840__SEQUENCE_6BIT(VBI1_SDID,  value)
#define CX25840_SET_VBI2_SDID(value) CX25840__SEQUENCE_6BIT(VBI2_SDID,  value)
#define CX25840_SET_VBI3_SDID(value) CX25840__SEQUENCE_6BIT(VBI3_SDID,  value)
#define CX25840_SET_CC_FIFO_RST(value) CX25840__SEQUENCE_6BIT(CC_FIFO_RST,  value)
#define CX25840_SET_GS1_FIFO_RST(value) CX25840__SEQUENCE_6BIT(GS1_FIFO_RST,  value)
#define CX25840_SET_GS2_FIFO_RST(value) CX25840__SEQUENCE_6BIT(GS2_FIFO_RST,  value)
#define CX25840_SET_WSS_FIFO_RST(value) CX25840__SEQUENCE_6BIT(WSS_FIFO_RST,  value)
#define CX25840_SET_HAMMING_TYPE(value) CX25840__SEQUENCE_6BIT(HAMMING_TYPE,  value)
#define CX25840_SET_CC_STAT(value) CX25840__SEQUENCE_14BIT(CC_STAT,  value)
#define CX25840_SET_CC_FIFO_DAT(value) CX25840__SEQUENCE_14BIT(CC_FIFO_DAT,  value)
#define CX25840_SET_GS1_STAT(value) CX25840__SEQUENCE_14BIT(GS1_STAT,  value)
#define CX25840_SET_GS1_FIFO_DAT(value) CX25840__SEQUENCE_14BIT(GS1_FIFO_DAT,  value)
#define CX25840_SET_GS2_STAT(value) CX25840__SEQUENCE_14BIT(GS2_STAT,  value)
#define CX25840_SET_GS2_FIFO_DAT(value) CX25840__SEQUENCE_14BIT(GS2_FIFO_DAT,  value)
#define CX25840_SET_WSS_STAT(value) CX25840__SEQUENCE_14BIT(WSS_STAT,  value)
#define CX25840_SET_WSS_FIFO_DAT(value) CX25840__SEQUENCE_14BIT(WSS_FIFO_DAT,  value)
#define CX25840_SET_VBI1_HDELAY(value) CX25840__SEQUENCE_14BIT(VBI1_HDELAY,  value)
#define CX25840_SET_VBI1_BITINC(value) CX25840__SEQUENCE_14BIT(VBI1_BITINC,  value)
#define CX25840_SET_VBI1_SLICE_DIST(value) CX25840__SEQUENCE_6BIT(VBI1_SLICE_DIST,  value)
#define CX25840_SET_VBI1_CRWIN(value) CX25840__SEQUENCE_14BIT(VBI1_CRWIN,  value)
#define CX25840_SET_VBI1_FRAME_CODE(value) CX25840__SEQUENCE_30BIT(VBI1_FRAME_CODE,  value)
#define CX25840_SET_VBI1_FC_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI1_FC_LENGTH,  value)
#define CX25840_SET_VBI1_CRI_TIME(value) CX25840__SEQUENCE_14BIT(VBI1_CRI_TIME,  value)
#define CX25840_SET_VBI1_CRI_MARGIN(value) CX25840__SEQUENCE_6BIT(VBI1_CRI_MARGIN,  value)
#define CX25840_SET_VBI1_CRI_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI1_CRI_LENGTH,  value)
#define CX25840_SET_VBI1_PAYLD_LENGTH(value) CX25840__SEQUENCE_14BIT(VBI1_PAYLD_LENGTH,  value)
#define CX25840_SET_VBI1_FORMAT_TYPE(value) CX25840__SEQUENCE_6BIT(VBI1_FORMAT_TYPE,  value)
#define CX25840_SET_VBI1_FIFO_MODE(value) CX25840__SEQUENCE_6BIT(VBI1_FIFO_MODE,  value)
#define CX25840_SET_VBI1_HAM_EN(value) CX25840__SEQUENCE_6BIT(VBI1_HAM_EN,  value)
#define CX25840_SET_VBI2_HDELAY(value) CX25840__SEQUENCE_14BIT(VBI2_HDELAY,  value)
#define CX25840_SET_VBI2_BITINC(value) CX25840__SEQUENCE_14BIT(VBI2_BITINC,  value)
#define CX25840_SET_VBI2_SLICE_DIST(value) CX25840__SEQUENCE_6BIT(VBI2_SLICE_DIST,  value)
#define CX25840_SET_VBI1_CRIWIN(value) CX25840__SEQUENCE_14BIT(VBI1_CRIWIN,  value)
#define CX25840_SET_VBI2_FRAME_CODE(value) CX25840__SEQUENCE_30BIT(VBI2_FRAME_CODE,  value)
#define CX25840_SET_VBI2_FC_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI2_FC_LENGTH,  value)
#define CX25840_SET_VBI2_CRI_TIME(value) CX25840__SEQUENCE_14BIT(VBI2_CRI_TIME,  value)
#define CX25840_SET_VBI2_CRI_MARGIN(value) CX25840__SEQUENCE_6BIT(VBI2_CRI_MARGIN,  value)
#define CX25840_SET_VBI2_CRI_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI2_CRI_LENGTH,  value)
#define CX25840_SET_VBI2_PAYLD_LENGTH(value) CX25840__SEQUENCE_14BIT(VBI2_PAYLD_LENGTH,  value)
#define CX25840_SET_VBI2_FORMAT_TYPE(value) CX25840__SEQUENCE_6BIT(VBI2_FORMAT_TYPE,  value)
#define CX25840_SET_VBI2_FIFO_MODE(value) CX25840__SEQUENCE_6BIT(VBI2_FIFO_MODE,  value)
#define CX25840_SET_VBI2_HAM_EN(value) CX25840__SEQUENCE_6BIT(VBI2_HAM_EN,  value)
#define CX25840_SET_VBI3_HDELAY(value) CX25840__SEQUENCE_14BIT(VBI3_HDELAY,  value)
#define CX25840_SET_VBI3_BITINC(value) CX25840__SEQUENCE_14BIT(VBI3_BITINC,  value)
#define CX25840_SET_VBI3_SLICE_DIST(value) CX25840__SEQUENCE_6BIT(VBI3_SLICE_DIST,  value)
#define CX25840_SET_VBI3_CRWIN(value) CX25840__SEQUENCE_14BIT(VBI3_CRWIN,  value)
#define CX25840_SET_VBI3_FRAME_CODE(value) CX25840__SEQUENCE_30BIT(VBI3_FRAME_CODE,  value)
#define CX25840_SET_VBI3_FC_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI3_FC_LENGTH,  value)
#define CX25840_SET_VBI3_CRI_TIME(value) CX25840__SEQUENCE_14BIT(VBI3_CRI_TIME,  value)
#define CX25840_SET_VBI3_CRI_MARGIN(value) CX25840__SEQUENCE_6BIT(VBI3_CRI_MARGIN,  value)
#define CX25840_SET_VBI3_CRI_LENGTH(value) CX25840__SEQUENCE_6BIT(VBI3_CRI_LENGTH,  value)
#define CX25840_SET_VBI3_PAYLD_LENGTH(value) CX25840__SEQUENCE_14BIT(VBI3_PAYLD_LENGTH,  value)
#define CX25840_SET_VBI3_FORMAT_TYPE(value) CX25840__SEQUENCE_6BIT(VBI3_FORMAT_TYPE,  value)
#define CX25840_SET_VBI3_FIFO_MODE(value) CX25840__SEQUENCE_6BIT(VBI3_FIFO_MODE,  value)
#define CX25840_SET_VBI3_HAM_EN(value) CX25840__SEQUENCE_6BIT(VBI3_HAM_EN,  value)
#define CX25840_SET_HBLANK_CNT(value) CX25840__SEQUENCE_14BIT(HBLANK_CNT,  value)
#define CX25840_SET_HACTIVE_CNT(value) CX25840__SEQUENCE_14BIT(HACTIVE_CNT,  value)
#define CX25840_SET_BGDEL_CNT(value) CX25840__SEQUENCE_14BIT(BGDEL_CNT,  value)
#define CX25840_SET_VBLANK_CNT(value) CX25840__SEQUENCE_14BIT(VBLANK_CNT,  value)
#define CX25840_SET_VACTIVE_CNT(value) CX25840__SEQUENCE_14BIT(VACTIVE_CNT,  value)
#define CX25840_SET_V656BLANK_CNT(value) CX25840__SEQUENCE_14BIT(V656BLANK_CNT,  value)
#define CX25840_SET_SRC_DECIM_RATIO(value) CX25840__SEQUENCE_14BIT(SRC_DECIM_RATIO,  value)
#define CX25840_SET_UV_LPF_SEL(value) CX25840__SEQUENCE_6BIT(UV_LPF_SEL,  value)
#define CX25840_SET_LUMA_LPF_SEL(value) CX25840__SEQUENCE_6BIT(LUMA_LPF_SEL,  value)
#define CX25840_SET_LCOMB_2LN_EN(value) CX25840__SEQUENCE_6BIT(LCOMB_2LN_EN,  value)
#define CX25840_SET_LCOMB_3LN_EN(value) CX25840__SEQUENCE_6BIT(LCOMB_3LN_EN,  value)
#define CX25840_SET_CCOMB_2LN_EN(value) CX25840__SEQUENCE_6BIT(CCOMB_2LN_EN,  value)
#define CX25840_SET_CCOMB_3LN_EN(value) CX25840__SEQUENCE_6BIT(CCOMB_3LN_EN,  value)
#define CX25840_SET_SC_STEP(value) CX25840__SEQUENCE_30BIT(SC_STEP,  value)
#define CX25840_SET_VBI_OFFSET(value) CX25840__SEQUENCE_6BIT(VBI_OFFSET,  value)
#define CX25840_SET_FIELD_COUNT(value) CX25840__SEQUENCE_14BIT(FIELD_COUNT,  value)
#define CX25840_SET_TEMPDEC(value) CX25840__SEQUENCE_6BIT(TEMPDEC,  value)
#define CX25840_SET_TDFIELD(value) CX25840__SEQUENCE_6BIT(TDFIELD,  value)
#define CX25840_SET_TDALGN(value) CX25840__SEQUENCE_6BIT(TDALGN,  value)
#define CX25840_SET_HR32(value) CX25840__SEQUENCE_6BIT(HR32,  value)
#define CX25840_SET_VPRES_VERT_EN(value) CX25840__SEQUENCE_6BIT(VPRES_VERT_EN,  value)
#define CX25840_SET_VT_LINE_CNT_HYST(value) CX25840__SEQUENCE_6BIT(VT_LINE_CNT_HYST,  value)
#define CX25840_SET_DEBOUNCE_COUNT(value) CX25840__SEQUENCE_6BIT(DEBOUNCE_COUNT,  value)
#define CX25840_SET_VGA_GAIN(value) CX25840__SEQUENCE_6BIT(VGA_GAIN,  value)
#define CX25840_SET_AGC_GAIN(value) CX25840__SEQUENCE_14BIT(AGC_GAIN,  value)
#define CX25840_SET_CLAMP_LEVEL(value) CX25840__SEQUENCE_6BIT(CLAMP_LEVEL,  value)
#define CX25840_SET_VBI_GATE_EN(value) CX25840__SEQUENCE_6BIT(VBI_GATE_EN,  value)
#define CX25840_SET_VGA_AUTO_EN(value) CX25840__SEQUENCE_6BIT(VGA_AUTO_EN,  value)
#define CX25840_SET_VGA_CRUSH_EN(value) CX25840__SEQUENCE_6BIT(VGA_CRUSH_EN,  value)
#define CX25840_SET_AGC_AUTO_EN(value) CX25840__SEQUENCE_6BIT(AGC_AUTO_EN,  value)
#define CX25840_SET_CLAMP_AUTO_EN(value) CX25840__SEQUENCE_6BIT(CLAMP_AUTO_EN,  value)
#define CX25840_SET_VGA_SYNC(value) CX25840__SEQUENCE_14BIT(VGA_SYNC,  value)
#define CX25840_SET_VGA_TRACK_RANGE(value) CX25840__SEQUENCE_14BIT(VGA_TRACK_RANGE,  value)
#define CX25840_SET_VGA_ACQUIRE_RANGE(value) CX25840__SEQUENCE_14BIT(VGA_ACQUIRE_RANGE,  value)
#define CX25840_SET_DCC_LOOP_GAIN(value) CX25840__SEQUENCE_6BIT(DCC_LOOP_GAIN,  value)
#define CX25840_SET_AGC_LOOP_GAIN(value) CX25840__SEQUENCE_6BIT(AGC_LOOP_GAIN,  value)
#define CX25840_SET_SYNC_LOOP_GAIN(value) CX25840__SEQUENCE_6BIT(SYNC_LOOP_GAIN,  value)
#define CX25840_SET_BP_LOOP_GAIN(value) CX25840__SEQUENCE_6BIT(BP_LOOP_GAIN,  value)
#define CX25840_SET_DFT_THRESHOLD(value) CX25840__SEQUENCE_14BIT(DFT_THRESHOLD,  value)
#define CX25840_SET_BP_PERCENT(value) CX25840__SEQUENCE_14BIT(BP_PERCENT,  value)
#define CX25840_SET_PLL_MAX_OFFSET(value) CX25840__SEQUENCE_22BIT(PLL_MAX_OFFSET,  value)
#define CX25840_SET_PLL_KI(value) CX25840__SEQUENCE_14BIT(PLL_KI,  value)
#define CX25840_SET_PLL_KD(value) CX25840__SEQUENCE_14BIT(PLL_KD,  value)
#define CX25840_SET_HTL_KI(value) CX25840__SEQUENCE_6BIT(HTL_KI,  value)
#define CX25840_SET_HTL_KD(value) CX25840__SEQUENCE_6BIT(HTL_KD,  value)
#define CX25840_SET_LCOMB_ERR_LIMIT(value) CX25840__SEQUENCE_14BIT(LCOMB_ERR_LIMIT,  value)
#define CX25840_SET_LUMA_THRESHOLD(value) CX25840__SEQUENCE_14BIT(LUMA_THRESHOLD,  value)
#define CX25840_SET_CCOMB_ERR_LIMIT(value) CX25840__SEQUENCE_14BIT(CCOMB_ERR_LIMIT,  value)
#define CX25840_SET_COMB_PHASE_LIMIT(value) CX25840__SEQUENCE_14BIT(COMB_PHASE_LIMIT,  value)
#define CX25840_SET_SYNC_TIP_INC(value) CX25840__SEQUENCE_6BIT(SYNC_TIP_INC,  value)
#define CX25840_SET_SYNC_TIP_REDUCE(value) CX25840__SEQUENCE_6BIT(SYNC_TIP_REDUCE,  value)
#define CX25840_SET_MAJ_SEL(value) CX25840__SEQUENCE_6BIT(MAJ_SEL,  value)
#define CX25840_SET_MAJ_SEL_EN(value) CX25840__SEQUENCE_6BIT(MAJ_SEL_EN,  value)
#define CX25840_SET_CRUSH_FREQ(value) CX25840__SEQUENCE_6BIT(CRUSH_FREQ,  value)
#define CX25840_SET_WTW_EN(value) CX25840__SEQUENCE_6BIT(WTW_EN,  value)
#define CX25840_SET_DFE_RST_MSK(value) CX25840__SEQUENCE_6BIT(DFE_RST_MSK,  value)
#define CX25840_SET_SRC_RST_MSK(value) CX25840__SEQUENCE_6BIT(SRC_RST_MSK,  value)
#define CX25840_SET_YCSEP_RST_MSK(value) CX25840__SEQUENCE_6BIT(YCSEP_RST_MSK,  value)
#define CX25840_SET_VTG_RST_MSK(value) CX25840__SEQUENCE_6BIT(VTG_RST_MSK,  value)
#define CX25840_SET_LUMA_RST_MSK(value) CX25840__SEQUENCE_6BIT(LUMA_RST_MSK,  value)
#define CX25840_SET_CHROMA_RST_MSK(value) CX25840__SEQUENCE_6BIT(CHROMA_RST_MSK,  value)
#define CX25840_SET_SCALE_RST_MSK(value) CX25840__SEQUENCE_6BIT(SCALE_RST_MSK,  value)
#define CX25840_SET_VBI_RST_MSK(value) CX25840__SEQUENCE_6BIT(VBI_RST_MSK,  value)
#define CX25840_SET_MVDET_RST_MSK(value) CX25840__SEQUENCE_6BIT(MVDET_RST_MSK,  value)
#define CX25840_SET_VOF_RST_MSK(value) CX25840__SEQUENCE_6BIT(VOF_RST_MSK,  value)
#define CX25840_SET_REG_RST_MSK(value) CX25840__SEQUENCE_6BIT(REG_RST_MSK,  value)
#define CX25840_SET_VD_SOFT_RST(value) CX25840__SEQUENCE_6BIT(VD_SOFT_RST,  value)
#define CX25840_SET_REV_ID(value) CX25840__SEQUENCE_14BIT(REV_ID,  value)
#define CX25840_SET_APL_DETECT_ENA(value) CX25840__SEQUENCE_6BIT(APL_DETECT_ENA,  value)
#define CX25840_SET_DL_ADDR_LB(value) CX25840__SEQUENCE_14BIT(DL_ADDR_LB,  value)
#define CX25840_SET_DL_ADDR_HB(value) CX25840__SEQUENCE_14BIT(DL_ADDR_HB,  value)
#define CX25840_SET_DL_DATA_CTL(value) CX25840__SEQUENCE_14BIT(DL_DATA_CTL,  value)
#define CX25840_SET_DL_MAP(value) CX25840__SEQUENCE_6BIT(DL_MAP,  value)
#define CX25840_SET_DL_AUTO_INC(value) CX25840__SEQUENCE_6BIT(DL_AUTO_INC,  value)
#define CX25840_SET_DL_ENABLE(value) CX25840__SEQUENCE_6BIT(DL_ENABLE,  value)
#define CX25840_SET_START_MICROCNTL(value) CX25840__SEQUENCE_6BIT(START_MICROCNTL,  value)
#define CX25840_SET_MOD_DET_STATUS0(value) CX25840__SEQUENCE_14BIT(MOD_DET_STATUS0,  value)
#define CX25840_SET_MOD_DET_STATUS1(value) CX25840__SEQUENCE_14BIT(MOD_DET_STATUS1,  value)
#define CX25840_SET_AUD_MODE_AUD_SYSTEM(value) CX25840__SEQUENCE_6BIT(AUD_MODE_AUD_SYSTEM,  value)
#define CX25840_SET_AUD_STANDARD(value) CX25840__SEQUENCE_6BIT(AUD_STANDARD,  value)
#define CX25840_SET_PREF_MODE(value) CX25840__SEQUENCE_6BIT(PREF_MODE,  value)
#define CX25840_SET_MUTE_NO_PREF_MODE(value) CX25840__SEQUENCE_6BIT(MUTE_NO_PREF_MODE,  value)
#define CX25840_SET_DE_EMPHASIS_TIME(value) CX25840__SEQUENCE_6BIT(DE_EMPHASIS_TIME,  value)
#define CX25840_SET_FM_DEVIATION(value) CX25840__SEQUENCE_6BIT(FM_DEVIATION,  value)
#define CX25840_SET_MICROCNTL_VIDEO_FORMAT(value) CX25840__SEQUENCE_6BIT(MICROCNTL_VIDEO_FORMAT,  value)
#define CX25840_SET_VIDEO_PRESENT(value) CX25840__SEQUENCE_6BIT(VIDEO_PRESENT,  value)
#define CX25840_SET_FORMAT_45MHZ(value) CX25840__SEQUENCE_6BIT(FORMAT_45MHZ,  value)
#define CX25840_SET_FORMAT_65MHZ(value) CX25840__SEQUENCE_6BIT(FORMAT_65MHZ,  value)
#define CX25840_SET_TUNER_OUTPUT_FORMAT(value) CX25840__SEQUENCE_6BIT(TUNER_OUTPUT_FORMAT,  value)
#define CX25840_SET_SOFT_RESET(value) CX25840__SEQUENCE_6BIT(SOFT_RESET,  value)
#define CX25840_SET_AC97_INT_DIS(value) CX25840__SEQUENCE_6BIT(AC97_INT_DIS,  value)
#define CX25840_SET_AMC_INT_DIS(value) CX25840__SEQUENCE_6BIT(AMC_INT_DIS,  value)
#define CX25840_SET_FC_INT_DIS(value) CX25840__SEQUENCE_6BIT(FC_INT_DIS,  value)
#define CX25840_SET_FDL_INT_DIS(value) CX25840__SEQUENCE_6BIT(FDL_INT_DIS,  value)
#define CX25840_SET_IFL_INT_DIS(value) CX25840__SEQUENCE_6BIT(IFL_INT_DIS,  value)
#define CX25840_SET_NLL_INT_DIS(value) CX25840__SEQUENCE_6BIT(NLL_INT_DIS,  value)
#define CX25840_SET_NBER_INT_DIS(value) CX25840__SEQUENCE_6BIT(NBER_INT_DIS,  value)
#define CX25840_SET_RDS_INT_DIS(value) CX25840__SEQUENCE_6BIT(RDS_INT_DIS,  value)
#define CX25840_SET_AC97_INT(value) CX25840__SEQUENCE_6BIT(AC97_INT,  value)
#define CX25840_SET_AMC_INT(value) CX25840__SEQUENCE_6BIT(AMC_INT,  value)
#define CX25840_SET_AFC_INT(value) CX25840__SEQUENCE_6BIT(AFC_INT,  value)
#define CX25840_SET_FDL_INT(value) CX25840__SEQUENCE_6BIT(FDL_INT,  value)
#define CX25840_SET_IFL_INT(value) CX25840__SEQUENCE_6BIT(IFL_INT,  value)
#define CX25840_SET_NLL_INT(value) CX25840__SEQUENCE_6BIT(NLL_INT,  value)
#define CX25840_SET_NBER_INT(value) CX25840__SEQUENCE_6BIT(NBER_INT,  value)
#define CX25840_SET_RDS_INT(value) CX25840__SEQUENCE_6BIT(RDS_INT,  value)
#define CX25840_SET_AAGC_HYST1(value) CX25840__SEQUENCE_6BIT(AAGC_HYST1,  value)
#define CX25840_SET_AAGC_HYST2(value) CX25840__SEQUENCE_6BIT(AAGC_HYST2,  value)
#define CX25840_SET_AAGC_TH(value) CX25840__SEQUENCE_6BIT(AAGC_TH,  value)
#define CX25840_SET_AAGC_GAIN_EN(value) CX25840__SEQUENCE_6BIT(AAGC_GAIN_EN,  value)
#define CX25840_SET_AAGC_DEFAULT(value) CX25840__SEQUENCE_6BIT(AAGC_DEFAULT,  value)
#define CX25840_SET_AAGC_DEFAULT_EN(value) CX25840__SEQUENCE_6BIT(AAGC_DEFAULT_EN,  value)
#define CX25840_SET_AFE_12DB_EN(value) CX25840__SEQUENCE_6BIT(AFE_12DB_EN,  value)
#define CX25840_SET_IF_SRC_MODE(value) CX25840__SEQUENCE_6BIT(IF_SRC_MODE,  value)
#define CX25840_SET_PHASE_FIX(value) CX25840__SEQUENCE_6BIT(PHASE_FIX,  value)
#define CX25840_SET_PH_CH_SEL(value) CX25840__SEQUENCE_6BIT(PH_CH_SEL,  value)
#define CX25840_SET_PH_DBX_SEL(value) CX25840__SEQUENCE_6BIT(PH_DBX_SEL,  value)
#define CX25840_SET_DEMATRIX_MODE(value) CX25840__SEQUENCE_6BIT(DEMATRIX_MODE,  value)
#define CX25840_SET_DMTRX_BYPASS(value) CX25840__SEQUENCE_6BIT(DMTRX_BYPASS,  value)
#define CX25840_SET_DEMATRIX_SEL_CTL(value) CX25840__SEQUENCE_14BIT(DEMATRIX_SEL_CTL,  value)
#define CX25840_SET_SA_IN_SHIFT(value) CX25840__SEQUENCE_6BIT(SA_IN_SHIFT,  value)
#define CX25840_SET_AC97_IN_SHIFT(value) CX25840__SEQUENCE_6BIT(AC97_IN_SHIFT,  value)
#define CX25840_SET_PATH1_SEL_CTL(value) CX25840__SEQUENCE_6BIT(PATH1_SEL_CTL,  value)
#define CX25840_SET_PATH1_AVC_RMS_CON(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_RMS_CON,  value)
#define CX25840_SET_PATH1_AVC_CR(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_CR,  value)
#define CX25840_SET_PATH1_AVC_STEREO(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_STEREO,  value)
#define CX25840_SET_PATH1_AVC_AT(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_AT,  value)
#define CX25840_SET_PATH1_AVC_RT(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_RT,  value)
#define CX25840_SET_PATH1_AVC_CG(value) CX25840__SEQUENCE_6BIT(PATH1_AVC_CG,  value)
#define CX25840_SET_SOFT1_MUTE_EN(value) CX25840__SEQUENCE_6BIT(SOFT1_MUTE_EN,  value)
#define CX25840_SET_SRC1_MUTE_EN(value) CX25840__SEQUENCE_6BIT(SRC1_MUTE_EN,  value)
#define CX25840_SET_SA_MUTE_EN(value) CX25840__SEQUENCE_6BIT(SA_MUTE_EN,  value)
#define CX25840_SET_PAR_MUTE_EN(value) CX25840__SEQUENCE_6BIT(PAR_MUTE_EN,  value)
#define CX25840_SET_AC97_MUTE_EN(value) CX25840__SEQUENCE_6BIT(AC97_MUTE_EN,  value)
#define CX25840_SET_PATH1_VOLUME(value) CX25840__SEQUENCE_14BIT(PATH1_VOLUME,  value)
#define CX25840_SET_PATH1_BAL_LEVEL(value) CX25840__SEQUENCE_14BIT(PATH1_BAL_LEVEL,  value)
#define CX25840_SET_PATH1_BAL_LEFT(value) CX25840__SEQUENCE_6BIT(PATH1_BAL_LEFT,  value)
#define CX25840_SET_PATH1_AVC_THRESH(value) CX25840__SEQUENCE_22BIT(PATH1_AVC_THRESH,  value)
#define CX25840_SET_PATH1_EQ_BAND_SEL(value) CX25840__SEQUENCE_6BIT(PATH1_EQ_BAND_SEL,  value)
#define CX25840_SET_PATH1_EQ_BASS_VOL(value) CX25840__SEQUENCE_6BIT(PATH1_EQ_BASS_VOL,  value)
#define CX25840_SET_PATH1_EQ_MID_VOL(value) CX25840__SEQUENCE_6BIT(PATH1_EQ_MID_VOL,  value)
#define CX25840_SET_PATH1_EQ_TREBLE_VOL(value) CX25840__SEQUENCE_6BIT(PATH1_EQ_TREBLE_VOL,  value)
#define CX25840_SET_PATH1_SC_RMS_CON(value) CX25840__SEQUENCE_6BIT(PATH1_SC_RMS_CON,  value)
#define CX25840_SET_PATH1_SC_CR(value) CX25840__SEQUENCE_6BIT(PATH1_SC_CR,  value)
#define CX25840_SET_PATH1_SC_STEREO(value) CX25840__SEQUENCE_6BIT(PATH1_SC_STEREO,  value)
#define CX25840_SET_PATH1_SC_AT(value) CX25840__SEQUENCE_6BIT(PATH1_SC_AT,  value)
#define CX25840_SET_PATH1_SC_RT(value) CX25840__SEQUENCE_6BIT(PATH1_SC_RT,  value)
#define CX25840_SET_PATH1_SC_THRESH(value) CX25840__SEQUENCE_22BIT(PATH1_SC_THRESH,  value)
#define CX25840_SET_PATH2_SEL_CTL(value) CX25840__SEQUENCE_6BIT(PATH2_SEL_CTL,  value)
#define CX25840_SET_PATH2_AVC_RMS_CON(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_RMS_CON,  value)
#define CX25840_SET_PATH2_AVC_CR(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_CR,  value)
#define CX25840_SET_PATH2_AVC_STEREO(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_STEREO,  value)
#define CX25840_SET_PATH2_AVC_AT(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_AT,  value)
#define CX25840_SET_PATH2_AVC_RT(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_RT,  value)
#define CX25840_SET_PATH2_AVC_CG(value) CX25840__SEQUENCE_6BIT(PATH2_AVC_CG,  value)
#define CX25840_SET_SOFT2_MUTE_EN(value) CX25840__SEQUENCE_6BIT(SOFT2_MUTE_EN,  value)
#define CX25840_SET_SRC2_MUTE_EN(value) CX25840__SEQUENCE_6BIT(SRC2_MUTE_EN,  value)
#define CX25840_SET_PATH2_VOLUME(value) CX25840__SEQUENCE_14BIT(PATH2_VOLUME,  value)
#define CX25840_SET_PATH2_BAL_LEVEL(value) CX25840__SEQUENCE_14BIT(PATH2_BAL_LEVEL,  value)
#define CX25840_SET_PATH2_BAL_LEFT(value) CX25840__SEQUENCE_6BIT(PATH2_BAL_LEFT,  value)
#define CX25840_SET_PATH2_AVC_THRESH(value) CX25840__SEQUENCE_22BIT(PATH2_AVC_THRESH,  value)
#define CX25840_SET_PATH2_EQ_BAND_SEL(value) CX25840__SEQUENCE_6BIT(PATH2_EQ_BAND_SEL,  value)
#define CX25840_SET_PATH2_EQ_BASS_VOL(value) CX25840__SEQUENCE_6BIT(PATH2_EQ_BASS_VOL,  value)
#define CX25840_SET_PATH2_EQ_MID_VOL(value) CX25840__SEQUENCE_6BIT(PATH2_EQ_MID_VOL,  value)
#define CX25840_SET_PATH2_EQ_TREBLE_VOL(value) CX25840__SEQUENCE_6BIT(PATH2_EQ_TREBLE_VOL,  value)
#define CX25840_SET_PATH2_SC_RMS_CON(value) CX25840__SEQUENCE_6BIT(PATH2_SC_RMS_CON,  value)
#define CX25840_SET_PATH2_SC_CR(value) CX25840__SEQUENCE_6BIT(PATH2_SC_CR,  value)
#define CX25840_SET_PATH2_SC_STEREO(value) CX25840__SEQUENCE_6BIT(PATH2_SC_STEREO,  value)
#define CX25840_SET_PATH2_SC_AT(value) CX25840__SEQUENCE_6BIT(PATH2_SC_AT,  value)
#define CX25840_SET_PATH2_SC_RT(value) CX25840__SEQUENCE_6BIT(PATH2_SC_RT,  value)
#define CX25840_SET_PATH2_SC_THRESH(value) CX25840__SEQUENCE_22BIT(PATH2_SC_THRESH,  value)
#define CX25840_SET_Sample_Rate_Converter_Status1_REG(value) CX25840__SEQUENCE_14BIT(Sample_Rate_Converter_Status1_REG,  value)
#define CX25840_SET_Sample_Rate_Converter_Status2_REG(value) CX25840__SEQUENCE_14BIT(Sample_Rate_Converter_Status2_REG,  value)
#define CX25840_SET_SRC1_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC1_PHASE_INC,  value)
#define CX25840_SET_SRC1_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC1_FIFO_RD_TH,  value)
#define CX25840_SET_SRC2_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC2_PHASE_INC,  value)
#define CX25840_SET_SRC2_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC2_FIFO_RD_TH,  value)
#define CX25840_SET_SRC3_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC3_PHASE_INC,  value)
#define CX25840_SET_SRC3_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC3_FIFO_RD_TH,  value)
#define CX25840_SET_SRC4_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC4_PHASE_INC,  value)
#define CX25840_SET_SRC4_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC4_FIFO_RD_TH,  value)
#define CX25840_SET_SRC5_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC5_PHASE_INC,  value)
#define CX25840_SET_SRC5_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC5_FIFO_RD_TH,  value)
#define CX25840_SET_SRC6_PHASE_INC(value) CX25840__SEQUENCE_22BIT(SRC6_PHASE_INC,  value)
#define CX25840_SET_SRC6_FIFO_RD_TH(value) CX25840__SEQUENCE_6BIT(SRC6_FIFO_RD_TH,  value)
#define CX25840_SET_PARALLEL1_SRC_SEL(value) CX25840__SEQUENCE_6BIT(PARALLEL1_SRC_SEL,  value)
#define CX25840_SET_PARALLEL2_SRC_SEL(value) CX25840__SEQUENCE_6BIT(PARALLEL2_SRC_SEL,  value)
#define CX25840_SET_SERIAL_AUDIO_SRC_SEL(value) CX25840__SEQUENCE_6BIT(SERIAL_AUDIO_SRC_SEL,  value)
#define CX25840_SET_AC97_SRC_SEL(value) CX25840__SEQUENCE_6BIT(AC97_SRC_SEL,  value)
#define CX25840_SET_BASEBAND_BYPASS_CTL(value) CX25840__SEQUENCE_14BIT(BASEBAND_BYPASS_CTL,  value)
#define CX25840_SET_SRC3_CLK_SEL(value) CX25840__SEQUENCE_6BIT(SRC3_CLK_SEL,  value)
#define CX25840_SET_SRC3_IN_SEL(value) CX25840__SEQUENCE_6BIT(SRC3_IN_SEL,  value)
#define CX25840_SET_SRC4_CLK_SEL(value) CX25840__SEQUENCE_6BIT(SRC4_CLK_SEL,  value)
#define CX25840_SET_SRC4_IN_SEL(value) CX25840__SEQUENCE_6BIT(SRC4_IN_SEL,  value)
#define CX25840_SET_SRC5_CLK_SEL(value) CX25840__SEQUENCE_6BIT(SRC5_CLK_SEL,  value)
#define CX25840_SET_SRC5_IN_SEL(value) CX25840__SEQUENCE_6BIT(SRC5_IN_SEL,  value)
#define CX25840_SET_SRC6_CLK_SEL(value) CX25840__SEQUENCE_6BIT(SRC6_CLK_SEL,  value)
#define CX25840_SET_SRC6_IN_SEL(value) CX25840__SEQUENCE_6BIT(SRC6_IN_SEL,  value)
#define CX25840_SET_SA_IN_BCN_DEL(value) CX25840__SEQUENCE_6BIT(SA_IN_BCN_DEL,  value)
#define CX25840_SET_SA_IN_WS_SEL(value) CX25840__SEQUENCE_6BIT(SA_IN_WS_SEL,  value)
#define CX25840_SET_SA_IN_RIGHT_JUST(value) CX25840__SEQUENCE_6BIT(SA_IN_RIGHT_JUST,  value)
#define CX25840_SET_SA_IN_SONY_MODE(value) CX25840__SEQUENCE_6BIT(SA_IN_SONY_MODE,  value)
#define CX25840_SET_SA_IN_MASTER_MODE(value) CX25840__SEQUENCE_6BIT(SA_IN_MASTER_MODE,  value)
#define CX25840_SET_SA_UP2X_BYPASS(value) CX25840__SEQUENCE_6BIT(SA_UP2X_BYPASS,  value)
#define CX25840_SET_SA_OUT_BCNT_DEL(value) CX25840__SEQUENCE_6BIT(SA_OUT_BCNT_DEL,  value)
#define CX25840_SET_SA_OUT_WS_SEL(value) CX25840__SEQUENCE_6BIT(SA_OUT_WS_SEL,  value)
#define CX25840_SET_SA_OUT_RIGHT_JUST(value) CX25840__SEQUENCE_6BIT(SA_OUT_RIGHT_JUST,  value)
#define CX25840_SET_SA_OUT_SONY_MODE(value) CX25840__SEQUENCE_6BIT(SA_OUT_SONY_MODE,  value)
#define CX25840_SET_SA_OUT_MASTER_MODE(value) CX25840__SEQUENCE_6BIT(SA_OUT_MASTER_MODE,  value)
#define CX25840_SET_AC97_SHUTDOWN(value) CX25840__SEQUENCE_6BIT(AC97_SHUTDOWN,  value)
#define CX25840_SET_AC97_WAKE_UP_SYNC(value) CX25840__SEQUENCE_6BIT(AC97_WAKE_UP_SYNC,  value)
#define CX25840_SET_AC97_RST_ACL(value) CX25840__SEQUENCE_6BIT(AC97_RST_ACL,  value)
#define CX25840_SET_AC97_UP2X_BYPASS(value) CX25840__SEQUENCE_6BIT(AC97_UP2X_BYPASS,  value)
#define CX25840_SET_ACLMSTR_BIT1(value) CX25840__SEQUENCE_6BIT(ACLMSTR_BIT1,  value)
#define CX25840_SET_ACLMSTR_4TO2_3_2_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_4TO2_3_2_,  value)
#define CX25840_SET_ACLMSTR_4TO2_4_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_4TO2_4_,  value)
#define CX25840_SET_ACLMSTR_12TO8_8__(value) CX25840__SEQUENCE_6BIT(ACLMSTR_12TO8_8__,  value)
#define CX25840_SET_ACLMSTR_12TO8_9_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_12TO8_9_,  value)
#define CX25840_SET_ACLMSTR_12TO8_10_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_12TO8_10_,  value)
#define CX25840_SET_ACLMSTR_12TO8_11_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_12TO8_11_,  value)
#define CX25840_SET_ACLMSTR_12TO8_12_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_12TO8_12_,  value)
#define CX25840_SET_ACLMSTR_16TO14_14_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_16TO14_14_,  value)
#define CX25840_SET_ACLMSTR_16TO14_15_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_16TO14_15_,  value)
#define CX25840_SET_ACLMSTR_16TO14_16_(value) CX25840__SEQUENCE_6BIT(ACLMSTR_16TO14_16_,  value)
#define CX25840_SET_ACLCTRL00(value) CX25840__SEQUENCE_6BIT(ACLCTRL00,  value)
#define CX25840_SET_ACLCTRL01_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL01_1_0_,  value)
#define CX25840_SET_ACLCTRL02_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL02_1_0_,  value)
#define CX25840_SET_ACLCTRL03_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL03_1_0_,  value)
#define CX25840_SET_ACLCTRL04_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL04_1_0_,  value)
#define CX25840_SET_ACLCTRL05_5_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL05_5_0_,  value)
#define CX25840_SET_ACLCTRL06_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL06_1_0_,  value)
#define CX25840_SET_ACLCTRL06_7_2_(value) CX25840__SEQUENCE_6BIT(ACLCTRL06_7_2_,  value)
#define CX25840_SET_ACLCTRL07_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL07_1_0_,  value)
#define CX25840_SET_ACLCTRL07_7_2_(value) CX25840__SEQUENCE_6BIT(ACLCTRL07_7_2_,  value)
#define CX25840_SET_ACLCTRL08_1_0_(value) CX25840__SEQUENCE_6BIT(ACLCTRL08_1_0_,  value)
#define CX25840_SET_ACLCTRL08_7_2_(value) CX25840__SEQUENCE_6BIT(ACLCTRL08_7_2_,  value)
#define CX25840_SET_ACLCMD_15_0_(value) CX25840__SEQUENCE_22BIT(ACLCMD_15_0_,  value)
#define CX25840_SET_ACLCMD_23_16_(value) CX25840__SEQUENCE_14BIT(ACLCMD_23_16_,  value)
#define CX25840_SET_ACLGPOUT_15_0_(value) CX25840__SEQUENCE_22BIT(ACLGPOUT_15_0_,  value)
#define CX25840_SET_ACLGPIN_15_0_(value) CX25840__SEQUENCE_22BIT(ACLGPIN_15_0_,  value)
#define CX25840_SET_ACLSTAT0_15_0_(value) CX25840__SEQUENCE_22BIT(ACLSTAT0_15_0_,  value)
#define CX25840_SET_ACLSTAT0_23_16_(value) CX25840__SEQUENCE_14BIT(ACLSTAT0_23_16_,  value)
#define CX25840_SET_LOWPOWER_0_(value) CX25840__SEQUENCE_6BIT(LOWPOWER_0_,  value)
#define CX25840_SET_LOWPOWER_7_1_(value) CX25840__SEQUENCE_14BIT(LOWPOWER_7_1_,  value)
#define CX25840_SET_LOWPOWER_20_8_(value) CX25840__SEQUENCE_14BIT(LOWPOWER_20_8_,  value)
#define CX25840_SET_ACL_TAG(value) CX25840__SEQUENCE_14BIT(ACL_TAG,  value)
#define CX25840_SET_ACL_LINK_UPDATE(value) CX25840__SEQUENCE_6BIT(ACL_LINK_UPDATE,  value)
#define CX25840_SET_RDS_I(value) CX25840__SEQUENCE_22BIT(RDS_I,  value)

#endif /* __CX25840_REGISTERS_H */
