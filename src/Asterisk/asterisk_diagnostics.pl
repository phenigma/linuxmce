#!/usr/bin/perl

use strict;
use diagnostics;
use IO::Socket;

my %VOIP_SERVERS = ();

&write_log("NEW RUN");

my $ast_run = &check_asterisk_running();
if($ast_run != 1)
{
    &write_log("Asterisk is NOT running will try to start");
    `/etc/init.d/asterisk start` || die "ASTERISK IS NOT INSTALLED";
    $ast_run = &check_asterisk_running();
    if($ast_run != 1)
    {
        &write_log("Asterisk STILL is NOT running, check /var/log/asterisk/full for details");
        &write_log("END RUN\n\n\n");
        exit(1);
    }
}
&write_log("Asterisk is running");

my $voip_unreg = &check_asterisk_register();
&write_log("VoIP list");
foreach my $prov (keys %{$VOIP_SERVERS{'sip'}})
{
    &write_log("    ".$prov." (".$VOIP_SERVERS{'sip'}{$prov}.")");
}
foreach my $prov (keys %{$VOIP_SERVERS{'iax'}})
{
    &write_log("    ".$prov." (".$VOIP_SERVERS{'iax'}{$prov}.")");
}

if($voip_unreg == 0)
{
    &write_log("Everything looks good");
    &write_log("END RUN\n\n\n");
    exit(0);
}

if(&check_network_ping() != 0)
{
    &write_log("PLEASE BE SURE YOU HAVE AN INTERNET CONNECTION");
    &write_log("END RUN\n\n\n");
    exit(1);

}

&write_log("Problems with :");
foreach my $prov (keys %{$VOIP_SERVERS{'sip'}})
{
    unless($VOIP_SERVERS{'sip'}{$prov} =~ /^Registered/)
    {
        my ($host,$port) = split(/[:]/,$prov);
    if($VOIP_SERVERS{'sip'}{$prov} =~ /^Rejected/)
    {
        &write_log("    HOST $host rejects you, please check voip account details: username/password/etc");
    }
    if(&check_network_ping($host) != 0)
    {
        &write_log("    HOST $host unreachable by regular ping")
    }
    if(&check_sip_ping($host) != 0)
    {
        &write_log("    HOST $host unreachable by sip ping")
    }
    &write_log("    --");
    }
}

foreach my $prov (keys %{$VOIP_SERVERS{'iax'}})
{
    unless($VOIP_SERVERS{'iax'}{$prov} =~ /^Registered/)
    {
        my ($host,$port) = split(/[:]/,$prov);
    if($VOIP_SERVERS{'iax'}{$prov} =~ /^Rejected/)
    {
        &write_log("    HOST $host rejects you, please check voip account details: username/password/etc");
    }
    if(&check_network_ping($host) != 0)
    {
        &write_log("    HOST $host unreachable by regular ping")
    }
    if(&check_iax_ping($host,$port) != 0)
    {
        &write_log("    HOST $host unreachable by iax ping")
    }
    &write_log("    --");
    }
}

&write_log("END RUN\n\n\n");
exit(0);

sub check_asterisk_running()
{
    open(PROCLIST, "ps auxww|");
    my $result = 0;
    while(<PROCLIST>)
    {{
        chomp;
        my @line=split(/[ \t]+/,$_,11);
        if((defined $line[0]) && (defined $line[10]))
        {
            if(($line[0] =~ /^asterisk$/) && ($line[10] =~ /^asterisk/))
            {
                $result = 1;
                last;
            }
        }
    }}
    close(PROCLIST);
    return $result;
}

sub check_asterisk_register()
{
    open(SIPLIST, "asterisk -rx 'sip show registry'|");
    my $result = 0;
    while(<SIPLIST>)
    {
        chomp;
        my @line=split(/[ \t]+/,$_,4);
        if((defined $line[0]) && (defined $line[3]))
        {
            unless(($line[3] =~ /^State/) || ($line[0] =~ /^Verbosity/))
            {
                $VOIP_SERVERS{'sip'}{$line[0]} = $line[3];
                unless($line[3] =~ /^Registered$/)
                {
                    $result++;
                }
            }
        }
    }
    close(SIPLIST);
    open(IAXLIST, "asterisk -rx 'iax2 show registry'|");
    while(<IAXLIST>)
    {
        chomp;
        my @line=split(/[ \t]+/,$_,5);
        if((defined $line[0]) && (defined $line[4]))
        {
            unless(($line[4] =~ /^State/) || ($line[0] =~ /^Verbosity/))
            {
                $VOIP_SERVERS{'iax'}{$line[0]} = $line[4];
                unless($line[4] =~ /^Registered$/)
                {
                    $result++;
                }
            }
        }
    }
    close(IAXLIST);
    return $result;
}

sub check_network_ping()
{
    my $host = shift;
    $host="www.google.com" unless(defined $host);
    return system("ping -c 5 $host");
}

sub check_sip_ping()
{
    my $host = shift;
    return 0; #NOT IMPLEMENTED YET
}

sub check_iax_ping()
{
    my $host = shift;
    my $host_port = shift;
    my $result = -1;

    socket(PING, PF_INET, SOCK_DGRAM, getprotobyname("udp"));

    my $src_call = "8000";
    my $dst_call = "0000";
    my $timestamp = "00000000";
    my $outbound_seq = "00";
    my $inbound_seq = "00";
    my $type = "06";
    my $iax_type = "1e";
    my $msg = pack "H24", $src_call . $dst_call . $timestamp . $outbound_seq . $inbound_seq . $type . $iax_type;

    my $ipaddr = gethostbyname($host);
    my $sendto = sockaddr_in($host_port,$ipaddr);

    send(PING, $msg, 0, $sendto) == length($msg) or die "cannot send to $host : $host_port : $!\n";

    my $MAXLEN = 1024;
    my $TIMEOUT = 5;

    eval {
        local $SIG{ALRM} = sub { die "alarm time out"; };
        alarm $TIMEOUT;

        while (1)
        {
            my $recvfrom = recv(PING, $msg, $MAXLEN, 0) or die "recv: $!";
            my ($port, $ipaddr) = sockaddr_in($recvfrom);
            my $respaddr = inet_ntoa($ipaddr);
            print "Response from $respaddr : $port\n";
            $result = 0;
        }
    };
    return $result;
}

sub write_log()
{
    my $now=localtime();
    my $msg = shift;
    my $LOG_FILE = "/var/log/pluto/asterisk_diagnostics.log";
    open(LOGFILE, ">> $LOG_FILE");
    printf(LOGFILE "%s - %s\n",$now ,$msg);
    close(LOGFILE);
}
