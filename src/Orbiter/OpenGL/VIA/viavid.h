/*
 * Copyright 1998-2006 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2006 S3 Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */



#ifndef _VIAVID_H
#define _VIAVID_H

/* API Version : Driver support current version*/
#define API_VERSION_NUMERIC(x,y) ((x << 16) | y)

/*
 * FOURCC definitions
 */
#define FOURCC_VIA     0x4E4B4C57  /*VIA*/
#define FOURCC_SUBP    0x50425553  /*SUBP*/
#define FOURCC_TV0     0x00325654  /*TV0*/
#define FOURCC_TV1     0x00315654  /*TV1*/
#define FOURCC_ALPHA   0x48504C41  /*ALPH*/
#define FOURCC_YUY2    0x32595559  /*YUY2*/
#define FOURCC_YV12    0x32315659  /*YV12*/
#define FOURCC_Y211    0x31313259
#define FOURCC_UYVY    0x59565955
#define FOURCC_YVYU    0x55595659
#define FOURCC_IYUV    0x56555949  /* #define FOURCC_IYUV   'VUYI'*/
#define FOURCC_I420    0x30323449  /* I420 */

#define FOURCC_HQVMPEG 0x31565148  /*HQV1*/
#define FOURCC_HQVTV0  0x32565148  /*HQV2*/
#define FOURCC_HQVTV1  0x33565148  /*HQV3*/
#define FOURCC_HQVYUY2 0x34565148  /*HQV4*/
#define FOURCC_HQVYV12 0x35565148  /*HQV5*/
#define FOURCC_NV12   0x3231564E   /* uncompressed overlay surface */
#define FOURCC_V410   0x30313456   
#define FOURCC_V444   0x34343456


/*
 * Actions for VMI functions
 */
#define CREATEDRIVER               0x00
#define DESTROYDRIVER              CREATEDRIVER +1
#define CREATESURFACE              CREATEDRIVER +2
#define DESTROYSURFACE             CREATEDRIVER +3
#define LOCKSURFACE                CREATEDRIVER +4
#define UNLOCKSURFACE              CREATEDRIVER +5
#define UPDATEOVERLAY              CREATEDRIVER +6
#define FLIP                       CREATEDRIVER +7  
#define SETALPHAWIN                CREATEDRIVER +8
#define BEGINPICTRE                CREATEDRIVER +9
#define BEGINPICTURE               CREATEDRIVER +9
#define ENDPICTURE                 CREATEDRIVER +10
#define SLICERECEIVEDATA           CREATEDRIVER +11
#define DRIVERPROC                 CREATEDRIVER +12
#define DISPLAYCONTROL             CREATEDRIVER +13
#define SUBPICTURE                 CREATEDRIVER +14
#define SETDEINTERLACEMODE         CREATEDRIVER +15
#define MPEGENABLE                 CREATEDRIVER +16  
#define MPEGGETPICTYPE             CREATEDRIVER +17  
#define FF_START                   CREATEDRIVER +18  
#define FF_END                     CREATEDRIVER +19
#define INITSURFACEMANAGER         CREATEDRIVER +20
#define FREESURFACEMANAGER         CREATEDRIVER +21
#define GETVIDEOCONTROL            CREATEDRIVER +22     
#define SETVIDEOCONTROL            CREATEDRIVER +23
#define UPDATEALPHA                CREATEDRIVER +24
#define SETTVOUT                   CREATEDRIVER +29
#define SETCAPINFO                 CREATEDRIVER +30
#define SLICERECEIVEDATAEX         CREATEDRIVER +31
#define SETVIDEOCOLOR              CREATEDRIVER +32
#define SETTVOVERLAY         CREATEDRIVER +33
//william
#define DISABLEALPHA            CREATEDRIVER+34
#define VIDEO_ALPHA     1
#define ThreeD_ALPHA    2

//william
#define DISABLEOVERLAY          CREATEDRIVER+35
#define ENABLEOVERLAY           CREATEDRIVER+36
#define VIDEOOVERLAY    1
#define THREEDOVERLAY   2


/* =========================================================================
 * New definition for VMI 2.0
 * =========================================================================*/
