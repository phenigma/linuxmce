#!/usr/bin/perl

use DBI;
require HTML::TokeParser;

($campaign,@words) = @ARGV;

if($campaign == "" && $words[0] == "") {
	print "USAGE:\n./SearchGoogle <Campaign> <Search Words>\n";
	exit(1);
}

print "Making search for campaign $campaign using \"@words\"\n";

$db = DBI->connect("dbi:mysql:database=campaign;host=10.0.0.150;user=root;password=") or die "Could not conect to Database";

$sql = "SELECT * from Search WHERE SearchWords='@words' AND FK_Campaign='$campaign'";
$st = $db->prepare($sql);
$st->execute();
if($row = $st->fetchrow_hashref()) {
	print "Search wards allready exist on this campaign: $campaign\n";
	exit(1);
}
$db->do("INSERT INTO Search(FK_Campaign,SearchWords) Values('$campaign','@words')");

$sql = "SELECT * from Search WHERE SearchWords='@words' AND FK_Campaign='$campaign'";
$st = $db->prepare($sql);
$st->execute();
if($row = $st->fetchrow_hashref()) {
	$pk_search = $row->{'PK_Search'};
} else {
	print "Cannot find search number\n";
	exit(1);
}

foreach $x (@words) {
	if($word eq "") {
		$word = $x;
	} else {
		$word = $word."+".$x;
	}
}

$count = 0;
while($out != 1) {
	`wget -q \"http://10.0.0.175/search/index.php?from=$count&range=10&search_string=$word&Submit=Search+Google\" -O SearchGoogle.html`;

	$p = HTML::TokeParser->new("SearchGoogle.html") || die "Can't open: $!";
	$p2 = HTML::TokeParser->new("SearchGoogle.html") || die "Can't open: $!";
	if(my $sexy_token = $p2->get_tag("/form")) {
		my $not_found = $p2->get_trimmed_text("/body");
		@err = split(/ /,$not_found);
		if($err[0] eq "Sorry") {
			print "No results found\n";
			exit(1);
		}
	}

	$cnt = 0;      
	while (my $token = $p->get_tag("a")) {
		$cnt = $cnt + 1;
		my $url = $token->[1]{href} || "-";
		my $text = $p->get_trimmed_text("/a");
		my $token = $p->get_tag("/a");
		while($token =~ m/\'/) {
			$token =~ s/\'/`/;
		}
		my $comment = $p->get_trimmed_text("a");
		while($comment =~ m/\'/) {
			$comment =~ s/\'/`/;
		}
		print ".";
		$db->do("INSERT INTO SearchResults(FK_Search,URL,Comments) Values('$pk_search','$url','$comment')");
	}

	if($cnt < 10) {
		print "Search finished\n";
		exit(1);
	}
	`rm SearchGoogle.html`;
	$count = $count + 10;
	print "$count Results added\n";
}