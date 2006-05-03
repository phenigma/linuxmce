#! /usr/bin/perl
#
# ivtvfwextract: Find and extract firmware for Hauppage WinTV-PVR-250 and 350.
#
# Copyright (C) 2003 Hans-Peter Nilsson.
# Distributable under the GNU General Public License,
# see <URL:http://www.gnu.org/copyleft/gpl.html>.
#
# Options:
#  --split:
#           Split each of the images into two files with 128k each.  Any
#           specified encoder and decoder firmware image names are taken
#           to be prefixes, and "-1.bin" and "-2.bin" are suffixed.
#
#           this is legacy. Use --no-split unless you have an old ver of
#           the driver
#
#  --no-split: (default)
#           Don't perform any split: write the full 256k into single
#           files.
#
#  --use-encoder-image=N, N = 1 or N = 2.
#           Retrieve the encoder image number N, emit error if only one.
#
#  --no-unzip:
#           The driver file isn't zipped file.
#
# $ARGV[0]: Location of mounted cdrom or filename of zipped drivers.
#           (default /mnt/cdrom).
#           Or the driver filename when you set "--no-unzip" option.
#
# $ARGV[1]: If this is a directory: location (default /lib/modules) where
#           to put firmware files ivtv-enc-fw.bin and ivtv-dec-fw.bin, or
#           when --split is in effect, files ivtv-enc-fw-1.bin,
#           ivtv-enc-fw-2.bin, ivtv-dec-fw-1.bin and ivtv-dec-fw-2.bin.
#           If this is a nonexistent filename or existing file: filename
#           (prefix if --split in effect) where to put encoder firmware.
#
# $ARGV[2]: Filename to put decoder firmware.
#
# Examples:
#    ivtvfwextract
#    ivtvfwextract --use-encoder-image=2 pvr48xxx.exe
#    ivtvfwextract --no-split /mnt/cdrom /tmp/enc.img /tmp/dec.img

use Getopt::Long;

$packed_driver_name = "pvr48xxx.exe";
$fw_driver_name = "hcwpvrp2.sys";
# for Yuan MPG600/MPG160
#$fw_driver_name = "p2driver.sys";
$decoder_magic = "a703000066bb55aa";
$encoder_magic = "a70d000066bb55aa";

# Remember to adjust the usage-text and the $encoder_path and
# $decoder_path settings if you change the default to --no-split.
$do_split = 0;
$do_not_split = -1;
$do_help = 0;
$use_encoder_img = -1;
$no_unzip = 0;

$default_readpath = "/mnt/cdrom";
$default_destpath = "/lib/modules";
$default_encname = "ivtv-fw-enc";
$default_decname = "ivtv-fw-dec";
sub bye;

# There's no useful version string to grep for, so let's just key off
# the "usage" message we get with no parameters specified.
bye ("Can't find required program unzip in your PATH")
    unless `unzip` =~ /Usage:/;

undef $bad;
# Dissect the parameters.
$result = GetOptions ("split" => \$do_split,
		      # No way to configure "no-" prefix instead of
		      # "no"-prefix?
		      "no-split" => \$do_not_split,
		      "help" => \$do_help,
		      "use-encoder-image=i" => \$use_encoder_img,
		      "no-unzip" => \$no_unzip);

$bad = "" # Already emitted by Getopt.
    if $result == 0;
$bad = ""
    if !defined ($bad) && $do_help;
$bad = "Can't have both --split and --no-split"
    if !defined ($bad) && $do_split == 1 && $do_not_split == 1;
$do_split = 0 if $do_not_split == 1;
$bad = "Only N=1 and N=2 allowed in --use-encoder-image=N"
    if (!defined ($bad) && $use_encoder_img != -1
	&& $use_encoder_img != 1 && $use_encoder_img != 2);

bye ("Too many parameters\n")
    if $#ARGV > 2;

