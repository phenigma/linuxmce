#!/usr/bin/perl
use strict;

use Tk;
use Tk::Font;
use Tk::BrowseEntry;

use Getopt::Long qw(:config no_ignore_case bundling);
use Video::Frequencies 0.03;
use Video::ivtv 0.13;
use Config::IniFiles;

my $version="1.10";

# +--------------------------+
# | * tuner O comp. O svideo |
# | ch: [                  ] |
# |                          |
# | +---+ +---+ +---+ +---+  |
# | | 1 | | 2 | | 3 | | ^ |  | ch up
# | +---+ +---+ +---+ +---+  |
# | +---+ +---+ +---+ +---+  |
# | | 4 | | 5 | | 6 | | v |  | ch dn
# | +---+ +---+ +---+ +---+  |
# | +---+ +---+ +---+ +---+  |
# | | 7 | | 8 | | 9 | | + |  | fine up
# | +---+ +---+ +---+ +---+  |
# | +---+ +---+ +---+ +---+  |
# | |100| | 0 | |   | | - |  | fine dn
# | +---+ +---+ +---+ +---+  |
# +--------------------------+

my $tvchan = "??";

my $state = 0;
my $digits = 0;
my $hundred = 0;
my $prechan = 0;
my $chan = -1;

# state
# 0 - ready for new input
# 1 - inputing

# fwd decl
sub change_chan;
sub change_input;

my %settings = (
  Channel           => 73,		# default to SCI-FI
  VideoStandard     => "NTSC",
  VideoDevice       => "/dev/video0",
  FrequencyTable    => "ntsc-cable",  # default to NTSC_CABLE mapping.
  ConfigFileName    => "$ENV{HOME}/.ivtvrc",
  UseConfigFile     => 0,
  UsingIvtvDriver   => 1,  # default to being able to use the ivtv "enhancements".
  # other settings
  Debug             => 0,
  TunerNum          => 0,
);

# map the Settings/Config file parameter to the command line variable that specifies it.
my %mappings = (
    "VideoDevice"       => "d|input",
    "FrequencyTable"    => "f|freqtable",
    "Debug"             => "debug",
    "TunerNum"          => "tuner-num",
  );

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

my $versionStr = "ptune-ui.pl $version for use with http://ivtv.sf.net/";
my $usageStr = <<"END_OF_USAGE";
$versionStr

Usage: ptune-ui.pl [--input VIDEO_DEV] [--freqtable FREQENCY MAP]
       [--tuner-num TUNERNUM] [--help] [--version] [--debug]

  -d/--input VIDEO_DEV: video device to work with
  --tuner-num TUNERNUM: Specify the tuner to use.
  -f/--freqtable FREQUENCY MAP: Specify the frequency mapping to use.
  -h/--help: display this help
  -v/--version: display the version of this program
  --debug: turns on debug output

Defaults:
 --input $settings{VideoDevice} --freqtable $settings{FrequencyTable} --tuner-num $settings{TunerNum}

 config file = '$settings{ConfigFileName}'

 Note:  This script relies on Perl Modules: Video::Frequencies, Video::ivtv,
 Config::IniFiles, Getopt::Long and Perl::Tk.
END_OF_USAGE

# handle user input here
my %opts;
GetOptions(\%opts, "help|h", "input|d=s", "version|v", "freqtable|f=s", "debug", "tuner-num=i");
foreach my $option (keys %opts)
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
    if ($option =~ /^(v|version)$/)
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

# verify input
if ( ! -c "$settings{VideoDevice}")
{
  usage(1, "Video Dev = '$settings{VideoDevice}' is invalid!  $!");
}

