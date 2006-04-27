#!/usr/bin/perl
#
# Filename..........: $RCSfile: IEEE1394.pm,v $
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
# Purpose: Functions for the IEEE1394 bus in hotplug-perl
#

package Hotplug::IEEE1394;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_ieee1394_modules
  load_ieee1394_modules
  load_all_ieee1394_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $KERNEL            = `/bin/uname -r`; chop($KERNEL);
my $IEEE1394_MAP_FILE = '/lib/modules/' . $KERNEL . '/modules.ieee1394map';
my @IEEE1394_MOD_MAP;
my $IEEE1394_DEV_DIR  = '/sys/bus/ieee1394/devices';

#
# FuncDsc: Private function to generate an array of IEEE1394 modules
# Param 1: nothing
# Returns: array containing all modules
#
sub get_ieee1394_map() {
  my @array;
  if ( -r $IEEE1394_MAP_FILE ) {
    open(FH,$IEEE1394_MAP_FILE) || die("Unable to open $IEEE1394_MAP_FILE\n");
    while (<FH>) {
      next if ! /^\s*([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s*\n$/;
      push(@array,"$1 $2 $3 $4 $5 $6");
    }
    close(FH);
  }
  return(@array);
}

#
# FuncDsc: Private function to get the modules for an IEEE1394 device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_ieee1394_modules() {
  my ($inDevice) = @_;
  my %hash;
  if ( $inDevice =~ /[0-9a-zA-Z]{16}/ ) {

    # Determine the actual path to the device
    my $devdir = readlink("$IEEE1394_DEV_DIR/$inDevice");
    $devdir =~ s/^(\.\.\/){3}(.*)\/($inDevice)$/\/sys\/$2\/$3/g;
    my $devsubdir = $devdir . "/" . $inDevice . "-0";

    # Initialize and pull all available device characteristics
    my $vendor_id    = '0x000000';
    my $model_id     = '0x000000';
    my $specifier_id = '0x000000';
    my $version      = '0x000000';
    if ( -f "$devdir/vendor_id" )       { $vendor_id    = &Hotplug::get_hex_value_from_file( "$devdir/vendor_id",6 ); }
    if ( -f "$devdir/model_id" )        { $model_id     = &Hotplug::get_hex_value_from_file( "$devdir/model_id",6 ); }
    if ( -f "$devsubdir/specifier_id" ) { $specifier_id = &Hotplug::get_hex_value_from_file( "$devsubdir/specifier_id",6 ); }
    if ( -f "$devsubdir/version" )      { $version      = &Hotplug::get_hex_value_from_file( "$devsubdir/version",6 ); }

    if (! @IEEE1394_MOD_MAP) {
      @IEEE1394_MOD_MAP = &get_ieee1394_map();
    }

    # Step through each module in the modules.ieee1394map
    foreach my $module (@IEEE1394_MOD_MAP) {
      if ( $module =~ /^([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s*$/ ) {
        # Finding the right ieee1394 module for a device is based on a bitmask of
        # a field in the modules.ieee1394map determined by the following pattern:
        #
        # Description         hex    binary           bit
        # ------------------+------+----------------+-------
        # Match Vendor       0x0001 1000000000000000 (bit 0)
        # Match Model        0x0002 0100000000000000 (bit 1)
        # Match Specifier    0x0004 0010000000000000 (bit 2)
        # Match Version      0x0008 0001000000000000 (bit 3)
        # ------------------+------+---+----------------+-------
        my @match = split(//,unpack("b*", pack("V", hex($2))));
        my $match = 1;
        if ( $match[0] == 1 && $match == 1 ) { # Match vendor_id
          unless ( $vendor_id && hex($vendor_id) == hex($3) ) {
            $match = 0;
          }
        }
        if ( $match[1] == 1 && $match == 1 ) { # Match model_id
          unless ( $model_id && hex($model_id) == hex($4) ) {
            $match = 0;
          }
        }
        if ( $match[2] == 1 && $match == 1 ) { # Match specifier
          unless ( $specifier_id && hex($specifier_id) == hex($5) ) {
            $match = 0;
          }
        }
        if ( $match[3] == 1 && $match == 1 ) { # Match version
          unless ( $version && hex($version) == hex($6) ) {
            $match = 0;
          }
        }
        if ( $match == 1 ) { # We have a match
          $hash{$1}++;
        }
      }

    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the IEEE1394_DEV_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_ieee1394_modules() {
  my @IEEE1394_MOD_MAP = &get_ieee1394_map();
  my %hash;
  if ( -d $IEEE1394_DEV_DIR ) {
    opendir(DH, $IEEE1394_DEV_DIR) || die("Unable to open $IEEE1394_DEV_DIR\n");
    for my $device ( grep { /^([0-9a-zA-Z]{16})$/ } readdir(DH) ) {
      %hash = (%hash, &get_ieee1394_modules($device));
    }
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific IEEE1394 device
# Param 1: IEEE1394 device GUID
# Returns: nothing
#
sub load_ieee1394_modules() {
  my ($inDevice) = @_;
  my %hash = &get_ieee1394_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all IEEE1394 devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_ieee1394_modules() {
  my %hash = &get_all_ieee1394_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::IEEE1394 - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::IEEE1394;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the IEEE1394 bus.

=head2 EXPORT

get_all_ieee1394_modules()

  This function returns the required modules for the all the IEEE1394 devices

load_ieee1394_modules($inDevice)

  This function loads the required modules for a specific IEEE1394 device when
  passed the GUID (16-digit hex value) idenfifying the device

load_all_ieee1394_modules()

  This function loads the required modules for the all the IEEE1394 devices

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
