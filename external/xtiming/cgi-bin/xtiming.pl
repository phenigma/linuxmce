#!/usr/bin/perl -w
###############################################################################
# Copyright (c) 2000 Ethan Fischer <allanon@crystaltokyo.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

use strict;
use CGI;
use POSIX;

use lib 'lib';
require 'template.pm';

my $verbose = 0;

# Define some standard stuff.
my $html_vars = {
	package_name  => "xtiming",
	package_major => 3,
	package_minor => 3,
	package_patch => 0,
	toplevel_path => ($ENV{SCRIPT_NAME} =~ m{^(.*?)/cgi-bin/(.*)$})[0],
	script_name   => ($ENV{SCRIPT_NAME} =~ m{^(.*?)/cgi-bin/(.*)$})[1],
};

my $cgi = new CGI;

my $param;
foreach ($cgi->param) {
	if ($cgi->param($_) =~ m{^[\d.]+$}) {
		$$param{$_} = $cgi->param($_);
	}
}
my $mode = calculate_mode($param);

if (!$$mode{valid}) {
	$$mode{WARN} = [ ];
	if ($$mode{low_hsf}) {
		push(@{$$mode{WARN}}, "Horizontal sync frequency below minimum of $$mode{min_hsf}kHz!");
	}
	if ($$mode{high_hsf}) {
		push(@{$$mode{WARN}}, "Horizontal sync frequency above maximum of $$mode{max_hsf}kHz!");
	}
	if ($$mode{low_rr}) {
		push(@{$$mode{WARN}}, "Refresh rate below minimum of $$mode{min_rr}Hz!");
	}
	if ($$mode{high_rr}) {
		push(@{$$mode{WARN}}, "Refresh rate above maximum of $$mode{max_rr}Hz!");
	}
	if ($$mode{high_dcf}) {
		push(@{$$mode{WARN}}, "Dot clock above maximum of $$mode{max_dcf}MHz!");
	}

	# Try to fix the mode with doublescan or interlace.
	my $foundfix = 0;
	if ($$mode{doublescan}) {
		$$param{doublescan} = undef;
		$$param{interlace} = undef;
		my $testmode = calculate_mode($param);
		if ($$testmode{valid}) {
			$foundfix = 1;
			push(@{$$mode{WARN}}, "Turning off doublescan is recommended.");
		}
	} elsif (!$$mode{doublescan}) {
		$$param{doublescan} = 1;
		$$param{interlace} = undef;
		my $testmode = calculate_mode($param);
		if ($$testmode{valid}) {
			$foundfix = 1;
			push(@{$$mode{WARN}}, "Doublescan is recommended.");
		}
	}
	if (!$foundfix && $$mode{interlace}) {
		$$param{doublescan} = undef;
		$$param{interlace} = undef;
		my $testmode = calculate_mode($param);
		if ($$testmode{valid}) {
			$foundfix = 1;
			push(@{$$mode{WARN}}, "Turning off interlace is recommended.");
		}
	} elsif (!$foundfix && !$$mode{interlace}) {
		$$param{doublescan} = undef;
		$$param{interlace} = 1;
		my $testmode = calculate_mode($param);
		if ($$testmode{valid}) {
			$foundfix = 1;
			push(@{$$mode{WARN}}, "Interlace is recommended.");
		}
	}
}

# If we haven't requested monitor specs yet, do so.
$$mode{request_monitor_specs} = defined $$param{have_monitor_specs} ? undef : 1;

print "content-type: text/html\n\n";
my $context = context_new();
context_set_macros($context, $html_vars);
context_set_macros($context, $mode);
print translate_fn(load_file_suffix(), $context);

