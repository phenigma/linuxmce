#!/bin/bash

IFS=$'\n'
out="out"
progs="progs"
dirdiff="diffs"
# place with old files
old="old"
# place of site with parsed files
www="/var/www/tribune"
# URL with real database
url="ftp://mon664jv:NPM602kp@ftp.tmstv.com/bigb"
# URL with test database
#url="ftp://big549bg:NOK248ti@ftp.tmstv.com/pub"
# getting and repacking channels data
wget -O - "$url/headend.fsf.Z"|uncompress>headend.fsf
wget -O - "$url/lineup.fsf.Z"|uncompress>lineup.fsf
tar cjf $www/headend.fsf.tar.bz2 headend.fsf
tar cjf $www/lineup.fsf.tar.bz2 lineup.fsf
rm lineup.fsf headend.fsf
wget -O - "$url/statrec.txt.Z"|uncompress>statrec.txt
tar cjf $www/statrec.txt.tar.bz2 statrec.txt
mv statrec.txt /tmp/statrec.txt

# getting programm data
wget -O - "$url/progrec.txt.Z"|uncompress>/tmp/progrec.txt
wget -O - "$url/skedrec.txt.Z"|uncompress>/tmp/skedrec.txt

echo -ne "">$progs/crew.txt
echo -ne "">$progs/genres.txt

# exuding crew
for i in `seq 17 3 136`; do 
	cut -f1,$i-$(($i+2)) -d'|' /tmp/progrec.txt|grep -e"[^|][^|][^|]$"|sed "s/$/|/g"|sort>> $progs/crew.txt
	echo "processed $i"
done

# exuding genres
for i in `seq 137 1 142`; do
	cut -f1,$i -d'|' /tmp/progrec.txt |grep -e"[^|][^|]$"|sort>> $progs/genres.txt
	echo "processed $i"
done

# removing crew and genres data from programs file
cut -f-16,143- -d'|' /tmp/progrec.txt>$progs/progrec.txt

# replacing roles from text to number
num=1
for i in `cat uniqueroles.txt`; do
	echo $num
	replace "|$i|" "|$num|" -- $progs/crew.txt
	num=$(($num+1))
done

# creating diffs
diff -u $old/progrec.txt $progs/progrec.txt>$dirdiff/progdiff.txt
grep -e"^+" $dirdiff/progdiff.txt|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/progplus.diff
grep -e"^-" $dirdiff/progdiff.txt|sed "s/^-//g"|cut -f1 -d"|"|grep -e"^[^-]">$dirdiff/progminus.diff
rm $dirdiff/progdiff.txt

diff -u $old/genres.txt $progs/genres.txt|grep -e"^+"|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/genres.diff
diff -u $old/crew.txt $progs/crew.txt|grep -e"^+"|sed "s/^+//g"|grep -e"^[^\+]">$dirdiff/crew.diff

tar cjf $www/progs.tar.bz2 $progs
tar cjf $www/diffs.tar.bz2 $dirdiff
tar cjf $www/uniqueroles.txt.tar.bz2 uniqueroles.txt
mv $progs/progrec.txt $old/progrec.txt
mv $progs/crew.txt $old/crew.txt
mv $progs/genres.txt $old/genres.txt


# cutting schedules to stations
for i in `cat /tmp/statrec.txt`; do
	# can we optimize this?
	station=`echo $i|cut -f1 -d'|'`
	
	echo "Parsing information for station $station"
	
	# creating new folder
	mkdir $out/$station

	# getting station description
	echo $i > $out/$station/statrec.txt

	# getting station schedule
	grep -e "^$station" /tmp/skedrec.txt > $out/$station/skedrec.txt

	cd $out

	tar jcf $www/stations/$station.tar.bz2 $station
	rm -rf $station

	cd ..
done

# removing temporary files
rm $dirdiff/progplus.diff $dirdiff/progminus.diff $dirdiff/genres.diff $dirdiff/crew.diff /tmp/skedrec.txt /tmp/statrec.txt /tmp/progrec.txt /tmp/headend.fsf
