#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=pluto_main;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

if($ARGV[1] eq "" && $ARGV[0] ne "m" && $ARGV[0] ne "t") {
	print "Usage: MakeChangeLog.pl <operation> <svn log file>\n";
	exit(1);
}

if($ARGV[0] eq "t") {
	trim($ARGV[1]);
	exit(1);
} elsif($ARGV[0] eq "m") {
	$input_file = $ARGV[1];
	print "Parsing file $input_file\n";
} else {
	print "Unkown operation (t=for trimming the log file, m=to modify the database with the log values)\n";
	exit(1);
}


open(FILE,$input_file);
@data = <FILE>;
close(FILE);

print "Adding";
foreach $line (@data) {
	chomp($line);
	($flag,@rest) = split(/\:/,$line);
	if($flag eq "cl") {
		$c = 0;
		foreach $l (@rest) {
			$c = $c + 1;
		}
		$size = $c - 1;
		for($i = 0 ; $i < $size ; $i++) {
			addchange($rest[$i],"* ".$rest[$size]."<br>");
#			sleep(2);
		}
	}
}
print "\n";

$db->disconnect();

sub addchange {
	$project = shift;
	$change = shift;
	
	$change =~ tr/'/`/;
#	$change = "* ".$change."<br>";
	$sql = "SELECT max(PK_Version) FROM Version";
	$st = $db->prepare($sql);
	$st->execute();
	print ".";
	if($row = $st->fetchrow_hashref()) {
		$build = $row->{'max(PK_Version)'};
	}
	$st->finish();
	
	if($project eq "orbiter" || $project eq "Orbiter" || $project eq "mobileorbiter" || $project eq "MobileOrbiter") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto Orbiter'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "myth" || $project eq "Myth") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto MythTV Plugin'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "bluetoothdongle" || $project eq "BluetoothDongle" || $project eq "Bluetooth" || $project eq "bluetoothDongle" || $project eq "bluetooth") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto Bluetooth Dongle'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	}  elsif($project eq "sql2cpp") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto sql2cpp'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "sqlcvs" || $project eq "sqlCVS") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto sqlCVS'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	}  elsif($project eq "PlutoVIP" || $project eq "plutovip") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto PlutoVIP'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	}  elsif($project eq "Xine" || $project eq "XinePlugin" || $project eq "xine" || $project eq "xineplugin") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto Xine Plugin'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "DiskDrive" || $project eq "diskdrive") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto Disk Drive'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "AppServer" || $project eq "appserver") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto App Server'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "Mozilla" || $project eq "mozilla") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto Mozilla Plugin'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "GC100" || $project eq "gc100") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto GC100'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "DCE" || $project eq "dce") {
		$sql = "SELECT PK_Package FROM Package WHERE HomePage is NOT NULL AND Description='Pluto DCE Router'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$pk = $row->{'PK_Package'};
		}
		$st->finish();
		$sql = "SELECT * FROM Package_Version WHERE FK_Package='$pk' AND FK_Version='$build'";
		$st = $db->prepare($sql);
		$st->execute();
		if($row = $st->fetchrow_hashref()) {
			$oldchange = $row->{'Comments'};
			$change = $oldchange.$change;
			$sql = "UPDATE Package_Version SET Comments='$change' WHERE FK_Package='$pk' AND FK_Version='$build'";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();	
		} else {
			$sql = "INSERT INTO Package_Version (FK_Package,FK_Version,Comments) Values('$pk','$build','$change')";
			$st2 = $db->prepare($sql);
			$st2->execute();
			$st2->finish();
		}
		$st->finish();
	} elsif($project eq "pluto" || $project eq "Pluto") {
		$localc = $change;
		addchange("xine",$localc);
		addchange("orbiter",$localc);
		addchange("diskdrive",$localc);
		addchange("bluetoothdongle",$localc);
		addchange("appserver",$localc);
		addchange("mozilla",$localc);
		addchange("myth",$localc);
		addchange("dce",$localc);
		addchange("plutovip",$localc);
		addchange("gc100",$localc);
	}
}

sub trim {
	$file = shift;
	open(FL,$file);
	@data = <FL>;
	close(FL);
	open(FL,">new.log");
	foreach $line (@data) {
		chomp($line);
		if($line ne "") {
			@splits = split(/\-/,$line);
			if($splits[0] ne "") {
				if($line =~ m/\|/) {
					@splits = split(/\|/,$line);
					print FL "$splits[1]|";
				} else {
					print FL "$line\n";
				}
			}
		}
	}
	close(FL);
	open(FL,"new.log");
	@data = <FL>;
	close(FL);
	open(FL,">new.log");
	foreach $line (@data) {
		chomp($line);
		@splits = split(/\|/,$line);
		@splits2 = split(/\:/,$splits[1]);
		if($splits2[0] eq "cl") {
			print FL "$line\n";
		}
	}
	close(FL);
}