#!/usr/bin/perl

########################
## Description: This is used for grabing remote config files the lirc
## sourceforge site and add them to the database. After grabing it will
## create a device template for every each remote grabbed.
## Writed by: Dan Harabagiu
## CopyRight: Pluto
########################

use DBI;

if($ARGV[0] eq "") {
	print "sync-remote-list <data-base>\n";
	exit(1);
}
if($ARGV[0] eq "x") {
	secret_step("pluto_dan");
	exit(1);
}
$DBASE = $ARGV[0];

print "Clearing Databses ";
system("cat first_step.sql | mysql -h 10.0.0.150");
print "[Done]\n";

print "Deleting main index.html file ";
system("rm -f index.html");
print "[Done]\n";

print "Getting the Directory List ";
system("wget -nv http://lirc.sourceforge.net/remotes/");
print "[Done]\n";

print "Reading File ";
open(FILE,"index.html");
@data = <FILE>;
close(FILE);
print "[Done]\n";

$db = DBI->connect("dbi:mysql:database=LIRC_Remote_Controls;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

print "Parsing File ";
foreach $line (@data) {
	chomp($line);
	@frag = split(/ /,$line);
	@fr = split(/\"/,$frag[4]);
	$dir = $fr[1];
	if($dir ne "" && $dir ne "/") {
		grab_dir("$dir");
	}
}
print "[Done]\n";
$db->disconnect();

print "Making Final Step ";
system("cat second_step.sql | mysql -h 10.0.0.150");
print "[Done]\n";

print "Adding other data for Device Templates ";
secret_step($DBASE);
print "[Done]\n";

sub grab_dir {
	$ldir = shift;
	print "Parsing $ldir\n";
	system("rm -f index.html");
	$line = "wget -nv http://lirc.sourceforge.net/remotes/".$ldir;
	system($line);
	open(FILE,"index.html");
	@ldata = <FILE>;
	close(FILE);
	foreach $lline (@ldata) {
		chomp($lline);
		@frag = split(/ /,$lline);
		if($frag[0] eq "<IMG") {
			if ($frag[7] ne "") {
				$file = $frag[7];
				@fr = split(/"/,$file);
				$file = $fr[1];
				print "$ldir$file\n";
				system("rm -f remote_config_file");
				$line = "wget -nv http://lirc.sourceforge.net/remotes/".$ldir.$file." -O remote_config_file";
				system($line);
				add_remote($ldir, $file, 0);
			}
		}
	}
}

sub add_remote {
	$man = shift;
	$remote = shift;
	$direct = shift;
	@fr = split(/\//,$man);
	$man = $fr[0];
	$info = "";
	open(FILE,"remote_config_file");
	@d = <FILE>;
	close(FILE);
	foreach $line (@d) {
		$info = $info.$line;
	}
	while($info =~ m/\'/) {
		$info =~ s/\'/`/;
	}
	$sql = "INSERT INTO LIRC_Remote_Controls (Manufacturer,Remote,Configuration) Values('$man','$remote','$info')";
	$st = $db->prepare($sql);
	$st->execute();
	$st->finish();
}

sub secret_step() {
	$dbname = shift;
	$dbb = DBI->connect("dbi:mysql:database=$dbname;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";
	$sql = "Select FK_DeviceTemplate FROM DeviceTemplate_DeviceData WHERE FK_DeviceData=59 AND IK_DeviceData<>''";
	$st = $dbb->prepare($sql);
	$st->execute();
	while($row = $st->fetchrow_hashref()) {
		$id_temp = $row->{'FK_DeviceTemplate'};
		$sql = "Select FK_DeviceTemplate FROM DeviceTemplate_DeviceData WHERE FK_DeviceData=62 AND IK_DeviceData<>'' AND FK_DeviceTemplate='$id_temp'";
		$st2 = $dbb->prepare($sql);
		$st2->execute();
		if($r = $st2->fetchrow_hashref()) {
			print "DeviceTemplate $id_temp have allready SerialPort\n";
		} else {
			$dbb->do("INSERT INTO DeviceTemplate_DeviceData(FK_DeviceTemplate,FK_DeviceData,IK_DeviceData,AllowedToModify,ShowInWizard) Values('$id_temp','62','1','1','1')");
		}
		$st2->finish();
		
		$sql = "Select FK_DeviceTemplate FROM DeviceTemplate_DeviceData WHERE FK_DeviceData=63 AND IK_DeviceData<>'' AND FK_DeviceTemplate='$id_temp'";
		$st2 = $dbb->prepare($sql);
		$st2->execute();
		if($r = $st2->fetchrow_hashref()) {
			print "DeviceTemplate $id_temp have allready LIRC Driver\n";
		} else {
			$dbb->do("INSERT INTO DeviceTemplate_DeviceData(FK_DeviceTemplate,FK_DeviceData,IK_DeviceData,AllowedToModify,ShowInWizard) Values('$id_temp','63','default','1','1')");
		}
		$st2->finish();
	}
	$dbb->disconnect();
}
