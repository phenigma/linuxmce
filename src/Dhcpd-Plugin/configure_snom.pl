#!/usr/bin/perl

use DBI;
#use strict;
use Socket;

$line;
foreach $line (@data) {
  ($option, $eq, $value) = split(/ /,$line);
  if($option eq "MySqlHost") {
    $DBHOST=$value;
    $FILEHOST=$value;
  } elsif ($option eq "MySqlUser") {
    $DBUSER=$value;
  } elsif ($option eq "MySqlPassword") {
    $DBPASSWD=$value;
  } elsif ($option eq "PK_Device") {
    $PK_Device=$value;
  }
}

$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL";

$sql = "select IPaddress from Device where Description='CORE'";
$st = $db_handle->prepare($sql);
$st->execute();
if($row = $st->fetchrow_hashref()) {
  $shost = $row->{'IPaddress'};
} else {
  $shost = "192.168.1.1";
}
  
if($ARGV[0] ne "-d") {
  print "<USAGE-1>\n$_[0] -d <Device Template> -i <IP> -m <mac address>\n";
  exit(-1);
} else {
  $Device_ID = $ARGV[1];
}

if($ARGV[2] ne "-i") {
  print "<UASGE-2>\n$_[0] -d <Device Template> -i <IP> -m <mac address>\n";
  exit(-1);
} else {
  $ip = $ARGV[3];
}

if($ARGV[4] ne "-m") {
  print "<USAGE-3>\n$_[0] -d <Device Template> -i <IP> -m <mac address>\n";
  exit(-1);
} else {
  $mac = $ARGV[5];
}

$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL";
$sql = "select FK_Device from Device_DeviceData where FK_Device=$Device_ID and FK_DeviceData=31 and IK_DeviceData<>''";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
while($row_ref = $statement->fetchrow_hashref())
{
  exit(0);
}

sleep(10);
$sql = "select IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
$ext = 0;
while($row_ref = $statement->fetchrow_hashref())
{
  $extention = $row_ref->{IK_DeviceData};
  if($extention > $ext) {
    $ext = $extention;
  }
}
if($ext == 0) {
  $ext = 4310
} else {
  $ext = $ext+1;
}

$sql = "update Device_DeviceData SET IK_DeviceData='$ext' WHERE FK_Device='$Device_ID' AND FK_DeviceData='31'";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";

$db_handle->disconnect();
system("/usr/pluto/bin/SyncronizeAsterisk.sh"); #Run igor sync config

open(FILE,">/var/www/snom$ext.htm");
print FILE "<html>\n<pre>\n";
print FILE "user_mailbox1: $ext\n";
print FILE "user_name1: $ext\n";
print FILE "user_realname1: $ext\n";
print FILE "user_host1: $shost\n";
print FILE "</pre>\n<html>\n";
close(FILE);

$host = $ip;
$port = 80;

$proto = getprotobyname('tcp');
$iaddr = inet_aton($host);
$paddr = sockaddr_in($port, $iaddr);

socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
$var = 0;
connect(SOCKET, $paddr) or $var=1;
while($var == 1) {
  connect(SOCKET, $paddr) or $var=2;
  if($var == 2) {
    $var = 1;
  } else {
    $var = 0;
  }
}
close SOCKET;

system("curl -d \"update_policy=ask_for_update&setting_server=http%3A%2F%2F$shost%2Fsnom$ext.htm&dns_domain=1control.com&dns_server1=$shost&dns_server2=aaa&http_user=&http_pass=&http_proxy=&http_port=&lcserver1=&lcserver2=&vlan=&SETTINGS=Save\" http://$ip/set_net_adv_en.htm > /dev/null");
system("curl http://$ip/set_base_en.htm?reboot=Reboot > /dev/null");
