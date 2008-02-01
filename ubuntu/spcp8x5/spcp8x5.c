/*
 * spcp8x5 USB to serial adaptor driver
 *
 * Copyright (C) 2006 Linxb (xubin.lin@worldplus.com.cn)
 * Copyright (C) 2006 S1 Corp.
 *
 * Original driver for 2.6.10 pl2303 driver by Greg Kroah-Hartman (greg@kroah.com)
 * Changes for 2.6.20 by Harald Klein <hari@vt100.at>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 * 
 */

#include "spcp8x5.h"

/*
 * Version Information
 */
#define DRIVER_VERSION 	"v0.04"
#define DRIVER_DESC 	"SPCP8x5 USB to serial adaptor driver"

static int debug;

// 
static struct usb_device_id id_table [] = {
	{ USB_DEVICE(SPCP8x5_PHILIPS_VID , SPCP8x5_PHILIPS_PID)},
	{ USB_DEVICE(SPCP8x5_INTERMATIC_VID , SPCP8x5_INTERMATIC_PID)},  
	{ USB_DEVICE(SPCP8x5_835_VID , SPCP8x5_835_PID)},
	{ USB_DEVICE(SPCP8x5_008_VID , SPCP8x5_008_PID) }, 
	{ USB_DEVICE(SPCP8x5_007_VID, SPCP8x5_007_PID) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver spcp8x5_driver = {
	.name 				= "spcp8x5",
	.probe 				= usb_serial_probe,
	.disconnect 		= usb_serial_disconnect,
	.id_table 			= id_table,
        .id_table =     id_table,
        .no_dynamic_id =        1,
};

// All of the device info needed for the spcp8x5 SIO serial converter 
static struct usb_serial_driver spcp8x5_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"SPCP8x5",
	},
	.id_table		= id_table,
	.num_interrupt_in 	= NUM_DONT_CARE,
	.num_bulk_in 		= 1,
	.num_bulk_out		= 1,
	.num_ports		= 1,
	.open 			= spcp8x5_open,
	.close 			= spcp8x5_close,
	.write 			= spcp8x5_write,
	.set_termios 		= spcp8x5_set_termios,
	.ioctl 			= spcp8x5_ioctl,
	.tiocmget 		= spcp8x5_tiocmget,
	.tiocmset 		= spcp8x5_tiocmset,	
	.write_room 		= spcp8x5_write_room,
	.read_bulk_callback 	= spcp8x5_read_bulk_callback,
	.write_bulk_callback	= spcp8x5_write_bulk_callback,
	.chars_in_buffer 	= spcp8x5_chars_in_buffer,
	.attach 		= spcp8x5_startup,
	.shutdown 		= spcp8x5_shutdown,	
};



struct Spcp8x5Priv {
	spinlock_t 			lock;
	struct RingBuf*		buf;
	int 				write_urb_in_use;				// the write urb in used or not
	enum Spcp8x5Type	type;					
	wait_queue_head_t 	delta_msr_wait;
	u8 					line_control;
	u8 					line_status;
	u8 					termios_initialized;
};

// name : spcp8x5_startup
// in 	: serial - the usb_serial struct create by usb serial subsys
// out	: int - 0.success,other.failed
// desc : when device plug in,this function would be called. 
//		: thanks to usb_serial subsystem,then do almost every things
//		: for us. And what we should do just alloc the buffer
static int spcp8x5_startup (struct usb_serial *serial)
{
	struct Spcp8x5Priv * priv;
	int i;
	enum Spcp8x5Type type = SPCP825_007_TYPE;

	if (serial->dev->descriptor.idProduct == 0x0201)
		type = SPCP825_007_TYPE;
	else if(serial->dev->descriptor.idProduct == 0x0231)
		type = SPCP835_TYPE;
	else if(serial->dev->descriptor.idProduct == 0x0235)
		type = SPCP825_008_TYPE;
	else if(serial->dev->descriptor.idProduct == 0x0204)
		type = SPCP825_INTERMATIC_TYPE;
	else if(serial->dev->descriptor.idProduct == 0x0471 && serial->dev->descriptor.idVendor == 0x081e)	
		type = SPCP825_PHILIP_TYPE;
	trace("device type = %d\n" , (int)type);
	
	for (i = 0; i < serial->num_ports; ++i) {
		priv = kmalloc (sizeof (struct Spcp8x5Priv), GFP_KERNEL);
		if (!priv)
			goto cleanup;
		
		memset (priv, 0x00, sizeof (struct Spcp8x5Priv));
		spin_lock_init(&priv->lock);
		priv->buf = alloc_ringbuf(SPCP8x5_BUF_SIZE);
		if (priv->buf == NULL) {
			kfree(priv);
			goto cleanup;
		}
		
		trace("(%d)priv = %p,priv->buf = %p\n" , i ,priv , priv->buf);
		
		init_waitqueue_head(&priv->delta_msr_wait);
		priv->type = type;
		usb_set_serial_port_data(serial->port[i] , priv);

	}
	
	create_proc_read_entry("spcp8x5" , 0 , NULL , spcp8x5_read_proc , serial); 
	
	trace("ok\n");
	return 0;

cleanup:
	for (--i; i>=0; --i) {
		priv = (struct Spcp8x5Priv *)usb_get_serial_port_data(serial->port[i]);
		free_ringbuf(priv->buf);
		kfree(priv);	
		usb_set_serial_port_data(serial->port[i] , NULL);
	}
	return -ENOMEM;
}

