#!/usr/bin/perl

#check params
unless(defined($ARGV[0]) && defined($ARGV[1]))
{
    print "USAGE :$0 <line_nr> <line_name> <phone_nr>\n";
    exit(-1);
}

my $LINE_NR = $ARGV[0];
my $LINE_NAME = $ARGV[1];

$LINE_NAME =~ s/(\w+)[-]out/$1/;

if (defined($ARGV[2]))
{
	my $PHONE_NR = $ARGV[2];
	`curl 'http://localhost/admin/config.php?display=did&extdisplay=$PHONE_NR/&action=delIncoming' &>/dev/null`;
}

####### del inphonex / incoming router ######
if ( $LINE_NAME eq "inphonex" )
{
 `curl 'http://localhost/admin/config.php?display=did&extdisplay=/&action=delIncoming' &>/dev/null`;
}
#############################################

`curl -o /tmp/curl.log 'http://localhost/admin/config.php?display=routing' &> /dev/null`;

open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
my $OUT_ROUTE = "";
while(<PAGE>)
{
    chomp;
    if($_ =~ /<a\s+[^>]+href[=]\"[^"]+?extdisplay[=]([^"]+)\">\d+\s+$LINE_NAME(|[-]out)<\/a>/)
    {
        $OUT_ROUTE = $1;
    }
}
close(PAGE);

`curl 'http://localhost/admin/config.php?display=routing&extdisplay=$OUT_ROUTE&action=delroute' &>/dev/null`;

`curl 'http://localhost/admin/config.php?display=trunks&extdisplay=OUT_$LINE_NR&action=deltrunk' &>/dev/null`;

`curl 'http://localhost/admin/config.php?display=routing&clk_reload=true' &>/dev/null`;
