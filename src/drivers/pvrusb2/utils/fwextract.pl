#!/usr/bin/perl

use strict;
use Getopt::Long;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use IO::File;
use IO::Dir;

my $vers='$Id: fwextract.pl 1386 2006-10-25 02:21:01Z isely $';

#
#  Copyright (C) 2005 Mike Isely <isely@pobox.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

# This script automates the extraction of firmware from the Hauppauge
# Windows driver files.  This script can also be used to assist in
# locating newer firmware in newer driver files.

# To extract firmware, just run the script, ensuring that the Windows
# driver files have been expanded into a "win_driver" subdirectory.
# (Alternatively, just name the relevant directory on the command.)
# The script will locate the firmware and extract it into which will
# be ready to use for upload into the hardware.  This script uses a
# table to recognize firmware files; it is able to seek and recognize
# multiple driver versions (depending on what's in the table).

# To teach the script about newer driver versions, you must first find
# an alternate way to manually extract the firmware files.  The way to
# accomplish that is going to depend heavily on the firmware in
# question and is outside the scope of this script.  But once you have
# the two firmware files manually extracted (and named appropriately),
# just run this script with the --search option.  This will cause it
# to use those firmware files as search keys into the Windows drivers.
# It will search through the driver files and find correct offsets and
# sizes for where the firmware has been "hidden".  It will then print
# out a Perl snippet that you can append to the $contentInfo structure
# further below.  Send that snippet back to me (Mike Isely
# <isely@pobox.com>) and I'll put it back into the master copy of this
# script.  Then others will be able to easily extract the newer
# firmware without having to go through the manual pain you will have
# just completed :-)

# -------------------------------------------------------------------------

# Default values for the Windows driver file directory and the names
# for the two firmware files to be extracted / searched.  Command line
# options may be used to change these (look at the end of this file -
# we're just using Getopt to do the dirty work for that).

my @driverDirPath;

# -------------------------------------------------------------------------

# Full default names of each file we can extract; as more file types
# are defined, list them here.
my $fileKeys = {
    'fw1-29xxx-01' => 'v4l-pvrusb2-29xxx-01.fw',
    'fw1-24xxx-01' => 'v4l-pvrusb2-24xxx-01.fw',
    'fw2' => 'v4l-cx2341x-enc.fw',
    'fw3' => 'v4l-cx25840.fw',
};

# -------------------------------------------------------------------------

# Model description of hardware related to given firmware key.  Some
# firmware images are very specific to a particular make & model of
# hardware.
my $hardwareIds = {
    'fw1-29xxx-01' => "Hauppauge PVR USB2 Model 29xxx",
    'fw1-24xxx-01' => "Hauppauge PVR USB2 Model 24xxx",
};

# -------------------------------------------------------------------------

# The table below tells the extraction process where to find the
# firmware data.  What the extraction process does is blindly MD5 sum
# each windows driver file and try to look that key up here.  (This is
# an extremely fast operation).  If the lookup succeeds, then we know
# we've located a file with something interesting in it.  What's
# "interesting" is described by the table entry, keyed by "fw1",
# "fw2", etc.  For each key, the nested data describes the offset and
# size of the data to extract and also includes another MD5 sum to
# ensure that the extraction was truly successful.

# Note that using MD5 sums effectively as the search key, we are
# completely independent of the driver file name and driver package
# version in which it was found.  To make the extractor work with
# additional driver versions, one need only add entries to this table
# - the extractor will do the rest.

# To add entries to this table you'll still have to hand edit this
# script, however you don't have to guess at the data.  Run this
# script with the --search option and manually-extracted versions of
# the firmware files nearby and the script will execute an exhaustive
# / fuzzy search to locate the embedded firmware.  Then it will print
# the resulting table entry that you must add below.

