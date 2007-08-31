#!/bin/bash
USER=$1
PASS=$2
echo "$(date) Logging" >> /var/log/pluto/tribune.log
if [ $# -ne 0 ]; then
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
	IFS=$'\n'
	for i in `cat $RESPONSE|grep -e "^<lineup "|cut -d"'" -f2,4,6,8,10,12`; do
		echo "`echo $i|sed "s/'/\t/g"`"
	done
#echo "1	Brisbane Direct TV"
#echo "2	Brisbane Comcast"
#echo "3	Brisbane Over the Air"
	rm $POSTFILE $RESPONSE
else
	echo "ERROR"
fi
