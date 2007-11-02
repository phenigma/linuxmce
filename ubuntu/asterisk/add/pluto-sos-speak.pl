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

my $db_handle;
my $sql;
my $statement;
my $row;
my @data;
my $secpluginid;
my $telpluginid;

#read pluto config
open(CONF,"/etc/pluto.conf");
@data=<CONF>;
close(CONF);
foreach $line (@data)
{
    my ($option, $eq, $value) = split(/ /,$line);
	chomp($value) if(defined($value));
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
$secpluginid=$row->{PK_Device};
$statement->finish();

$sql = "select PK_Device from Device where FK_DeviceTemplate=34;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO TELECOM PLUGIN\n";
    exit(1);
}
$telpluginid=$row->{PK_Device};
$statement->finish();

$sql = "select FK_Device, IK_DeviceData from Device_DeviceData where FK_DeviceData='124';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
my $selected_devices="";
while($row = $statement->fetchrow_hashref())
{
	if($row->{IK_DeviceData} eq '1')
	{
		$selected_devices .= "," if(length($selected_devices)>0);
		$selected_devices .= $row->{FK_Device};
	}
}
$statement->finish();

unless(length($selected_devices)>0)
{
    print STDERR "NO DEVICES TO SPEAK IN THE HOUSE\n";
    exit(1);
}


$sql = "select IK_DeviceData from Device_DeviceData where (FK_Device in ($selected_devices)) and (FK_DeviceData='31');";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
@data=();
while($row = $statement->fetchrow_hashref())
{
	push(@data,$row->{IK_DeviceData});
}

for(my $i=0;defined($data[$i]);$i++)
{
	my $phone=$data[$i];
	print STDERR "Will call ".$phone."\n";
	`/usr/pluto/bin/MessageSend localhost -targetType device $secpluginid $telpluginid 1 414 75 $phone 81 plutosecurity 83 997`;
}

$statement->finish();
$db_handle->disconnect();
