#!/usr/bin/perl

use DBI;

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
				add_remote($ldir, $file);
			}
		}
	}
}

sub add_remote {
	$man = shift;
	$remote = shift;
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
