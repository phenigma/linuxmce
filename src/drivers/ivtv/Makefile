KERNEL=2.6.10-1-686


clean:
	$(MAKE) -C driver/ clean

modules: FORCE
	cd driver/ && KDIR=/lib/modules/$(KERNEL)/build $(MAKE) -e 



FORCE:
