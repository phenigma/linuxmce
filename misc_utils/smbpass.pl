#!/usr/bin/perl

use Crypt::SmbHash qw(lmhash nthash);
$passwd = "thepassword";
$lmnt = lmhash(passwd) . ":" . nthash(passwd);
print "$lmnt\n";
