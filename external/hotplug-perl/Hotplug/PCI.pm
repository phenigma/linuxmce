#!/usr/bin/perl
#
# Filename..........: $RCSfile: PCI.pm,v $
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
# Purpose: Hotplug-Perl PCI functions
#

package Hotplug::PCI;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_pci_modules
  load_pci_modules
  load_all_pci_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $KERNEL       = `/bin/uname -r`; chop($KERNEL);
my $PCI_MAP_FILE = '/lib/modules/' . $KERNEL . '/modules.pcimap';
my @PCI_MOD_MAP;
my $PCI_DEV_DIR  = '/sys/bus/pci/devices';

#
# FuncDsc: Private function to generate an array of PCI modules
# Param 1: nothing
# Returns: array containing all modules
#
sub get_pci_map() {
  my @array;
  if ( -r $PCI_MAP_FILE ) {
    open(FH,$PCI_MAP_FILE) || die("Unable to open $PCI_MAP_FILE\n");
    while (<FH>) {
      next if ! /^\s*([0-9a-zA-Z\-_]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\n$/;
      push(@array,"$1 $2 $3 $4 $5 $6 $7");
    }
    close(FH);
  }
  return(@array);
}

#
# FuncDsc: Private function to get the modules for a PCI device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_pci_modules() {
  my ($inDevice) = @_;
  my $PCI_WILDCARD_ID = '0xffffffff';
  my %hash;
  if ( $inDevice =~ /[^:]*:.*:.*\..*/ ) {
    my $vendor           = &Hotplug::get_hex_value_from_file( "$PCI_DEV_DIR/$inDevice/vendor",8 );
    my $device           = &Hotplug::get_hex_value_from_file( "$PCI_DEV_DIR/$inDevice/device",8 );
    my $subsystem_vendor = &Hotplug::get_hex_value_from_file( "$PCI_DEV_DIR/$inDevice/subsystem_vendor",8 );
    my $subsystem_device = &Hotplug::get_hex_value_from_file( "$PCI_DEV_DIR/$inDevice/subsystem_device",8 );
    my $class            = &Hotplug::get_hex_value_from_file( "$PCI_DEV_DIR/$inDevice/class",8 );
    if (! @PCI_MOD_MAP) {
      @PCI_MOD_MAP = &get_pci_map();
    }
    foreach my $module (@PCI_MOD_MAP) {
      if ( $module =~ /^([0-9a-zA-Z\-_]+) ($vendor|$PCI_WILDCARD_ID) ($device|$PCI_WILDCARD_ID) ($subsystem_vendor|$PCI_WILDCARD_ID) ($subsystem_device|$PCI_WILDCARD_ID) (0x[0-9a-fA-F]+) (0x[0-9a-fA-F]+)/ ) {
        my $temp_class    = sprintf("0x%08x",hex($class) & hex($7));
        if ( $temp_class eq $6 ) {
          $hash{$1}++;
        }
      }
    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the PCI_DEV_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_pci_modules() {
  @PCI_MOD_MAP = &get_pci_map();
  my %hash;
  if ( -d $PCI_DEV_DIR ) {
    opendir(DH, $PCI_DEV_DIR) || die("Unable to open $PCI_DEV_DIR\n");
    for my $device ( grep { /[^:]*:.*:.*\..*/ } readdir(DH) ) {
      %hash = (%hash, &get_pci_modules($device));
    }
    closedir(DH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific PCI device
# Param 1: PCI device
# Returns: nothing
#
sub load_pci_modules() {
  my ($inDevice) = @_;
  my %hash = &get_pci_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all PCI devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_pci_modules() {
  my %hash = &get_all_pci_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::PCI - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::PCI;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the PCI bus.

=head2 EXPORT

get_all_pci_modules()

  This function returns the required modules for the all the PCI devices

load_pci_modules()

  This function loads the required modules for a specific PCI device when
  passed a device identifier in the following form: 0000:00:1d.7 (See
  the /sys/bus/pnp/devices/ directory for other examples).

load_all_pci_modules()

  This function loads the required modules for the all the PCI devices

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
