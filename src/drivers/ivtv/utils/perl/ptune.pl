#!/usr/bin/perl
#
# $Header: /cvsroot/ivtv/ivtv/ivtv/utils/ptune.pl,v 1.12 2003/07/16 03:30:39 pcxuser Exp $
#
use strict;

use Getopt::Long qw(:config no_ignore_case bundling);
use Video::Frequencies;
use Video::ivtv 0.13;
use Config::IniFiles;

my $version="1.11";

my %settings = (
  Channel           => 4,		# default to the ivtv default channel
  VideoDevice       => "/dev/video0",
  FrequencyTable    => "ntsc-cable",  # default to NTSC_CABLE mapping.
  Frequency         => "",
  ConfigFileName    => "$ENV{HOME}/.ivtvrc",
  UseConfigFile     => 0,
  UsingIvtvDriver   => 1,  # default to being able to use the ivtv "enhancements".
  # other settings
  Debug             => 0,
  TunerNum          => 0,
);

# map the Settings/Config file parameter to the command line variable that specifies it.
my %mappings = (
    "Channel"           => "c|channel",
    "VideoDevice"       => "d|input",
    "FrequencyTable"    => "f|freqtable",
    "Frequency"         => "F|frequency",
    "Debug"             => "debug",
    "TunerNum"          => "tuner-num",
  );

my @profileNames=(); # list of user defined sections to work with in the config file.
my %configIni;      # config hash we tie to for Config::IniFiles.
my $ivtvObj = Video::ivtv->new();

# check for devfs support
if ( -e "/dev/.devfsd" )
{
  $settings{VideoDevice} = "/dev/v4l/video0";
}

# check for the config file
if (-f $settings{ConfigFileName})
{
  $settings{UseConfigFile} = 1;

  # tie to it.
  tie %configIni, 'Config::IniFiles', (-file => $settings{ConfigFileName}) or die "Error: Opening config file '$settings{ConfigFileName}' failed! $!\n";

  my $profile = "defaults";
  if (exists $configIni{$profile})
  {
    # update the defaults stored.
    foreach my $arg (keys %mappings)
    {
      if (exists $configIni{$profile}{$arg})
      {
        $settings{$arg} = $configIni{$profile}{$arg};
        #print "settings{$arg} = '" . $settings{$arg} . "'\n";
      }
    }
  }
  else
  {
    print "Warning: config file '$settings{ConfigFileName}' exists but does not have the\n[$profile] section!  Use record-v4l2.pl to create it.\n\n";
  }
}
else  # create the config file
{
  print "Auto Creating config file $settings{ConfigFileName}...\n";
  my $profile = "defaults";

  # we have to create the config file and tie to it.
  tie %configIni, 'Config::IniFiles', () or die "Error: Initializing config file '$settings{ConfigFileName}' failed! $!\n";

  # now set the name to work with.
  tied(%configIni)->SetFileName($settings{ConfigFileName}) or die "Error: Setting config file to '$settings{ConfigFileName}' failed! $!\n";

  $configIni{$profile} = {};  # make sure the section exists.

  foreach my $arg (keys %mappings)
  {
    $configIni{$profile}{$arg} = $settings{$arg};
    print "configIni{$profile}{$arg} = '" . $settings{$arg} . "'\n" if $settings{Debug};
  }

  # write the config file out.
  tied(%configIni)->RewriteConfig or die "Error: Writing config file '$settings{ConfigFileName}' failed!  $!\n";
}

# build up the "custom" frequency table
my %customMap = ();
foreach my $profileName (keys %configIni)
{
  next if $profileName =~ /^(defaults)$/;

  if (exists $configIni{$profileName}{Frequency})
  {
    $customMap{$profileName} = $configIni{$profileName}{Frequency};
  }
}
$CHANLIST{custom} = \%customMap;

my $versionStr = "ptune.pl $version for use with http://ivtv.sf.net/";
my $usageStr = <<"END_OF_USAGE";
$versionStr

