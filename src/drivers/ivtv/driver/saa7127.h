#ifndef _SAA7127_H
#define _SAA7127_H

/*
 **********************************************************************
 *
 * Define's
 *
 *
 **********************************************************************
 */


#define SAA7127_DRIVER_VERSION "0.3"


#define I2C_SAA7127_ADDRESS  0x88

#define SAA7127_VIDEO_ENABLE        0x01
#define SAA7127_VIDEO_DISABLE       0x00

#define SAA7127_INPUT_TESTIMAGE     0x01
#define SAA7127_INPUT_NORMAL        0x00

/*
 * SAA7127 registers
 */

#define SAA7127_REG_STATUS                           0x00
/* (registers 0x01-0x25 unused.) */
#define SAA7127_REG_WIDESCREEN_CONFIG                0x26
#define SAA7127_REG_WIDESCREEN_ENABLE                0x27
#define SAA7127_REG_BURST_START                      0x28
#define SAA7127_REG_BURST_END                        0x29
#define SAA7127_REG_COPYGEN_0                        0x2a
#define SAA7127_REG_COPYGEN_1                        0x2b
#define SAA7127_REG_COPYGEN_2                        0x2c
#define SAA7127_REG_OUTPUT_PORT_CONTROL              0x2d
/* (registers 0x2e-0x37 unused.) */
#define SAA7127_REG_GAIN_LUMINANCE_RGB               0x38
#define SAA7127_REG_GAIN_COLORDIFF_RGB               0x39
#define SAA7127_REG_INPUT_PORT_CONTROL_1             0x3A
/* (registers 0x3b-0x53 undefined) */
#define SAA7129_REG_FADE_KEY_COL2		     0x4F
/**/
#define SAA7127_REG_CHROMA_PHASE                     0x5A
#define SAA7127_REG_GAINU                            0x5B
#define SAA7127_REG_GAINV                            0x5C
#define SAA7127_REG_BLACK_LEVEL                      0x5D
#define SAA7127_REG_BLANKING_LEVEL                   0x5E
#define SAA7127_REG_VBI_BLANKING                     0x5F
/* (register 0x60 unused) */
#define SAA7127_REG_DAC_CONTROL                      0x61
#define SAA7127_REG_BURST_AMP                        0x62
#define SAA7127_REG_SUBC3                            0x63
#define SAA7127_REG_SUBC2                            0x64
#define SAA7127_REG_SUBC1                            0x65
#define SAA7127_REG_SUBC0                            0x66
#define SAA7127_REG_LINE_21_ODD_0                    0x67
#define SAA7127_REG_LINE_21_ODD_1                    0x68
#define SAA7127_REG_LINE_21_EVEN_0                   0x69
#define SAA7127_REG_LINE_21_EVEN_1                   0x6A
#define SAA7127_REG_RCV_PORT_CONTROL                 0x6B
#define SAA7127_REG_VTRIG                            0x6C
#define SAA7127_REG_HTRIG_HI                         0x6D
#define SAA7127_REG_MULTI                            0x6E
#define SAA7127_REG_CLOSED_CAPTION                   0x6F
#define SAA7127_REG_RCV2_OUTPUT_START                0x70
#define SAA7127_REG_RCV2_OUTPUT_END                  0x71
#define SAA7127_REG_RCV2_OUTPUT_MSBS                 0x72
#define SAA7127_REG_TTX_REQUEST_H_START              0x73
#define SAA7127_REG_TTX_REQUEST_H_DELAY_LENGTH       0x74
#define SAA7127_REG_CSYNC_ADVANCE_VSYNC_SHIFT        0x75
#define SAA7127_REG_TTX_ODD_REQ_VERT_START           0x76
#define SAA7127_REG_TTX_ODD_REQ_VERT_END             0x77
#define SAA7127_REG_TTX_EVEN_REQ_VERT_START          0x78
#define SAA7127_REG_TTX_EVEN_REQ_VERT_END            0x79
#define SAA7127_REG_FIRST_ACTIVE                     0x7A
#define SAA7127_REG_LAST_ACTIVE                      0x7B
#define SAA7127_REG_MSB_VERTICAL                     0x7C
/* (register 0x7d unused) */
#define SAA7127_REG_DISABLE_TTX_LINE_LO_0            0x7E
#define SAA7127_REG_DISABLE_TTX_LINE_LO_1            0x7F



