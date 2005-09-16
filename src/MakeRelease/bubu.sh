version=1

if ! MakeRelease -a -o 7 -n / -s /home/samba/builds/Windows_Output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,204,118,303,128,162,191,195,280,272,363,364,341 > /home/MakeRelease/MakeRelease2.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 7 -n / -s /home/MakeRelease/trunk -r 10 -v $version -b -k 211,214,233,256,219,220 > /home/MakeRelease/MakeRelease3.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 12 -n / -s /home/samba/builds/Windows_Output/ -r 15 -v $version -b -k 119 > /home/MakeRelease/MakeRelease4.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 8 -n / -s /home/samba/builds/Windows_Output/ -r 16 -v $version -b -k 119 > /home/MakeRelease/MakeRelease5.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

