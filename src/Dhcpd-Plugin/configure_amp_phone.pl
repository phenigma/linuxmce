#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

#declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;

my @data;
my $line;

my $DBHOST;
my $FILEHOST;
my $DBUSER;
my $DBPASSWD;
my $PK_Device;

my $db_handle;
my $sql;
my $statement;
my $shost;
my $row;

my $ext;

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
    $Device_MAC = $ARGV[5]; #???useless???
}

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
$sql = "select FK_Device from Device_DeviceData where FK_Device=$Device_ID and FK_DeviceData=31 and IK_DeviceData<>''";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
if($row = $statement->fetchrow_hashref())
{
    #exit if this device is already configured
    exit(0);
}

#find next available extension
$sql = "select IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$ext = 0;
while($row = $statement->fetchrow_hashref())
{
    my $extention = $row->{IK_DeviceData};
    if($extention > $ext)
    {
        $ext = $extention;
    }
}
if($ext == 0)
{
    $ext = 3314;
}
else
{
    $ext = $ext+1;
}

#update Device_DeviceData table with extension
$sql = "update Device_DeviceData SET IK_DeviceData='$ext' WHERE FK_Device='$Device_ID' AND FK_DeviceData='31'";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$db_handle->disconnect();

#update Device table with IPAddress
$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL";
$sql = "update Device set IPaddress='$Device_IP' where PK_Device='$Device_ID'";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$db_handle->disconnect();

#add data in asterisk databases (used by AMP) assuming it's a SIP phone.
#looks like we can't do many inserts in one statement :(
$db_handle = DBI->connect("dbi:mysql:database=asterisk;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL";
$sql = "insert into sip values ($ext, 'account', '$ext', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'accountcode', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'secret', '$ext', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'canreinvite', 'no', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'context', 'from-internal', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'dtmfmode', 'rfc2833', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'host', '$Device_IP', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'type', 'friend',0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'mailbox', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'username', '$ext', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'nat', 'no', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'port', '5060', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'qualify', 'no', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'callgroup', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'pickupgroup', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'disallow', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'allow', '', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'record_in', 'On-Demand', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'record_out', 'On-Demand', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$sql = "insert into sip values ($ext, 'callerid', '$ext <$ext>', 0);";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$db_handle->disconnect();

#run AMP's scripts to generate asterisk's config
`/var/www/pluto-admin/amp/admin/retrieve_sip_conf_from_mysql.pl`;

#reload asterisk
`asterisk -r -x reload`;
