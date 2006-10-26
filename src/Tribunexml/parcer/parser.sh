#!/bin/bash

IFS=$'\n'
out="out"
progs="progs"
dirdiff="diffs"
old="old"
www="/var/www/tribune"
# URL with real database
#url="ftp://mon664jv:NPM602kp@ftp.tmstv.com/bigb"
# URL with test database
url="ftp://big549bg:NOK248ti@ftp.tmstv.com/pub"

wget -O - "$url/progrec.txt.Z"|uncompress>progrec.txt
wget -O - "$url/statrec.txt.Z"|uncompress>statrec.txt
wget -O - "$url/skedrec.txt.Z"|uncompress>skedrec.txt

echo -ne "">$progs/crew.txt
echo -ne "">$progs/genres.txt

for i in `seq 17 3 136`; do 
	cut -f1,$i-$(($i+2)) -d'|' progrec.txt|grep -e"[^|][^|][^|]$"|sed "s/$/|/g"|sort>> $progs/crew.txt
	echo "processed $i"
done

for i in `seq 137 1 142`; do
	cut -f1,$i -d'|' progrec.txt |grep -e"[^|][^|]$"|sort>> $progs/genres.txt
	echo "processed $i"
done

cut -f-16,143- -d'|' progrec.txt>$progs/progrec.txt

num=1
for i in `cat uniqueroles.txt`; do
	echo $num
	replace "|$i|" "|$num|" -- $progs/crew.txt
	num=$(($num+1))
done

diff -u $old/progrec.txt $progs/progrec.txt>$dirdiff/progdiff.txt
grep -e"^+" $dirdiff/progdiff.txt|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/progplus.diff
grep -e"^-" $dirdiff/progdiff.txt|sed "s/^-//g"|cut -f1 -d"|"|grep -e"^[^-]">$dirdiff/progminus.diff
rm $dirdiff/progdiff.txt

diff -u $old/genres.txt $progs/genres.txt|grep -e"^+"|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/genres.diff
diff -u $old/crew.txt $progs/crew.txt|grep -e"^+"|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/crew.diff

tar cjf $www/$progs.tar.bz2 $progs
tar cjf $www/$dirdiff.tar.bz2 $dirdiff
mv $progs/progrec.txt $old/progrec.txt
mv $progs/crew.txt $old/crew.txt
mv $progs/genres.txt $old/genres.txt


for i in `cat statrec.txt`; do
	# can we optimize this?
	station=`echo $i|cut -f1 -d'|'`
	
	echo "Parsing information for station $station"
	
	# creating new folder
	mkdir $out/$station

	# getting station description
	echo $i > $out/$station/statrec.txt

	# getting station schedule
	grep -e "^$station" skedrec.txt > $out/$station/skedrec.txt

	cd $out

	tar jcf $www/stations/$station.tar.bz2 $station
	rm -rf $station

	cd ..
done