Usage: ptune.pl [--channel CHANNEL] [--input VIDEO_DEV]
       [--freqtable FREQENCY MAP] [--frequency FREQUENCY]
       [--profile PROFILE] [--list-freqtable] [--list-channels]
       [--tuner-num TUNERNUM] [--help] [--version] [--debug]

  -c/--channel CHANNEL: channel number to switch to
      NOTE: You can also specify the channel by itself.
            Ex.  record-v4l2.pl 73
            would change to channel 73 using the default settings
            or the settings from your ~/.ivtvrc config file.
  -d/--input VIDEO_DEV: video device to work with
  -f/--freqtable FREQUENCY MAP: Specify the frequency mapping to use.
  -F/--frequency FREQUENCY: Specify the frequency to tune to.
                ex. 517250 = NTSC Cable 73 (SCiFi)
  --tuner-num TUNERNUM: Specify the tuner to use.
  -L/--list-freqtable: list all available frequency mappings that
       Video::Frequencies knows
  --list-channels: lists all channels and their frequencies for the
       specified frequency table being used.
  -P/--profile PROFILE: Override defaults and command line values with the
              config entries in the section labeled [PROFILE] from the
              config file $settings{ConfigFileName}.
              Examples: -P NTSC, -P 22

              You can specify this option multiple times and each successive
              profile will overlay the defaults and any previous profiles.
  -h/--help: display this help
  -v/--version: display the version of this program
  --debug: turns on debug output

Notes:
  If you specify both -c/--channel and -F/--frequency
    then -F/--frequency will take precedence.

Defaults:
 --channel $settings{Channel} --input $settings{VideoDevice} --freqtable $settings{FrequencyTable} --tuner-num $settings{TunerNum}


 config file = '$settings{ConfigFileName}'

 Note:  This script relies on Perl Modules: Video::Frequencies, Video::ivtv,
 Config::IniFiles and Getopt::Long.
END_OF_USAGE

# handle user input here
my %opts;
#getopts('c:hd:vf:F:L', \%opts);
GetOptions(\%opts, "channel|c=s", "help|h", "input|d=s", "version|v", "freqtable|f=s", "frequency|F=i",
                   "list-freqtable|L", "list-channels", "debug", "profile|P=s@", "tuner-num=i");
if (scalar keys %opts == 0 && @ARGV == 0)
{
  usage(0, "");
}
foreach my $option (sort keys %opts)
{
  my $found = 0;
  foreach my $mapName (keys %mappings)
  {
    if ($option =~ /^($mappings{$mapName})$/)
    {
      $settings{$mapName} = $opts{$option};
      $found = 1;
      print "$mapName = '$opts{$option}'\n" if $settings{Debug};
    }
  }
  if (!$found)
  {
    # handle the non-settings cases.
    if ($option =~ /^(L|list-freqtable)$/)
    {
      my $errStr = "\nAvailable Frequency Mappings:\n";
      foreach my $name (sort keys %CHANLIST)
      {
        $errStr .= "$name\n";
      }
      print "$versionStr\n$errStr";
      exit 0;
    }
    elsif ($option eq "list-channels")
    {
      my $errStr = "\nAvailable Channels for $settings{FrequencyTable}:\n";
      foreach my $name (sort { $a <=> $b } keys %{$CHANLIST{$settings{FrequencyTable}}})
      {
        $errStr .= "$name\t= $CHANLIST{$settings{FrequencyTable}}->{$name}\n";
      }
      print "$versionStr\n$errStr";
      exit 0;
    }
    elsif ($option =~ /^(P|profile)$/)
    {
      @profileNames = @{$opts{$option}};
    }
    elsif ($option =~ /^(v|version)$/)
    {
      print "$versionStr\n";
      exit 0;
    }
    elsif ($option =~ /^(h|help)$/)
    {
      usage(0, "");
    }
    else
    {
      usage(1, "-$option is an unknown option!");
    }
  }
}

if (@profileNames)
{
  # loop over all profiles the user specified.
  foreach my $profileName (@profileNames)
  {
    print "profile = '$profileName'\n" if $settings{Debug};
    # for now the profile can not be "defaults".
    if ($profileName eq "defaults")
    {
      error(1, "Profile = '$profileName' is invalid!");
    }
    if (exists $configIni{$profileName})
    {
      # update defaults/override command line arguments that exist in this profile.
      my $profile = $profileName;

      foreach my $arg (keys %mappings)
      {
        foreach my $option (split(/\|/, $mappings{$arg})) # handle the long/short command option versions
        {
          #print "arg = '$arg', option = '$option'\n" if $settings{Debug};
          if (exists $configIni{$profile}{$arg} && !(exists $opts{$option}))
          {
            $settings{$arg} = $configIni{$profile}{$arg};
            print "settings{$arg} = '" . $settings{$arg} . "'\n" if $settings{Debug};
            last;
          }
        }
      }
    }
    else
    {
      error(1, "Profile = '$profileName' does not exist!\n\nYou must use record-v4l2.pl and specify -S/--save to create it\nor manually edit the config file and add the section.");
    }
  }
}

