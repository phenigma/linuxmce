#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=LIRC_Remote_Controls;host=10.0.0.150;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

while($cmd ne "exit") {
	print "LIRC-map: ";
	$cmd = <STDIN>;
	chomp($cmd);
	@argvs = split(/ /,$cmd);
	$cmd = $argvs[0];
	if($cmd eq "list") {
		if($argvs[1] ne "") {
			list_mappings($argvs[1]);
		} else {
			list_mappings();
		}
	} elsif($cmd eq "help") {
		print_help();
	} elsif($cmd eq "add") {
		add_map($argvs[1],$argvs[2]." ".$argvs[3]." ".$argvs[4]." ".$argvs[5]);
	} elsif($cmd eq "clear") {
		dump_database();
	} elsif($cmd eq "token") {
		add_token($argvs[1],$argvs[2]);
	} else {
		print_help();
	}
}

$db->disconnect();

sub add_token {
	$comment = shift;
	$token = shift;
	
	if($comment eq "" || $token eq "") {
		print_help();
		return;
	}
	
	while($token =~ m/\'/) {
		$token =~ s/\'/`/;
	}

	$sql = "select ID,Tokens from Mappings where Comment='$comment'";
	$state = $db->prepare($sql);
	$state->execute();
	if($row = $state->fetchrow_hashref()) {
		$id = $row->{'ID'};
		$oldtoken = $row->{'Tokens'};
		$state->finish();
		$oldtoken = $oldtoken.$token."\n";
		$sql = "update Mappings set Tokens='$oldtoken' where ID='$id'";
		$state = $db->prepare($sql);
		$state->execute();
		$state->finish();	
	} else {
		print "--> There are no Comment: [$comment] mapped\n";
		return;
	}
}

sub dump_database {
	$sql = "delete from Mappings";
	$state = $db->prepare($sql);
	$state->execute();
	$state->finish();
}

sub add_map {
	$comment = shift;
	$message = shift;
	if($comment eq "" || $message eq "") {
		print_help();
		return;
	}
	while($message =~ m/\'/) {
		$message =~ s/\'//;
	}
	while($comment =~ m/\'/) {
		$comment =~ s/\'//;
	}

	$sql = "select * from Mappings where Comment='$comment' or Message='$message'";
	$state = $db->prepare($sql);
	$state->execute();
	if($row = $state->fetchrow_hashref()) {
		print "--> Mapping allready exist. Either there is the Comment or the Message allready mapped\n";
		return;
		$state->finish();
	} else {
		$state->finish();
		$sql = "insert into Mappings(Comment,Message) VALUES('$comment','$message')";
		$state = $db->prepare($sql);
		$state->execute();
		$state->finish();
	}
}

sub print_help {
	print "Usage:\nlist [comment]- listing all the mapping, or the tokens for the comment\nhelp - for this help\nadd <comment> <message> - adding a new command mapping\ntoken <comment> <token>\n";
}

sub list_mappings {
	$comment = shift;
	if($comment eq "") {
		$sql = "select Comment, Message from Mappings";
		$state = $db->prepare($sql);
		$state->execute();
		while($row = $state->fetchrow_hashref()) {
			$cmnt = $row->{'Comment'};
			$mapp = $row->{'Message'};
			print "Comment=[$cmnt] Message=[$mapp]\n";
		}
		$state->finish();
	} else {
		$sql = "select Tokens from Mappings where Comment='$comment'";
		$state = $db->prepare($sql);
		$state->execute();
		if($row = $state->fetchrow_hashref()) {
			$token = $row->{'Tokens'};
			@lines = split(/\n/,$token);
			foreach $line (@lines) {
				chomp($line);
				print "Comment=[$comment] Token=[$line]\n";
			}
		}
		$state->finish();
	}
}