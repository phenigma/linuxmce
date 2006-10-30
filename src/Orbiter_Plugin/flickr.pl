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
#use strict;
use vars qw($dest $xs $r @buff $child_count);

$child_count = 5;

$SIG{CHLD}='sig_child';

# Config section. ###########################################

my $fKey  = '74e14e217ff6bfb670ccec36c0aa122b'; # the flickr key
$dest  = '/home/flickr'; # Destination folder
my $daycount = 0;
my ($tDays, $minW, $minH, $api, $response, $id, $buff, $IMGS, $search_string);

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
	}
	
	if ($ARGV[3]) {
		$search_string = $ARGV[3];
		print "\tSearch string:$search_string\n";
	}

if ($#ARGV < 0) {
        $tDays = 5;
        $minW = 1200;
        $minH = 700;
	$search_string = '';
	#$search_string = 'red cars';
        print "[flickr.pl] Using default values - days: $tDays, width: $minW, height: $minH, no search string \n";
        }
                                               
 
if (!-d $dest) {
	mkdir("$dest"); 
}

#deleting old files;
#delete_old();

$api = new Flickr::API({'key' => $fKey});

if ($search_string){
	
        $response = $api->execute_method('flickr.photos.search',{
                'tags'=>"$search_string",
                'sort'=>'interestingness-desc'
        });
        $xs = new XML::Simple;
        $r=XMLin($response->{_content});

        foreach $id (sort keys %{$r->{'photos'}->{'photo'}}) {
                 $IMGS->{$id}->{'secret'}=$r->{'photos'}->{'photo'}->{$id}->{'secret'};
		#print ">>>>$IMGS->{$id}->{'secret'}<<<<\n";
        }
	my $child_pid;
        foreach $id (keys %{$IMGS}) {
                $response = $api->execute_method('flickr.photos.getInfo',{'photo_id' => $id , 'secret' => $IMGS->{$id}->{'secret'}});
                if ($response->{success} == 1) {
			print "get info for $id\n";
			$r=XMLin($response->{_content});
			$IMGS->{$id}->{'time'}=$r->{'photo'}->{'dates'}->{'posted'};
			$IMGS->{$id}->{'format'}=$r->{'photo'}->{'originalformat'};
			$IMGS->{$id}->{'username'}=$r->{'photo'}->{'owner'}->{'username'};
			$IMGS->{$id}->{'download'} = 0;
			$response= $api->execute_method('flickr.photos.getSizes',{'photo_id' => $id});
			if ($response->{success} == 1) {
				#print "get size for $id\n";
				$r=XMLin($response->{_content});
				foreach $buff (@{$r->{'sizes'}->{'size'}}) {
					if ($buff->{'width'} >= $minW && $buff->{'height'} >= $minH) {
						$IMGS->{$id}->{'width'}= $buff->{'width'};
						$IMGS->{$id}->{'height'}= $buff->{'height'};
						$IMGS->{$id}->{'source'}= $buff->{'source'};
						$IMGS->{$id}->{'download'} = 1;
						print "$IMGS->{$id}->{'width'} x $IMGS->{$id}->{'height'} for $id\n";
					}
				}
				if ($IMGS->{$id}->{'download'} == 1){
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

                } else {
                        print STDERR "Skipped $id during an error.\n";
                        print STDERR "Error Message : $response->{error_message}\n";
                }
	}
	#get_files($search_string);
} else {
	my $child_pid;
	while ( $daycount < $tDays ) {
	
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
				$IMGS->{$id}->{'secret'}=$r->{'photos'}->{'photo'}->{$id}->{'secret'};
			}
			foreach $id (keys %{$IMGS}) {
				$response = $api->execute_method('flickr.photos.getInfo',{'photo_id' => $id , 'secret' => $IMGS->{$id}->{'secret'}});
				if ($response->{success} == 1) {
					$r=XMLin($response->{_content});
					$IMGS->{$id}->{'time'}=$r->{'photo'}->{'dates'}->{'posted'};
					#$IMGS->{$id}->{'title'}=$r->{'photo'}->{'title'};
					#$IMGS->{$id}->{'description'}=$r->{'photo'}->{'description'};
					#$IMGS->{$id}->{'taken'}=$r->{'photo'}->{'dates'}->{'taken'};
					#$IMGS->{$id}->{'tags'}="";
					$IMGS->{$id}->{'username'}=$r->{'photo'}->{'owner'}->{'username'};
					#$IMGS->{$id}->{'realname'}=$r->{'photo'}->{'owner'}->{'realname'};
					#$IMGS->{$id}->{'location'}=$r->{'photo'}->{'owner'}->{'location'};
					$IMGS->{$id}->{'format'}=$r->{'photo'}->{'originalformat'};
					$IMGS->{$id}->{'download'} = 0;
					if ($IMGS->{$id}->{'time'} > time()-($tDays*24*60*60)) {
						$response= $api->execute_method('flickr.photos.getSizes',{'photo_id' => $id});
						if ($response->{success} == 1) {
							$r=XMLin($response->{_content});
							foreach $buff (@{$r->{'sizes'}->{'size'}}) {
								if ($buff->{'width'} >= $minW && $buff->{'height'} >= $minH) {
									$IMGS->{$id}->{'width'}= $buff->{'width'};
									$IMGS->{$id}->{'height'}= $buff->{'height'};
									$IMGS->{$id}->{'source'}= $buff->{'source'};
									$IMGS->{$id}->{'download'} = 1;
								}
							}
							if ($IMGS->{$id}->{'download'} == 1){
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
						} else {
							print STDERR "Skipped $id during an error.\n";
							print STDERR "Error Message : $response->{error_message}\n";
						}
					} else {
						$IMGS->{$id}->{'download'} = 0;
					}
				} else {
					print STDERR "Skipped $id during an error.\n";
					print STDERR "Error Message : $response->{error_message}\n";
				}
			}
		}#if
		$daycount++;
	}#while
}

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

sub delete_old{
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
	
	# Remove old files
	my $totalFiles = `find /home/flickr/ -name '*.jpg'  | wc -l`;
	my $extraFiles = $noOfFilesToKeep - $totalFiles;
	my $listToRemove = `find /home/flickr/ -name '*.jpg' | tail $extraFiles`;
	my @arrayToRemove = split (/\n/, $listToRemove);
	
	foreach ( @arrayToRemove ) {
		`rm -f $_`;
	}
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