# verify input

if (@ARGV)
{
  if (exists $opts{c} || exists $opts{channel})
  {
    print "Warning: ignoring channel argument and using '$ARGV[0]' instead.\n";
  }
  $settings{Channel} = $ARGV[0];
}

if ( ! -c "$settings{VideoDevice}")
{
  usage(1, "Video Dev = '$settings{VideoDevice}' is invalid!  $!");
}

if ($settings{TunerNum} !~ /^(\d)$/)
{
  error(1, "TunerNum = '$settings{TunerNum}' is invalid!");
}

if (!exists $CHANLIST{$settings{FrequencyTable}})
{
  usage(1, "Frequency Table = '$settings{FrequencyTable}' is invalid!");
}

my $freq;

if (exists $opts{F} || exists $opts{frequency} || $settings{Frequency}) # the user may have specified a Frequency in their config file
{
  if ($settings{Frequency} !~ /^(\d+)$/)
  {
    usage(1, "Frequency = '$settings{Frequency}' is invalid!");
  }
  $freq = $settings{Frequency};
}
# now verify that the channel exists in the frequency table!
else
{
  if (!$settings{Channel})
  {
    usage(1, "channel = '$settings{Channel}' is invalid!");
  }
  if (!exists $CHANLIST{$settings{FrequencyTable}}->{$settings{Channel}})
  {
    usage(1, "Channel = '$settings{Channel}' does not exist in Frequency Table '$settings{FrequencyTable}'!");
  }
  $freq = $CHANLIST{$settings{FrequencyTable}}->{$settings{Channel}};
}

my $driverf = ($freq * 16)/1000;
print "" . ((exists $opts{F} || exists $opts{frequency} || $settings{Frequency}) ? "Frequency" : "Ch.$settings{Channel}") . ": $freq $driverf\n";

open(my $tuner, "<$settings{VideoDevice}")
  or die "unable to open: $!";
my $tunerFD = fileno($tuner);

# get the current capabilities.
my @capabilities = $ivtvObj->getCapabilities($tunerFD);
if (@capabilities != keys %{$ivtvObj->{capIndexes}})
{
  error(1, "getCapabilities() failed!");
}
if ($capabilities[$ivtvObj->{capIndexes}{driver}] ne "ivtv")
{
  $settings{UsingIvtvDriver} = 0;  # we can't use the ivtv "enhancements".
  print "Warning:  V4l2 driver = '$capabilities[$ivtvObj->{capIndexes}{driver}]' does not support the ivtv \"enhancements\"!\n";
  print "          All codec related options will be ignored.\n\n";
}

if (!$ivtvObj->setFrequency($tunerFD, $settings{TunerNum}, $driverf))
{
  die "Error:  setFrequency($driverf) failed!\n";
}

close($tuner);

# usage(returnValue, ErrorString)
# returnValue = 1 or 0
# ErrorString = message you want to tell the user, but only if returnValue = 1.
sub usage
{
  my $errorCode = shift;
  my $error = shift;

  print $usageStr;
  print "\nError:  $error\n" if ($errorCode == 1);
  print "$error\n" if ($errorCode == 2);

  exit $errorCode;
}

# error(returnValue, ErrorString)
# returnValue = 1 or 0
# ErrorString = message you want to tell the user.
sub error
{
  my $errorCode = shift;
  my $error = shift;

  print "$versionStr";
  print "\nError:  $error\n";

  exit $errorCode;
}

# VIDIOCSFREQ = _IOW('v',15,ulong)
#.......o .......o .......o .......o
#01000000 00000100 01110110 00000111
#40 04 76 0f

__END__

# use Video::Capture::V4l;

# my $tuner = new Video::Capture::V4l
#  or die "unable to open device: $!";

# set channel
# set tuner std

# $tuner->freq($freq);