#define BEGINFRAME  	           CREATEDRIVER +40
#define RENDERFRAME 	           CREATEDRIVER +41

/* take a snapshort support [2004.12.02 seanzhang] */
#define SNAPSHOT		    CREATEDRIVER+42

#define SETHWFEATURES              CREATEDRIVER+43

/*
 *  sub-picture surfaces ( frame buffers ) we use
 */
#define NUM_FRAME_BUFFERS_SUB           2

/*
 * Structures for SubPicture surface information
 */
typedef struct _SUBDEVICE
{
	unsigned char * lpSUBOverlaySurface[NUM_FRAME_BUFFERS_SUB];   /*Max 2 Pointers to SUB Overlay Surface*/
	unsigned long  dwSUBPhysicalAddr[NUM_FRAME_BUFFERS_SUB];     /*Max 2 Physical address to SUB Overlay Surface*/
	unsigned long  dwPitch;                  /*SUB frame buffer pitch*/
	unsigned long  gdwSUBSrcWidth;           /*SUB Source Width*/
	unsigned long  gdwSUBSrcHeight;          /*SUB Source Height*/
	unsigned long  gdwSUBDstWidth;           /*SUB Destination Width*/
	unsigned long  gdwSUBDstHeight;          /*SUB Destination Height*/
	unsigned long  gdwSUBDstLeft;            /*SUB Position : Left*/
	unsigned long  gdwSUBDstTop;             /*SUB Position : Top*/
}SUBDEVICE;
typedef SUBDEVICE * LPSUBDEVICE;

/*
 *  S/W decode surfaces ( frame buffers ) we use
 */
#define NUM_FRAME_BUFFERS_SW           2


/*
 *  Structures for H/W overlay only surface information
 */   
typedef struct _SWDEVICE
{
	unsigned char * lpSWOverlaySurface[NUM_FRAME_BUFFERS_SW];   /* Max 2 Pointers to SW Overlay Surface*/
	unsigned long  dwSWPhysicalAddr[NUM_FRAME_BUFFERS_SW];     /*Max 2 Physical address to SW Overlay Surface */
	unsigned long  dwSWCbPhysicalAddr[NUM_FRAME_BUFFERS_SW];  /* Physical address to SW Cb Overlay Surface, for YV12 format use */
	unsigned long  dwSWCrPhysicalAddr[NUM_FRAME_BUFFERS_SW];  /* Physical address to SW Cr Overlay Surface, for YV12 format use */
	unsigned long  dwHQVAddr[3];             /* Physical address to HQV surface -- CLE_C0   */
	/*unsigned long  dwHQVAddr[2];*/                         /*Max 2 Physical address to SW HQV Overlay Surface*/
	unsigned long  dwWidth;                  /*SW Source Width, not changed*/
	unsigned long  dwHeight;                 /*SW Source Height, not changed*/
	unsigned long  dwPitch;                  /*SW frame buffer pitch*/
	unsigned long  gdwSWSrcWidth;           /*SW Source Width, changed if window is out of screen*/
	unsigned long  gdwSWSrcHeight;          /*SW Source Height, changed if window is out of screen*/
	unsigned long  gdwSWDstWidth;           /*SW Destination Width*/
	unsigned long  gdwSWDstHeight;          /*SW Destination Height*/
	unsigned long  gdwSWDstLeft;            /*SW Position : Left*/
	unsigned long  gdwSWDstTop;             /*SW Position : Top*/
	unsigned long  dwDeinterlaceMode;        /*BOB / WEAVE*/
}SWDEVICE;
typedef SWDEVICE * LPSWDEVICE;

/*
 *  Structures for alpha surface information
 */   
typedef struct _ALPHADEVICE
{
	unsigned char * lpALPOverlaySurface;
	unsigned long  dwALPPhysicalAddr;
	unsigned long  dwPitch;
	unsigned long  gdwALPSrcWidth;
	unsigned long  gdwALPSrcHeight;
	unsigned long  gdwALPDstWidth;
	unsigned long  gdwALPDstHeight;
	unsigned long  gdwALPDstLeft;
	unsigned long  gdwALPDstTop;
}ALPHADEVICE;
typedef ALPHADEVICE * LPALPHADEVICE;


