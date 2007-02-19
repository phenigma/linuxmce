#!/bin/bash

# place of site with parsed files
www="/var/www/tribune"
# URL with real database
url="ftp://mon664jv:NPM602kp@ftp.tmstv.com/bigb"
# getting and repacking channels data
wget -O - "$url/headend.fsf.Z"|uncompress>"$www/headend.fsf"
wget -O - "$url/lineup.fsf.Z"|uncompress>"$www/lineup.fsf"
wget -O - "$url/statrec.txt.Z"|uncompress>"$www/statrec.txt"
# getting programm data
wget -O - "$url/progrec.txt.Z"|uncompress>"$www/progrec.txt"
wget -O - "$url/skedrec.txt.Z"|uncompress>"$www/skedrec.txt"

