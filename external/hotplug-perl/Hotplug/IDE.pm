#!/usr/bin/perl
#
# Filename..........: $RCSfile: IDE.pm,v $
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
# Purpose: IDE bus functions for hotplug-perl
#

package Hotplug::IDE;
$VERSION = '0.01';
@EXPORT = qw(
  get_all_ide_modules
  load_ide_modules
  load_all_ide_modules
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

require Hotplug;

my $IDE_PROC_DIR = '/proc/ide';
my $IDE_SYS_DIR  = '/sys/block';

#
# FuncDsc: Private function to get the modules for an IDE device
# Param 1: Device identifier
# Returns: hash containing any modules in the key
#
sub get_ide_modules() {
  my ($inDevice) = @_;
  my %hash;
  # Looking for detected devices (IDE_PROC_DIR) with a media type
  # that do not have drivers loaded yet (IDE_SYS_DIR)
  if ( -r "$IDE_PROC_DIR/$inDevice/media" && ! -e "$IDE_SYS_DIR/$inDevice" ) {
    my $media_type = &Hotplug::get_text_value_from_file("$IDE_PROC_DIR/$inDevice/media");
    if ($media_type eq 'disk') {
      $hash{"ide-disk"}++;
    } elsif ($media_type eq 'cdrom') {
      $hash{"ide-cd"}++;
    } elsif ($media_type eq 'tape') {
      $hash{"ide-tape"}++;
    } elsif ($media_type eq 'floppy') {
      $hash{"ide-floppy"}++;
    } else {
      $hash{"ide-generic"}++;
    }
  }
  return(%hash);
}

#
# FuncDsc: This function walks the IDE_PROC_DIR and checks each device for necessary modules
# Param 1: nothing
# Returns: hash containing any modules in the key
#
sub get_all_ide_modules() {
  my %hash;
  if ( -d $IDE_PROC_DIR ) {
    opendir(DH, $IDE_PROC_DIR) || die("Unable to open $IDE_PROC_DIR\n");
    for my $device ( grep { /hd[a-z]/ } readdir(DH) ) {
      %hash = (%hash, &get_ide_modules($device));
    }
    closedir(DH);
  }
  return(%hash);
}

#
# FuncDsc: This function loads the modules for a specific IDE device
# Param 1: IDE device
# Returns: nothing
#
sub load_ide_modules() {
  my ($inDevice) = @_;
  my %hash = &get_ide_modules($inDevice);
  &Hotplug::load_modules(%hash);
}

#
# FuncDsc: This function loads the modules for a all IDE devices
# Param 1: nothing
# Returns: nothing
#
sub load_all_ide_modules() {
  my %hash = &get_all_ide_modules();
  &Hotplug::load_modules(%hash);
}

1;
__END__

=head1 NAME

Hotplug::IDE - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug::IDE;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash. This module focuses on the IDE bus.

=head2 EXPORT

get_all_ide_modules()

  This function returns the required modules for the all the IDE devices

load_ide_modules($inDevice)

  This function loads the required modules for a specific IDE device when
  passed a device name like 'hda' or 'hdc'.

load_all_ide_modules()

  This function loads the required modules for the all the IDE devices

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
