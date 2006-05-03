#!/usr/bin/perl
# config-editor.pl created by James A. Pattie <james@pcxperience.com> 2003-06-11
# Copyright 2003, released under the GPL.
# Purpose: to provide a graphical editor for the ~/.ivtvrc config file used
# by the perl scripts of the ivtv project.

#
# You can always get the latest version of the script from cvs at
# http://ivtv.sourceforge.net/
#

# 20030611 -  1.0  - Initial version that builds the framework.
# 20030626 -  1.1  - Updated to cover the Audio -> AudioBitmask changes and config file versioning.
# 20030713 -  1.2  - Updated to cover the MSP Matrix options and config file version change.
# 20040613 -  1.3  - Updated to cover the MSP Matrix options being gone and the new GOP_END ioctl.

use strict;

use Tk;
use Tk::Font;
use Tk::BrowseEntry;

use Getopt::Long qw(:config no_ignore_case bundling);
use Video::Frequencies 0.03;
use Config::IniFiles;

my $version="1.3";
my $cfgVersion = "3";
my $cfgVersionStr = "_configVersion_";  # hopefully unique [defaults] value to let me know what version the config file is.

my %settings = (
  ConfigFileName    => "$ENV{HOME}/.ivtvrc",
  # other settings
  Debug             => 0,
);

# map the Settings parameter to the command line variable that specifies it.
my %mappings = (
    "Debug"             => "debug",
  );

my %defaultSettings = (
  Channel           => 4,		# default to the ivtv default channel
  RecordDuration    => 3595,		# default to 59 minutes 55 seconds (in seconds) - This lets 2 back to back cron jobs work!
  InputNum          => 0,      # TV-Tuner 1
  InputName         => "Tuner 1",
  OutputDirectory   => ".",
  VideoDevice       => "/dev/video0",
  VideoWidth        => "720",	# 720x480-fullscreen NTSC
  VideoHeight       => "480",
  VideoStandard     => "NTSC",  # NTSC, PAL or SECAM
  VideoType         => "mpeg",  # mpeg, yuv
  Bitrate           => "6500000",
  PeakBitrate       => "8000000",  # peak bitrate
  Aspect            => 2,
  AudioBitmask      => 0x00e9,
  BFrames           => 3,
  DNRMode           => 0,
  DNRSpatial        => 0,
  DNRTemporal       => 0,
  DNRType           => 0,
  Framerate         => 0,
  FramesPerGOP      => 15,
  GOPClosure        => 1,
  Pulldown          => 0,
  StreamType        => 0,  # 10 = DVD format (almost)
  OutputFileName    => "video.mpg",
  FrequencyTable    => "ntsc-cable",  # default to NTSC_CABLE mapping.
  Frequency         => "", # user specified frequency.
  ResetCardSettings => 1,
  DontRecord        => 0,  # default to always recording data.
  DirectoryFormatString => "%I-%c-%d", # format string used to define the sub directory under OutputDirectory
  DateTimeFormatString  => "+%Y%m%d-%H%M", # format string used to represent the date/time if the user wants it in their DirectoryFormatString
  # other settings
  CaptureLastGOP    => 1,  # default to trying to Capture the Last GOP of the encoder stream.
  Debug             => 0,
  TunerNum          => 1,
);

my %configIni;      # config hash we tie to for Config::IniFiles.
my $changesUnsaved = 0; # keeps track of the current state of the config file (if it needs to be saved)

# global variables
my $profile = "";
my $attribute = "";
my $attributeValue = "";

# check for devfs support
if ( -e "/dev/.devfsd" )
{
  $defaultSettings{VideoDevice} = "/dev/v4l/video0";
}