/*
 * Structures for LOCK surface
 */
typedef struct _DDLOCK
{
	unsigned long     dwVersion;
	unsigned long     dwFourCC;
	unsigned long     dwPhysicalBase;
	SUBDEVICE       SubDev[2];   /*for two HW MPEG*/
	SWDEVICE    SWDevice;
	ALPHADEVICE ALPDevice;

} DDLOCK;
typedef DDLOCK * LPDDLOCK;


/*
 * Structures for create surface
 */
typedef struct _DDSURFACEDESC
{
	unsigned long     dwSize;      /* size of the DDSURFACEDESC structure*/
	unsigned long     dwFlags;     /* determines what fields are valid*/
	unsigned long     dwHeight;    /* height of surface to be created*/
	unsigned long     dwWidth;     /* width of input surface*/
	unsigned long     lPitch;      /* distance to start of next line(return value)*/
	unsigned long     dwBackBufferCount;     /* number of back buffers requested*/
	void *    	  lpSurface;             /* pointer to the surface memory*/
	unsigned long     dwColorSpaceLowValue;  /* low boundary of color space that is to*/
	/* be treated as Color Key, inclusive*/
	unsigned long     dwColorSpaceHighValue; /* high boundary of color space that is*/
	/* to be treated as Color Key, inclusive*/
	unsigned long     dwFourCC;              /* (FOURCC code)*/
} DDSURFACEDESC;
typedef DDSURFACEDESC * LPDDSURFACEDESC;


/*
 *  * structure for passing information to UpdateOverlay fn
 */
typedef struct _RECTL
{
	unsigned long     left;
	unsigned long     top;
	unsigned long     right;
	unsigned long     bottom;
} RECTL;


typedef struct _ALPHACTRL{
	unsigned char type;
	unsigned char ConstantFactor;   /* only valid in bit0 - bit3 */
	unsigned long AlphaEnable;
} ALPHACTRL , *LPALPHACTRL;


#define ALPHA_CONSTANT  0x01
#define ALPHA_STREAM    0x02
#define ALPHA_DISABLE   0x03
#define ALPHA_GRAPHIC   0x04
#define ALPHA_COMBINE   0x05

/*
 * Return value of VMI
 */
#define VMI_OK                              0x00
#define VMI_ERR                             0x01
#define VMI_ERR_NO_X_WINDOW                 VMI_ERR +1
#define VMI_ERR_CANNOT_OPEN_VIDEO_DEVICE    VMI_ERR +2
#define VMI_ERR_CANNOT_USE_IOCTL            VMI_ERR +3
#define VMI_ERR_CANNOT_CREATE_SURFACE       VMI_ERR +4
#define VMI_ERR_CANNOT_SHMGET		  VMI_ERR+5
#define VMI_ERR_CANNOT_SHMAT				  VMI_ERR+6

/*
 * Structure of VMI information
 */
typedef struct _VMI_INFO_PARAM
{
	unsigned long     dwAPIVersion;		/* [in/out] for AP & driver sync the status */
	unsigned long     handle;	        /* [out] for 2 H/W mpeg2 */
} VMI_INFO_PARAM, * LPVMI_INFO_PARAM;


/*
 * Exported Driver functions
 */
unsigned long VMI_Create(LPVMI_INFO_PARAM lpVMI_Info);
unsigned long VMI_DriverProc(LPVMI_INFO_PARAM lpVMI_Info, unsigned long dwAction, void *lpParam1, void *lpParam2);
unsigned long VMI_Destroy(LPVMI_INFO_PARAM lpVMI_Info);
unsigned long VMI_QueryHWCapability(unsigned long dwAction, void *lpHWCapability);

/*
 *  dwAction for VMI_QueryHWCapability functions
 */
#define HW_MPEG_Capability             0x00
#define HW_VID_Capability                0x01
#define HW_OVERLAY_Capability       0x02

/* definition for HW_VID_Capability */
#define HW_VID_ALPHA16     0x00000001
#define HW_VID_ALPHA256    0x00000002

#endif  /* end of _VIAVID_H */
