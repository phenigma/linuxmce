#
# Filename..........: $RCSfile: Hotplug.pm,v $
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
# Purpose: This is the general Perl module containing shared functions for the
#          other perl modules.
#

package Hotplug;
$VERSION = '0.01';
@EXPORT = qw(
  generate_blacklist
  get_hex_value_from_file
  get_text_value_from_file
  run_hotplug_scripts
  load_module
);
require Exporter;
@ISA = ( Exporter );

use vars qw( $VERSION @EXPORT @ISA );
use 5.008004;
use strict;
use warnings;

#
# Variables needed for this module
#
my $HOTPLUG_DIR    = '/etc/hotplug-perl';           # Location of the main hotplug-perl directory
my $HOTPLUGD_DIR   = '/etc/hotplug.d';              # Location of the per-device hotplug scripts
my $BLACKLIST_FILE = $HOTPLUG_DIR . '/blacklist';   # The primary hotplug-perl blacklist
my $BLACKLIST_DIR  = $HOTPLUG_DIR . '/blacklist.d'; # A directory containing other blacklists
my $MODPROBE       = '/sbin/modprobe';              # The location of the modprobe command
my $SYSMODULE      = '/sys/module';                 # The sysfs directory showing the loaded modules
my $LOGFILE        = '/var/log/hotplug-perl.log';   # The logfile used by the logmsg() subroutine
my $ENABLE_LOGGING = 1;                             # 1 to log messages or comment out to disable logging
my %BLACKLIST;

#
# FuncDsc: Simple logging system
# Param 1: string containing log message
# Param 2: value '1' to exit after message
# Returns: nothing
#
sub logmsg() {
  my ($msg,$exit) = @_;
  if ($ENABLE_LOGGING == '1') {
    if ( open(FH,">>$LOGFILE") ) {
      if ( $ENV{'SEQNUM'} ) {
        print FH $ENV{'SEQNUM'} . ": $msg\n";
      } else {
        print FH "NOSEQ: $msg\n";
      }
      close(FH);
    }
  }
  if ($exit) { exit; }
}

#
# FuncDsc: Generates a hash containing a unified module blacklist
# No Parameters
# Returns: hash containing blacklist with modules as keys
#
sub generate_blacklist() {
  my @files = ($BLACKLIST_FILE);
  my @dirs  = ($BLACKLIST_DIR);
  if ( -d "/etc/hotplug/blacklist.d" ) {
    push(@dirs, "/etc/hotplug/blacklist.d");
  }
  if ( -f "/etc/hotplug/blacklist" ) {
    push(@files, "/etc/hotplug/blacklist");
  }
  my %hash;
  foreach my $dir (@dirs) {
    opendir(DH, $dir) || &logmsg("Unable to open $dir\n");
    for my $file ( grep { /^[^\.]/ } readdir(DH) ) {
      push(@files,"$dir/$file");
    }
    closedir(DH);
  }
  foreach my $file (@files) {
    if ( -r $file ) {
      open(FH,$file) || die("Unable to open $file\n");
      while (<FH>) {
        next if /^#/;
        next unless /([0-9a-zA-Z_\-]+)\n$/;
        $hash{$1}++;
      }
      close(FH);
    }
  }
  return(%hash);
}

#
# FuncDsc: Get hex value from /proc or /sys file
# Param 1: Full path to a file
# Param 2: Length of zero-padded hex value
# Returns: formated string representing hex value (0x0000)
#
sub get_hex_value_from_file() {
  my ($file,$size) = @_;
  my $value = '0x%0' . $size . 'x';
  if ( -r $file ) {
    my $hex_value;
    open(FH,$file) || die("Unable to open $file\n");
    while (<FH>) {
      next if ! /(0x)?([0-9a-fA-F]+)\n$/;
      $hex_value = sprintf($value,hex($2));
    }
    close(FH);
    return($hex_value);
  }
}

#
# FuncDsc: Get text value from /proc or /sys file
# Param 1: Full path to a file
# Returns: string from the file
#
sub get_text_value_from_file() {
  my ($file) = @_;
  if ( -r $file ) {
    my $text_value;
    open(FH,$file) || die("Unable to open $file\n");
    while (<FH>) {
      next if ! /([0-9a-zA-Z_\-]+)\n$/;
      $text_value = $1;
    }
    close(FH);
    return($text_value);
  }
}

#
# FuncDsc: Run helper hotplug scripts
# Param 1: The subsystem to run scripts for
# Returns: nothing
#
sub run_hotplug_scripts() {
  my ($subsystem) = @_;
  if ( -d "$HOTPLUGD_DIR/$subsystem" ) {
    opendir(DH, "$HOTPLUGD_DIR/$subsystem") || die("Unable to open $HOTPLUGD_DIR/$subsystem\n");
    for my $script ( grep { /.*\.hotplug/ } readdir(DH) ) {
      if ( -x "$HOTPLUGD_DIR/$subsystem/$script" ) {
        &logmsg("Running $HOTPLUGD_DIR/$subsystem/$script");
        my @CMD = ( "$HOTPLUGD_DIR/$subsystem/$script", $subsystem );
        system(@CMD);
      }
    }
    closedir(DH);
  }
}

#
# FuncDsc: Load module if not already loaded or in blacklist
# Param 1: module name
# Returns: nothing
#
sub load_modules() {
  my (%modules) = @_;
  unless (%BLACKLIST) {
    %BLACKLIST = &generate_blacklist();
  }
  while ( my ($key,$val) = each(%modules) ) {
    if ( ! $BLACKLIST{$key} ) {
      if ( ! -d "$SYSMODULE/$key" ) {
        &logmsg("Loading $key");
        my @CMD = ( $MODPROBE, "-q", "$key" );
        system(@CMD);
      } else {
        &logmsg("Module $key already loaded");
      }
    }
  }
}

1;
__END__

=head1 NAME

Hotplug - Part of a set of Perl extensions to perform system hotplugging under Linux 2.6

=head1 SYNOPSIS

  use Hotplug;

=head1 DESCRIPTION

This extention was written in an effort to improve the performance of the
hotplug implementation written in Bash.

=head2 EXPORT

generate_blacklist()

  Generates a list of modules to be exempted from loading.

get_hex_value_from_file($file)

  Opens, reads, formats and returns the hex value stored in a file usually
  located in the /proc or /sys filesystems.

get_text_value_from_file($file)

  Opens, reads and returns the text value stored in a file usually located in
  the /proc or /sys filesystems.

load_module($module)

  Loads a module if it is not already loaded or blacklisted.

=head1 SEE ALSO

http://opensource.idealcorp.com/hotplug-perl/

=head1 AUTHOR

Anthony L. Awtrey, <tony@idealcorp.com>

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
