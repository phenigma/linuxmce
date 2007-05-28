#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

sub getIP {
        my $dbh = DBI->connect('dbi:mysql:pluto_main');
        my $sth = $dbh->prepare("SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7");
        $sth->execute || die "Sql Error";
        my $row = $sth->fetchrow_hashref;
        my $IP = $row->{IPaddress};
        return $IP;
}

#declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;
my $Device_EXT;
my $IntIP; 

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

$IntIP = getIP();
if ($IntIP eq "") {
        $IntIP="192.168.80.1";
}

#sync with AMP
`/usr/pluto/bin/sync_pluto2amp.pl $Device_ID`;

open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);
$Device_MAC =~ s/[^0-9A-Fa-f]//g;

$Device_MAC = uc($Device_MAC);

my $str_SIPDefault = "";
my $str_SIPMAC = "";

$str_SIPDefault .= "proxy1_address: \"$IntIP\"\n";
$str_SIPDefault .= "proxy1_port: 5060\n";
$str_SIPDefault .= "line1_name: \"\"\n";
$str_SIPDefault .= "line1_authname: \"\"\n";
$str_SIPDefault .= "line1_password: \"\"\n";
$str_SIPDefault .= "line1_shortname: \"\"\n";
$str_SIPDefault .= "proxy2_address: \"\"\n";
$str_SIPDefault .= "proxy2_port: 5060\n";
$str_SIPDefault .= "line2_name: \"\"\n";
$str_SIPDefault .= "line2_authname: \"\"\n";
$str_SIPDefault .= "line2_password: \"\"\n";
$str_SIPDefault .= "line2_shortname: \"\"\n";
$str_SIPDefault .= "proxy3_address: \"\"\n";
$str_SIPDefault .= "proxy3_port: 5060\n";
$str_SIPDefault .= "line3_name: \"\"\n";
$str_SIPDefault .= "line3_authname: \"\"\n";
$str_SIPDefault .= "line3_password: \"\"\n";
$str_SIPDefault .= "line3_shortname: \"\"\n";
$str_SIPDefault .= "proxy4_address: \"\"\n";
$str_SIPDefault .= "proxy4_port: 5060\n";
$str_SIPDefault .= "line4_name: \"\"\n";
$str_SIPDefault .= "line4_authname: \"\"\n";
$str_SIPDefault .= "line4_password: \"\"\n";
$str_SIPDefault .= "line4_shortname: \"\"\n";
$str_SIPDefault .= "proxy5_address: \"\"\n";
$str_SIPDefault .= "proxy5_port: 5060\n";
$str_SIPDefault .= "line5_name: \"\"\n";
$str_SIPDefault .= "line5_authname: \"\"\n";
$str_SIPDefault .= "line5_password: \"\"\n";
$str_SIPDefault .= "line5_shortname: \"\"\n";
$str_SIPDefault .= "proxy6_address: \"\"\n";
$str_SIPDefault .= "proxy6_port: 5060\n";
$str_SIPDefault .= "line6_name: \"\"\n";
$str_SIPDefault .= "line6_authname: \"\"\n";
$str_SIPDefault .= "line6_password: \"\"\n";
$str_SIPDefault .= "line6_shortname: \"\"\n";
$str_SIPDefault .= "proxy_register: 1\n";
$str_SIPDefault .= "timer_register_expires: 180\n";
$str_SIPDefault .= "preferred_codec: g711ulaw\n";
$str_SIPDefault .= "tos_media: 5\n";
$str_SIPDefault .= "dtmf_inband: 1\n";
$str_SIPDefault .= "dtmf_outofband: avt\n";
$str_SIPDefault .= "dtmf_db_level: 3\n";
$str_SIPDefault .= "timer_t1: 500\n";
$str_SIPDefault .= "timer_t2: 4000\n";
$str_SIPDefault .= "sip_retx: 10\n";
$str_SIPDefault .= "sip_invite_retx: 6\n";
$str_SIPDefault .= "timer_invite_expires: 180\n";
$str_SIPDefault .= "tftp_cfg_dir: \"./\"\n";
$str_SIPDefault .= "dnd_control: 0\n";
$str_SIPDefault .= "callerid_blocking: 0\n";
$str_SIPDefault .= "anonymous_call_block: 0\n";
$str_SIPDefault .= "dtmf_avt_payload: 101\n";
$str_SIPDefault .= "sync: 1\n";
$str_SIPDefault .= "phone_label: \"Pluto\"\n";

$str_SIPMAC .= "line1_name: \"$Device_EXT\"\n";
$str_SIPMAC .= "line1_authname: \"$Device_EXT\"\n";
$str_SIPMAC .= "line1_password: \"$Device_EXT\"\n";
$str_SIPMAC .= "line1_shortname: \"$Device_EXT\"\n";

open(FILE,"> /tftpboot/SIPDefault.cnf");
print FILE $str_SIPDefault;
close(FILE);

open(FILE,"> /tftpboot/SIP$Device_MAC.cnf");
print FILE $str_SIPMAC;
close(FILE);
