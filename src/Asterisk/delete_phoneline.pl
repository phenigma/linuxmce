#!/usr/bin/perl

#check params
unless(defined($ARGV[0]) && defined($ARGV[1]) && defined($ARGV[2]))
{
    print "USAGE :$0 <line_nr> <line_name> <phone_nr>\n";
    exit(-1);
}

my $LINE_NR = $ARGV[0];
my $LINE_NAME = $ARGV[1];
my $PHONE_NR = $ARGV[2];

$LINE_NAME =~ s/(\w+)[-]out/$1/;

system("curl 'http://localhost/pluto-admin/amp/admin/config.php?display=6&extdisplay=OUT_$LINE_NR&action=deltrunk' &>/dev/null ");
system("curl 'http://localhost/pluto-admin/amp/admin/config.php?display=7&extdisplay=$PHONE_NR/&action=delIncoming' &>/dev/null");

system("curl 'http://localhost/pluto-admin/amp/admin/config.php?display=8' > /tmp/curl.log");
open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
my $OUT_ROUTE = "";
while(<PAGE>)
{
    chomp;
    if($_ =~ /<a\s+[^>]+href[=]\"[^"]+?extdisplay[=]([^"]+)\">\d+\s+$LINE_NAME<\/a>/)
    {
        $OUT_ROUTE = $1;
    }
}
close(PAGE);
system("curl 'http://localhost/pluto-admin/amp/admin/config.php?display=8&extdisplay=$OUT_ROUTE&action=delroute' &>/dev/null");
system("curl 'http://localhost/pluto-admin/amp/admin/config.php?display=8&clk_reload=true' &>/dev/null");

