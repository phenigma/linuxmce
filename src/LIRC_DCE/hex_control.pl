#!/usr/bin/perl

##################################
## Description: This script is reading the config files for the name of the buttons,
## and then search them into the mappings database as tokens, and then write into
## the devicetemplate_devicedata the devicedata called Mapping in the format
## hex_code|message<end of line terminator known as \n> that will be used by the
## LIRC wrapper
## Writed by: Dan Harabagiu
## CopyRight: Pluto
##################################

use DBI;

if($ARGV[0] eq "") {
	print "Usage: hex_control.pl <database> [-d <template>]\n";
	exit(1);
}
$dbase = $ARGV[0];

$db = DBI->connect("dbi:mysql:database=$dbase;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";
$db2 = DBI->connect("dbi:mysql:database=LIRC_Remote_Controls;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

if($ARGV[1] eq "" && $ARGV[2] eq "") {
	$sql = "select * from DeviceTemplate_DeviceData where FK_DeviceData='59'";
} elsif($ARGV[1] eq "-d" && $ARGV[2] ne "") {
	$sql = "select * from DeviceTemplate_DeviceData where FK_DeviceData='59' AND FK_DeviceTemplate='$ARGV[2]'";
} else {
	print "Usage: hex_control.pl <database> [-d <template>]\n";
	exit(1);
}
$state = $db->prepare($sql);
$state->execute();
while($row = $state->fetchrow_hashref()) {
	$config_file = $row->{'IK_DeviceData'};
	@data = split(/\n/,$config_file);
	$template = $row->{'FK_DeviceTemplate'};
	$work_it = 0;
	if($config_file ne "") {
#		$sql = "delete from DeviceTemplate_DeviceData where FK_DeviceTemplate='$template' and FK_DeviceData='60'";
#		$st = $db->prepare($sql);
#		$st->execute();
#		$st->finish();
		$db->do("delete from DeviceTemplate_DeviceData where FK_DeviceTemplate='$template' and FK_DeviceData='60'");
		$errflg = 0;
		$hexmap = "";
		$count = 0;
		foreach $line (@data) {
			@parts = split(/\n/,$line);
			$line = $parts[0];
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
							$count = $count + 1;
#							print "Maping for [$key]\n";
						} else {
#							print "Unknown Mapping for key $key\n";
							$errflg = 1;;
						}
					}
				}
			}
		}
		print "Adding hexmap for device template: $template rows $count\n";
#		$sqla = "insert into DeviceTemplate_DeviceData(IK_DeviceData,FK_DeviceData,FK_DeviceTemplate) Values('$hexmap','60','$template')";
#		$sta = $db->prepare($sqla);
#		$sta->execute();
#		$sta->finish();
#		if($errflg == 1) {
#			exit(1);
#		}
		$db->do("insert into DeviceTemplate_DeviceData(IK_DeviceData,FK_DeviceData,FK_DeviceTemplate) Values('$hexmap','60','$template')");
	}
}

$db->disconnect();
$db2->disconnect();

sub hex_to_token {
	$k = shift;
	$h = shift;
	$sql = "select * from Mappings";
	$st = $db2->prepare($sql);
	$st->execute();
	while($rowu = $st->fetchrow_hashref()) {
		$comment = $rowu->{'Comment'};
		$token = $rowu->{'Tokens'};
		$message = $rowu->{'Message'};
		@tokens = split(/\n/,$token);
		foreach $el (@tokens) {
			if($el eq $k) {
				$st->finish();
				return $message;
			}
		}
	}
	$st->finish();
	return;
}

sub get_util {
	$in = shift;
	$c = 0;
	$l = "";
	chomp($in);
	if($in =~ m/\n/ || $in =~ m/\t/ || $in =~ m/\r/ || $in =~ m/\f/ || $in =~ m/\b/ ) {
		$in =~ s/\n//;
		$in =~ s/\t//;
		$in =~ s/\r//;
		$in =~ s/\f//;
		$in =~ s/\b//;		
	}
	@prel = split(/ /,$in);
	foreach $x (@prel) {
		if($x ne "") {
			$good[$c] = $x;
			$c = $c + 1;
		}
	}
	return @good;
}