bye << "EOF"
$bad
  Usage:
   $0 --help
  or
   $0 [--split (default) | --no-split]
      [--use-encoder-image=1 | --use-encoder-image=2]
      [--no-unzip]
      [driver-location (default $default_readpath) or driver filename (with --no-unzip)
       [driver-destdir (default $default_destpath)
        | encoder-image (default $default_destpath/$default_encname)
          decoder-image (default $default_destpath/$default_decname)]]
  Examples:
   $0
   $0 --use-encoder-image=2 pvr48xxx.exe
   $0 --no-split /mnt/cdrom /tmp/enc.img /tmp/dec.img
EOF
    if defined $bad;

$readpath = $#ARGV > -1 ? $ARGV[0] : $default_readpath;

$destpath = $#ARGV > 0 ? $ARGV[1] : $default_destpath;
if (-d $destpath) {
    bye ("Second parameter ($destpath) of three unexpectedly a directory")
	if $#ARGV > 1;
    bye ("Can't write to path for firmware, \"$destpath\"")
	unless -w $destpath;

    if ($do_split) {
	$encoder_path = "$destpath/$default_encname";
	$decoder_path = "$destpath/$default_decname";
    } else {
	$encoder_path = "$destpath/$default_encname.bin";
	$decoder_path = "$destpath/$default_decname.bin";
    }
} else {
    bye ("Missing decoder path parameter (encoder path: $destpath)")
	if $#ARGV != 2;
    $encoder_path = $destpath;
    $decoder_path = $ARGV[2];
}

# Did we get a directory for the driver?
if (-d $readpath) {
    # Then find the driver somewhere there.
    $driverfile
	= `find $readpath -iname $packed_driver_name -print 2>/dev/null | head -1`;
    chomp $driverfile;
    bye ("Can't find \"$packed_driver_name\" in \"$readpath\"")
	unless $driverfile ne "";
} else {
    # Must be the path for the packed driver itself.
    $driverfile = $readpath;
}

bye ("\"$driverfile\" not accessible")
    unless -f $driverfile && -r $driverfile;

if ($no_unzip) {
    $fw_driver_path = $readpath;
} else {
    # That file is expected to be a self-extracting archive, though we
    # can just throw it to unzip.
    $tmpdir = (defined ($ENV{"TMPDIR"}) ? $ENV{"TMPDIR"} : "/tmp") . "/ivtvex.$$";
    bye ("Can't create temporary directory $tmpdir")
        unless mkdir $tmpdir;

    $unzipcmd = "unzip -L -d $tmpdir $driverfile $fw_driver_name";
    bye ("Can't perform \"$unzipcmd\"")
        unless defined `$unzipcmd 2>/dev/null`;
    $fw_driver_path = "$tmpdir/$fw_driver_name";
}


# Read in the file.
bye ("Can't open $fw_driver_path: $!")
    unless open FWFILE, "<$fw_driver_path";
bye ("Can't set binary mode for $fw_driver_path")
    unless binmode FWFILE;
bye ("Can't read in $fw_driver_path")
    unless ((read FWFILE, $fwraw, -s "$fw_driver_path")
	    == -s "$fw_driver_path");
bye ("Can't close $fw_driver_path")
    unless close FWFILE;

# Find the special markers.
# Aww, can't use index() on binary data (i.e. zero-terminated strings).
# Can we assume there'll be no match on odd nybble boundaries?
# Well, we'll treat that like "multiple images".
$fwhex = unpack ("H*", $fwraw);

$decoder_index = index ($fwhex, $decoder_magic) / 2;
# No use adding --use-decoder-image willy-nilly-just-in-case.
bye ("Confused: multiple decoder images found")
    if index ($fwhex, $decoder_magic, $decoder_index * 2 + 1) != -1;
bye ("Can't find decoder image in $fw_driver_path")
    unless $decoder_index != -0.5;
$decoder_image = substr ($fwraw, $decoder_index, 256 * 1024);

$encoder_index = index ($fwhex, $encoder_magic) / 2;
bye ("Can't find encoder image in $fw_driver_path")
    unless $encoder_index != -0.5;

$next_encoder_index
    = index ($fwhex, $encoder_magic, $encoder_index * 2 + 1) / 2;