my $contentInfo = {
  bf1cc4da051b1db04091562c86c691ff => {
    comment => "pvrusb2_23_31351",
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "987584",
	size => "8192"
      },
      fw2 => {
	md5_hex => "ab75947ef1b086e26f9b08e628baa02e",
	offset => "201152",
	size => "262144"
      }
    }
  },
  "02e4934b8525db1fee074f57a0c27304" => {
    comment => 'CDROM 2.5.22329 (credit: Eliette Mathey <elmat52@yahoo.fr>)',
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "1065472",
	size => "8192"
      },
      fw2 => {
	md5_hex => "79c5daf4cde87036c834a314b4929fb1",
	offset => "279040",
	size => "262144"
      }
    }
  },
  b100615d9497d205b065a985bc9d73c3 => {
    comment => "pvrusb2_inf_26c (related to pvrusb2_26_23097)",
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "1072928",
	size => "8192"
      },
      fw2 => {
	md5_hex => "79c5daf4cde87036c834a314b4929fb1",
	offset => "286496",
	size => "262144"
      }
    }
  },
  "6576e6cc206a4b5469345e7ebe578abf" => {
    comment => "pvrusb2_inf (related to pvrusb2_26_23055)",
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "1072992",
	size => "8192"
      },
      fw2 => {
	md5_hex => "79c5daf4cde87036c834a314b4929fb1",
	offset => "286560",
	size => "262144"
      }
    }
  },
  "9c5fc27cd3a5c2307c911cad92c95689" => {
    comment => "pvrusb2_27_23178",
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "1073216",
	size => "8192"
      },
      fw2 => {
        md5_hex => "79c5daf4cde87036c834a314b4929fb1",
        offset => "286784",
	size => "262144"
      }
    }
  },
  "9cebcd0f6d47e0ae1676d667d7fb54b0" => {
    comment => 'DriverVer=03/04/2005,2.2.73.23063 (credit: Fedor Bezrukov <Fedor.Bezrukov@epfl.ch>)',
    contents => {
      'fw1-29xxx-01' => {
	md5_hex => "3b7288416144467369ed2b5b10cf931d",
	offset => "1068320",
	size => "8192"
      },
      fw2 => {
	md5_hex => "79c5daf4cde87036c834a314b4929fb1",
	offset => "281888",
	size => "262144"
      }
    }
  },
  "a86445d3763546b04e22110a31d3704f" => {
    comment => 'Shipped with first 24xxx models (credit: Richard Vieira <rever75@gmail.com>)',
    contents => {
      'fw1-24xxx-01' => {
        md5_hex => "34d213394328adf78e2fc9f1411691b0",
        offset => "1414752",
        size => "8192"
      },
      fw2 => {
        md5_hex => "d85cb08382395390dc95ac6ebc2205f9",
        offset => "292448",
        size => "262144"
      },
      fw3 => {
        md5_hex => "c5749da3eb141e87634ff1e2c1e6f390",
        offset => "1422944",
        size => "13837"
      }
    }
  },
  "1b570b23375b817498007ccf085c27c3" => {
    comment => 'pvrusb2_2273_24023.zip (also on CDs included with 24xxx models in April 2006)',
    contents => {
      "fw1-24xxx-01" => {
        md5_hex => "34d213394328adf78e2fc9f1411691b0",
        offset => "1415264",
        size => "8192"
      },
      fw2 => {
        md5_hex => "d85cb08382395390dc95ac6ebc2205f9",
        offset => "292960",
        size => "262144"
      },
      fw3 => {
        md5_hex => "c5749da3eb141e87634ff1e2c1e6f390",
        offset => "1423456",
        size => "13837"
      }
    }
  },
  "eedb017a8cc2e8dc9825d1a1b3e4f034" => {
    comment => 'pvrusb2_inf_24117.zip (for both 24xxx and 29xxx devices)',
    contents => {
      "fw1-24xxx-01" => {
        md5_hex => "ffed594f0edea15de8c19cfd13bc4adf",
        offset => "1417088",
        size => "8192"
      },
      "fw1-29xxx-01" => {
        md5_hex => "3b7288416144467369ed2b5b10cf931d",
        offset => "1081216",
        size => "8192"
      },
      fw2 => {
        md5_hex => "d85cb08382395390dc95ac6ebc2205f9",
        offset => "294784",
        size => "262144"
      },
      fw3 => {
        md5_hex => "5c22fd758ea64a1b8df7e2eb59da0842",
        offset => "1425280",
        size => "12559"
      }
    }
  }
};


# -------------------------------------------------------------------------

# This is all utility code that I've ripped from other code that I've
# written...

