#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

my $DEVICE_ID="";
my $DEVICE_IP="";
my $DEVICE_EXT="";

if($#ARGV < 3 || $ARGV[0] ne "-d" || $ARGV[2] ne "-r")
{
    print "<USAGE>\n$0 -d <Device ID> -r <IP>\n";
    exit(-1);
}
else
{
    $DEVICE_ID = $ARGV[1];
    $DEVICE_IP = $ARGV[3];
}

my $CONF_HOST="localhost";
my $CONF_USER="root";
my $CONF_PASSWD="";

open(CONF,"/etc/pluto.conf") or die "Could not open pluto config";
while(<CONF>)
{
	chomp;
	my ($option, $eq, $value) = split(/ /,$_);
	if($option eq "MySqlHost")
	{
		$CONF_HOST=$value;
	}
	elsif ($option eq "MySqlUser")
	{
		$CONF_USER=$value;
	}
	elsif ($option eq "MySqlPassword")
	{
		$CONF_PASSWD=$value;
	}
}
close(CONF);

my $DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
my $DB_SQL = "select IK_DeviceData from Device_DeviceData where FK_Device=$DEVICE_ID and FK_DeviceData=31";
my $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
my $DB_ROW = $DB_STATEMENT->fetchrow_hashref();
$DEVICE_EXT = $DB_ROW->{'IK_DeviceData'} if ($DB_ROW);
$DB_STATEMENT->finish();
$DB_PL_HANDLE->disconnect();

print "Running /usr/pluto/bin/simplephone $DEVICE_EXT $DEVICE_EXT $DEVICE_IP\n";
while(1)
{
	`/usr/pluto/bin/simplephone $DEVICE_EXT $DEVICE_EXT $DEVICE_IP`;
	sleep(5);
}