# check for the config file
if (-f $settings{ConfigFileName})
{
  # tie to it.
  tie %configIni, 'Config::IniFiles', (-file => $settings{ConfigFileName}) or die "Error: Opening config file '$settings{ConfigFileName}' failed! $!\n";
  
  my $profile = "defaults";
  if (exists $configIni{$profile})
  {
    my $saveFile = 0;
    # check version of the config file.
    if (!exists $configIni{$profile}{$cfgVersionStr})
    {
      print "Updating config file to version 1...\n";

      # first version config file!  Update the Audio -> AudioBitmask entries.
      $configIni{$profile}{$cfgVersionStr} = $cfgVersion;

      # find all entries that have Audio and move to AudioBitmask.
      foreach my $p (keys %configIni)
      {
        if (exists $configIni{$p}{Audio})
        {
          $configIni{$p}{AudioBitmask} = $configIni{$p}{Audio};
          delete $configIni{$p}{Audio};
        }
      }

      $saveFile = 1;  # signal we need to save the config changes.
    }
    if ($configIni{$profile}{$cfgVersionStr} != $cfgVersion)
    {
      # we need to upgrade
      if ($configIni{$profile}{$cfgVersionStr} == 1)
      {
        print "Updating config file to version 2...\n";
        # add the MSP Matrix related options.
        $configIni{$profile}{SetMSPMatrix} = $settings{SetMSPMatrix};
        $configIni{$profile}{MSPInput} = $settings{MSPInput};
        $configIni{$profile}{MSPOutput} = $settings{MSPOutput};
        $configIni{$profile}{MSPSleep} = $settings{MSPSleep};
        $configIni{$profile}{$cfgVersionStr} = 2;
        $saveFile = 1;
      }
      if ($configIni{$profile}{$cfgVersionStr} == 2)
      {
        print "Updating config file to version 3...\n";
        # remove the MSP Matrix related options from all profiles.
        foreach my $p (keys %configIni)
        {
          foreach my $k (qw(SetMSPMatrix MSPInput MSPOutput MSPSleep))
          {
            if (exists $configIni{$p}{$k})
            {
              delete $configIni{$p}{$k};
            }
          }
        }
        $configIni{$profile}{CaptureLastGOP} = 1;
        $configIni{$profile}{$cfgVersionStr} = 3;
        $saveFile = 1;
      }
    }
    
    if ($saveFile)
    {
      # now save the updated config file before we continue.
      tied(%configIni)->RewriteConfig or die "Error: Writing config file '$settings{ConfigFileName}' failed!  $!\n";
    }
  }
  else
  {
    print "Creating [defaults] section...\n";
    # make the [defaults] section appear.
    $configIni{$profile} = {};  # make sure the section exists.

    foreach my $arg (keys %defaultSettings)
    {
      $configIni{$profile}{$arg} = $defaultSettings{$arg};
      print "configIni{$profile}{$arg} = '" . $defaultSettings{$arg} . "'\n" if $settings{Debug};
    }

    # set the config file version
    $configIni{$profile}{$cfgVersionStr} = $cfgVersion;

    # write the config file out.
    tied(%configIni)->RewriteConfig or die "Error: Writing config file '$settings{ConfigFileName}' failed!  $!\n";
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

  foreach my $arg (keys %defaultSettings)
  {
    $configIni{$profile}{$arg} = $defaultSettings{$arg};
    print "configIni{$profile}{$arg} = '" . $defaultSettings{$arg} . "'\n" if $settings{Debug};
  }

  # set the config file version
  $configIni{$profile}{$cfgVersionStr} = $cfgVersion;

  # write the config file out.
  tied(%configIni)->RewriteConfig or die "Error: Writing config file '$settings{ConfigFileName}' failed!  $!\n";
}

my $versionStr = "config-editor.pl $version for use with http://ivtv.sf.net/";
my $usageStr = <<"END_OF_USAGE";
$versionStr

Usage: config-editor.pl [--help] [--version] [--debug]

  -h/--help: display this help
  -v/--version: display the version of this program
  --debug: turns on debug output

Defaults:
 config file = '$settings{ConfigFileName}'

 Note:  This script relies on Perl Modules: Config::IniFiles and Getopt::Long.
END_OF_USAGE

# handle user input here
my %opts;
GetOptions(\%opts, "help|h", "version|v", "debug");
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

my $mw = MainWindow->new;
$mw->title("ivtv Config Editor - $version");

my @frame;
# create any frames needed
$frame[0] = $mw->Frame( -borderwidth => 1, -relief => "ridge" )->pack( -fill => "x" );
$frame[1] = $mw->Frame( -borderwidth => 0 )->pack( -fill => "both", -expand => 1 );
$frame[2] = $frame[1]->Frame( -borderwidth => 0, -label => "Profiles" )->pack( -side => "left", -fill => "y" );
$frame[3] = $frame[1]->Frame( -borderwidth => 0, -label => "Attributes" )->pack( -side => "left", -fill => "y" );
$frame[4] = $frame[1]->Frame( -borderwidth => 0, -label => "Value" )->pack( -side => "left", -fill => "y" );
$frame[5] = $frame[2]->Frame( -borderwidth => 0, )->pack( -side => "top", -fill => "y" );
$frame[6] = $frame[2]->Frame( -borderwidth => 0, )->pack( -side => "bottom", -fill => "x" );
$frame[7] = $frame[3]->Frame( -borderwidth => 0, )->pack( -side => "top", -fill => "y" );
$frame[8] = $frame[3]->Frame( -borderwidth => 0, )->pack( -side => "bottom", -fill => "x" );
$frame[9] = $frame[1]->Frame( -borderwidth => 0, -label => "New Attributes" )->pack( -side => "left", -fill => "y" );
$frame[10] = $mw->Frame( -borderwidth => 0, -label => "Status", -labelPack => [ -side => "left" ] )->pack( -fill => "x");

# create the menu
my $menub = $frame[0]->Menubutton(-text => "File", -tearoff => 0)->pack(-side => "left", -anchor => "n");

# create the menu items.
$menub->AddItems(["command" => "Save", -command => \&saveFile, -state => "disabled"],
                 "-",
                 ["command" => "Exit", -command => \&doExit]);

# verify that the defaults profile is created.  If not, create.
my $profileName = "defaults";
if (!exists $configIni{$profileName})
{
  $configIni{$profileName} = {};  # make sure the section exists.

  foreach my $arg (keys %defaultSettings)
  {
    $configIni{$profileName}{$arg} = $defaultSettings{$arg};
    print "configIni{$profileName}{$arg} = '" . $defaultSettings{$arg} . "'\n" if $settings{Debug};
  }

  signalSaveNeeded();
}

# define the list of available profiles to work with
my $profileLB = $frame[5]->Scrolled("Listbox", -selectmode => "single", -scrollbars => "osoe", -exportselection => 0)->pack(-side => "left");
$profileLB->bind("<Button-1>", \&selectProfile);

for my $profileName (sort keys %configIni)
{
  $profileLB->insert("end", $profileName);
}
# create the profile entry field
my $valueEntry = $frame[6]->Entry(-textvariable => \$profile)->pack(-side => "top", -fill => "x");
# create the create profile button
my $createProfileB = $frame[6]->Button(-text => "Create", -command => \&createProfile)->pack(-side => "left");
# create the delete profile button
my $deleteProfileB = $frame[6]->Button(-text => "Delete", -command => \&deleteProfile, -state => "disabled")->pack(-side => "right");

# create the attributes list
my $attributeLB = $frame[7]->Scrolled("Listbox", -selectmode => "single", -scrollbars => "osoe", -exportselection => 0)->pack(-side => "left");
$attributeLB->bind("<Button-1>", \&selectAttribute);
# create the delete attribute button
my $deleteAttributeB = $frame[8]->Button(-text => "Delete", -command => \&deleteAttribute, -state => "disabled")->pack(-side => "right");

# define the value edit field
my $valueEntry = $frame[4]->Entry(-textvariable => \$attributeValue)->pack(-side => "top", -fill => "x");
# create the update attribute button
my $updateAttributeB = $frame[4]->Button(-text => "Update Attribute", -command => \&updateAttribute, -state => "disabled")->pack();

# create the new attributes list
my $newAttributeLB = $frame[9]->Scrolled("Listbox", -selectmode => "single", -scrollbars => "osoe", -exportselection => 0)->pack(-side => "top", -fill => "y");
$newAttributeLB->bind("<Button-1>", \&selectNewAttribute);

# populate the new attributes list
foreach my $arg (sort keys %defaultSettings)
{
  $newAttributeLB->insert("end", $arg);
}

# create the status text area
my $statusField = $frame[10]->Scrolled("Text", -scrollbars => "osoe", -state => "disabled", -wrap => "word", -height => "5")->pack(-side => "left", -fill => "x");
# create the tags needed for the status area
$statusField->tagConfigure("error", -foreground => "red");

MainLoop;

sub doExit
{
  if ($changesUnsaved)
  {
    # we can't exit unless they want to lose the changes
    # this code branch should not be reached due to the current implementation of Save/Exit being disabled, etc.
  }
  else
  {
    exit;
  }
}

sub saveFile
{
  # make sure the config version string exists.
  if (!exists $configIni{defaults}{$cfgVersionStr})
  {
    $configIni{defaults}{$cfgVersionStr} = $cfgVersion;
  }

  # write the config file out.
  tied(%configIni)->RewriteConfig or die "Error: Writing config file '$settings{ConfigFileName}' failed!  $!\n";

  displayStatus("Saving config file '$settings{ConfigFileName}': ok");

  $changesUnsaved = 0;

  # enable the Exit menu option
  $menub->entryconfigure(2, -state => "normal");
  # disable the Save menu option
  $menub->entryconfigure(0, -state => "disabled");
}

sub signalSaveNeeded
{
  $changesUnsaved = 1;
  # enable the Save menu option
  $menub->entryconfigure(0, -state => "normal");
  # disable the Exit menu option
  $menub->entryconfigure(2, -state => "disabled");
}

sub selectProfile
{
  my $sel = $profileLB->curselection();
  if ($sel ne "")
  {
    $profile = $profileLB->get($sel);
    displayStatus("Selected Profile: " . $profile);

    # now populate the attributes list box after first clearing it of any old values.
    $attributeLB->delete(0, 'end');
    foreach my $attribute (sort keys %{$configIni{$profile}})
    {
      if ($attribute ne $cfgVersionStr)  # don't show the user the config version string.
      {
        $attributeLB->insert("end", $attribute);
      }
    }

    # enable the delete profile button
    $deleteProfileB->configure(-state => "normal");

    # disable the update attribute button
    $updateAttributeB->configure(-state => "disabled");

    # reset the attribute value field to nothing.
    $attributeValue = "";
  }
}

sub createProfile
{
  if ($profile)
  {
    if (!exists $configIni{$profile})
    {
      displayStatus("Creating Profile [$profile]...");
      $configIni{$profile} = {};

      # now display the profiles available.
      $profileLB->delete(0, 'end');
      for my $profileName (sort keys %configIni)
      {
        $profileLB->insert("end", $profileName);
      }
      
      signalSaveNeeded();
    }
    else
    {
      displayStatus("Profile [$profile] already exists.  Not creating.");
    }
  }
  else
  {
    displayError("You must specify a profile to create!");
  }
}

sub deleteProfile
{
  if ($profile)
  {
    if ($profile ne "defaults")
    {
      displayStatus("Deleting Profile [$profile]");

      signalSaveNeeded();

      delete $configIni{$profile};
      $profile = "";

      # remove all the profile entries and re-populate
      $profileLB->delete(0, "end");
      for my $profile (sort keys %configIni)
      {
        $profileLB->insert("end", $profile);
      }

      # remove all attribute entries
      $attributeLB->delete(0, 'end');

      # make sure attribute value is empty
      $attributeValue = "";

      # disable the update attribute button
      $updateAttributeB->configure(-state => "disabled");

      # disable the delete profile button
      $deleteProfileB->configure(-state => "disabled");

      # disable the delete attribute button
      $deleteAttributeB->configure(-state => "disabled");
    }
    else
    {
      displayError("You can not delete the [defaults] profile!");
    }
  }
  else
  {
    displayError("You must select a Profile to delete!");
  }
}

sub selectAttribute
{
  my $sel = $attributeLB->curselection();
  if ($sel ne "")
  {
    $attribute = $attributeLB->get($sel);
    $attributeValue = $configIni{$profile}{$attribute};
    displayStatus("Selected Attribute: $attribute, value = '$attributeValue'");

    # enable the update attribute button
    $updateAttributeB->configure(-state => "normal");

    # enable the delete attribute button
    $deleteAttributeB->configure(-state => "normal");
  }
}

sub selectNewAttribute
{
  my $sel = $newAttributeLB->curselection();
  if ($sel ne "")
  {
    $attribute = $newAttributeLB->get($sel);
    if (!$profile)
    {
      displayError("You must select a profile first!");
      return;
    }
    if (!exists $configIni{$profile}{$attribute} || $configIni{$profile}{$attribute} ne $defaultSettings{$attribute})
    {
      $attributeValue = $defaultSettings{$attribute};
      $configIni{$profile}{$attribute} = $attributeValue;
      signalSaveNeeded();

      displayStatus("Created Attribute: $attribute, value = '$attributeValue'");

      # remove all attribute entries and re-populate
      $attributeLB->delete(0, 'end');
      foreach my $attribute (sort keys %{$configIni{$profile}})
      {
        $attributeLB->insert("end", $attribute);
      }

      # enable the update attribute button
      $updateAttributeB->configure(-state => "normal");

      # enable the delete attribute button
      $deleteAttributeB->configure(-state => "normal");
      
      # unselect the New Attribute selection
      $newAttributeLB->selectionClear($sel);
    }
  }
}

sub deleteAttribute
{
  delete $configIni{$profile}{$attribute};

  # remove all attribute entries and re-populate
  $attributeLB->delete(0, 'end');
  foreach my $attribute (sort keys %{$configIni{$profile}})
  {
    $attributeLB->insert("end", $attribute);
  }

  # make sure attribute value is empty
  $attributeValue = "";

  # disable the update attribute button
  $updateAttributeB->configure(-state => "disabled");

  # disable the delete attribute button
  $deleteAttributeB->configure(-state => "disabled");
}

sub updateAttribute
{
  if ($configIni{$profile}{$attribute} ne $attributeValue)
  {
    signalSaveNeeded();

    # I really should do some validation...
    $configIni{$profile}{$attribute} = $attributeValue;

    # disable the update attribute button
    $updateAttributeB->configure(-state => "disabled");
  }
}

sub displayStatus
{
  my $output = shift;

  # enable writing to the status box
  $statusField->configure(-state => "normal");

  $statusField->insert('end lineend', $output . "\n");
  
  # disable writing to the status box
  $statusField->configure(-state => "disabled");

  $statusField->see('end');
}

sub displayError
{
  my $output = shift;

  # enable writing to the status box
  $statusField->configure(-state => "normal");

  $statusField->insert('end', "Error", "error");

  # disable writing to the status box
  $statusField->configure(-state => "disabled");

  displayStatus(": $output");
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
