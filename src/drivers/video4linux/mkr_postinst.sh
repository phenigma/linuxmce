	for i in /lib/modules/2.6.10-1-686/extra/*.ko; do 
		MODULE=`echo $i | sed s#[^/]*/##g`;
		cd /lib/modules/2.6.10-1-686/;
		find . -path ./extra -prune -o -name $MODULE -print -exec mv \{} \{}.saved \; ;
	done

	echo -n "Running depmod .... "
	depmod -a
	echo "done"
