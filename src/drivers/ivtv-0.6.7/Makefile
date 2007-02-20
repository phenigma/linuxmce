all clean install: version_check
	make -C driver $@
	make -C i2c-drivers $@
	make -C utils $@
	make -C test $@

version_check:
	@UNAME=`uname -r | sed 's/-.*//' | cut -d . -f 1-3`; \
	if [ $$UNAME '>' "2.6.16" ]; then \
		echo ivtv-0.6.0 does NOT support kernels '>' 2.6.16! ; \
		echo See www.ivtvdriver.org for more information. ; \
		false; \
	elif [ $$UNAME == "2.6.16" ]; then \
		true; \
	elif [ $$UNAME == "2.6.15" -a -e /etc/redhat-release ]; then \
		if grep Bordeaux /etc/redhat-release >/dev/null; then \
			true; \
		else \
			echo ivtv-0.6.0 does NOT support kernels '<' 2.6.16! ; \
			echo See www.ivtvdriver.org for more information. ; \
			false; \
		fi; \
	else \
		echo ivtv-0.6.0 does NOT support kernels '<' 2.6.16! ; \
		echo See www.ivtvdriver.org for more information. ; \
		false; \
	fi

distclean: clean
