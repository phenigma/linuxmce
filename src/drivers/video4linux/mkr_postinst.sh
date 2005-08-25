	for i in /lib/modules/2.6.10-1-686/extra/*.ko; do 
		MODULE=`echo $i | sed s#[^/]*/##g`;
		cd /lib/modules/2.6.10-1-686/;
		find . -path ./extra -prune -o -name $MODULE -print -exec mv \{} \{}.saved \; ;
	done

	echo -n "Running depmod .... "
	depmod -a
	echo "done"

	if ! grep -q '^cx88-ivtv$' /etc/modules; then
		echo cx88-ivtv >>/etc/modules
	fi
	modprobe cx88-ivtv || /bin/true
