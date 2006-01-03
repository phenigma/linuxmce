#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

my $DECLARED_USERNAME;
my $DECLARED_USERPASSWD;
my $DECLARED_NUMBER;
my $DECLARED_PREFIX = "9";

my $TRUNK_URL = 'http://localhost/pluto-admin/amp/admin/config.php?display=6&tech=SIP';
my %TRUNK_VARS = ();
my $TRUNK_DATA = "";

my $OUT_URL = 'http://localhost/pluto-admin/amp/admin/config.php?display=8';
my %OUT_VARS = ();
my $OUT_DATA = "";

my $IN_URL = 'http://localhost/pluto-admin/amp/admin/config.php?display=7';
my %IN_VARS = ();
my $IN_DATA = "";

#check params
unless (defined($ARGV[0]) && defined($ARGV[1]) && defined($ARGV[2]))
{
    print "USAGE :$0 <username> <password> <phone_number> [<prefix_to_use_the_line>]\n";
    exit(-1);
}

#fix permissions on each run
`chmod g+w /etc/asterisk/*`;

$DECLARED_USERNAME=$ARGV[0];
$DECLARED_USERPASSWD=$ARGV[1];
$DECLARED_NUMBER=$ARGV[2];
$DECLARED_PREFIX=$ARGV[3] if(defined($ARGV[3]));


### ADD TRUNK
$TRUNK_VARS{'display'}="6";
$TRUNK_VARS{'extdisplay'}="";
$TRUNK_VARS{'action'}="addtrunk";
$TRUNK_VARS{'tech'}="sip";
$TRUNK_VARS{'outcid'}="";
$TRUNK_VARS{'maxchans'}="";
$TRUNK_VARS{'dialrules'}=$DECLARED_PREFIX."|.";
$TRUNK_VARS{'autopop'}="";
$TRUNK_VARS{'dialoutprefix'}="";
$TRUNK_VARS{'channelid'}="inphonex";
$TRUNK_VARS{'peerdetails'} ="allow=alow\n";
$TRUNK_VARS{'peerdetails'}.="context=from-internal\n";
$TRUNK_VARS{'peerdetails'}.="disallow=all\n";
$TRUNK_VARS{'peerdetails'}.="host=sip.inphonex.com\n";
$TRUNK_VARS{'peerdetails'}.="username=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'}.="user=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'}.="fromuser=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'}.="secret=$DECLARED_USERPASSWD\n";
$TRUNK_VARS{'peerdetails'}.="fromdomain=inphonex.com\n";
$TRUNK_VARS{'peerdetails'}.="dtmfmode=inband\n";
$TRUNK_VARS{'peerdetails'}.="nat=yes\n";
$TRUNK_VARS{'peerdetails'}.="qualify=yes\n";
$TRUNK_VARS{'peerdetails'}.="type=peer\n";
$TRUNK_VARS{'peerdetails'}.="canreinvite=no\n";
$TRUNK_VARS{'peerdetails'}.="insecure=very\n";

$TRUNK_VARS{'usercontext'}=$DECLARED_NUMBER;
$TRUNK_VARS{'userconfig'}=
$TRUNK_VARS{'userconfig'}.="context=from-ptsn\n";
$TRUNK_VARS{'userconfig'}.="secret=$DECLARED_USERPASSWD\n";;
$TRUNK_VARS{'userconfig'}.="type=user\n";

$TRUNK_VARS{'register'}="$DECLARED_USERNAME:$DECLARED_USERPASSWD\@sip.inphonex.com";

foreach my $var (keys %TRUNK_VARS)
{
	my $str = $TRUNK_VARS{$var};
	$str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
	$TRUNK_DATA	.=$var."=".$str."&";
}
`curl -d '$TRUNK_DATA' '$TRUNK_URL' > /dev/null`;


### ADD OUTGOING ROUTING
`curl '$OUT_URL&extdisplay=001-9_outside&action=delroute' > /tmp/curl.log`;
open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
my $OUT_ROUTE = "";
while(<PAGE>)
{
	chomp;
    if($_ =~ /[<]option value[=]\"([^\"]+)\"[>]SIP\/inphonex[<]\/option[>]/)
    {
        $OUT_ROUTE=$1;
    }
}
close(PAGE);
$OUT_VARS{'display'}="8";
$OUT_VARS{'extdisplay'}="";
$OUT_VARS{'action'}="addroute";
$OUT_VARS{'routename'}="inphonex";
$OUT_VARS{'routepass'}="";
$OUT_VARS{'dialpattern'}=$DECLARED_PREFIX."|.";
$OUT_VARS{'trunkpriority[0]'}=$OUT_ROUTE;
exit unless($OUT_ROUTE ne "");
foreach my $var (keys %OUT_VARS)
{
	my $str = $OUT_VARS{$var};
	$str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
	$OUT_DATA .=$var."=".$str."&";
}
`rm -f /tmp/curl.log ; curl -d '$OUT_DATA' '$OUT_URL' > /dev/null`;

### ADD INCOMING ROUTING
$IN_VARS{'display'}="7";
$IN_VARS{'extdisplay'}="";
$IN_VARS{'action'}="addIncoming";
$IN_VARS{'extension'}=$DECLARED_NUMBER;
$IN_VARS{'goto0'}="extension";
$IN_VARS{'extension0'}="10".$1 if($OUT_ROUTE=~/(\d)$/);
foreach my $var (keys %IN_VARS)
{
	my $str = $IN_VARS{$var};
	$str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
	$IN_DATA.=$var."=".$str."&";
}
`curl -d '$IN_DATA' '$IN_URL' > /dev/null`;

#run AMP's scripts to generate asterisk's config
`curl 'http://localhost/pluto-admin/amp/admin/config.php?display=6&clk_reload=true' > /dev/null`;
#create telecom defaults
`/usr/pluto/bin/create_telecom_defaults.pl`;
#reload asterisk
`asterisk -r -x reload`;
