#!/usr/bin/perl
#
# Filename..........: $RCSfile: ISAPNP.pm,v $
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

package Hotplug::ISAPNP;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_isapnp_modules
  load_isapnp_modules
  load_all_isapnp_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $KERNEL          = `/bin/uname -r`; chop($KERNEL);
my $ISAPNP_MAP_FILE = '/lib/modules/' . $KERNEL . '/modules.alias';
my @ISAPNP_MOD_MAP;
my $ISAPNP_DEV_DIR  = '/sys/bus/pnp/devices';

#
# FuncDsc: Private function to generate an array of ISAPNP modules
# Param 1: nothing
# Returns: array containing all modules
#
sub get_isapnp_map() {
  my @array;
  if ( -r $ISAPNP_MAP_FILE ) {
    open(FH,$ISAPNP_MAP_FILE) || die("Unable to open $ISAPNP_MAP_FILE\n");
    while (<FH>) {
      next if ! /^\s*alias\s+pnp:d(PNP[0-9a-fA-f]+)\s+([0-9a-zA-Z\-_]+)\n$/;
      push(@array,"$1 $2");
    }
    close(FH);
    # These PNP IDs Not provided by their respective modules:
    push(@array,"PNP0400 parport_pc");
    push(@array,"PNP0401 parport_pc");
    push(@array,"PNP0510 irtty-sir");
    push(@array,"PNP0511 irtty-sir");
    push(@array,"PNP0700 floppy");
    push(@array,"PNP0800 pcspkr");
    push(@array,"PNP0b00 rtc");
    push(@array,"PNP0303 atkbd");
    push(@array,"PNP0f13 psmouse");
    push(@array,"PNPb02f analog");
  }
  return(@array);
}

#
# FuncDsc: Private function to get the modules for an ISAPNP device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_isapnp_modules() {
  my ($inDevice) = @_;
  my %hash;
  if ( -e "$ISAPNP_DEV_DIR/$inDevice" && -r "$ISAPNP_DEV_DIR/$inDevice/id" ) {
    my $id = &Hotplug::get_text_value_from_file("$ISAPNP_DEV_DIR/$inDevice/id");
    if (! @ISAPNP_MOD_MAP) {
      @ISAPNP_MOD_MAP = &get_isapnp_map();
    }
    foreach my $module (@ISAPNP_MOD_MAP) {
      if ( $module =~ /^\s*($id)\s+([0-9a-zA-Z\-_]+)\s*$/ ) {
        $hash{$2}++;
      }
    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the ISAPNP_DEV_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_isapnp_modules() {
  @ISAPNP_MOD_MAP = &get_isapnp_map();
  my %hash;
  if ( -d $ISAPNP_DEV_DIR ) {
    opendir(DH, $ISAPNP_DEV_DIR) || die("Unable to open $ISAPNP_DEV_DIR\n");
    for my $device ( grep { /[0-9a-fA-F]+:[0-9a-fA-F]+/ } readdir(DH) ) {
      %hash = (%hash, &get_isapnp_modules($device));
    }
    closedir(DH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific ISAPNP device
# Param 1: ISAPNP Device
# Returns: nothing
#
sub load_isapnp_modules() {
  my ($inDevice) = @_;
  my %hash = &get_isapnp_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all ISAPNP devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_isapnp_modules() {
  my %hash = &get_all_isapnp_modules();
  &Hotplug::load_modules(%hash);
}



1;
__END__

=head1 NAME

Hotplug::ISAPNP - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::ISAPNP;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the ISAPNP bus.

=head2 EXPORT

get_all_isapnp_modules()

  This function returns the required modules for the all the ISAPNP devices

load_isapnp_modules($inDevice)

  This function loads the required modules for a specific ISAPNP device when
  passed a device name in the form of two hex values seperated by a colon.

load_all_isapnp_modules()

  This function loads the required modules for the all the ISAPNP devices

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
