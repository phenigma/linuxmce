#!/usr/bin/perl

$data = <STDIN>;
while ($data) {
	chomp($data);
	($code,$unk,$key,$remote) = split(/ /,$data);
	system("/usr/pluto/bin/MessageSend 127.0.0.1 -targetType template 0 73 1 315 113 \"$key\"");
	$data = <STDIN>;
}