// name : spcp8x5_shutdonw
// in	: serial -  the usb serial object
// out 	: null
// desc : call when the device plug out. free all the memory alloced by probe
void spcp8x5_shutdown (struct usb_serial *serial)
{
	int i;
	struct Spcp8x5Priv * priv ;	

	for(i=0 ; i<serial->num_ports ; i++)
	{
		priv = (struct Spcp8x5Priv *)usb_get_serial_port_data(serial->port[i]);
		if(priv)
		{
			trace("(%d)priv = %p , priv->buf = %p\n" , i , priv , priv->buf);
			free_ringbuf(priv->buf);
			kfree(priv);
			usb_set_serial_port_data(serial->port[i] , NULL);
			priv = NULL;
		}
	}
	remove_proc_entry("spcp8x5" , NULL);
	trace("ok\n");
}

// name : spcp8x5_open
// in	: port - the usb serial port
//		: filp - the device be opened
// out	: int - success or not
// desc : opetn the serial port. do some usb system call. set termios and get the
//		: line status of the device. then submit the read urb
static int spcp8x5_open (struct usb_serial_port *port, struct file *filp)
{
	struct ktermios tmp_termios;
	struct usb_serial *serial = port->serial;
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	int ret;
 	unsigned long flags;
	u8 status = 0x30;		// status 0x30 means DSR and CTS = 1 other CDC RI and delta = 0
 
	dbg("%s -  port %d", __FUNCTION__, port->number);

	usb_clear_halt(serial->dev, port->write_urb->pipe);
	usb_clear_halt(serial->dev, port->read_urb->pipe);

	ret =  usb_control_msg (serial->dev, usb_sndctrlpipe (serial->dev, 0),
			     0x09, 0x00 ,
			     0x01, 0x00, NULL , 0x00 , 100);
	if( ret )
	{
		trace("set_config error\n");
		return ret;
	}

	spin_lock_irqsave (&priv->lock, flags);
	if (port->tty->termios->c_cflag & CBAUD)
		priv->line_control = MCR_DTR | MCR_RTS;
	else
		priv->line_control = 0;
	spin_unlock_irqrestore (&priv->lock, flags);

	spcp8x5_set_ctrlLine(serial->dev, priv->line_control , priv->type);

	/* Setup termios */
	if (port->tty) {
		spcp8x5_set_termios (port, &tmp_termios);
	}
	
	spcp8x5_get_msr( serial->dev , &status , priv->type );

	// may be we should update uart status here but now we did not do
	spin_lock_irqsave (&priv->lock, flags);
	priv->line_status = status & 0xf0 ;
	spin_unlock_irqrestore (&priv->lock, flags);

	//FIXME: need to assert RTS and DTR if CRTSCTS off

	dbg("%s - submitting read urb", __FUNCTION__);
	port->read_urb->dev = serial->dev;
	ret = usb_submit_urb (port->read_urb , GFP_KERNEL);
	if (ret) 
	{
		trace("submit read urb err\n");
		spcp8x5_close (port, NULL);
		return -EPROTO;
	}
	return 0;
}

