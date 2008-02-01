/*
 *  spcp8x5 USB to serial adaptor driver header file
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 * 
 */
#ifndef __SPCP8x5_H__
#define __SPCP8x5_H__
 
#define SPCP8x5_007_VID		0x04FC
#define SPCP8x5_007_PID		0x0201
#define SPCP8x5_008_VID		0x04fc 
#define SPCP8x5_008_PID		0x0235 
#define SPCP8x5_PHILIPS_VID	0x0471 
#define SPCP8x5_PHILIPS_PID	0x081e
#define SPCP8x5_INTERMATIC_VID	0x04FC 
#define SPCP8x5_INTERMATIC_PID	0x0204
#define SPCP8x5_835_VID		0x04fc
#define SPCP8x5_835_PID		0x0231
 
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/proc_fs.h>
#include <linux/syscalls.h>
#include <linux/usb/serial.h>
#include "ringbuf.h"

//#define DEBUG_VERSION	0
#ifdef	DEBUG_VERSION

  #define trace(str,args...) 	printk("%s:%s(%d): "str,\
					__TIME__,__FUNCTION__,__LINE__,##args)

//  #define trace(str,args...) 
  #define trace2(str,args...)	printk(KERN_DEBUG"%s:%s(%d): "str,\
					__TIME__,__FUNCTION__,__LINE__,##args)
#else
  #define trace(str,args...)
  #define trace2(str,args...)
#endif

#define SPCP8x5_IOC_MAGIC		'k'
#define SPCP8x5_IOC_DEBUG		_IO(SPCP8x5_IOC_MAGIC , 15)

#define IOCTL_DBG_SETCTRLLINE	0x01

struct spcp8x5_usb_ctrl_arg
{
	u8 	type;
	u8	cmd;
	u8	cmd_type;
	u16	value;
	u16	index;
	u16	length;
};

#define SPCP8x5_CLOSING_WAIT		(30*HZ)	// wait 30s before close

#define SPCP8x5_BUF_SIZE			1024


// spcp8x5 spec register define
#define MCR_CONTROL_LINE_RTS		0x02
#define MCR_CONTROL_LINE_DTR		0x01
#define MCR_DTR						0x01
#define MCR_RTS						0x02

#define MSR_STATUS_LINE_DCD			0x80
#define MSR_STATUS_LINE_RI			0x40
#define MSR_STATUS_LINE_DSR			0x20
#define MSR_STATUS_LINE_CTS			0x10

// verdor command here , we should define myself
#define SET_DEFAULT					0x40			
#define SET_DEFAULT_TYPE			0x20

#define SET_UART_FORMAT				0x40
#define SET_UART_FORMAT_TYPE		0x21
#define SET_UART_FORMAT_SIZE_5		0x00
#define SET_UART_FORMAT_SIZE_6		0x01
#define SET_UART_FORMAT_SIZE_7		0x02
#define SET_UART_FORMAT_SIZE_8		0x03
#define SET_UART_FORMAT_STOP_1		0x00
#define SET_UART_FORMAT_STOP_2		0x04
#define SET_UART_FORMAT_PAR_NONE	0x00
#define SET_UART_FORMAT_PAR_ODD		0x10
#define SET_UART_FORMAT_PAR_EVEN	0x30
#define SET_UART_FORMAT_PAR_MASK	0xD0
#define SET_UART_FORMAT_PAR_SPACE	0x90

#define GET_UART_STATUS_TYPE		0xc0
#define GET_UART_STATUS				0x22
#define GET_UART_STATUS_MSR			0x06

#define SET_UART_STATUS				0x40
#define SET_UART_STATUS_TYPE		0x23
#define SET_UART_STATUS_MCR			0x0004
#define SET_UART_STATUS_MCR_DTR		0x01
#define SET_UART_STATUS_MCR_RTS		0x02
#define SET_UART_STATUS_MCR_LOOP	0x10

#define SET_WORKING_MODE			0x40
#define SET_WORKING_MODE_TYPE		0x24
#define SET_WORKING_MODE_U2C		0x00
#define SET_WORKING_MODE_RS485		0x01
#define SET_WORKING_MODE_PDMA		0x02
#define SET_WORKING_MODE_SPP		0x03

#define SET_FLOWCTL_CHAR			0x40
#define SET_FLOWCTL_CHAR_TYPE		0x25

#define GET_VERSION					0xc0
#define GET_VERSION_TYPE			0x26

#define SET_REGISTER				0x40
#define SET_REGISTER_TYPE			0x27
	
#define	GET_REGISTER				0xc0
#define GET_REGISTER_TYPE			0x28

#define SET_RAM						0x40
#define SET_RAM_TYPE				0x31

#define GET_RAM						0xc0
#define GET_RAM_TYPE				0x32
// verdor cmd define end

// how come ???
#define UART_STATE					0x08
#define UART_STATE_TRANSIENT_MASK	0x74
#define UART_DCD					0x01
#define UART_DSR					0x02
#define UART_BREAK_ERROR			0x04
#define UART_RING					0x08
#define UART_FRAME_ERROR			0x10
#define UART_PARITY_ERROR			0x20
#define UART_OVERRUN_ERROR			0x40
#define UART_CTS					0x80

enum Spcp8x5Type
{
	SPCP825_007_TYPE,
	SPCP825_008_TYPE,
	SPCP825_PHILIP_TYPE,
	SPCP825_INTERMATIC_TYPE,
	SPCP835_TYPE,
};

// system call
static int spcp8x5_startup (struct usb_serial *serial);
static void spcp8x5_shutdown (struct usb_serial *serial);
static int spcp8x5_open (struct usb_serial_port *port, struct file *filp);
static void spcp8x5_close (struct usb_serial_port *port, struct file *filp);
static void spcp8x5_read_bulk_callback (struct urb *urb);
static int spcp8x5_write (struct usb_serial_port *port, \
			const unsigned char *buf, int count);
static void spcp8x5_send(struct usb_serial_port * port);
static void spcp8x5_write_bulk_callback (struct urb *urb);
static int spcp8x5_ioctl (struct usb_serial_port *port, struct file *file,
			 unsigned int cmd, unsigned long arg);
static void spcp8x5_set_termios (struct usb_serial_port *port, \
								struct ktermios *old);
static int spcp8x5_tiocmset (struct usb_serial_port *port, struct file *file,
			    unsigned int set, unsigned int clear);
static int spcp8x5_tiocmget (struct usb_serial_port *port, struct file *file);
																
static int spcp8x5_write_room(struct usb_serial_port *port);
static int spcp8x5_chars_in_buffer(struct usb_serial_port *port);								
static int spcp8x5_read_proc(char * page , char ** start , \
		off_t offset , int count , int *eof , void * data);
		
// private assistant funcitons
static int spcp8x5_wait_modem_info(struct usb_serial_port *port, unsigned int arg);
static int spcp8x5_set_ctrlLine(struct usb_device* dev , u8 value , enum Spcp8x5Type type);
static int spcp8x5_get_msr( struct usb_device*dev , u8* status , enum Spcp8x5Type type);
static void spcp8x5_set_workMode( struct usb_device* dev , \
							u16 value , u16 index , enum Spcp8x5Type type);
static int spcp8x5_ioctl_debug(struct usb_serial_port * port , \
			struct spcp8x5_usb_ctrl_arg * usr_arg);							
	

#endif