if ($settings{TunerNum} !~ /^(\d)$/)
{
  error(1, "TunerNum = '$settings{TunerNum}' is invalid!");
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

if (!exists $CHANLIST{$settings{FrequencyTable}})
{
  usage(1, "Frequency Table = '$settings{FrequencyTable}' is invalid!");
}

# enumerations
my @standards;
my %name2std;
my @inputs;
my %name2input;
# Current settings (Input, Channel, Standard)
my $input = 0;
my $curinput;
my $frequency;
my $std;
my $curstd = "???";
my $curfreqtable = $settings{FrequencyTable};
my $curchannel = "";
my $curFreq = 0;

my $tuner;
my $err;
my $v4l2input;

my $tunerFD;

# card probe flag
if (1) {
  ##
  ## probe the card
  ##
  open($tuner, "<$settings{VideoDevice}")
    or die "unable to open: $!";
  $tunerFD = fileno($tuner);

  my $i;

  $err=0;
  # Capabilities and Is it V4L2 compat?

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

  # get the current video standard
  $std = $ivtvObj->getStandard($tunerFD);
  if ($std > 0)
  {
    printf("Standard: 0x%08x\n",$std) if ($settings{Debug});
  }
  else
  {
    die "Error: getStandard() failed!\n";
  }

  # get the current input
  $input = $ivtvObj->getInput($tunerFD);
  if ($input < 0)
  {
    die "Error: getInput() failed!\n";
  }
  printf("Input: 0x%08x\n",$input) if ($settings{Debug});

  my $done=0;
  # Standards
  for ($i=0; !$done; ++$i)
  {
    my($index,$std_id,$name,$frameperiod_n,$frameperiod_d,$framelines) = $ivtvObj->enumerateStandard($tunerFD, $i);
    if ($index == -1)
    {
      $done = 1;
    }
    else
    {
      printf("%d 0x%08x %s %d/%d %d\n",$index,$std_id,$name,$frameperiod_n,$frameperiod_d,$framelines) if ($settings{Debug});
      push @standards, [($name,$std_id)];
      $name2std{$name} = $std_id;
      if( (($std_id & $std) == $std))
      {
        $curstd = $name;
        $settings{VideoStandar} = $name;
      }
    }
  }

  $done=0;
  # Inputs
  for ($i=0; !$done; ++$i)
  {
    my($index,$name,$type,$audioset,$tuner,$std,$status) = $ivtvObj->enumerateInput($tunerFD, $i);
    if ($index == -1)
    {
      $done = 1;
    }
    else
    {
      push @inputs, $name;
      $name2input{$name} = $index;
    }
  }
  $curinput = $inputs[$input];

  # frequency's a little funny, you need to specify a valid tuner. WTF?

} else {
$input = 2;
@inputs = ( "Composite 1",
			"Composite 2",
			"Tuner 1",
			"SVideo 1",
			"SVideo 2" );
}
my $mw = MainWindow->new;

my $i=0;
my $j=0;

my @frame;
my @blab = ("1","2","3","^",
			"4","5","6","v",
			"7","8","9","+",
			"100","0"," ","-");

# radio, entry, keypad
for $i (0..1) {
  $frame[$i] = $mw->Frame( -borderwidth => 0 );
  $frame[$i]->pack( -fill => 'x' );
}

$frame[2] = $mw->Frame( -borderwidth => 0 );
$frame[2]->pack( -fill => "both", -expand => 1 );

$i = 0;
my @wid;
# foreach my $label (("Tuner","Comp.","SVideo")) {
#   $wid[$i] = $frame[0]->Radiobutton(
# 	-text => $label,
# 	-variable => \$input,
# 	-command => [\&change_input],
# 	-value => $i );
#   $wid[$i]->pack( -side => "left" );
#   ++$i;
# }

my $label;

# -state => "readonly",
my $sentry = $frame[0]->BrowseEntry(-state => "readonly",
									-browsecmd => \&change_std,
									-variable => \$curstd);
foreach $label (@standards) {
  $sentry->insert( "end", $label->[0] );
}
$sentry->pack( -side => "left", -fill => "x", -expand => 1 );

my $bentry = $frame[0]->BrowseEntry(-state => "readonly",
									-browsecmd => \&change_input,
									-variable => \$curinput);
foreach $label (@inputs) {
  $bentry->insert( "end", $label );
}
$bentry->pack( -side => "left", -fill => "x", -expand => 1 );

my $fentry = $frame[1]->BrowseEntry(-state => "readonly",
									-browsecmd => \&change_freqtable,
									-variable => \$curfreqtable);
foreach $label (sort keys %CHANLIST) {
  $fentry->insert( "end", $label );
}
$fentry->pack( -side => "left", -fill => "x", -expand => 1 );

my $centry = $frame[1]->BrowseEntry(-state => "readonly",
									-browsecmd => \&change_channel,
									-variable => \$curchannel);
foreach $label (sort { $a <=> $b } keys %{$CHANLIST{$curfreqtable}}) {
  $centry->insert( "end", $label );
}
$centry->pack( -side => "left", -fill => "x", -expand => 1 );

my $efont = $mw->Font( -size => 24 );

my $entry = $frame[1]->Entry( -state => "disabled",
							  -background => "white",
							  -justify => "right",
							  -font => \$efont,
							  -textvariable => \$tvchan );
$entry->pack( -side => "left", -fill => "x", -expand => 1 );

# keypad frame
$frame[3] = $frame[2]->Frame( -borderwidth => 0 );
$frame[3]->pack( -side=> "left", -fill => "both", -expand => 1 );

$j=3;
for $i (4..7) {
  $frame[$i] = $frame[3]->Frame( -borderwidth => 0 );
  for my $k (0..3) {
	$wid[$j] = $frame[$i]->Button( -text => $blab[$j-3],
								   -command => [\&change_chan,$blab[$j-3]],
								   -width => 1 );

	$wid[$j]->pack( -side => "left", -fill => "both", -expand => 1 );
	++$j;
  }
  $frame[$i]->pack( -fill => "both", -expand => 1 );
}

# enter button
# $wid[3] = $frame[2]->Button( -text => "Enter" );
# $wid[3]->pack( -side => "left", -fill => 'y' );

MainLoop;

sub do_chan {
  if($prechan)
  {
    if ($prechan =~ /^(\d+)$/)
    {
      $chan = $prechan + (100*$hundred);
    }
    else
    {
      $chan = $prechan;  # string channel.
    }
    my $freq = $CHANLIST{$settings{FrequencyTable}}->{$chan};
    if ($freq)
    {
      my $drfreq = ($freq * 16)/1000;
      $curFreq = $freq;

      if (!$ivtvObj->setFrequency($tunerFD, $settings{TunerNum}, $drfreq))
      {
        die "Error:  changeChannel($chan) failed!\n";
      }
      print "Channel changed to $chan, freq = '$freq'\n";
    }
    else
    {
      print "Channel $chan doesn't exist in $settings{FrequencyTable}!\n";
    }
  }


  $tvchan = ($chan =~ /^(-1)$/)?"??":$chan;

  $prechan = 0;
  $hundred = 0;
  $state = 0;
  $digits = 0;
}

sub tuneFrequency
{
  my $drfreq = ($curFreq * 16)/1000;
  if (!$ivtvObj->setFrequency($tunerFD, $settings{TunerNum}, $drfreq))
  {
    die "Error:  setFrequency($drfreq) failed!\n";
  }
  print "Frequency tuned to '$curFreq'\n";
}

sub change_chan {
  my ($key) = @_;
  print "channel input $key\n";

  if( $key =~ /[ v^]/ ) {
	if( $key eq ' ' ) {
	  $prechan = 0;
	} elsif( $key eq '^' ) {
	  $prechan = $chan + 1 if ($chan =~ /^(\d+)$/);
	} elsif( $key eq 'v' ) {
	  $prechan = $chan - 1 if ($chan =~ /^(\d+)$/);
	}

	do_chan();

  } elsif( $key =~ /[\-+]/ ) {
    $curFreq += 1 if ($key eq "+");
    $curFreq -= 1 if ($key eq "-" && $curFreq > 1);
    tuneFrequency();
  } elsif( $key == 100 ) {
	++$state;
	$hundred = 1;
	$tvchan = $prechan + 100*$hundred;
  } else {

	$prechan = ($prechan * 10) + $key;
	$tvchan = $prechan + 100*$hundred;
	++$state;

	if(++$digits>1) {
	  do_chan();
	}
  }

  if($state == 1) {
	$entry->configure( -background => '#e0ffe0' );
  } elsif ( $state == 0 ) {
	$entry->configure( -background => 'white' );
  }
}

sub change_input {
  my $preinput = $name2input{$curinput};
  if($preinput != $input)
  {
    $input = $preinput;
    print "input now $curinput, #$preinput\n";
    my $result = $ivtvObj->setInput($tunerFD, $input);
    if (!$result)
    {
      die "Error:  setInput($input) failed!\n";
    }
  }
}

sub change_std {
  my $standard = $name2std{$curstd};
  if($standard != $std)
  {
    $std = $standard;
    print "standard now $curstd, #$standard\n";
    my $result = $ivtvObj->setStandard($tunerFD, $standard);
    if (!$result)
    {
      die "Error:  setStandard($standard) failed!\n";
    }
  }
}

sub change_freqtable
{
  if ($curfreqtable ne $settings{FrequencyTable})
  {
    $settings{FrequencyTable} = $curfreqtable;
    print "frequency table now $curfreqtable\n";

    # flush the contents of the $centry drop-down combo
    # and re-populate with the new frequency table contents.

    $centry->delete(0, 'end');
    foreach my $label (sort { $a <=> $b } keys %{$CHANLIST{$curfreqtable}}) {
      $centry->insert( "end", $label );
    }
  }
}

# changes to the selected channel from the drop-down.
sub change_channel
{
  $prechan = $curchannel;
  do_chan();
}

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


__END__
