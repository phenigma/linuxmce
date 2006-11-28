#!/usr/bin/perl
# usage : flickr.pl D W H S 
# D - day search range
# W - picture width
# H - picture height
# S - search string

use Flickr::API;
use XML::Simple;
use Image::Magick;
use Data::Dumper;
use DBI;
use POSIX qw(ceil floor);
#use strict;
use vars qw($dest $xs $r @buff $child_count $minW $minH @fileList);

$child_count = 5;

$SIG{CHLD}='sig_child';

if (-e '/tmp/flickr_start'){
	my $line = `tail -n 1 /tmp/flickr_start`;
	chomp($line);
	if ($line eq 'Pictures downloaded'){
		my $currentTime = `date +%s`;
		my $lastRunTime = `stat --format=%Z /tmp/flickr_start`;
		my $nrSeconds = $currentTime - $lastRunTime;
		if ($nrSeconds > 18000){
			`touch /tmp/flickr_start`;
		} else {
			exit (1);
		}
	}
} else {
	`touch /tmp/flickr_start`;
}

# Config section. ###########################################

my $fKey  = '74e14e217ff6bfb670ccec36c0aa122b'; # the flickr key
$dest  = '/home/flickr'; # Destination folder
my $daycount = 0;
my ($tDays, $api, $response, $id, $buff, $IMGS, $search_string);

# Code body ##############################################
# my ($tDays,$minW,$minH,$api,$flag,$response,$xs,$r,$id,$buff,$IMGS,$ua,@buff,$tag,@tags,$finaldst,$fms,@out,$ffield,$search_string);
# my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);

# check script arguments 

if ( $#ARGV >= 0 ) {

	if ($ARGV[0]) {
		$tDays = $ARGV[0];
		print "Search days: $tDays";
	}
			
	if ($ARGV[1]) {
		$minW = $ARGV[1];
		print "\tMin width: $minW";
	}
					
	if ($ARGV[2]) {
		$minH = $ARGV[2];
		print "\tMin height: $minH\n";
	}

	if ($ARGV[3]) {
		$maxW = $ARGV[3];
		print "\tMin width: $minW";
	}
					
}

if ($#ARGV < 0) {
        $tDays = 5;
        $minW = 1000;
        $minH = 700;
	$search_string = '';
	#$search_string = 'red cars';
        print "[flickr.pl] Using default values - days: $tDays, width: $minW, height: $minH, no search string \n";
}
                                               
 
if (!-d $dest) {
	mkdir("$dest"); 
}

$api = new Flickr::API({'key' => $fKey});
my ($max_number, $picture_nr);
#$max_number = 100;
$max_number = getMaxNrFiles();
$picture_nr = 0;

