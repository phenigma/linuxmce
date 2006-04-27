#!/usr/bin/perl
#
# Filename..........: $RCSfile: INPUT.pm,v $
# Original Author...: Anthony L. Awtrey
# Version...........: $Revision: 1.1 $
# Last Modified By..: $Author: aawtrey $
# Last Modified On..: $Date: 2005/06/06 14:38:35 $
#
# Copyright 2005 I.D.E.A.L. Technology Corporation
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# Purpose: Perl reimplementation of hotplug with a focus on performance
#

package Hotplug::INPUT;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_input_modules
  load_input_modules
  load_all_input_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $KERNEL         = `/bin/uname -r`; chop($KERNEL);
my $INPUT_MAP_FILE = '/lib/modules/' . $KERNEL . '/modules.inputmap';
my @INPUT_MOD_MAP;
my $INPUT_DEV_FILE = '/proc/bus/input/devices';

#
# FuncDsc: Private function to generate an array of INPUT modules
# Param 1: nothing
# Returns: array containing all modules
#
sub get_input_map() {
  my @array;
  if ( -r $INPUT_MAP_FILE ) {
    open(FH,$INPUT_MAP_FILE) || die("Unable to open $INPUT_MAP_FILE\n");
    # module matchBits bustype vendor product version evBits keyBits relBits absBits mscBits ledBits sndBits ffBits driver_info
    while (<FH>) {
      next if ! /^\s*([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9\:]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\n$/;
      push(@array,"$1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13 $14 $15");
    }
    close(FH);
  }
  return(@array);
}

#
# FuncDsc: Private function to determine a bitwise match between two values
# Param 1: Device bit field
# Param 2: Module bit field
# Returns: Either 1 (match) or 0 (no match)
#
sub match_bits() {
  my ($devbits,$modbits) = @_;
  if ( $devbits eq '' ) { return 1; }
  if ( $modbits =~ /:/ || $devbits =~ /:/ ) {
    my @modarray = split(/:/,$modbits);
    my @devarray = split(/:/,$devbits);
    for ( my $i=0; $i<@modarray; $i++ ) {
      unless ( (hex($modarray[$i]) & hex($devarray[$i])) == hex($modarray[$i])) { return 0; }
    }
    return 1;
  } else {
    if ( ( hex($modbits) & hex($devbits) ) == hex($modbits) ) {
      return 1;
    } else {
      return 0;
    }
  }
}

#
# FuncDsc: Private function to get the modules for an INPUT device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_input_modules() {
  my (%inDevice) = @_;
  my %hash;
  if (! @INPUT_MOD_MAP) {
    @INPUT_MOD_MAP = &get_input_map();
  }
  foreach my $module (@INPUT_MOD_MAP) {
    if ( $module =~ /^\s*([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9\:]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)$/ ) {
      # Finding the right input module for a device is based on a bitmask of
      # a field in the modules.inputmap determined by the following pattern:
      #
      # Description         hex    binary           bit
      # ------------------+------+----------------+--------
      # Match Bus          0x0001 1000000000000000 (bit 0)
      # Match Vendor       0x0002 0100000000000000 (bit 1)
      # Match Product      0x0004 0010000000000000 (bit 2)
      # Match Version      0x0008 0001000000000000 (bit 3)
      # Match EV           0x0010 0000100000000000 (bit 4)
      # Match KEY          0x0020 0000010000000000 (bit 5)
      # Match REL          0x0040 0000001000000000 (bit 6)
      # Match ABS          0x0080 0000000100000000 (bit 7)
      # Match MSC          0x0100 0000000010000000 (bit 8)
      # Match LED          0x0200 0000000001000000 (bit 9)
      # Match SND          0x0400 0000000000100000 (bit 10)
      # Match FF           0x0800 0000000000010000 (bit 11)
      # ------------------+------+---+----------------+-------
      my @match = split(//,unpack("b*", pack("V", hex($2))));
      my $match = 1;
      if ( $match[0] == 1 && $match == 1 ) { # Match Bus
        unless ( $inDevice{'Bus'} && $inDevice{'Bus'} eq $3 ) {
          $match = 0;
        }
      }
      if ( $match[1] == 1 && $match == 1 ) { # Match Vendor
        unless ( $inDevice{'Vendor'} && $inDevice{'Vendor'} eq $4 ) {
          $match = 0;
        }
      }
      if ( $match[2] == 1 && $match == 1 ) { # Match Product
        unless ( $inDevice{'Product'} && $inDevice{'Product'} eq $5 ) {
          $match = 0;
        }
      }
      if ( $match[3] == 1 && $match == 1 ) { # Match Version
        unless ( $inDevice{'Version'} && $inDevice{'Version'} eq $6 ) {
          $match = 0;
        }
      }
      if ( $match[4] == 1 && $match == 1 ) { # Match EVbits
        $match = &match_bits($inDevice{'EV'}, $7);
      }
      if ( $match[5] == 1 && $match == 1 ) { # Match KEYbits
        $match = &match_bits($inDevice{'KEY'}, $8);
      }
      if ( $match[6] == 1 && $match == 1 ) { # Match RELbits
        $match = &match_bits($inDevice{'REL'}, $9);
      }
      if ( $match[7] == 1 && $match == 1 ) { # Match ABSbits
        $match = &match_bits($inDevice{'ABS'}, $10);
      }
      if ( $match[8] == 1 && $match == 1 ) { # Match MSCbits
        $match = &match_bits($inDevice{'MSC'}, $11);
      }
      if ( $match[9] == 1 && $match == 1 ) { # Match LEDbits
        $match = &match_bits($inDevice{'LED'}, $12);
      }
      if ( $match[10] == 1 && $match == 1 ) { # Match SNDbits
        $match = &match_bits($inDevice{'SND'}, $13);
      }
      if ( $match[11] == 1 && $match == 1 ) { # Match FFbits
        $match = &match_bits($inDevice{'FF'}, $14);
      }
      if ( hex($2) == 0 && hex($15) == 0 ) {
        $match = 0;
      }
      if ( $match == 1 ) {
        $hash{$1}++;
      }
    }
  }
  return(%hash);
}

#
# FuncDsc: This function reads the INPUT_DEV_FILE and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_input_modules() {
  @INPUT_MOD_MAP = &get_input_map();
  my %hash;
  my %device_hash;
  if ( -f $INPUT_DEV_FILE ) {
    open(FH, $INPUT_DEV_FILE) || die("Unable to open $INPUT_DEV_FILE\n");
    while (<FH>) {
      my $line = $_;
      if ( $line =~ /^I:\s+Bus=([0-9a-fA-F]+)\s+Vendor=([0-9a-fA-F]+)\s+Product=([0-9a-fA-F]+)\s+Version=([0-9a-fA-F]+)\s*\n$/ ) {
        $device_hash{'Bus'}     = "0x$1";
        $device_hash{'Vendor'}  = "0x$2";
        $device_hash{'Product'} = "0x$3";
        $device_hash{'Version'} = "0x$4";
      } elsif ( $line =~ /^N:\s+Name="([^"]+)"\s*\n$/ ) {
        $device_hash{'Name'} = $1;
      } elsif ( $line =~ /^P:\s+Phys=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'Phys'} = $1;
        $device_hash{'Phys'} =~ s/\s+$//g;
      } elsif ( $line =~ /^H:\s+Handlers=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'Handlers'} = $1;
        $device_hash{'Handlers'} =~ s/\s+$//g;
      } elsif ( $line =~ /^B:\s+EV=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'EV'} = $1;
        $device_hash{'EV'} =~ s/\s+$//g;
      } elsif ( $line =~ /^B:\s+KEY=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'KEY'} = $1;
        $device_hash{'KEY'} =~ s/\s+$//g;
        $device_hash{'KEY'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+REL=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'REL'} = $1;
        $device_hash{'REL'} =~ s/\s+$//g;
        $device_hash{'REL'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+ABS=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'ABS'} = $1;
        $device_hash{'ABS'} =~ s/\s+$//g;
        $device_hash{'ABS'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+MSC=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'MSC'} = $1;
        $device_hash{'MSC'} =~ s/\s+$//g;
        $device_hash{'MSC'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+LED=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'LED'} = $1;
        $device_hash{'LED'} =~ s/\s+$//g;
        $device_hash{'LED'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+SND=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'SND'} = $1;
        $device_hash{'SND'} =~ s/\s+$//g;
        $device_hash{'SND'} =~ s/ /:/g;
      } elsif ( $line =~ /^B:\s+FF=([0-9a-zA-Z\ \-_:]+)\s*\n$/ ) {
        $device_hash{'FF'} = $1;
        $device_hash{'FF'} =~ s/\s+$//g;
        $device_hash{'FF'} =~ s/ /:/g;
      } elsif ( $line =~ /^\n$/ ) {
        %hash = (%hash, &get_input_modules(%device_hash));
        %device_hash = ();
      } 
    }
    closedir(FH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific INPUT device
# Param 1: INPUT device described in a hash (see doc below)
# Returns: nothing
#
sub load_input_modules() {
  my (%inDevice) = @_;
  my %hash = &get_input_modules(%inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all INPUT devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_input_modules() {
  my %hash = &get_all_input_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::INPUT - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::INPUT;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the INPUT bus.

The Input device is not enumerated in the /sys filesystem like the other
busses. Therefore to enumerate the initial list, a device hash is
constructed by reading the /proc/bus/input/devices file. The hash should
contain at least:

  $device_hash{'Bus'}     = hex value of the bus id
  $device_hash{'Vendor'}  = hex value of the vendor id
  $device_hash{'Product'} = hex value of the product id
  $device_hash{'Version'} = hex value of the version

These other fields are optional, except when a match must be satisfied using
one of them.

  $device_hash{'Name'}    = text string with the name of the device
  $device_hash{'Phys'}    =
  $device_hash{'EV'}      = Device supplies event data
  $device_hash{'KEY'}     = Device supplies key data
  $device_hash{'REL'}     = Device supplies relative positional data
  $device_hash{'ABS'}     = Device supplies absolute positional data
  $device_hash{'MSC'}     = Device supplies misc data
  $device_hash{'LED'}     = Device supplies led data
  $device_hash{'SND'}     = Device supplies sound data
  $device_hash{'FF'}      = Device supplies ff data

The device hash is only passed to the load_input_modules() function.

=head2 EXPORT

get_all_input_modules()

  This function returns the required modules for the all the INPUT devices

load_input_modules(%inDevice)

  This function loads the required modules for a specific INPUT device when
  passed a %hash containing the following keys:

    Bus
    Vendor
    Product
    Version

load_all_input_modules()

  This function loads the required modules for the all the INPUT devices

=head1 SEE ALSO

http://opensource.idealcorp.com/hotplug-perl/

=head1 AUTHOR

Anthony L. Awtrey, <lt>tony@idealcorp.com<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2005 by I.D.E.A.L. Technology Corporation

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

=cut
