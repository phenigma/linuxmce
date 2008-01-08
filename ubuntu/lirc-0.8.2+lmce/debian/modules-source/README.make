Building the LIRC kernel drivers my hand:
----------------------------------------

Unless you what to debug something, help in the development of LIRC or in the
maintenance of this debian packages you should probably read README and forget
about this file.

To build the modules you should run:

	make driver_name
		A certain driver.
	make debconf
		to build the drivers you selected on debconf's prompt
	make
		to build all modules. 

The Makefiles assume that your current kernel sources are in /usr/src/linux;
if they are not there, redefine KSRC variable when running make:

	make driver_name KSRC=/my/kernel/source/

Some drivers have special requierements:

	gpio:	
		needs bttv version 0.7.45 or higher (present on 2.4 kernels).
		If you are using the bttv tarball, copy it to the kernel.

	i2c:	
		needs the new I2C stack (present on 2.4 kernels). You can get
		it at http://www2.lm-sensors.nu/~lm78/

If you want to change any default values, like ports, irqs, ... you may change
the with debconf:

	dpkg-reconfigure lirc-modules-source

After successfully running make you will get all requiered kernel modules (.o
files) under directory "modules/" , which are the driver you need for lirc to
support your hardware, to install them run:

	make install depmod

And then you should load them (posibly via "modconf" utility).


This are the drivers you need for some of the supported hardware:
----------------------------------------------------------------

Hardware						Driver
=============================				=============
Custom parallel-port receiver				parallel
Serial-port driver					serial
Serial InfraRed (IRDA)					sir
TV cards:
	Hauppauge and PixelView				i2c
	FlyVideo98, Avermedia, MiRO and many others	gpio
ITE IT8705 and IT8712 CIR ports				it87
	 on the ECS K7S5A

You should of course also have a look at any README files in the
subdirectories of the drivers you want to compile.
