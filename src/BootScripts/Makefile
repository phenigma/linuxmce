shfiles = Config_Ops.sh Spawn_Device.sh Start_Bluetooth_Dongle.sh Start_DCERouter.sh Start_Orbiter.sh Start_X.sh Start_ratpoison.sh Startup_Scripts.sh
files = $(shfiles) pluto.func

all:
	for i in $(files); do ln -sf "../BootScripts/$$i" ../bin; done
