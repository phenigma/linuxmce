#!/bin/bash

modlist="cx88_blackbird cx8802 cx8800 cx88xx"
revlist=""
for i in $modlist; do revlist="$i $revlist"; done

echo "(re)building modules..."
make || exit 1
echo "(re)installing modules..."
sudo make install || exit 2
echo -n "unloading modules... "
sudo rmmod cx88_blackbird cx8802 cx8800 cx88xx || exit 3
echo "done."
sudo depmod -a

echo -n "(re)loading modules... "
sudo modprobe cx88xx || exit 4
echo -n " cx88xx "
sudo modprobe cx8800 || exit 5
echo -n " cx8800 "
sudo modprobe cx8802 || exit 6
echo -n " cx8802 "
sudo modprobe cx88_blackbird || exit 7
echo -n " cx88_blackbird "
echo " done."
