#!/usr/bin/perl
# usage : flickr.pl X Y Z
# X - day search range
# Y - picture width
# Z - picture height

use Flickr::API;
use XML::Simple;
use LWP::UserAgent;
use Image::Magick;
use Data::Dumper;
use strict;

# Config section. ###########################################

my $fKey  = '74e14e217ff6bfb670ccec36c0aa122b'; # the flickr key
my $tmp   = '/tmp'; # tmp folder
my $dest  = '/home/flickr'; # Destination folder
my $daycount = 0;

# Code body ##############################################
my ($tDays,$minW,$minH,$api,$flag,$response,$xs,$r,$id,$buff,$IMGS,$ua,@buff,$tag,@tags,$finaldst,$fms,@out,$ffield);
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);

# check script arguments 

if ( $#ARGV >= 0 ) {

if ($ARGV[0] != "" ) {
        $tDays = $ARGV[0];
        print "Search days: $tDays";
        }
                
if ($ARGV[1] != "" ) {
        $minW = $ARGV[1];
        print "\tMin width: $minW";
        }
                                
if ($ARGV[2] != "" ) {
        $minH = $ARGV[2];
        print "\tMin height: $minH\n";
        }
}

if ($#ARGV < 0) {
        $tDays = 5;
        $minW = 1200;
        $minH = 700;
        print "[flickr.pl] Using default values - days: $tDays, width: $minW, height: $minH \n";
        }
                                               
## 
if (!-d $dest) {
	mkdir("$dest"); }

$api = new Flickr::API({'key' => $fKey});

while ( $daycount < $tDays ) {

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime (time()-($daycount*24*60*60));

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
	$IMGS->{$id}->{'title'}=$r->{'photo'}->{'title'};
	$IMGS->{$id}->{'description'}=$r->{'photo'}->{'description'};
	$IMGS->{$id}->{'taken'}=$r->{'photo'}->{'dates'}->{'taken'};
	$IMGS->{$id}->{'tags'}="";
	$IMGS->{$id}->{'username'}=$r->{'photo'}->{'owner'}->{'username'};
	$IMGS->{$id}->{'realname'}=$r->{'photo'}->{'owner'}->{'realname'};
	$IMGS->{$id}->{'location'}=$r->{'photo'}->{'owner'}->{'location'};
	$IMGS->{$id}->{'format'}=$r->{'photo'}->{'originalformat'};
	foreach $buff (sort keys %{$r->{'photo'}->{'tags'}->{'tag'}} ) {
	   if ($buff=~ /\d+\-\d+\-\d+/) {
		$IMGS->{$id}->{'tags'}.=$r->{'photo'}->{'tags'}->{'tag'}->{$buff}->{'content'}.",";
	   } else { # Single tag
		$IMGS->{$id}->{'tags'}=$r->{'photo'}->{'tags'}->{'tag'}->{'content'};
	   }
	}

	$IMGS->{$id}->{'tags'}=~ s/\,$//;
        $IMGS->{$id}->{'download'} = 0;
#       $flag=0;
#        foreach $tag (@tags) {
#          if ($tag =~ /$IMGS->{$id}->{'tags'}/i) {
#                $flag=1;          }        
#	}
#       if ($IMGS->{$id}->{'time'} > time()-($tDays*24*60*60) && $flag == 1) {
	if ($IMGS->{$id}->{'time'} > time()-($tDays*24*60*60)) {
	  $response= $api->execute_method('flickr.photos.getSizes',{'photo_id' => $id});
	  if ($response->{success} == 1) {
	    $r=XMLin($response->{_content});
	    foreach $buff (@{$r->{'sizes'}->{'size'}}) {
#		if ($buff->{'width'} => $minW && $buff->{'height'} => $minH  && $IMGS->{$id}->{'tags'}=~ /$tag/ ){
		if ($buff->{'width'} => $minW && $buff->{'height'} => $minH) {
		    $IMGS->{$id}->{'width'}= $buff->{'width'};
		    $IMGS->{$id}->{'height'}= $buff->{'height'};
		    $IMGS->{$id}->{'source'}= $buff->{'source'};
		    $IMGS->{$id}->{'download'} = 1;
	  
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

# Step 2 - get files.
$ua = LWP::UserAgent->new;
$ua->timeout(5);

foreach $buff (keys %{$IMGS}) {
  if ($IMGS->{$buff}->{'download'} == 1) {

#
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($IMGS->{$buff}->{'time'});

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

#
    $response = $ua->get($IMGS->{$buff}->{'source'});
    if ($response->is_success) {
	if (!open(TMP,">$tmp/$buff.$IMGS->{$buff}->{'format'}")) {
	   print "[flickr.pl] Cannot write $tmp/$buff.$IMGS->{$buff}->{'format'}.\n";
	} else {
#	    print "Writing $tmp/$buff.$IMGS->{$buff}->{'format'} !\n";
            print TMP $response->content;
	    close(TMP);
# touch lock file
        if (!open(TMP2,">$tmp/$buff.$IMGS->{$buff}->{'format'}.lock")) {
		print "[flickr.pl] Cannot write $tmp/$buff.$IMGS->{$buff}->{'format'}.lock !\n";
        } else {
            print TMP2 " ";
            close(TMP2); }
# end lock file
	    $xs=Image::Magick->new;
	    $r=$xs->Read("$tmp/$buff.$IMGS->{$buff}->{'format'}");
	    warn "$r" if "$r";
	    $r = $xs->Annotate( font=>'/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf', 
			   	pointsize=>20, 
			   	fill=>'green', 
			   	geometry=>'0,0,+4,+14',
			   	text=>$IMGS->{$buff}->{'username'});
	    warn "$r" if "$r";
	$finaldst = $dest."/".$year."/".$mon."/".$mday."/".$buff.".".$IMGS->{$buff}->{'format'};
	if ( -e "$finaldst" )
	{ 
	  print "[flickr.pl] Filename $finaldst exists on the disk.\n";
	} else {
	  $r=$xs->Write($dest."/".$year."/".$mon."/".$mday."/".$buff.".".$IMGS->{$buff}->{'format'});
	  print "[flickr.pl] Writing file $finaldst.\n";
	}
# first messagesend	    
#	print "Fire-ing first messagesend event for $finaldst\n";
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
	    unlink("$tmp/$buff.$IMGS->{$buff}->{'format'}");
	    unlink("$tmp/$buff.$IMGS->{$buff}->{'format'}.lock");
# second message send
#	print "Fire-ing second messagesend event\n";
	qx | /usr/pluto/bin/MessageSend dcerouter -targetType template -o 0 2 1 391 145 "$ffield" 122 30 |;
	}
    } else {
            print STDERR $response->status_line."\n";
    }
  }
}


} else {
  print "[flickr.pl] Date: $year-$mon-$mday => Error Message : $response->{error_message} !\n";
#  print STDERR "Cannot connect to flickr! => ";
#  print STDERR "Success : $response->{success}\n";
#  print STDERR "Error : $response->{error_code}\n";
#  print STDERR "Error Message : $response->{error_message} !\n";
#  print STDERR "Tree : $response->{_content}\n";	

  } $daycount++;
}

