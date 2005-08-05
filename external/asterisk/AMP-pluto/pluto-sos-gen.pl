#!/usr/bin/perl -w

use strict;
use diagnostics;
use DBI;

#$|=1;

my $line;
my $DBHOST;
my $FILEHOST;
my $DBUSER;
my $DBPASSWD;
my $PK_Device;

my $db_handle;
my $sql;
my $statement;
my $row;
my @data;

my $mode=0;

#read pluto config
open(CONF,"/etc/pluto.conf");
@data=<CONF>;
close(CONF);
foreach $line (@data)
{
    my ($option, $eq, $value) = split(/ /,$line);
    if($option eq "MySqlHost")
    {
        $DBHOST=$value;
        $FILEHOST=$value;
    }
    elsif ($option eq "MySqlUser")
    {
        $DBUSER=$value;
    }
    elsif ($option eq "MySqlPassword")
    {
        $DBPASSWD=$value;
    }
    elsif ($option eq "PK_Device")
    {
        $PK_Device=$value;
    }
}

#connect to pluto_main database
$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL";
$sql = "select PK_Device from Device where FK_DeviceTemplate=33;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO SECURITY PLUGIN\n";
    exit(1);
}

$sql = "select IK_DeviceData from Device_DeviceData where FK_Device=\'".$row->{PK_Device}."\' and FK_DeviceData='96';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO DEVICEDATA FOR SECURITY PLUGIN\n";
    exit(1);
}

@data = split(/[,]/,$row->{IK_DeviceData});

for(my $i=0;defined($data[$i]);$i+=2)
{
	my $j=int(($i+2)/2);
	print STDERR "[".$data[$i]."]=".$data[$i+1]."\n";
	`flite -t "To call the $data[$i] press $j" -o /tmp/pluto-security-sos-option$j.wav`;
	`/usr/bin/sox /tmp/pluto-security-sos-option$j.wav -r 8000 -c 1 /tmp/pluto-security-sos-option$j.gsm resample -ql`;
}

