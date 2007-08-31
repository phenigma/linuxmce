#!/bin/bash

USER=$1
PASS=$2
LINEUP=$3

echo "$(date) Logging" >> /var/log/pluto/tribune.log
if [ -z $LINEUP ]; then
	echo "ERROR"
else
	POSTFILE="/tmp/DDpost"
	RESPONSE="/tmp/DDresponce"
	echo -ne "<?xml version='1.0' encoding='utf-8'?>
<SOAP-ENV:Envelope
xmlns:SOAP-ENV='http://schemas.xmlsoap.org/soap/envelope/'
xmlns:xsd='http://www.w3.org/2001/XMLSchema'
xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'
xmlns:SOAP-ENC='http://schemas.xmlsoap.org/soap/encoding/'>
<SOAP-ENV:Body>
<ns1:download  xmlns:ns1='urn:TMSWebServices'>
<startTime xsi:type='xsd:dateTime'>">$POSTFILE
	echo -ne `date +%Y-%m-%dT00:00:00Z`>>$POSTFILE
	echo -ne "</startTime>
<endTime xsi:type='xsd:dateTime'>">>$POSTFILE
	echo -ne `date +%Y-%m-%dT00:00:00Z`>>$POSTFILE
	echo -ne "</endTime>
</ns1:download>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
">>$POSTFILE

	/usr/bin/wget -q --http-user=$USER --http-passwd=$PASS --post-file=$POSTFILE --header=Accept-Encoding:gzip "http://webservices.schedulesdirect.tmsdatadirect.com/schedulesdirect/tvlistings/xtvdService" --output-document=-|gzip -d>$RESPONSE
	echo "OK"
	BEGINL="\\<lineup id\\=\\'$LINEUP\\'"
	ENDL="\\<\\/lineup>"
	ENDS="\\<\\/station>"
	IFS=$'\n'
	for map in `cat $RESPONSE|perl -n -e '$f=/^'$BEGINL'/../^'$ENDL'$/;if($f&&/^'$ENDL'$/){exit()};if($f&&!/^'$BEGINL'/){print;}'`; do
		for station in `echo $map|grep -e "map"|cut -d"'" -f2`; do
			BEGINS="\\<station id\\=\\'$station\\'>"
			echo -ne "$station\t"
			echo `cat $RESPONSE|perl -n -e '$f=/^'$BEGINS'/../^'$ENDS'$/;if($f&&/^'$ENDS'$/){exit()};if($f&&!/^'$BEGINS'/){print ;}'|grep -e "callSign"|sed "s/</>/g"|cut -d'>' -f3`
		done
	done
	rm $POSTFILE $RESPONSE
#echo "1	CNN"
#echo "2	HBO"
#echo "3	NBC"
fi