// name : spcp8x5_close
// in	: port - the serial port
//		: filp - the device file,no used
// out	: null
// desc : close the serial port. We should wait for data sending to device 
//		: 1st and then kill all urb.
static void spcp8x5_close (struct usb_serial_port *port, struct file *filp)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int c_cflag;
	int bps;
	long timeout;
	wait_queue_t wait;						
	int result;
	
	dbg("%s - port %d", __FUNCTION__, port->number);

	// wait for data to drain from the buffer 
	spin_lock_irqsave(&priv->lock, flags);
	timeout = SPCP8x5_CLOSING_WAIT;
	init_waitqueue_entry(&wait, current);
	add_wait_queue(&port->tty->write_wait, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (ringbuf_avail_data(priv->buf) == 0
		|| timeout == 0 || signal_pending(current) )
			break;
		spin_unlock_irqrestore(&priv->lock, flags);
		timeout = schedule_timeout(timeout);	
		spin_lock_irqsave(&priv->lock, flags);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&port->tty->write_wait, &wait);
	trace("wait 4 data drain from buffer ok\n");

	// clear out any remaining data in the buffer 
	clear_ringbuf(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	// wait for characters to drain from the device 
	// (this is long enough for the entire all byte 
	// spcp8x5 hardware buffer to drain with no flow 
	// control for data rates of 1200 bps or more, 
	// for lower rates we should really know how much 
	// data is in the buffer to compute a delay 
	// that is not unnecessarily long) 
	bps = tty_get_baud_rate(port->tty);
	if (bps > 1200)
		timeout = max((HZ*2560)/bps,HZ/10);
	else
		timeout = 2*HZ;
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(timeout);
	trace("wait 4 device send data ok\n");

	// clear control lines
	if (port->tty) {
		c_cflag = port->tty->termios->c_cflag;
		if (c_cflag & HUPCL) {
			spin_lock_irqsave(&priv->lock, flags);
			priv->line_control = 0;
			spin_unlock_irqrestore (&priv->lock, flags);
			spcp8x5_set_ctrlLine (port->serial->dev, 0 , priv->type);
		}
	}
	trace("clear control lines ok\n");

	// kill urb
	if( port->write_urb != NULL )
	{
		result = usb_unlink_urb( port->write_urb );
		if( result )
		{
			trace("usb_unlink_urb(write_urb) = %d\n" , result);
		}
	}	
	result = usb_unlink_urb( port->read_urb );
	if( result )
	{
		trace("usb_unlink_urb(read_urb) = %d\n" , result);
	}
	trace("ok\n");
}

// name : spcp8x5_read_bulk_callback
// in	: urb - the urb with callback
//		: regs - regiser no used
// out	: null
// desc : bulk read call back function. check the status of the urb. if 
//		: transfer failed return. then update the status and the tty 
//		: send data to tty subsys. submit urb again. 
static void spcp8x5_read_bulk_callback (struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int i;
	int result;
	u8 status = 0;
	char tty_flag;

	trace("port %d\n", port->number);
	trace("start , urb->status = %d , urb->actual_length = %d\n" ,
		urb->status , urb->actual_length );
	trace("read_buf = %s\n" , data);

	// check the urb status
	if (urb->status) {
		trace("urb->status = %d\n", urb->status);
		if (!port->open_count) {
			trace("port is closed, exiting.\n");
			return;
		}
		if (urb->status == -EPROTO) {
			// spcp8x5 mysteriously fails with -EPROTO 
			// reschedule the read 
			trace("caught -EPROTO, resubmitting the urb\n");
			urb->status = 0;
			urb->dev = port->serial->dev;
			result = usb_submit_urb(urb , GFP_KERNEL);
			if (result)
				trace("faile submitting read urb %d\n" ,\
					 result);
			return;
		}
		trace("unable to handle the error, exiting.\n");
		return;
	}

	// get tty_flag from status
	tty_flag = TTY_NORMAL;


	spin_lock_irqsave(&priv->lock, flags);
	status = priv->line_status;
	priv->line_status &= ~UART_STATE_TRANSIENT_MASK;		
	spin_unlock_irqrestore(&priv->lock, flags);
	wake_up_interruptible (&priv->delta_msr_wait);		// wake up the wait for termios

	// break takes precedence over parity, 
	// which takes precedence over framing errors 
	if (status & UART_BREAK_ERROR )
		tty_flag = TTY_BREAK;
	else if (status & UART_PARITY_ERROR)
		tty_flag = TTY_PARITY;
	else if (status & UART_FRAME_ERROR)
		tty_flag = TTY_FRAME;
	trace("tty_flag = %d\n",tty_flag);

        tty = port->tty;
        if (tty && urb->actual_length) {
                tty_buffer_request_room(tty, urb->actual_length + 1);
                /* overrun is special, not associated with a char */
                if (status & UART_OVERRUN_ERROR)
                        tty_insert_flip_char(tty, 0, TTY_OVERRUN);
                for (i = 0; i < urb->actual_length; ++i)
                        tty_insert_flip_char(tty, data[i], tty_flag);
                tty_flip_buffer_push(tty);
        }

	// Schedule the next read _if_ we are still open 
	if (port->open_count) {
		urb->dev = port->serial->dev;
		result = usb_submit_urb(urb , GFP_KERNEL);
		if (result)
			trace("faile submitting read urb %d\n" , result);
	}

	return;
}