sub calculate_mode {
	my ($param) = @_;
	my $HST = $$param{hst} ? $$param{hst} : 3.8;
	my $VST = $$param{vst} ? $$param{vst} : 150;
	my $HR  = $$param{hr}  ? $$param{hr}  : 0;
	my $VR  = $$param{vr}  ? $$param{vr}  : 0;
	my $RR  = $$param{rr}  ? $$param{rr}  : 0;
	my $DCF = $$param{dcf} ? $$param{dcf} : 0;
	my $doublescan = $$param{doublescan} ? 1 : 0;
	my $interlace  = $$param{interlace}  ? 1 : 0;

	my $min_hsf = defined($$param{min_hsf}) && length($$param{min_hsf}) ? $$param{min_hsf} : 0;
	my $max_hsf = defined($$param{max_hsf}) && length($$param{max_hsf}) ? $$param{max_hsf} : 999999;
	my $min_rr  = defined($$param{min_rr})  && length($$param{min_rr})  ? $$param{min_rr}  : 0;
	my $max_rr  = defined($$param{max_rr})  && length($$param{max_rr})  ? $$param{max_rr}  : 999999;
	my $max_dcf = defined($$param{max_dcf}) && length($$param{max_dcf}) ? $$param{max_dcf} : 999999;

	# Convert HST and VST from microseconds to seconds.
	$HST *= 0.000001;
	$VST *= 0.000001;

	# Horizontal resolution must be a multiple of eight.
	$HR = round($HR, 8);

	# Convert dot clock from MHz to Hz.
	$DCF *= 1000000;

	# Factor in doublescan and/or interlace.
	$DCF /= $doublescan + 1;
	$RR /= $interlace + 1;

	# Set some magic numbers.
	my ($F1,$F2) = (1.30,1.05);
	my ($HGT1,$HGT2) = (32,32);

	# Precalculate blanking lengths and frame lengths.
	my $HBL = $HGT1 + round($HST * $DCF, 8) + $HGT2;
	my $VBL = POSIX::ceil($VR * ($F2 - 1));
	my $HFL = $HR + $HBL;
	my $VFL = $VR + $VBL;

	# VFL must be odd for interlaced modes.
	$VFL += (1 - $VFL % 2) if $interlace;

	if (!$RR) {
		# Don't do anything here; we recalculate RR below anyway.
	} elsif (!$HR && !$VR) {
		my ($ax, $ay) = ($$param{aspect_x}, $$param{aspect_y});
		if ($ax !~ m{^\d+$} || $ax <= 0) { $ax = 4; }
		if ($ay !~ m{^\d+$} || $ay <= 0) { $ay = 3; }
		my $aspect = $ay / $ax;
		my $A = $RR * $aspect * $F2;
		my $B = $RR * $aspect * $F2 * $HBL;
		my $C = -$DCF;
		$HR = round((-$B + sqrt($B * $B - 4 * $A * $C)) / (2 * $A), 8);
		$VR = int($HR * $aspect);
	} elsif (!$HR) {
		$HR = round($DCF / ($RR * $VFL) - $HBL, 8);
	} elsif (!$VR) {
		$VR = int($DCF / ($RR * $HFL * $F2));
	} else { # default is to recalculate DCF
		$DCF = $RR * ($HR + $HGT1 + $HGT2) * $VFL / (1 - $RR * $HST * $VFL);
	}

	# Calculate the real blanking lengths and frame lengths.
	$HBL = $HGT1 + round($HST * $DCF, 8) + $HGT2;
	$VBL = POSIX::ceil($VR * ($F2 - 1));
	$HFL = $HR + $HBL;
	$VFL = $VR + $VBL;

	# VFL must be odd for interlaced modes.
	$VFL += (1 - $VFL % 2) if $interlace;

	# Recalculate RR, based on the actual values of DCF, VFL, and HFL.
	$RR = ($VFL * $HFL > 0) ? $DCF / ($VFL * $HFL) : 0;

	# VGT1 and VGT2 are calculated, not preset.  Put half of the remaining 
	# VBL (after subtracting VSL) in VGT1 and half in VGT2.  If the remainder 
	# is odd, put the extra tick in VGT2.
	my $VSL = POSIX::ceil($VST * $DCF / $HFL);
	my $VGT1 = int(($VBL - $VSL) / 2);
	my $VGT2 = $VBL - ($VGT1 + $VSL);

	my $modename = sprintf('"%dx%d@%d%s"', $HR, $VR, $RR * ($interlace + 1), $interlace ? "i" : $doublescan ? "d" : "");
	my $mode = {
		MODE       => sprintf('%s%*s%7.2f  %4d %4d %4d %4d   %4d %4d %4d %4d%s%s', 
		                      $modename, 17 - length($modename), "", 
		                      ($doublescan + 1) * $DCF / 1000000, 
		                      $HR, $HR + $HGT1, $HFL - $HGT2, $HFL, 
		                      $VR, $VR + $VGT1, $VFL - $VGT2, $VFL, 
		                      $doublescan ? " doublescan" : "", 
		                      $interlace ? " interlace" : ""),
	  HSF        => round(($doublescan + 1) * $DCF / $HFL / 1000, 0.01),
		HST        => round($HST * 1000000, 0.01),
		VST        => round($VST * 1000000, 0.01),
		HR         => $HR,
		VR         => $VR,
		RR         => $$param{rr}  ? $$param{rr}  : round($RR * ($interlace + 1), 0.01),
		ACTUAL_RR  => round($RR * ($interlace + 1), 0.01),
		DCF        => round(($doublescan + 1) * $DCF / 1000000, 0.01),
		min_hsf    => $min_hsf,
		max_hsf    => $max_hsf,
		min_rr     => $min_rr,
		max_rr     => $max_rr,
		max_dcf    => $max_dcf,
		doublescan => $$param{doublescan} ? "checked" : "",
		interlace  => $$param{interlace} ? "checked" : "",
		aspect_x   => $$param{aspect_x},
		aspect_y   => $$param{aspect_y},
	};

	# Set warning flags.
	$$mode{low_hsf}  = $$mode{HSF} < $$mode{min_hsf} ? 1 : undef,
	$$mode{high_hsf} = $$mode{HSF} > $$mode{max_hsf} ? 1 : undef,
	$$mode{low_rr}   = $$mode{RR} < $$mode{min_rr} ? 1 : undef,
	$$mode{high_rr}  = $$mode{RR} > $$mode{max_rr} ? 1 : undef,
	$$mode{high_dcf} = $$mode{DCF} > $$mode{max_dcf} ? 1 : undef,
	$$mode{valid}    = ($$mode{low_hsf} || $$mode{high_hsf} || $$mode{low_rr} || $$mode{high_rr} || $$mode{high_dcf}) ? undef : 1;

	return $mode;
}

sub round {
	my ($num,$dec) = @_;
	return int($num / $dec) * $dec;
}

sub load_file {
	my ($filename) = @_;
	my $html = "";
	if (open(FP, $filename)) {
		my $delim = $/;
		undef $/;
		$html = <FP>;
		$/ = $delim;
		close(FP);
	}
	return $html;
}

sub load_file_suffix {
	my ($suffix) = @_;
	my $filename = ($ENV{SCRIPT_NAME} =~ m{([^/]+)[.][^.]+$})[0];
	if (defined $suffix) {
		$filename = "html/$filename-$suffix.html";
	} else {
		$filename = "html/$filename.html";
	}
	return load_file($filename);
}