if ($search_string){
        $response = $api->execute_method('flickr.photos.search',{
                'tags'=>"$search_string",
                'sort'=>'interestingness-desc'
        });
	if ($response->{success} == 1) {
		$xs = new XML::Simple;
		$r=XMLin($response->{_content});
		foreach $id (sort keys %{$r->{'photos'}->{'photo'}}) {
			last if ($picture_nr >= $max_number);
			my ($width, $height, $source, $download) = getPictureDimensions($id, $api);
			if ($download){
				#print ">>poza $id cu nr $picture_nr<<<\n";
				$IMGS->{$id}->{'secret'}=$r->{'photos'}->{'photo'}->{$id}->{'secret'};
				$IMGS->{$id}->{'width'}= $width;
				$IMGS->{$id}->{'height'}= $height;
				$IMGS->{$id}->{'source'}= $source;
				#$IMGS->{$id}->{'download'} = $download;
				$picture_nr++;
			}
		}
	}
	my $child_pid;
        foreach $id (keys %{$IMGS}) {
                $response = $api->execute_method('flickr.photos.getInfo',{'photo_id' => $id , 'secret' => $IMGS->{$id}->{'secret'}});
                if ($response->{success} == 1) {
			#print "get info for $id\n";
			$r=XMLin($response->{_content});
			$IMGS->{$id}->{'time'}=$r->{'photo'}->{'dates'}->{'posted'};
			$IMGS->{$id}->{'format'}=$r->{'photo'}->{'originalformat'};
			$IMGS->{$id}->{'username'}=$r->{'photo'}->{'owner'}->{'username'};
			$response= $api->execute_method('flickr.photos.getSizes',{'photo_id' => $id});
			if (!isFileOnDisk($id, $IMGS->{$id}, $search_string)){
				wait_child();
				if (!defined($child_pid = fork())) {
					die "cannot fork: $!";
				} elsif ($child_pid) {
					# I'm the parent
					next;
				} else {
					# I'm the child
					get_files($search_string, $IMGS->{$id}, $id);
					exit(0);
				} 
			}
                } else {
                        print STDERR "Skipped $id during an error.\n";
                        print STDERR "Error Message : $response->{error_message}\n";
                }
	}
	#get_files($search_string);
} else {
	my $child_pid;
	while ( ($daycount < $tDays) and ($picture_nr < $max_number)) {
		my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime (time()-($daycount*24*60*60));
		$year=1900+$year;
		$mon=$mon+1;
		$mon="0".$mon if($mon <= 9);
		$mday="0".$mday if ($mday <=9);

		print "[flickr.pl] Date: $year-$mon-$mday => Searching for files maching your criteria in gallery.\n";
		$response = $api->execute_method('flickr.interestingness.getList', {  
			'date' => "$year-$mon-$mday",
			'page' => '1',
			'per_page' => '50'
			});

		if ($response->{success} == 1) {
			$xs = new XML::Simple;
			$r=XMLin($response->{_content});
			foreach $id (sort keys %{$r->{'photos'}->{'photo'}}) {
				last if ($picture_nr >= $max_number);
				my ($width, $height, $source, $download) = getPictureDimensions($id, $api);
				if ($download){
					#print ">>poza $id cu nr $picture_nr<<<\n";
					$IMGS->{$id}->{'secret'}=$r->{'photos'}->{'photo'}->{$id}->{'secret'};
					$IMGS->{$id}->{'width'}= $width;
					$IMGS->{$id}->{'height'}= $height;
					$IMGS->{$id}->{'source'}= $source;
					#$IMGS->{$id}->{'download'} = $download;
					$picture_nr++;
				}
			}
		}
		$daycount++;
	}#while

	foreach $id (keys %{$IMGS}) {
		$response = $api->execute_method('flickr.photos.getInfo',{'photo_id' => $id , 'secret' => $IMGS->{$id}->{'secret'}});
		if ($response->{success} == 1) {
			$r=XMLin($response->{_content});
			$IMGS->{$id}->{'time'}=$r->{'photo'}->{'dates'}->{'posted'};
			$IMGS->{$id}->{'username'}=$r->{'photo'}->{'owner'}->{'username'};
			$IMGS->{$id}->{'format'}=$r->{'photo'}->{'originalformat'};
			#look for the file existence
			if (!isFileOnDisk($id, $IMGS->{$id},'')){
				if ($IMGS->{$id}->{'time'} > time()-($tDays*24*60*60)) {
					#print "Intru pt $id\n";
					wait_child();
					if (!defined($child_pid = fork())) {
						die "cannot fork: $!";
					} elsif ($child_pid) {
						# I'm the parent
						next;
					} else {
						# I'm the child
						get_files('', $IMGS->{$id}, $id);
						exit(0);
					} 
				}
			}
		} else {
			print STDERR "Skipped $id during an error.\n";
			print STDERR "Error Message : $response->{error_message}\n";
		}
	}
}

open DATA, ">/tmp/flickr_start" or die "can't open /tmp/flickr_start";
print DATA 'Pictures downloaded';
close(DATA);

#deleting old files;
delete_old();

sub get_files{
	my $pattern = shift;
	my $image = shift;
	my $buff = shift;
	
	my ($finaldst, $fms, @out, $ffield);

	if (($pattern ne '') and (!-d $dest."/".'tags')){
		if (!mkdir($dest."/".'tags')) {
			die "Cannot create ".$dest."/".'tags'."\n";
		}
	} 
	
	if ($pattern eq '') {
		my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($image->{'time'});
		$year=1900+$year;
		$mon=$mon+1;
		$mon="0".$mon if($mon <= 9);
		$mday="0".$mday if ($mday <=9);

		if (!-d $dest."/".$year) {
			if (!mkdir($dest."/".$year)) {
				die "Cannot create ".$dest."/".$year."\n";
			}
		}
	
		if (!-d $dest."/".$year."/".$mon) {
			if (!mkdir($dest."/".$year."/".$mon)) {
				die "Cannot create ".$dest."/".$year."/".$mon."\n";
			}
		}
		
		if (!-d $dest."/".$year."/".$mon."/".$mday) {
			if (!mkdir($dest."/".$year."/".$mon."/".$mday)) {
				die "Cannot create ".$dest."/".$year."/".$mon."/".$mday."\n";
			}
		}

		$finaldst = $dest."/".$year."/".$mon."/".$mday."/".$buff.".".$image->{'format'};
	} else {
		$finaldst = $dest."/".'tags'."/".$buff.".".$image->{'format'};
	}

	`wget $image->{'source'} -O $finaldst 1>/dev/null 2>/dev/null`;

	$xs=Image::Magick->new;
	$r=$xs->Read("$finaldst");
	warn "$r" if "$r";
	open TEST, ">>/home/alex/testflickr";
	print TEST "--Image: $finaldst \n";
	print TEST "-------------------\n";
	print TEST "Old image width: $image->{'width'} and height: $image->{'height'}\n";
	if (($image->{'width'} > 1024 || $image->{'height'} > 1024)||
	    ($image->{'width'} > 1024 && $image->{'height'} > 1024)) {
		if($image->{'width'} > $image->{'height'}){
			$image->{'height'} = floor(($image->{'height'}/$image->{'width'})*1024);
			$image->{'height'} = 768 if($image->{'height'} > 768);
			$image->{'width'} = 1024;
		} 
		elsif($image->{'height'} > $image->{'width'}){
			$image->{'width'} = floor(($image->{'width'}/$image->{'height'})*1024);
			$image->{'width'} = 768 if($image->{'width'} > 768);
			$image->{'height'} = 1024;
		}
		elsif($image->{'height'} == $image->{'width'}){
			$image->{'width'} = 768;
			$image->{'height'} = 768;
		}
	}
	print TEST "New image width: $image->{'width'} and height: $image->{'height'}\n";
	print TEST "-------------------\n";
	close(TEST);
	$r=$xs->Scale(width=>$image->{'width'}, 
		      height=>$image->{'height'});
	$r = $xs->Annotate( font=>'/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf', 
				pointsize=>20, 
				fill=>'red', 
				geometry=>'0,0,+20,+40',
				text=>chr(169).$image->{'username'});
	warn "$r" if "$r";
	$r=$xs->Write($finaldst);
	print "[flickr.pl] Writing file $finaldst.\n";
				
	my $fms = qx | /usr/pluto/bin/MessageSend dcerouter -targetType template -o 0 2 1 819 13 "$finaldst" |; 
				
	## If the router is not available for the moment
	while ( $fms =~ m/Cannot communicate with router/ ) {
		printf "Waiting for router to come up";
		sleep 10;
		$fms = qx | /usr/pluto/bin/MessageSend dcerouter -targetType template -o 0 2 1 819 13 "$finaldst" |;
	}
				
	$fms =~ s/\n//g;
	@out = split (/:/, $fms);
	$ffield = $out[2];
	warn "$r" if "$r";
					
	# second message send
	#	print "Fire-ing second messagesend event\n";
	qx | /usr/pluto/bin/MessageSend dcerouter -targetType template -o 0 2 1 391 145 "$ffield" 122 30 |;
}

