	KVER=2.6.15-pluto-1-686
	for i in /lib/modules/"$KVER"/extra/*.ko; do 
		MODULE=`echo $i | sed s#[^/]*/##g`;
		cd /lib/modules/"$KVER"/;
		find . -path ./extra -prune -o -name $MODULE -print -exec mv \{} \{}.saved \; ;
	done

	echo -n "Running depmod .... "
	depmod -a
	echo "done"