if ($next_encoder_index != -0.5) {
    bye ("Confused: at least two encoder images found\n"
	 . " but no --use-encoder-image=N (N=1,2) given")
	if $use_encoder_img == -1;
    $encoder_index = $next_encoder_index
	if ($use_encoder_img == 2);
    # What, you really think there should be a *loop* for N?
    $next_encoder_index
	= index ($fwhex, $encoder_magic, $next_encoder_index * 2 + 1);
    bye ("Confused: three encoder images found.  What's going on here?")
	if $next_encoder_index != -1;
} else {
    bye ("Confused: only one encoder image found\n"
	 . " but --use-encoder-image=$use_encoder_img specified")
	if $use_encoder_img != -1;
}
$encoder_image = substr ($fwraw, $encoder_index, 256 * 1024);

# Write out the images to the awaiting files and we're done.
if ($do_split) {
    bye ("Can't open encoder image file $encoder_path-1.bin")
	unless open ENCFILE, ">$encoder_path-1.bin";
    bye ("Can't set binary mode for $fw_driver_path-1.bin")
	unless binmode ENCFILE;
    bye ("Can't write encoder image to $encoder_path-1.bin")
	unless (syswrite (ENCFILE, $encoder_image,
			  length ($encoder_image) / 2)
		== length ($encoder_image) / 2);
    bye ("Can't close encoder image file $encoder_path-1.bin")
	unless close ENCFILE;

    bye ("Can't open encoder image file $encoder_path-2.bin")
	unless open ENCFILE, ">$encoder_path-2.bin";
    bye ("Can't set binary mode for $fw_driver_path-2.bin")
	unless binmode ENCFILE;
    bye ("Can't write encoder image to $encoder_path-2.bin")
	unless (syswrite (ENCFILE, $encoder_image,
			  length ($encoder_image) / 2,
			  length ($encoder_image) / 2)
		== length ($encoder_image) / 2);
    bye ("Can't close encoder image file $encoder_path-2.bin")
	unless close ENCFILE;

    bye ("Can't open decoder image file $decoder_path-1.bin")
	unless open DECFILE, ">$decoder_path-1.bin";
    bye ("Can't set binary mode for $fw_driver_path-1.bin")
	unless binmode DECFILE;
    bye ("Can't write decoder image to $decoder_path-1.bin")
	unless (syswrite (DECFILE, $decoder_image,
			   length ($decoder_image) / 2)
		== length ($decoder_image) / 2);
    bye ("Can't close decoder image file $decoder_path-1.bin")
	unless close DECFILE;

    bye ("Can't open decoder image file $decoder_path-2.bin")
	unless open DECFILE, ">$decoder_path-2.bin";
    bye ("Can't set binary mode for $fw_driver_path-2.bin")
	unless binmode DECFILE;
    bye ("Can't write decoder image to $decoder_path-2.bin")
	unless (syswrite (DECFILE, $decoder_image,
			  length ($decoder_image) / 2,
			  length ($decoder_image) / 2)
		== length ($decoder_image) / 2);
    bye ("Can't close decoder image file $decoder_path-2.bin")
	unless close DECFILE;
} else {
    bye ("Can't open encoder image file $encoder_path")
	unless open ENCFILE, ">$encoder_path";
    bye ("Can't set binary mode for $fw_driver_path")
	unless binmode ENCFILE;
    bye ("Can't write encoder image to $encoder_path")
	unless ((syswrite ENCFILE, $encoder_image)
		== length ($encoder_image));
    bye ("Can't close encoder image file $encoder_path")
	unless close ENCFILE;
    bye ("Can't open decoder image file $decoder_path")
	unless open DECFILE, ">$decoder_path";
    bye ("Can't set binary mode for $fw_driver_path")
	unless binmode DECFILE;
    bye ("Can't write decoder image to $decoder_path")
	unless ((syswrite DECFILE, $decoder_image)
		== length ($decoder_image));
    bye ("Can't close decoder image file $decoder_path")
	unless close DECFILE;
}
bye ("");

# Graceful exit, hopefully.
sub bye {
    $args = join ("", @_);
    `rm -rf $tmpdir 2>/dev/null` if defined $tmpdir;
    die ("$0: $args\n") if $args ne "";
    exit 0;
}