// name : spcp8x5_write_bulk_callback
// in	: urb - the urb
//		: regs - register of the cpu. no used
// out	: null
// desc : this is the call back function for write urb. NOTE we should not 
//		: sleep in this routine. check the urb return code and then submit
//		: the write urb again to hold the write loop
static void spcp8x5_write_bulk_callback (struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	int result;

	trace("port %d", port->number);
//	PDEBUG("urb->status = %d\n" , urb->status);
//	PDEBUG("Enoent = %d , eshutdown = %d , econneset = %d\n" , -ENOENT , -ESHUTDOWN ,-ECONNRESET  );

	switch (urb->status) {
		case 0:
			// success 
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN:
			// this urb is terminated, clean up 
			trace("urb shutting down with status: %d", (int)urb->status);
			priv->write_urb_in_use = 0;
			return;
		default:
			// error in the urb, so we have to resubmit it 
			dbg("%s - Overflow in write", __FUNCTION__);
			dbg("%s - nonzero write bulk status received: %d", \
				__FUNCTION__, urb->status);
			port->write_urb->transfer_buffer_length = 1;
			port->write_urb->dev = port->serial->dev;
			result = usb_submit_urb (port->write_urb , GFP_KERNEL);
			if (result)
				trace("failed resubmitting write urb,%d\n" , \
				result);
			else
				return;
	}

	priv->write_urb_in_use = 0;

	// send any buffered data 
	spcp8x5_send(port);
}

// name : spcp8x5_write
// in	: port - usb serial port
//		: buf - the buffer of data for writing
//		: count - the count of data for writing
// out 	: int - how many data has written
// desc : write data to ring buffer. and then start the write 
//		: transfer
static int spcp8x5_write (struct usb_serial_port *port, \
			 const unsigned char *buf, int count)
{
	struct Spcp8x5Priv * priv = usb_get_serial_port_data(port);
	unsigned long flags;

	trace("port %d, %d bytes\n", port->number, count);

	if (!count)
		return count;

	spin_lock_irqsave(&priv->lock, flags);
	count = put_ringbuf(priv->buf, buf, count);
	spin_unlock_irqrestore(&priv->lock, flags);

	spcp8x5_send(port);

	return count;
}

// name : spcp8x5_send
// in	: port - the usb serial port
// out	: null
// desc : get data from ring buffer and then write to usb bus
static void spcp8x5_send(struct usb_serial_port * port)
{
	int count, result;
	struct Spcp8x5Priv * priv = usb_get_serial_port_data(port);
	unsigned long flags;

//	trace("port %d\n", port->number);

	spin_lock_irqsave( \
						&priv->lock, \
						flags);

	if (priv->write_urb_in_use) {
		trace("write urb still used\n");
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}

	// send the 1st urb for writting
	memset(port->write_urb->transfer_buffer , 0x00 , port->bulk_out_size);
	count = get_ringbuf(priv->buf, port->write_urb->transfer_buffer,
		port->bulk_out_size);
//	trace("write_urb->transfer_buffer = %c , count = %d\n" , *j(char *)port->write_urb->transfer_buffer , count);

//	PDEBUG("trans_buf = %p\n" , port->write_urb->transfer_buffer);
//	PDEBUG("urb->trans_buffer = %s\n" , (char *)port->write_urb->transfer_buffer);
	if (count == 0) {
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}

	priv->write_urb_in_use = 1;		// update the urb status 

	spin_unlock_irqrestore(&priv->lock, flags);

	port->write_urb->transfer_buffer_length = count;
	port->write_urb->dev = port->serial->dev;
	
//	PDEBUG("write_callback = %p\n" , spcp8x5_write_bulk_callback);
//	PDEBUG("urb->context->serial->type->write_bulk_callback = %p\n" ,((struct usb_serial_port *)port->write_urb->context)->serial->type->write_bulk_callback);
//	PDEBUG("write_urb->dev = %p\n" , port->write_urb->dev);
	
	result = usb_submit_urb (port->write_urb, GFP_KERNEL);
	if (result) {
		trace("failed submitting write urb, error %d\n", result);
		priv->write_urb_in_use = 0;
		// TODO: reschedule spcp8x5_send
	}

//	PDEBUG("urb->tf_buf_len = %d , usb_submit_urb() = %d\n" , count , result);

	schedule_work(&port->work);
}