sub delete_old {
	# Remove old files
	#my $totalFiles = `find /home/flickr/ -name '*.jpg'  | wc -l`;
	my $listOfFiles = `find /home/flickr/ -name '*.jpg'`;
	my @arrayOfFiles = split (/\n/, $listOfFiles);
	
	foreach ( @arrayOfFiles ) {
		if (!isFileInList($_)){
			`rm -f $_`;
		}
	}
}

sub isFileInList {
	my $file = shift;

	foreach (@fileList) {
		return 1 if ($_ eq $file);
	}

	return 0;
}

sub getMaxNrFiles {
	my $dbh = DBI->connect('dbi:mysql:pluto_main');
	my $sth = $dbh->prepare("
		SELECT 
			IK_DeviceData 
		FROM 
			Device_DeviceData 
			INNER JOIN Device ON Device_DeviceData.FK_Device = Device.PK_Device
		WHERE 
			Device_DeviceData.FK_DeviceData=177 
			AND 
			Device.FK_DeviceTemplate = 12
	");
	$sth->execute || die "Sql Error";
	my $row = $sth->fetchrow_hashref;
	my $noOfFilesToKeep = $row->{IK_DeviceData};
	return $noOfFilesToKeep;
}

sub isFileOnDisk {
	my $id = shift;
	my $image = shift;
	my $pattern = shift;
	my $finaldst;

	if ($pattern eq '') 
	{
		my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($image->{'time'});
		$year=1900+$year;
		$mon=$mon+1;
		$mon="0".$mon if($mon <= 9);
		$mday="0".$mday if ($mday <=9);
		$finaldst = $dest."/".$year."/".$mon."/".$mday."/".$id.".".$image->{'format'};
	} else {
		$finaldst = $dest."/".'tags'."/".$buff.".".$image->{'format'};
	}
	push (@fileList, $finaldst);
	#print "destinatia: $finaldst \n";
	if (!-e $finaldst){
		return 0;
	}
	return 1;
}

sub getPictureDimensions {
	my $id = shift;
	my $api = shift;
	my ($buff, $width, $height, $source, $download);
	my $response= $api->execute_method('flickr.photos.getSizes',{'photo_id' => $id});
	if ($response->{success} == 1) {
		my $r=XMLin($response->{_content});
		foreach $buff (@{$r->{'sizes'}->{'size'}}) {
			if ($buff->{'width'} >= $minW && $buff->{'height'} >= $minH) {
				#last if ($buff->{'width'} > $maxW || $buff->{'height'} >= $maxH);
				$width = $buff->{'width'};
				$height = $buff->{'height'};
				$source = $buff->{'source'};
				$download = 1;
			}
		}
		return ($width, $height, $source, $download);
	}
	return (0,0,0,0)
}

sub sig_child {

	my $waitedpid;
	while (($waitedpid = waitpid(-1,WNOHANG)) > 0) {
		print "reaped $waitedpid\n";
	}
										       
	$child_count++;
	
	$SIG{CHLD} = \&sig_child; # loathe sysV
}

sub wait_child {
	while ($child_count <=0){
		sleep(1);
	}
	$child_count--;
}
