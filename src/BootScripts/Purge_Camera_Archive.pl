#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=pluto_main;host=localhost;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

$devices = "";
$datas = "";

$sql = "SELECT PK_DeviceCategory FROM DeviceCategory WHERE Description='Surveillance Cameras'";
$st = $db->prepare($sql) or die "Error in code";
$st->execute() or die "Error on execute";
while($local_row = $st->fetchrow_hashref()) {
	$devicecat = $local_row->{'PK_DeviceCategory'};
	$sql = "SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory='$devicecat'";
	$st2 = $db->prepare($sql) or die "Error in code";
	$st2->execute() or die "Error on execute";
	while($local = $st2->fetchrow_hashref()) {
		$devicetemplate = $local->{'PK_DeviceTemplate'};
		$sql = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$devicetemplate'";
		$st3 = $db->prepare($sql) or die "Error in code";
		$st3->execute() or die "Error on execute";
		while($row = $st3->fetchrow_hashref()) {
			$deviceid = $row->{'PK_Device'};
			$sql = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$deviceid' AND FK_DeviceData='54'";
			$st4 = $db->prepare($sql) or die "Error in code";
			$st4->execute() or die "Error on execute";
			if($rowl = $st4->fetchrow_hashref()) {
				$data = $rowl->{'IK_DeviceData'};
				if($devices eq "") {
					$devices = $deviceid.",";
				} else {
					$devices = $devices.$deviceid.",";
				}
				if($datas eq "") {
					$datas = $data.",";
				} else {
					$datas = $datas.$data.",";
				}
			}
			$st4->finish();
		}
		$st3->finish();
	}
	$st2->finish();
}
$st->finish();

$db->disconnect();

if($devices eq "") {
	print "We have found no camera devices so we will not purge. Exiting...\n";
	exit(-1);
}

if($datas eq "") {
	print "We have found no data concernig purge option. Exiting...\n";
	exit(-1);
}

print "$devices\n";
print "$datas\n";

opendir(DIR, /var/log) || die "can' T to opendir #: $!";
@dots = grep {/^\./ && - F" # _ "} readdir(DIR); 
to closedir DIR;

foreach $line (@dots) {
	print "[$line]\n";
}