// name : spcp8x5_set_termios
// in	: port - the serial port
//		: old_termios - the old term of the port,maybe we should
//		: 		test this parm for real transfer
// out	: null
// desc : set the serial param for transfer. we should check if 
//		: we really need to transfer. then if be set flow contorl
//		: we should do this too.
static void spcp8x5_set_termios (struct usb_serial_port *port, \
								struct ktermios *old_termios)
{
	struct usb_serial *serial = port->serial;
	struct Spcp8x5Priv * priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int cflag = port->tty->termios->c_cflag;
	unsigned int old_cflag = old_termios->c_cflag;
	unsigned short uartdata;
	unsigned char buf[2] = {0,0};
	int baud;
	int i;
	u8 control;

	if ((!port->tty) || (!port->tty->termios)) {
		trace("no tty structures\n");
		return;
	}
	
	// for the 1st time call this function
	spin_lock_irqsave(&priv->lock, flags);
	if (!priv->termios_initialized) {
		*(port->tty->termios) = tty_std_termios;
		port->tty->termios->c_cflag = B115200 | CS8 | CREAD | HUPCL | CLOCAL;
		priv->termios_initialized = 1;
	}
	spin_unlock_irqrestore(&priv->lock, flags);	

	// check that they really want us to change something 
	if (old_termios) 
	{
		if ((cflag == old_cflag) && \
			(RELEVANT_IFLAG(port->tty->termios->c_iflag) == RELEVANT_IFLAG(old_termios->c_iflag))) 
		{
		    trace("nothing to change...\n");
		    return;
		}
	}
	
	// set DTR/RTS active 
	spin_lock_irqsave(&priv->lock, flags);
	control = priv->line_control;
	if ((old_cflag & CBAUD) == B0) {
		priv->line_control |= MCR_DTR;
		if (!(old_cflag & CRTSCTS)) {
			priv->line_control |= MCR_RTS;
		}
	}
	if (control != priv->line_control) {
		control = priv->line_control;
		spin_unlock_irqrestore(&priv->lock, flags);
		spcp8x5_set_ctrlLine(serial->dev, control , priv->type);
	} else {
		spin_unlock_irqrestore(&priv->lock, flags);
	}


	// Set Baud Rate
	baud = 0;
	switch (cflag & CBAUD) {

		case B300:	baud = 300;	buf[0] = 0x00;	break;
		case B600:	baud = 600;	buf[0] = 0x01;	break;
		case B1200:	baud = 1200;	buf[0] = 0x02;	break;
		case B2400:	baud = 2400;	buf[0] = 0x03;	break;
		case B4800:	baud = 4800;	buf[0] = 0x04;	break;
		case B9600:	baud = 9600;	buf[0] = 0x05;	break;
		case B19200:	baud = 19200;	buf[0] = 0x07;	break;
		case B38400:	baud = 38400;	buf[0] = 0x09;	break;
		case B57600:	baud = 57600;	buf[0] = 0x0a;	break;
		case B115200:	baud = 115200;	buf[0] = 0x0b;	break;
		case B230400:	baud = 230400;	buf[0] = 0x0c;	break;
		case B460800:	baud = 460800;	buf[0] = 0x0d;	break;
		case B921600:	baud = 921600;	buf[0] = 0x0e;	break;
//		case B1200000:	baud = 1200000; buf[0] = 0x0f;	break;
//		case B2400000:	baud = 2400000; buf[0] = 0x10;	break;
		case B3000000:	baud = 3000000; buf[0] = 0x11;	break;
//		case B6000000:	baud = 6000000; buf[0] = 0x12;	break;
		default:
			err ("spcp825 driver does not support the baudrate requested, using default of 9600.");
		case B0:
		case B1000000:
						baud = 115200;	buf[0] = 0x0b;	break;
	}
//	trace("baud = %d\n", baud);

