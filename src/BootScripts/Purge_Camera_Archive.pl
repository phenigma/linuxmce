#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=pluto_main;host=localhost;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

$devices = "";

$sql = "SELECT PK_DeviceCategory FROM DeviceCategory WHERE Description='Surveillance Cameras'";
$st = $db->prepare($sql) or die "Error in code";
$st->execute() or die "Error on execute";
while($local_row = $st->fetchrow_hashref()) {
	$devicecat = $local_row->{'PK_DeviceCategory'};
	print "We have found the category $devicecat\n";
	$sql = "SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory='$devicecat'";
	$st2 = $db->prepare($sql) or die "Error in code";
	$st2->execute() or die "Error on execute";
	while($local = $st2->fetchrow_hashref()) {
		$devicetemplate = $local->{'PK_DeviceTemplate'};
		print "We have found device template $devicetemplate\n";
		$sql = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$devicetemplate'";
		$st3 = $db->prepare($sql) or die "Error in code";
		$st3->execute() or die "Error on execute";
		while($row = $st3->fetchrow_hashref()) {
			print "We have one\n";
			$deviceid = $row->{'PK_Device'};
			if($devices eq "") {
				$devices = $deviceid;
			} else {
				$devices = $devices.",".$deviceid;
			}
		}
	}
}

$db->disconnect();

print "$devices\n";
