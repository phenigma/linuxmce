#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

#declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;
my $Device_EXT;

#check params
if($#ARGV < 5 || $ARGV[0] ne "-d" || $ARGV[2] ne "-i" || $ARGV[4] ne "-m")
{
    print "<USAGE-1>\n$0 -d <Device ID> -i <IP> -m <mac address>\n";
    exit(-1);
}
else
{
    $Device_ID = $ARGV[1];
    $Device_IP = $ARGV[3];
    $Device_MAC = $ARGV[5];
}

#sync with AMP (practically do nothing but create a new extension number)
`/usr/pluto/bin/sync_pluto2amp.pl $Device_ID`;

open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);
$Device_MAC =~ s/[^0-9A-Fa-f]//g;
$Device_MAC = uc($Device_MAC);

my $str_XMLDefault = "";
my $str_SEPDefault = "";
my $str_DEV = "";
my $str_LINE = "";
my $str_FILE = "";

$str_XMLDefault .= "<?xml version=\"1.0\"?>\n";
$str_XMLDefault .= "<Default>\n";
$str_XMLDefault .= "\t<callManagerGroup>\n";
$str_XMLDefault .= "\t\t<members>\n";
$str_XMLDefault .= "\t\t\t<member priority=\"0\">\n";
$str_XMLDefault .= "\t\t\t\t<callManager>\n";
$str_XMLDefault .= "\t\t\t\t\t<ports>\n";
$str_XMLDefault .= "\t\t\t\t\t\t<ethernetPhonePort>2000</ethernetPhonePort>\n";
$str_XMLDefault .= "\t\t\t\t\t</ports>\n";
$str_XMLDefault .= "\t\t\t\t\t<processNodeName>192.168.80.1</processNodeName>\n";
$str_XMLDefault .= "\t\t\t\t</callManager>\n";
$str_XMLDefault .= "\t\t\t</member>\n";
$str_XMLDefault .= "\t\t</members>\n";
$str_XMLDefault .= "\t</callManagerGroup>\n";
$str_XMLDefault .= "\t<servicesURL>http://192.168.80.1/pluto-admin/ServicesMenu.php</servicesURL>\n";
$str_XMLDefault .= "</Default>\n";

open(FILE,"> /tftpboot/XmlDefault.cnf.xml");
print FILE $str_XMLDefault;
close(FILE);

$str_SEPDefault .= "<device>\n";
$str_SEPDefault .= "<devicePool>\n";
$str_SEPDefault .= "<name>Default</name>\n";
$str_SEPDefault .= "<dateTimeSetting>\n";
$str_SEPDefault .= "<name>CMLocal</name>\n";
$str_SEPDefault .= "<dateTemplate>D.M.Y</dateTemplate>\n";
$str_SEPDefault .= "<timeZone></timeZone>\n";
$str_SEPDefault .= "</dateTimeSetting>\n";
$str_SEPDefault .= "<callManagerGroup>\n";
$str_SEPDefault .= "<members>\n";
$str_SEPDefault .= "<member priority=\"0\">\n";
$str_SEPDefault .= "<callManager>\n";
$str_SEPDefault .= "<ports>\n";
$str_SEPDefault .= "<ethernetPhonePort>2000</ethernetPhonePort>\n";
$str_SEPDefault .= "</ports>\n";
$str_SEPDefault .= "<processNodeName>192.168.80.1</processNodeName>\n";
$str_SEPDefault .= "</callManager>\n";
$str_SEPDefault .= "</member>\n";
$str_SEPDefault .= "</members>\n";
$str_SEPDefault .= "</callManagerGroup>\n";
$str_SEPDefault .= "<srstInfo>\n";
$str_SEPDefault .= "<name>Enable</name>\n";
$str_SEPDefault .= "<srstOption>Enable</srstOption>\n";
$str_SEPDefault .= "<userModifiable>true</userModifiable>\n";
$str_SEPDefault .= "<ipAddr1>192.168.80.1</ipAddr1>\n";
$str_SEPDefault .= "<port1>2000</port1>\n";
$str_SEPDefault .= "<ipAddr2></ipAddr2>\n";
$str_SEPDefault .= "<port2>2000</port2>\n";
$str_SEPDefault .= "<ipAddr3></ipAddr3>\n";
$str_SEPDefault .= "<port3>2000</port3>\n";
$str_SEPDefault .= "</srstInfo>\n";
$str_SEPDefault .= "<mlppDomainId>-1</mlppDomainId>\n";
$str_SEPDefault .= "<mlppIndicationStatus>Default</mlppIndicationStatus>\n";
$str_SEPDefault .= "<preemption>Default</preemption>\n";
$str_SEPDefault .= "</devicePool>\n";
$str_SEPDefault .= "<loadInformation></loadInformation>\n";
$str_SEPDefault .= "<vendorConfig>\n";
$str_SEPDefault .= "<disableSpeaker>false</disableSpeaker>\n";
$str_SEPDefault .= "<disableSpeakerAndHeadset>false</disableSpeakerAndHeadset>\n";
$str_SEPDefault .= "<forwardingDelay>1</forwardingDelay>\n";
$str_SEPDefault .= "<pcPort>0</pcPort>\n";
$str_SEPDefault .= "<settingsAccess>1</settingsAccess>\n";
$str_SEPDefault .= "<garp>0</garp>\n";
$str_SEPDefault .= "<voiceVlanAccess>0</voiceVlanAccess>\n";
$str_SEPDefault .= "<videoCapability>1</videoCapability>\n";
$str_SEPDefault .= "<autoSelectLineEnable>0</autoSelectLineEnable>\n";
$str_SEPDefault .= "<webAccess>1</webAccess>\n";
$str_SEPDefault .= "<daysDisplayNotActive>1,2,3,4,5,6,7</daysDisplayNotActive>\n";
$str_SEPDefault .= "<displayOnTime>08:30</displayOnTime>\n";
$str_SEPDefault .= "<displayOnDuration>11:30</displayOnDuration>\n";
$str_SEPDefault .= "<displayIdleTimeout>00:05</displayIdleTimeout>\n";
$str_SEPDefault .= "</vendorConfig>\n";
$str_SEPDefault .= "<versionStamp></versionStamp>\n";
$str_SEPDefault .= "<userLocale>\n";
$str_SEPDefault .= "<name></name>\n";
$str_SEPDefault .= "<uid>1</uid>\n";
$str_SEPDefault .= "<langCode>en</langCode>\n";
$str_SEPDefault .= "<version>4.0(1)</version>\n";
$str_SEPDefault .= "<winCharSet>iso-8859-1</winCharSet>\n";
$str_SEPDefault .= "</userLocale>\n";
$str_SEPDefault .= "<networkLocale></networkLocale>\n";
$str_SEPDefault .= "<networkLocaleInfo>\n";
$str_SEPDefault .= "<name></name>\n";
$str_SEPDefault .= "<uid>64</uid>\n";
$str_SEPDefault .= "<version>4.0(1)</version>\n";
$str_SEPDefault .= "</networkLocaleInfo>\n";
$str_SEPDefault .= "<deviceSecurityMode>1</deviceSecurityMode>\n";
$str_SEPDefault .= "<idleTimeout>120</idleTimeout>\n";
$str_SEPDefault .= "<authenticationURL></authenticationURL>\n";
$str_SEPDefault .= "<directoryURL></directoryURL>\n";
$str_SEPDefault .= "<idleURL></idleURL>\n";
$str_SEPDefault .= "<informationURL></informationURL>\n";
$str_SEPDefault .= "<messagesURL></messagesURL>\n";
$str_SEPDefault .= "<proxyServerURL></proxyServerURL>\n";
$str_SEPDefault .= "<servicesURL>http://192.168.80.1/pluto-admin/ServicesMenu.php</servicesURL>\n";
$str_SEPDefault .= "<dscpForSCCPPhoneConfig>96</dscpForSCCPPhoneConfig>\n";
$str_SEPDefault .= "<dscpForSCCPPhoneServices>0</dscpForSCCPPhoneServices>\n";
$str_SEPDefault .= "<dscpForCm2Dvce>96</dscpForCm2Dvce>\n";
$str_SEPDefault .= "</device>\n";

open(FILE,"> /tftpboot/SEPDefault7970.cnf.xml");
print FILE $str_SEPDefault;
close(FILE);

`ln -s /tftpboot/SEPDefault7970.cnf.xml /tftpboot/SEP$Device_MAC.cnf.xml`;

$str_DEV .= ";<pl_dev_$Device_ID>\n";
$str_DEV .= "type = 7970\n";
$str_DEV .= "description = SEP$Device_MAC\n";
$str_DEV .= "autologin = $Device_EXT\n";
$str_DEV .= "transfer = on\n";
$str_DEV .= "park = on\n";
$str_DEV .= "speeddial = \n";
$str_DEV .= "cfwdall = off\n";
$str_DEV .= "cfwdbusy = off\n";
$str_DEV .= "dtmfmode = inband\n";
$str_DEV .= "device => SEP$Device_MAC\n";
$str_DEV .= ";</pl_dev_$Device_ID>\n";

$str_LINE .= ";<pl_line_$Device_ID>\n";
$str_LINE .= "id = $Device_EXT\n";
$str_LINE .= "pin = \n";
$str_LINE .= "label = $Device_EXT\n";
$str_LINE .= "description = $Device_EXT\n";
$str_LINE .= "context = from-internal\n";
$str_LINE .= "incominglimit = 2\n";
$str_LINE .= "transfer = on\n";
$str_LINE .= "secondary_dialtone_digits = 9\n";
$str_LINE .= "secondary_dialtone_tone = 0x22\n";
$str_LINE .= "line => $Device_EXT\n";
$str_LINE .= ";</pl_line_$Device_ID>\n";

{
	local $/ = undef;
	open(FILE,"< /etc/asterisk/sccp.conf") or die "Ugly";
	$str_FILE = <FILE>;
	close(FILE);	
}

if( $str_FILE =~ /\[devices\].+?;<pl_dev_$Device_ID>.+?;<\/pl_dev_$Device_ID>.+?\[lines\].+?;<pl_line_$Device_ID>.+?;<\/pl_line_$Device_ID>/gms)
{
	$str_FILE =~ s/(\[devices\].+?);<pl_dev_$Device_ID>.+?;<\/pl_dev_$Device_ID>(.+?\[lines\].+?);<pl_line_$Device_ID>.+?;<\/pl_line_$Device_ID>/$1$str_DEV$2$str_LINE/gms;
}
else
{
	$str_FILE =~ s/\[lines\]/${str_DEV}\n[lines]/gms;
	$str_FILE .= "\n".$str_LINE;
}

$str_FILE =~ s/\n\n+/\n/gms;
$str_FILE =~ s/\[/\n\[/gms;
$str_FILE =~ s/;<pl/\n;<pl/gms;
$str_FILE =~ s/\]\n+;<pl/\]\n;<pl/gms;

open(FILE,"> /etc/asterisk/sccp.conf") or die "Ugly";
print FILE $str_FILE;
close(FILE);


### Update Cisco 7970 Orbiter
my $DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=192.168.80.1;user=root;password=;") or die "Could not connect to MySQL";
my $DB_SQL = "select PK_Device from Device where FK_DeviceTemplate=1727 and FK_Device_ControlledVia=$Device_ID";
my $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
if(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
	my $ORB_ID = $DB_ROW->{'PK_Device'};
	$DB_STATEMENT->finish();
	$DB_SQL = "update Device_DeviceData SET IK_DeviceData='$Device_IP' WHERE FK_Device='$ORB_ID' AND FK_DeviceData='118'";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
}
$DB_PL_HANDLE->disconnect();

#reload asterisk
`/etc/init.d/asterisk reload`;
