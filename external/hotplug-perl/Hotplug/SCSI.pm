#!/usr/bin/perl
#
# Filename..........: $RCSfile: SCSI.pm,v $
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

package Hotplug::SCSI;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_scsi_modules
  load_scsi_modules
  load_all_scsi_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $SCSI_DEV_DIR  = '/sys/bus/scsi/devices';

#
# FuncDsc: Private function to get the modules for a SCSI device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_scsi_modules() {
  my ($inDevice) = @_;
  my %hash;
  my $waiting_for_file = 0;
  my $media_type;
  while ( ! -r "$SCSI_DEV_DIR/$inDevice/type" ) {
    if ($waiting_for_file > 4) { &Hotplug::logmsg("ERROR! No $SCSI_DEV_DIR/$inDevice/type",1); }
    select(undef, undef, undef, 0.25); # Sleep for 0.25 seconds
    $waiting_for_file++;
  }
  if ( -e "$SCSI_DEV_DIR/$inDevice/type" ) {
    $media_type = &Hotplug::get_text_value_from_file("$SCSI_DEV_DIR/$inDevice/type");
    if ($media_type eq '0') {
      $hash{"sd_mod"}++;
    } elsif ($media_type eq '1') {
      $hash{"st"}++;
    } elsif ($media_type eq '4') {
      $hash{"sr_mod"}++;
    } elsif ($media_type eq '5') {
      $hash{"sr_mod"}++;
      $hash{"sg"}++;
    } elsif ($media_type eq '6') {
      $hash{"sg"}++;
    } elsif ($media_type eq '7') {
      $hash{"sd_mod"}++;
    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the SCSI_DEV_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_scsi_modules() {
  my %hash;
  if ( -d "$SCSI_DEV_DIR" ) {
    opendir(DH, $SCSI_DEV_DIR) || die("Unable to open $SCSI_DEV_DIR\n");
    for my $device ( grep { /[0-9a-fA_F]+:[0-9a-fA_F]+:[0-9a-fA_F]+:[0-9a-fA_F]+/ } readdir(DH) ) {
      %hash = (%hash, &get_scsi_modules($device));
    }
    closedir(DH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific SCSI device
# Param 1: SCSI device
# Returns: nothing
#
sub load_scsi_modules() {
  my ($inDevice) = @_;
  my %hash = &get_scsi_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for all SCSI devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_scsi_modules() {
  my %hash = &get_all_scsi_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::SCSI - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::SCSI;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the SCSI bus.

=head2 EXPORT

get_all_scsi_modules()

  This function returns the required modules for the all the SCSI devices

load_scsi_modules($inDevice)

  This function loads the required modules for a specific SCSI device when
  passed a device identifier composed of 4 hex digits seperated by colons.
  (i.e. 0000:0000:0000:0000  See /sys/bus/scsi/devices for more examples)

load_all_scsi_modules()

  This function loads the required modules for the all the SCSI devices

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