	// Set Data Length : 00:5bit, 01:6bit, 10:7bit, 11:8bit
	if (cflag & CSIZE) {
		switch (cflag & CSIZE) {
			case CS5:
				buf[1] |= SET_UART_FORMAT_SIZE_5;	break;
			case CS6:
				buf[1] |= SET_UART_FORMAT_SIZE_6;	break;
			case CS7:
				buf[1] |= SET_UART_FORMAT_SIZE_7;	break;
			default:
			case CS8:
				buf[1] |= SET_UART_FORMAT_SIZE_8;	break;
		}
//		trace("data bits = %x\n", buf[1]&0x03 );
	}

	/* Set Stop bit2 : 0:1bit 1:2bit */
	buf[1] |= (cflag & CSTOPB) ? SET_UART_FORMAT_STOP_2 : SET_UART_FORMAT_STOP_1;
//	trace("stop bits = %d\n",  (buf[1]&0x04)+1 );

	/* Set Parity bit3-4 01:Odd 11:Even */
	if (cflag & PARENB) {
		buf[1] |= (cflag & PARODD) ? 
		SET_UART_FORMAT_PAR_ODD : SET_UART_FORMAT_PAR_EVEN ;
	} else {
		buf[1] |= SET_UART_FORMAT_PAR_NONE;
	}

	uartdata = buf[0] | buf[1]<<8;
	trace("wValue = %#x\n" , uartdata);

	i = usb_control_msg (serial->dev, usb_sndctrlpipe (serial->dev, 0),
			     SET_UART_FORMAT_TYPE, SET_UART_FORMAT, 
			     uartdata, 0, NULL, 0, 100);
	if (i < 0)
		err("Set UART format %#x failed (error = %d)", uartdata, i);
	dbg ("0x21:0x40:0:0  %d\n", i);

	if( cflag & CRTSCTS)
	{
		// enable hardware flow control
		spcp8x5_set_workMode( serial->dev , 0x000a , SET_WORKING_MODE_U2C , priv->type);
	}	
	return;
}

static int spcp8x5_ioctl (struct usb_serial_port *port, struct file *file,
			 unsigned int cmd, unsigned long arg)
{
	dbg("%s (%d) cmd = 0x%04x", __FUNCTION__, port->number, cmd);

	switch (cmd) {
	
		case SPCP8x5_IOC_DEBUG:
			return spcp8x5_ioctl_debug(port , \
				(struct spcp8x5_usb_ctrl_arg *)arg);

		case TIOCMIWAIT:
			dbg("%s (%d) TIOCMIWAIT", __FUNCTION__,  port->number);
			return spcp8x5_wait_modem_info(port, arg);

		default:
			dbg("%s not supported = 0x%04x", __FUNCTION__, cmd);
			break;
	}

	return -ENOIOCTLCMD;
}

static int spcp8x5_wait_modem_info(struct usb_serial_port *port, unsigned int arg)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int prevstatus;
	unsigned int status;
	unsigned int changed;

	spin_lock_irqsave (&priv->lock, flags);
	prevstatus = priv->line_status;
	spin_unlock_irqrestore (&priv->lock, flags);

	while (1) {
		interruptible_sleep_on(&priv->delta_msr_wait);			// wake up in bulk read
		/* see if a signal did it */
		if (signal_pending(current))
			return -ERESTARTSYS;
		
		spin_lock_irqsave (&priv->lock, flags);
		status = priv->line_status;
		spin_unlock_irqrestore (&priv->lock, flags);
		
		changed=prevstatus^status;
		
		if (((arg & TIOCM_RNG) && (changed & MSR_STATUS_LINE_RI)) ||
		    ((arg & TIOCM_DSR) && (changed & MSR_STATUS_LINE_DSR)) ||
		    ((arg & TIOCM_CD)  && (changed & MSR_STATUS_LINE_DCD)) ||
		    ((arg & TIOCM_CTS) && (changed & MSR_STATUS_LINE_CTS)) ) {
			return 0;
		}
		prevstatus = status;
	}
	/* NOTREACHED */
	return 0;
}

static int spcp8x5_ioctl_debug(struct usb_serial_port * port , \
			struct spcp8x5_usb_ctrl_arg * usr_arg)
{
	struct usb_serial * serial = port->serial;
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	struct spcp8x5_usb_ctrl_arg * arg;

	if(serial == NULL)
	{
		trace2("usb serial ptr NULL\n");
		return	-1;
	}
	
	arg = kmalloc(sizeof(struct spcp8x5_usb_ctrl_arg) , GFP_KERNEL);
	if(arg == NULL)
	{	
		trace2("could not get cmd arg struct \n");
		return -ENOMEM;
	}
	memset(arg , 0x00 , sizeof(struct spcp8x5_usb_ctrl_arg));

