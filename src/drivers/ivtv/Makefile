KERNEL=2.6.10-1-686


clean:
	$(MAKE) -C driver/ clean
	rm -rf modules/

modules: FORCE
	cd driver/ && KDIR=/lib/modules/$(KERNEL)/build $(MAKE) -e 
	mkdir modules/
	cp driver/*.ko modules/


FORCE:
