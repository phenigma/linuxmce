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
my $str_DEV = "";
my $str_LINE = "";
my $str_FILE = "";

$str_XMLDefault .= "<?xml version=\"1.0\"?>\n";
$str_XMLDefault .= "<Default>\n";
$str_XMLDefault .= "<callManagerGroup>\n";
$str_XMLDefault .= "<members>\n";
$str_XMLDefault .= "<member priority=\"0\">\n";
$str_XMLDefault .= "<callManager>\n";
$str_XMLDefault .= "<ports>\n";
$str_XMLDefault .= "<ethernetPhonePort>2000</ethernetPhonePort>\n";
$str_XMLDefault .= "</ports>\n";
$str_XMLDefault .= "<processNodeName>192.168.80.1</processNodeName>\n";
$str_XMLDefault .= "</callManager>\n";
$str_XMLDefault .= "</member>\n";
$str_XMLDefault .= "</members>\n";
$str_XMLDefault .= "</callManagerGroup>\n";
$str_XMLDefault .= "<servicesURL>http://192.168.80.1/pluto-admin/ServicesMenu.php</servicesURL>\n";
$str_XMLDefault .= "</Default>\n";

$str_DEV .= ";<pl_dev_$Device_ID>\n";
$str_DEV .= "type = 7970\n";
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
$str_LINE .= "pin = $Device_EXT\n";
$str_LINE .= "label = $Device_EXT\n";
$str_LINE .= "description = $Device_EXT\n";
$str_LINE .= "context = from-internal\n";
$str_LINE .= "incominglimit = 2\n";
$str_LINE .= "transfer = on\n";
$str_LINE .= "secondary_dialtone_digits = 9\n";
$str_LINE .= "secondary_dialtone_tone = 0x22\n";
$str_LINE .= "line => $Device_EXT\n";
$str_LINE .= ";</pl_line_$Device_ID>\n";

open(FILE,"> /tftpboot/XmlDefault.cnf.xml");
print FILE $str_XMLDefault;
close(FILE);

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

#reload asterisk
`asterisk -r -x reload`;