	if(copy_from_user(arg , usr_arg , sizeof(struct spcp8x5_usb_ctrl_arg)))
	{
		trace("could not get user data");
		return -EFAULT;
	}
	
	trace2("arg->type = %#x, \n	\
			arg->cmd = %#x, \n	\
			arg->cmd_type = %#x, \n	\
			arg->value = %#x, \n	\
			arg->index = %#x, \n	\
			arg->length = %#x, \n",	\
			arg->type , arg->cmd , arg->cmd_type,	\
			arg->value , arg->index , arg->length);
			
	switch( arg->type )
	{
		case IOCTL_DBG_SETCTRLLINE:
			spcp8x5_set_ctrlLine(serial->dev , 0x00 , priv->type);
			break;
			
		default:
			break;
	}		

	return 0;
}

static int spcp8x5_tiocmset (struct usb_serial_port *port, struct file *file,
			    unsigned int set, unsigned int clear)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	u8 control;

	spin_lock_irqsave (&priv->lock, flags);
	if (set & TIOCM_RTS)
		priv->line_control |= MCR_RTS;
	if (set & TIOCM_DTR)
		priv->line_control |= MCR_DTR;
	if (clear & TIOCM_RTS)
		priv->line_control &= ~MCR_RTS;
	if (clear & TIOCM_DTR)
		priv->line_control &= ~MCR_DTR;
	control = priv->line_control;
	spin_unlock_irqrestore (&priv->lock, flags);

	return spcp8x5_set_ctrlLine (port->serial->dev, control , priv->type);
}

static int spcp8x5_tiocmget (struct usb_serial_port *port, struct file *file)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int mcr;
	unsigned int status;
	unsigned int result;

	spin_lock_irqsave (&priv->lock, flags);
	mcr = priv->line_control;
	status = priv->line_status;
	spin_unlock_irqrestore (&priv->lock, flags);

	result = ((mcr & MCR_DTR)				? TIOCM_DTR : 0)
		  | ((mcr & MCR_RTS)				? TIOCM_RTS : 0)
		  | ((status & MSR_STATUS_LINE_CTS)	? TIOCM_CTS : 0)
		  | ((status & MSR_STATUS_LINE_DSR)	? TIOCM_DSR : 0)
		  | ((status & MSR_STATUS_LINE_RI)	? TIOCM_RI  : 0)
		  | ((status & MSR_STATUS_LINE_DCD)	? TIOCM_CD  : 0);

	return result;
}

// name : spcp8x5_write_room
// in	: port - the serial port
// out	: int - space room in driver
// desc : get the avail space room in ring buffer
static int spcp8x5_write_room(struct usb_serial_port *port)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	int room = 0;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	room = ringbuf_avail_space(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	return room;
}

