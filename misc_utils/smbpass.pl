#!/usr/bin/perl

use Crypt::SmbHash qw(lmhash nthash);
$passwd = $ARGV[0];
$lmnt = lmhash($passwd) . ":" . nthash($passwd);
print "$lmnt\n";