/*
 **********************************************************************
 *  
 *  Enumurations
 *
 **********************************************************************
 */


/* Enumeration for the Video Standard */


enum saa7127_video_norm       {
                                SAA7127_VIDEO_NORM_NTSC,
                                SAA7127_VIDEO_NORM_PAL
                              };


/* Enumeration for the Supported input types */

enum saa7127_input_type      {
                                SAA7127_INPUT_TYPE_NORMAL,
                                SAA7127_INPUT_TYPE_TEST_IMAGE
                             };


/* Enumeration for the Supported Output signal types */

enum saa7127_output_type      {
                                SAA7127_OUTPUT_TYPE_BOTH,
                                SAA7127_OUTPUT_TYPE_COMPOSITE,
                                SAA7127_OUTPUT_TYPE_SVIDEO,
                                SAA7127_OUTPUT_TYPE_RGB,
                                SAA7127_OUTPUT_TYPE_YUV_C,
                                SAA7127_OUTPUT_TYPE_YUV_V
                              };

/* Enumeration for the enable/disabeling the output signal */

enum saa7127_enable_type      {
                                SAA7127_DISABLE,
                                SAA7127_ENABLE
                              };
/* Enumeration for the turning on/off the CC signal */

enum saa7127_cc_enable_type  {
                                SAA7127_CC_DISABLE,
                                SAA7127_CC_ODD,
                                SAA7127_CC_EVEN,
                                SAA7127_CC_BOTH
                              };

/* Enumeration for the turning on/off the VPS signal */

enum saa7127_vps_enable_type  {
                                SAA7127_VPS_DISABLE,
                                SAA7127_VPS_ENABLE
                              };

/* Enumeration for the turning on/off the Wide screen signal for Wide screen TV */

enum saa7127_wss_enable_type  {
                                SAA7127_WSS_DISABLE,
                                SAA7127_WSS_ENABLE
                              };

/* Enumeration for the selecting the different Wide screen mode */

enum saa7127_wss_mode_type    {
                                SAA7127_WSS_MODE_4_3_FULL_FORMAT,           /* full format 4:3 */
                                SAA7127_WSS_MODE_BOX_14_9_C,           /* box 14:9 c */
                                SAA7127_WSS_MODE_BOX_14_9_TOP,              /* box 14:9 top */
                                SAA7127_WSS_MODE_BOX_16_9_C,           /* box 16:9 c */
                                SAA7127_WSS_MODE_BOX_16_9_TOP,              /* box 16:9 top */
                                SAA7127_WSS_MODE_SMALL_BOX_16_9_C,     /* box > 16:9 c */
                                SAA7127_WSS_MODE_4_3_14_9_FULL_FORMAT,      /* full format 4:3 with 14:9 c letterbox content */
                                SAA7127_WSS_MODE_16_9_ANAMORPHIC            /* full format 16:9 (anamorphic) */
                              };

struct saa7127_vps_data {
	u8 data[5];
};

struct saa7127_reg {
	u8 reg;
	u8 val;
};

#define	ENCODER_SET_REG		_IOW('e', 100, struct saa7127_reg)
#define	ENCODER_GET_REG		_IOWR('e', 101, struct saa7127_reg)
#define ENCODER_ENABLE_WSS 	_IOW('e', 102, int)
#define ENCODER_SET_WSS_MODE 	_IOW('e', 103, int)
#define ENCODER_ENABLE_CC 	_IOW('e', 104, int)
#define ENCODER_SET_CC_DATA 	_IOW('e', 105, int)
#define ENCODER_ENABLE_VPS 	_IOW('e', 106, int)
#define ENCODER_SET_VPS_DATA 	_IOW('e', 107, struct saa7127_vps_data)

#endif // _SAA7127_H