// name : spcp8x5_chars_in_buffer
// in	: port - serial port
// out	: int - avail data in driver
// desc : get the number of avail data in write ring buffer
static int spcp8x5_chars_in_buffer(struct usb_serial_port *port)
{
	struct Spcp8x5Priv *priv = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	chars = ringbuf_avail_data(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	return chars;
}

// name : spcp8x5_read_proc
// in	: page - page for data write to 
//		: start - no used
//		: offset - no used
//		: count - no used
//		: data - no used
// out	: eof - is end of?
//		: int - number of bytes write to 
// desc : create a file in proc(/proc/spcp8x5). when read the /proc/spcp8x5,the system would
//		: call this routine for data to user space.
static int spcp8x5_read_proc(char * page , char ** start , off_t offset , int count , int *eof , void * data)
{
	int len = 0;
	struct usb_serial * serial = (struct usb_serial *)data;
	struct usb_serial_port * port;
	struct Spcp8x5Priv * priv;
	int i;
	
	if( serial == NULL)
	{
		trace("could not get serial\n");
		goto spcp8x5_read_proc_exit;
	}

	for(i=0 ; i<serial->num_ports ; ++i)
	{
		port = serial->port[i];
		if(port == NULL)
			break;
		priv = usb_get_serial_port_data(serial->port[i]);
		if(priv == NULL)
			break;

		len += sprintf(page+len , "line_control = 0x%x\n" , priv->line_control);
		len += sprintf(page+len , "line_status = 0x%x\n" , priv->line_status);
		len += sprintf(page+len , "write_urb_in_use = %d\n" , priv->write_urb_in_use);
		len += sprintf(page+len , "type = %d\n" , (int)priv->type);
		len += sprintf(page+len , "termios_initialized = %d\n" , priv->termios_initialized);
		len += sprintf(page+len , "buf_data_avail = %d\n" , ringbuf_avail_data(priv->buf));
		len += sprintf(page+len , "write_room() = %d\n" , spcp8x5_write_room(port));
		len += sprintf(page+len , "chars_in_buffer() = %d\n" , spcp8x5_chars_in_buffer(port));
	}

spcp8x5_read_proc_exit:
	*eof = 1;   // set 1 means no more data 
	return len; 
}

// name : spcp8x5_set_ctrlLine
// in	: dev - the usb serial dev
// 		: value - the value of control line
// 		: type - device type
// out	: int - success or not
// desc : set the modem control line of the device. NOTE spcp825-007 not supported this
static int spcp8x5_set_ctrlLine(struct usb_device* dev , u8 value , enum Spcp8x5Type type)
{
	int retval;
	u8 mcr = 0 ;

	if( type == SPCP825_007_TYPE )
		return -EPERM;

 	mcr = (unsigned short) value;
	retval = usb_control_msg (dev, usb_sndctrlpipe (dev, 0),
				  SET_UART_STATUS_TYPE,
				  SET_UART_STATUS,
				  mcr, 0x04, NULL, 0, 100);
	if( retval != 0 )
	{
		trace("usb_control_msg return %#x\n" , retval);
	}
	return retval;
}

// name : spcp8x5_get_msr
// in	: dev - the device
//		: type - the device type
// out	: status - the MSR(modem status register)
//		: int - success or not
// desc : get the modem status register of the device,NOTE spcp825-007 not supported this
static int spcp8x5_get_msr( struct usb_device* dev , u8* status , enum Spcp8x5Type type)
{
	int ret;
	
	if( type == SPCP825_007_TYPE )
		return -EPERM;		// I return Permited not support here but seem inval device is more fix
	if( status == NULL )
		return -EINVAL;
		
	ret = usb_control_msg (dev, usb_rcvctrlpipe (dev, 0),
			     GET_UART_STATUS, GET_UART_STATUS_TYPE,
			     0, GET_UART_STATUS_MSR, status, 1, 100);
	if (ret < 0)
	{
		trace("Get MSR = %#x failed (error = %d)",(int) status, (int)ret);
		return ret;
	}
		
	trace("0xc0:0x22:0:6  %d - %x ", (int)ret, (int)status);
	return 0;
}

// name : spcp8x5_set_workMode
// in	: dev - the device object
//		: value - the value of sub mode(by spec)
//		: index - the value for main mode(by spec)
//		: type - the device type
// out	: NULL
// desc : select the work mode. NOTE this function not supported by spcp825-007
static void spcp8x5_set_workMode( struct usb_device* dev , \
							u16 value , u16 index , enum Spcp8x5Type type)
{
	int ret;
	
	if( type == SPCP825_007_TYPE )
		return ;		// I return Permited not support here but seem inval device is more fix
	
	ret = usb_control_msg( dev, 
				    usb_sndctrlpipe( dev, 0),
				    SET_WORKING_MODE_TYPE,
				    SET_WORKING_MODE,
				    value, index , NULL, 0, 100);
	trace("value = %#x , index = %#x\n" , value , index);
	if( ret<0 )
	{
		trace("RTSCTS usb_control_msg(enable flowctrl) = %d\n" , ret);
	}
}

// name : spcp8x5_init
// in	: null
// out	: int - 0.success,other.failed
// desc : call when insmod the module into system. register this driver to the
//		: usb serial sub system.
static int __init spcp8x5_init (void)
{
	int retval;
	retval = usb_serial_register(&spcp8x5_device);
	if (retval)
		goto failed_usb_serial_register;
	retval = usb_register(&spcp8x5_driver);
	if (retval)
		goto failed_usb_register;
	info(DRIVER_DESC " " DRIVER_VERSION);
	return 0;
failed_usb_register:
	usb_serial_deregister(&spcp8x5_device);
failed_usb_serial_register:
	return retval;
}

// name : spcp8x5_exit
// in 	: null
// out	: null
// desc : called when rmmod the module. unregister the driver from
//		: usb serial sub system
static void __exit spcp8x5_exit (void)
{
	usb_deregister(&spcp8x5_driver);
	usb_serial_deregister(&spcp8x5_device);
}


module_init(spcp8x5_init);
module_exit(spcp8x5_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");

module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug enabled or not");

