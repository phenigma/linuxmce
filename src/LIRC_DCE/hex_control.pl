#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=pluto_main;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";
$db2 = DBI->connect("dbi:mysql:database=LIRC_Remote_Controls;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

$sql = "select * from DeviceTemplate_DeviceData where FK_DeviceData='59'";
$state = $db->prepare($sql);
$state->execute();
while($row = $state->fetchrow_hashref()) {
	$config_file = $row->{'IK_DeviceData'};
	@data = split(/\n/,$config_file);
	$template = $row->{'FK_DeviceTemplate'};
	$work_it = 0;
	if($config_file ne "") {
		$sql = "delete from DeviceTemplate_DeviceData where FK_DeviceTemplate='$template' and FK_DeviceData='60'";
		$st = $db->prepare($sql);
		$st->execute();
		$st->finish();
		$errflg = 0;
		foreach $line (@data) {
			@parts = get_util($line);
			if($parts[0] eq "begin" && $parts[1] eq "codes") {
				$work_it = 1;
			} elsif($parts[0] eq "end" && $parts[1] eq "codes") {
				$work_it = 0;
			}
			if($work_it == 1) {
				$key = $parts[0];
				$hex = $parts[1];
				if($key ne "begin") {
					if($hex =~ m/0x00/) {
						$mesg = hex_to_token($key,$hex);
						if($mesg ne "") {
							$hexmap = $hexmap.$hex."|".$mesg."\n";
						} else {
							print "Unknown Mapping for key $key\n";
							$errflg = 1;;
						}
					}
				}
			}
		}
		print "Adding hexmap for device template: $template\n";
		$sql = "insert into DeviceTemplate_DeviceData(IK_DeviceData,FK_DeviceData,FK_DeviceTemplate) Values('$hexmap','60','$template')";
		$st = $db->prepare($sql);
		$st->execute();
		$st->finish();
		if($errflg == 1) {
			exit(1);
		}
	}
}

$db->disconnect();
$db2->disconnect();

sub hex_to_token {
	$k = shift;
	$h = shift;
	$sql = "select * from Mappings";
	$state = $db2->prepare($sql);
	$state->execute();
	while($row = $state->fetchrow_hashref()) {
		$comment = $row->{'Comment'};
		$token = $row->{'Tokens'};
		$message = $row->{'Message'};
		@tokens = split(/\n/,$token);
		foreach $el (@tokens) {
			if($el eq $k) {
				$state->finish();
				return $message;
			}
		}
	}
	$state->finish();
	return;
}

sub get_util {
	$in = shift;
	@prel = split(/ /,$in);
	$c = 0;
	foreach $x (@prel) {
		if($x ne "") {
			$good[$c] = $x;
			$c = $c + 1;
		}
	}
	return @good;
}