# This transforms the string such that it can be safely printed
# between double-quotes and then eval'ed back in at a later time to
# produce the original string value:
#  - Escape double quotes
#  - Escape dollar sign
#  - Escape '@' sign
#  - Escape newline
#  - Escape backslash
#  - Hexify any non-printing character
sub perlQuoteStr {
    my $str = shift;

    $str =~ s/([\$\@"\\])/\\$1/g;
    $str =~ s/([\n])/\\n/g;
    $str =~ s{([\x00-\x1f\x80-\xff])}{sprintf('\\x%02x',ord($1))}ge;

    return $str;
}

# Return string quoted or bare if it's safe to represent the string as
# a bareword.
sub perlSafeEmitBareStr {
    my $str = shift;
    # Two tests here: First only strings composed entirely of letters,
    # digits and the underscore - with the first character being a
    # letter - passes.  Then fail any string which might be mistaken
    # for a "v-string" (yuck).  If we fail, we quote the whole thing.
    # Otherwise we'll just return the bareword.
    if (($str =~ /^[a-zA-Z]\w+$/) && !($str =~ /^v\d+$/)) {
	return $str;
    }
    return '"' . perlQuoteStr($str) . '"';
}

# Return string quoted or bare if it is just a single digit.
sub perlSafeEmitStr {
    my $str = shift;
    if ($str =~ /^\d$/) {
	# Single-digit case - easily handled
	return $str;
    }
    return '"' . perlQuoteStr($str) . '"';
}

sub structureWriteToHandle2 {
    my $fh = shift;
    my $var = shift;
    my $prefix = shift;
    my ($v1,$v2);

    if (! defined $var) {
	print $fh 'undef';
	return;
    }
    $v2 = ref $var;
    if ($v2 eq "") {
	print $fh perlSafeEmitStr($var);
	return;
    }
    if ($v2 eq 'ARRAY') {
	my $oldPrefix = $prefix;
	$prefix = $prefix . "  ";
	print $fh "[\n";
	for ($v1 = 0; $v1 < scalar(@$var); $v1++) {
	    if ($v1) {
		print $fh ",\n";
	    }
	    print $fh $prefix;
	    structureWriteToHandle2($fh,$var->[$v1],$prefix);
	}
	print $fh "\n";
	print $fh $oldPrefix;
	print $fh "]";
	return;
    }
    if ($v2 eq 'HASH') {
	my $key;
	my $oldPrefix = $prefix;
	$prefix = $prefix . "  ";
	print $fh "{\n";
	$v1 = 0;
	foreach $key (sort keys %$var) {
	    if ($v1) {
		print $fh ",\n";
	    }
	    print $fh $prefix;
	    print $fh perlSafeEmitBareStr($key) . ' => ';
	    structureWriteToHandle2($fh,$var->{$key},$prefix);
	    $v1++;
	}
	print $fh "\n";
	print $fh $oldPrefix;
	print $fh "}";
	return;
    }

    return;
}

sub structureWriteToHandle {
    my $fh = shift;
    my $var = shift;
    my $prefix = shift;

    structureWriteToHandle2($fh,$var,$prefix);
    print $fh "\n";
}


# -------------------------------------------------------------------------

# Suck a file into a data structure so that we can manipulate it.
sub fetchFileData {
    my $fname = shift;
    my $fh = new IO::File;
    my $dontDieFl = shift;
    my $cnt;

    my $info = {
	name => $fname,
	data => '',
    };

    if (! $fh->open("<$fname")) {
	return undef if ($dontDieFl);
	die "Failed to open $fname for reading";
    }

    while ($fh->read($info->{data},16384,length $info->{data})) { }

    $info->{size} = length $info->{data};
    $info->{md5_hex} = md5_hex($info->{data});
    return $info;
};

# Print a <value> / <value> string to the terminal.  We do this so
# that the user doesn't think that the script is hung.   This is a
# helper for searchForFuzzySequence.
sub printProgress {
    my $ctxt = shift;
    my $idx = shift;
    my $str;

    print "\r" if (defined $ctxt->{dirty});
    $ctxt->{dirty} = !0;
    $str = sprintf "%7d / %7d",$idx,$ctxt->{cnt};
    print "$str";
}

# Clean up after printing a <value> / <value> string to the terminal.
# We do this so that the user doesn't think that the script is hung.
# This is a helper for searchForFuzzySequence.
sub clearProgress {
    my $ctxt = shift;
    my $idx = shift;
    return if (! defined $ctxt->{dirty});
    printProgress($ctxt,$idx);
    print "\n";
}

# Search for a needle in a haystack.  Pass in the haystack and the
# needle and try to find where in the haystack it is a residing.
# Return value is the offset within the haystack where it's been found
# or undef.  This implementation is a slow "fuzzy" search.  We
# tolerate up to 1% of the data bytes being incorrect, which allows us
# to use slightly changed firmware data sucked from the hardware as a
# search key (the 8051 firmware in fact seems to always come back with
# a single modified byte).
sub searchForFuzzySequence {
    my $needleInfo = shift;
    my $haystackInfo = shift;
    my ($idx1,$idx2);
    my $errCnt;
    my $errMaxCnt;
    my $needleCheckSize;
    my $needleRef = \$needleInfo->{data};
    my $haystackRef = \$haystackInfo->{data};
    my $needleSize = length $$needleRef;
    my $haystackSize = length $$haystackRef;
    my $pctxt = { };

    return undef if ($haystackSize < $needleSize);

    my $range = $haystackSize - $needleSize;

    $errMaxCnt = int($needleSize * 0.002);
    $needleCheckSize = int($needleSize * 0.75);

#    print "Fuzzy check info:\n";
#    print "Checking $needleCheckSize out of $needleSize bytes\n";
#    print "Will tolerate up to $errMaxCnt errors\n";

    $pctxt->{cnt} = $range;
    for ($idx1 = 0; $idx1 < $range; $idx1++) {
	if ($idx1 && !($idx1 % 3000)) {
	    printProgress($pctxt,$idx1);
	}
	$errCnt = 0;
	for ($idx2 = 0; $idx2 < $needleCheckSize; $idx2++) {
	    next if (substr($$haystackRef,$idx1+$idx2,1) eq
		     substr($$needleRef,$idx2,1));
	    $errCnt++;
	    last if ($errCnt > $errMaxCnt);
	}
	next if ($idx2 < $needleCheckSize);
	clearProgress($pctxt,$idx1);
	return $idx1;
    }
    clearProgress($pctxt,$idx1);
    return undef;
};

# Search for a needle in a haystack.  Pass in the haystack and the
# needle and try to find where in the haystack it is a residing.
# Return value is the offset within the haystack where it's been found
# or undef.  This is a fast, exact search.
sub searchForSequence {
    my $needleInfo = shift;
    my $haystackInfo = shift;
    my $idx;
    my $needleSize = length $needleInfo->{data};
    my $haystackSize = length $haystackInfo->{data};

    return undef if ($haystackSize < $needleSize);

    my $range = $haystackSize - $needleSize;
    my $ssize = 4;

    # Pure paranoia
    $ssize = $needleSize if ($ssize > $needleSize);

    for ($idx = 0; $idx < $range; $idx++) {
	# Just check the first 4 bytes initially since this will be fast
	next if (substr($haystackInfo->{data},$idx,$ssize) ne
		 substr($needleInfo->{data},0,$ssize));
	# If we get here, then it's probably a match, but let's check
	# the entire data block now.
	next if (substr($haystackInfo->{data},$idx,$needleSize) ne
		 $needleInfo->{data});
	return $idx;
    }
    return undef;
};

# Helper function for recording info in a data structure about
# firmware we've found.
sub noteResult {
    my $result = shift;
    my $fileInfo = shift;
    my $fwInfo = shift;

    print "Found $fwInfo->{type} in $fileInfo->{name}" .
	" at offset $fwInfo->{offset}\n";
    $result->{$fileInfo->{md5_hex}}->{contents}->{$fwInfo->{type}} = {
	offset => $fwInfo->{offset},
	size => $fwInfo->{size},
	md5_hex => md5_hex(substr($fileInfo->{data},
				  $fwInfo->{offset},
				  $fwInfo->{size})),
    };
}

# Helper function to search a single file for firmware.  This only
# does an exact search.
sub searchASingleFile {
    my $fileData = shift;
    my $fwInfo = shift;
    my $offset;
    my $key;

    foreach $key (sort keys %$fwInfo) {
	$offset = searchForSequence($fwInfo->{$key},$fileData);
	if (defined $offset) {
	    $fwInfo->{$key}->{offset} = $offset;
	}
    }
}

# Quick and dirty function to grab the contents of a directory.
sub getDirEntries {
    my $dirName = shift;
    my $dirh;
    my @entries;

    $dirh = new IO::Dir $dirName;

    return undef if (! defined $dirh);

    @entries = sort $dirh->read();

    return \@entries;
}

# Generate a full list of files, including all nested subdirectories.
# Pass in a starting point or a reference to an array of places to
# check.
sub getRecursiveDirEntries {
    my $dirName= shift;
    my @entries;
    my @searchList;
    my $subList;
    my $subKey;
    my $key;
    if (ref $dirName eq 'ARRAY') {
	push @searchList,@$dirName;
    } else {
	push @searchList,$dirName;
    }

    for (;;) {
	$key = shift @searchList;
	last if (!defined $key);
	if (-d $key) {
	    $subList = getDirEntries($key);
	    foreach $subKey (@$subList) {
		next if ($subKey eq '..');
		next if ($subKey eq '.');
		if ($key ne '.') {
		    push @searchList,"$key/$subKey";
		} else {
		    push @searchList,"$subKey";
		}
	    }
	    next;
	}
	if (-f $key) {
	    push @entries,$key;
	}
    }
    return \@entries;
}

sub getRecursiveDirEntriesOrFail {
    my $searchList = shift;
    my $files = getRecursiveDirEntries($searchList);

    if (scalar(@$files) < 1) {
	die "Failed to locate any files to scan anywhere starting with @$searchList.\n";
    }

    return $files;
}

sub doSearchDriver {
    my $comment = shift;
    my $fwInfo = { };
    my $fileInfo;
    my ($v1,$v2,$v3);
    my $key;
    my $item;
    my $doneFl;
    my $result = {
    };

    # First read in the already expected two firmware image files.
    print "Searching for the following embedded file data:\n";
    foreach $key (sort keys %$fileKeys) {
	$item = fetchFileData($fileKeys->{$key},1);
	next if (!defined $item);
	print "  $key (file $fileKeys->{$key})\n";
	$item->{type} = $key;
	$fwInfo->{$key} = $item;
    }

    # Grab set of files we're going to scan
    my $files = getRecursiveDirEntriesOrFail(\@driverDirPath);

    print "Performing exact search in @driverDirPath\n";

    # Look in each file for our expected firmware, using an exact
    # search.  This is fastest.
    foreach $v1 (@$files) {
 	$fileInfo = fetchFileData($v1);
	print "Searching $fileInfo->{name} (size=$fileInfo->{size})\n";
	searchASingleFile($fileInfo,$fwInfo);
	$doneFl = 1;
	foreach $key (sort keys %$fwInfo) {
	    if (! exists $fwInfo->{$key}->{offset}) {
		$doneFl = undef;
		next;
	    }
	    next if ($fwInfo->{$key}->{got});
	    # Found something
	    $result->{$fileInfo->{md5_hex}}->{comment} = $comment;
	    noteResult($result,$fileInfo,$fwInfo->{$key});
	    $fwInfo->{$key}->{got} = 1;
	}
	return $result if ($doneFl);
    }

    print "Performing fuzzy search in @driverDirPath\n";

    # First look in files where we had previously found something
    foreach $v1 (@$files) {
	$fileInfo = fetchFileData($v1);
	next if (!exists $result->{$fileInfo->{md5_hex}});
	print "Fuzzy searching $fileInfo->{name}" .
	    " (size=$fileInfo->{size})\n";
	$doneFl = 1;
	foreach $key (sort keys %$fwInfo) {
	    next if ($fwInfo->{$key}->{got});
	    print "--> Searching for $key ($fileKeys->{$key})\n";
	    $v3 = searchForFuzzySequence($fwInfo->{$key},$fileInfo);
	    if (! defined $v3) {
		$doneFl = undef;
		next;
	    }
	    $fwInfo->{$key}->{offset} = $v3;
	    noteResult($result,$fileInfo,$fwInfo->{$key});
	    $fwInfo->{$key}->{got} = 1;
	}
	return $result if ($doneFl);
    }

    # Now look in files where we had not previously found something
    foreach $v1 (@$files) {
	$fileInfo = fetchFileData($v1);
	next if (exists $result->{$fileInfo->{md5_hex}});
	print "Fuzzy searching $fileInfo->{name}" .
	    " (size=$fileInfo->{size})\n";
	$doneFl = 1;
	foreach $key (sort keys %$fwInfo) {
	    next if ($fwInfo->{$key}->{got});
	    print "--> Searching for $key ($fileKeys->{$key})\n";
	    $v3 = searchForFuzzySequence($fwInfo->{$key},$fileInfo);
	    if (! defined $v3) {
		$doneFl = undef;
		next;
	    }
	    $fwInfo->{$key}->{offset} = $v3;
	    $result->{$fileInfo->{md5_hex}}->{comment} = $comment;
	    noteResult($result,$fileInfo,$fwInfo->{$key});
	    $fwInfo->{$key}->{got} = 1;
	}
	return $result if ($doneFl);
    }
    return $result;
}

# This logic assumes that the two firmware files already exist and
# attempts to locate where the data has been embedded within the windows
# driver files.  All files are searched, and if we find the embedded
# data, we'll print out a Perl data structure that can be inserted
# into this program's $contentInfo structure which will tell this
# program how to later extract the firmware files from those same places.
sub searchDriver {
    my $comment = shift;
    my $result = doSearchDriver($comment);

    # Print out results...
    if (scalar(keys %$result)) {
	print 'Please add this fragment info the $contentInfo' .
	    ' structure of this program:';
	print "\n";
	structureWriteToHandle(\*STDOUT,$result);
	print <<EOF;
I also strongly encourage you to send me (Mike Isely <isely\@pobox.com>) the
above snippet so that I can update the master copy of this script and
others can benefit from your effort.
EOF
	return 0;
    } else {
	print "Failed to find firmware data within Windows driver files\n";
	return 1;
    }
}

# Extract firmware from parent file
sub doSubExtract {
    my $fileInfo = shift;
    my $fwContentInfo = shift;
    my $fwKey = shift;
    my $fwInfo;
    my $fc;

    return undef if (! defined $fwContentInfo->{contents}->{$fwKey});

    $fc = $fwContentInfo->{contents}->{$fwKey};

    $fwInfo = {
	key => $fwKey,
	parent => $fileInfo->{name},
	comment => $fwContentInfo->{comment},
	device => $fwContentInfo->{device},
	md5_hex => $fc->{md5_hex},
	size => $fc->{size},
	offset => $fc->{offset},
	data => substr($fileInfo->{data},
		       $fc->{offset},
		       $fc->{size}),
    };
    if (exists $hardwareIds->{$fwKey}) {
	$fwInfo->{hardwareId} = $hardwareIds->{$fwKey};
    }

    return $fwInfo;
}

# Write firmware to a file and report about what we've written.
sub doSubWriteback {
    my $fwInfo = shift;
    my $fh = new IO::File;
    my $md5_info;
    my $retCode = undef;
    my $fname = $fwInfo->{name};

    if (!defined $fwInfo) {
	print STDERR "Error: Failed to locate data for writing $fname\n";
	return $retCode;
    }

    die "Failed to open $fname for writing" unless $fh->open(">$fname");

    print $fh $fwInfo->{data};

    $fh->close();

    print "Wrote $fwInfo->{key} (file $fname):\n";
    print "  Size $fwInfo->{size} bytes\n";
    if (defined $fwInfo->{comment}) {
	print "  Driver package $fwInfo->{comment}\n";
    }
    if (defined $fwInfo->{hardwareId}) {
	print "  Intended hardware: \"$fwInfo->{hardwareId}\"\n";
    }
    print "  Source $fwInfo->{parent} offset $fwInfo->{offset}\n";
    $md5_info = md5_hex($fwInfo->{data});
    if ($md5_info ne $fwInfo->{md5_hex}) {
	print "  Warning: MD5 mismatch on extracted content for $fname\n";
	print "  MD5 Expected: $fwInfo->{md5_hex}\n";
	print "  MD5      Got: $md5_info\n";
    } else {
	print "  MD5 verified ($md5_info)\n";
	$retCode = 1;
    }

    return $retCode;
}

# This scans a directory of Windows driver files and attempts to
# recognize files which contain firmware.  For those it finds, it
# executes the extraction process.
sub doExtract {
    my $fileInfo;
    my $fwInfo = { };
    my ($v1,$v2,$v3);
    my ($rc1,$rc2);
    my $key;
    my $doneFl = undef;
    my %devNotice;
    my $devCount;
    my @devs;

    # Grab set of files we're going to scan
    my $files = getRecursiveDirEntriesOrFail(\@driverDirPath);

    # Look in each file for our expected firmware
    foreach $v1 (@$files) {
	$fileInfo = fetchFileData($v1);

	$fileInfo->{md5_hex} = md5_hex($fileInfo->{data});
	next if (! exists $contentInfo->{$fileInfo->{md5_hex}});

	print "Found firmware data in $v1\n";

	$v3 = $contentInfo->{$fileInfo->{md5_hex}};

	foreach $key (sort keys %{$v3->{contents}}) {
	    next if (defined $fwInfo->{$key});
	    if (!defined $v3->{contents}->{$key}) {
		$fwInfo->{$key} = undef;
		next;
	    }
	    $fwInfo->{$key} = doSubExtract($fileInfo,$v3,$key);
	}

	$doneFl = 1;
	foreach $key (keys %$fwInfo) {
	    if (!defined $fwInfo->{$key}) {
		$doneFl = undef;
		last;
	    }
	}
	last if ($doneFl);
    }

    if ($doneFl) {
	foreach $key (keys %$fwInfo) {
	    if (defined $fwInfo->{$key}) {
		if (defined $fileKeys->{$key}) {
		    $fwInfo->{$key}->{name} = $fileKeys->{$key};
		}
	    } else {
		print "Failed to locate firmware key $key";
		if (defined $fileKeys->{$key}) {
		    print " (name $fileKeys->{$key})";
		}
		print "\n";
		$doneFl = undef;
		next;
	    }
	    if (!defined $fwInfo->{$key}->{name}) {
		print "Can't write firmware for key $key; no name defined\n";
		$doneFl = undef;
		next;
	    }
	    if (!doSubWriteback($fwInfo->{$key})) {
		$doneFl = undef;
	    }
	    if (exists $hardwareIds->{$key}) {
		$devNotice{$hardwareIds->{$key}} = 1;
	    }
	}
    }

    if (! $doneFl) {
	print "Failed to successfully 1 or more firmware files\n";
	return 1;
    }

    @devs = keys %devNotice;
    $devCount = scalar @devs;
    if ($devCount > 0) {
	print <<EOF;

Important Note:
  One or more of the firmware files extracted is specific to a
  particular type and model of device.  Please double check that you
  in fact are using that named device (see below).  Some driver CDs
  can include firmware for multiple different hardware releases and we
  want to make sure that you've extracted the right one.

EOF
    }
    if ($devCount == 1) {
	print "  Device model is: $devs[0]\n";
    } elsif ($devCount > 1) {
	print "  Devices are:\n",
	my $foo;
	for ($foo = 0; $foo < $devCount; $foo++) {
	    print "    $devs[$foo]\n";
	}
    }
    print "\n";
    return 0;
}

# Our "main"
sub mainFunc {
    my $searchMode = 0;
    my $retCode;
    my $comment;
    my $key;
    my %f2Names;

    print "fwextract version: $vers\n";
    exit 1 if (!GetOptions("fwFile=s" => \%f2Names,
			   "comment=s" => \$comment,
			   "driverDir=s" => \@driverDirPath,
			   "search!" => \$searchMode));
    push @driverDirPath,@ARGV;
    if (scalar(@driverDirPath) == 0) {
	push @driverDirPath,"win_driver";
    }
    print "driverDirPath is @driverDirPath\n";
    if (scalar keys %f2Names) {
	$fileKeys = { };
	foreach $key (keys %f2Names) {
	    $fileKeys->{$key} = $f2Names{$key};
	    print "Image $key is $fileKeys->{$key}\n";
	}
    }

    if ($searchMode) {
	print "Searching for firmware images in driver directory\n";
	$retCode = searchDriver($comment);
    } else {
	print "Extracting firmware files from driver directory\n";
	$retCode = doExtract();
    }

    exit $retCode;
}

# Don't autoflush stdout so that progress printing works OK.
autoflush STDOUT !0;

mainFunc();

