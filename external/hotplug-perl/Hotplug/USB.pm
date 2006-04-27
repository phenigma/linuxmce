#!/usr/bin/perl
#
# Filename..........: $RCSfile: USB.pm,v $
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

package Hotplug::USB;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_usb_modules
  load_usb_modules
  load_all_usb_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA %ENV );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $KERNEL       = `/bin/uname -r`; chop($KERNEL);
my $USB_MAP_FILE = '/lib/modules/' . $KERNEL . '/modules.usbmap';
my @USB_MOD_MAP;
my $USB_DEV_DIR  = '/sys/bus/usb/devices';

#
# FuncDsc: Private function to generate an array of USB modules
# Param 1: nothing
# Returns: array containing all modules
#
sub get_usb_map() {
  my @array;
  if ( -r $USB_MAP_FILE ) {
    open(FH,$USB_MAP_FILE) || die("Unable to open $USB_MAP_FILE\n");
    while (<FH>) {
      next if ! /^\s*([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\n$/;
      push(@array,"$1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13");
    }
    close(FH);
  }
  return(@array);
}

#
# FuncDsc: Private function to get the modules for a USB device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_usb_modules() {
  my ($inDevice) = @_;
  my %hash;
  if ( $inDevice =~ /^([0-9]\-[0-9])$/ || $inDevice =~ /^([0-9]\-[0-9])\:([0-9])\.([0-9])$/ ) {

    # Wait for the sysfs device link to appear
    my $waiting_for_file = 0;
    while ( ! -l "$USB_DEV_DIR/$inDevice" ) {
      if ($waiting_for_file > 4) { &Hotplug::logmsg("ERROR! No $USB_DEV_DIR/$inDevice",1); }
      select(undef, undef, undef, 0.25); # Sleep for 0.25 seconds
      $waiting_for_file++;
    }

    # Determine the actual path to the device
    my $devdir = readlink("$USB_DEV_DIR/$inDevice");
    $devdir =~ s/^(\.\.\/){3}(.*)($inDevice)$/\/sys\/$2/g;

    # Skip hub class devices
    if ( -f "$devdir/bDeviceClass" && ( hex(&Hotplug::get_hex_value_from_file( "$devdir/bDeviceClass",4 )) != hex('0x09') ) ) {

      # Pull all available device characteristics
      my $idVendor; my $idProduct; my $bcdDevice;
      my $bDeviceClass; my $bDeviceSubClass; my $bDeviceProtocol;
      my $bInterfaceClass; my $bInterfaceSubClass; my $bInterfaceProtocol;
      if ( -f "$devdir/idVendor" )                     { $idVendor           = &Hotplug::get_hex_value_from_file( "$devdir/idVendor",4 ); }
      if ( -f "$devdir/idProduct" )                    { $idProduct          = &Hotplug::get_hex_value_from_file( "$devdir/idProduct",4 ); }
      if ( -f "$devdir/bcdDevice" )                    { $bcdDevice          = &Hotplug::get_hex_value_from_file( "$devdir/bcdDevice",4 ); }
      if ( -f "$devdir/bDeviceClass" )                 { $bDeviceClass       = &Hotplug::get_hex_value_from_file( "$devdir/bDeviceClass",2 ); }
      if ( -f "$devdir/bDeviceSubClass" )              { $bDeviceSubClass    = &Hotplug::get_hex_value_from_file( "$devdir/bDeviceSubClass",2 ); }
      if ( -f "$devdir/bDeviceProtocol" )              { $bDeviceProtocol    = &Hotplug::get_hex_value_from_file( "$devdir/bDeviceProtocol",2 ); }
      if ( -f "$devdir/$inDevice/bInterfaceClass" )    { $bInterfaceClass    = &Hotplug::get_hex_value_from_file( "$devdir/$inDevice/bInterfaceClass",2 ); }
      if ( -f "$devdir/$inDevice/bInterfaceSubClass" ) { $bInterfaceSubClass = &Hotplug::get_hex_value_from_file( "$devdir/$inDevice/bInterfaceSubClass",2 ); }
      if ( -f "$devdir/$inDevice/bInterfaceProtocol" ) { $bInterfaceProtocol = &Hotplug::get_hex_value_from_file( "$devdir/$inDevice/bInterfaceProtocol",2 ); }

      # Generate the USB_MOD_MAP if necessary
      if ( ! @USB_MOD_MAP ) {
        @USB_MOD_MAP = &get_usb_map();
      }

      # Step through each module in the modules.usbmap
      foreach my $module (@USB_MOD_MAP) {
        if ( $module =~ /^([0-9a-zA-Z\-_]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+)/ ) {
          # Finding the right usb module for a device is based on a bitmask of
          # a field in the modules.usbmap determined by the following pattern:
          #
          # Description         hex    binary           bit
          # ------------------+------+----------------+-------
          # Match Vendor       0x0001 1000000000000000 (bit 0)
          # Match Product      0x0002 0100000000000000 (bit 1)
          # Match Dev Low      0x0004 0010000000000000 (bit 2)
          # Match Dev High     0x0008 0001000000000000 (bit 3)
          # Match Dev Class    0x0010 0000100000000000 (bit 4)
          # Match Dev Subclass 0x0020 0000010000000000 (bit 5)
          # Match Dev Protocol 0x0040 0000001000000000 (bit 6)
          # Match Int Class    0x0080 0000000100000000 (bit 7)
          # Match Int Subclass 0x0100 0000000010000000 (bit 8)
          # Match Int Protocol 0x0200 0000000001000000 (bit 9)
          # ------------------+------+---+----------------+-------
          my @match = split(//,unpack("b*", pack("V", hex($2))));
          my $match = 1;
          if ( $match[0] == 1 && $match == 1 ) { # Match idVendor
            unless ( $idVendor && hex($idVendor) == hex($3) ) {
              $match = 0;
            }
          }
          if ( $match[1] == 1 && $match == 1 ) { # Match idProduct
            unless ( $idProduct && hex($idProduct) == hex($4) ) {
              $match = 0;
            }
          }
          if ( $match[2] == 1 && $match == 1 ) { # Match bcdDevice low
            unless ( $bcdDevice && hex($bcdDevice) > hex($5) ) {
              $match = 0;
            }
          }
          if ( $match[3] == 1 && $match == 1 ) { # Match bcdDevice high
            unless ( $bcdDevice && hex($bcdDevice) < hex($6) ) {
              $match = 0;
            }
          }
          if ( $match[4] == 1 && $match == 1 ) { # Match bDeviceClass
            unless ( $bDeviceClass && hex($bDeviceClass) == hex($7) ) {
              $match = 0;
            }
          }
          if ( $match[5] == 1 && $match == 1 ) { # Match bDeviceSubClass
            unless ( $bDeviceSubClass && hex($bDeviceSubClass) == hex($8) ) {
              $match = 0;
            }
          }
          if ( $match[6] == 1 && $match == 1 ) { # Match bDeviceProtocol
            unless ( $bDeviceProtocol && hex($bDeviceProtocol) == hex($9) ) {
              $match = 0;
            }
          }
          if ( $match[7] == 1 && $match == 1 ) { # Match bInterfaceClass
            unless ( $bInterfaceClass && hex($bInterfaceClass) == hex($10) ) {
              $match = 0;
            }
          }
          if ( $match[8] == 1 && $match == 1 ) { # Match bInterfaceSubClass
            unless ( $bInterfaceSubClass && hex($bInterfaceSubClass) == hex($11) ) {
              $match = 0;
            }
          }
          if ( $match[9] == 1 && $match == 1 ) { # Match bInterfaceProtocol
            unless ( $bInterfaceProtocol && hex($bInterfaceProtocol) == hex($12) ) {
              $match = 0;
            }
          }
          if ( $match == 1 ) { # We have a match
            $hash{$1}++;
          }
        }
      }
    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the USB_DEV_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_usb_modules() {
  @USB_MOD_MAP = &get_usb_map();
  my %hash;
  if ( -d $USB_DEV_DIR ) {
    opendir(DH, $USB_DEV_DIR) || die("Unable to open $USB_DEV_DIR\n");
    for my $device ( grep { /^([0-9]+\-[0-9]+)$/ } readdir(DH) ) {
      %hash = (%hash, &get_usb_modules($device));
    }
    rewinddir(DH);
    for my $device ( grep { /^([0-9]+\-[0-9]+\:[0-9]+\.[0-9]+)$/ } readdir(DH) ) {
      %hash = (%hash, &get_usb_modules($device));
    }
    closedir(DH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific USB device
# Param 1: USB device
# Returns: nothing
#
sub load_usb_modules() {
  my ($inDevice) = @_;
  my %hash = &get_usb_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all USB devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_usb_modules() {
  my %hash = &get_all_usb_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::USB - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::USB;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the USB bus.

=head2 EXPORT

get_all_usb_modules()

  This function returns the required modules for the all the USB devices

load_usb_modules($inDevice)

  This function loads the required modules for a specific USB device when
  passed a device identifier in one of two forms: 2-1 or  2-1:1.0  ( See
  /sys/bus/usb/devices for more examples)

load_all_usb_modules()

  This function loads the required modules for the all the USB devices

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
