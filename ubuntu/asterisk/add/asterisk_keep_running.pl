#!/usr/bin/perl

use strict;
use diagnostics;
use IO::Socket;

my %VOIP_SERVERS = ();
my $LOG_MESSAGE = "";
my $TROUBLE_LEVEL = 0;
my $SECONDS = 0;

my $ast_run = &check_asterisk_running();
sleep(5);
$ast_run += &check_asterisk_running();
unless($ast_run eq 2)
{
    &write_log("Asterisk is NOT running will try to start");
    system("/etc/init.d/asterisk start");
    sleep(5);
    $ast_run = &check_asterisk_running();
    if($ast_run != 1)
    {
        $TROUBLE_LEVEL = 7;
        &write_log("Asterisk STILL NOT running, check /var/log/asterisk/full for details");
        &send_message("Asterisk is not running, voip will be unavailable");
    }
}
sleep(5);
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
}

while (1 eq 1)
{
    my $rin = '';
    $TROUBLE_LEVEL = 0;
    vec($rin,fileno(STDIN),1) = 1;
    my $nfound = select($rin, undef, undef, 1);
    if($nfound > 0)
    {
        my $line = <STDIN>;
        chomp($line);
        $line =~ s/^.{16}(.+)$/$1/;
        if($line =~ /Raw Hangup/i)
        {
            $TROUBLE_LEVEL = 2;
        }
        if($line =~ /TOO LAGGED/)
        {
            $TROUBLE_LEVEL = 4;
        }
        if($line =~ /UNREACHABLE/)
        {
            $TROUBLE_LEVEL = 4;
        }
        if($line =~ /Registration for .+? timed out/)
        {
            $TROUBLE_LEVEL = 4;
        }
        if($line =~ /REACHABLE/)
        {
            $TROUBLE_LEVEL = 1;
        }
        if($line =~ /Call accepted by/i)
        {
            $TROUBLE_LEVEL = 1;
        }
        if($line =~ /Call rejected by/i)
        {
            $TROUBLE_LEVEL = 5;
        }
        if($line =~ /is circuit-busy/i)
        {
            $TROUBLE_LEVEL = 3;
        }
        if($line =~ /No such host/i)
        {
            $TROUBLE_LEVEL = 6;
        }
        if($line =~ /Host .+? not found/i)
        {
            $TROUBLE_LEVEL = 6;
        }
        if($line =~ /Got a FRAME_CONTROL/i)
        {
            $TROUBLE_LEVEL = 2;
        }
        if($line =~ /Accepting AUTHENTICATED call from/i)
        {
            $TROUBLE_LEVEL = 1;
        }
        if($line =~ /Executing Dial/i)
        {
            $TROUBLE_LEVEL = 1;
        }
        if($TROUBLE_LEVEL > 0)
        {
            &write_log($line);
            $LOG_MESSAGE = "";
            if(($TROUBLE_LEVEL == 2) || ($TROUBLE_LEVEL == 4) || ($TROUBLE_LEVEL == 6))
            {
                &check_network_conditions();
                if($TROUBLE_LEVEL == 6)
                {
                    &write_log("Will try to restart asterisk because of possible error");
                    system("/usr/sbin/asterisk -rx 'reload'");
                    #system("/etc/init.d/asterisk stop");
                    #system("/etc/init.d/asterisk start");
                    $ast_run = &check_asterisk_running();
                    if($ast_run != 1)
                    {
                        $TROUBLE_LEVEL = 7;
                        &write_log("Asterisk STILL NOT running, check /var/log/asterisk/full for details");
                        &send_message("Asterisk is not running, voip will be unavailable");
                    }
                    unless($LOG_MESSAGE eq "")
                    {
                        &send_message($LOG_MESSAGE);
                    }
                }
            }
            if($TROUBLE_LEVEL == 3)
            {
                &send_message("The line is busy");
            }
            if($TROUBLE_LEVEL == 5)
            {
                &send_message("The number you dialed is not available");
            }
        }
    }
    else
    {
        $SECONDS ++;
        if($SECONDS % 60 == 0)
        {
            `chmod -R ug+rx /var/lib/asterisk/sounds/voicemail`;
        }
        #check in about 5 minutes (may be more because not ticking when performing test)
        if($SECONDS > 300)
        {
            $SECONDS = 0;
            &write_log("Still alive...");

            $ast_run = &check_asterisk_running();
            unless($ast_run eq 1)
            {
                &write_log("Asterisk is NOT running will try to start");
                system("/usr/sbin/asterisk -rx 'stop now'");
                system("/etc/init.d/asterisk start");
                sleep(5);
                $ast_run = &check_asterisk_running();
                if($ast_run != 1)
                {
                    $TROUBLE_LEVEL = 7;
                    &write_log("Asterisk STILL NOT running, check /var/log/asterisk/full for details");
                    &send_message("Asterisk is not running, voip will be unavailable");
                }
            }
            $LOG_MESSAGE = "";
            &check_network_conditions();
            unless($LOG_MESSAGE eq "")
            {
                &send_message($LOG_MESSAGE);
            }
        }
    }
}

sub write_log()
{
    my $msg = shift;
    my $now = localtime();
    $LOG_MESSAGE .= "$msg\n";
    my $LOG_FILE = "/var/log/pluto/asterisk_keep_running.log";
    open(LOGFILE, ">> $LOG_FILE");
    printf(LOGFILE "%s - %s\n",$now ,$msg);
    close(LOGFILE);
}

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
            if(($line[0] =~ /^asterisk$/) && ($line[10] =~ /\/usr\/sbin\/asterisk/))
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
    %VOIP_SERVERS = ();
    open(SIPLIST, "asterisk -rx 'sip show registry'|");
    my $result = 0;
    while(<SIPLIST>)
    {
        chomp;
        my @line=split(/[ \t]+/,$_,4);
        if((defined $line[0]) && (defined $line[3]))
        {
            unless(($line[3] =~ /^State/) || ($line[0] =~ /^Verbosity/) || ($line[0] =~ /^Core/))
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
            unless(($line[4] =~ /^State/) || ($line[0] =~ /^Verbosity/) || ($line[0] =~ /^Core/))
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
    return system("/bin/ping -c 5 $host");
}

sub check_traceroute()
{
    my $host = shift;
    $host="www.google.com" unless(defined $host);
    return system("echo -n -e \"\\n\\n`date`\\n\";/usr/sbin/traceroute $host >> /var/log/asterisk/traceroute.log");
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

    send(PING, $msg, 0, $sendto) == length($msg) or return -1;

    my $MAXLEN = 1024;
    my $TIMEOUT = 5;

    eval {
        local $SIG{ALRM} = sub { die "alarm time out"; };
        alarm $TIMEOUT;

        while (1)
        {
            my $recvfrom = recv(PING, $msg, $MAXLEN, 0) or return -1;
            my ($port, $ipaddr) = sockaddr_in($recvfrom);
            my $respaddr = inet_ntoa($ipaddr);
            print "Response from $respaddr : $port\n";
            $result = 0;
        }
    };
    return $result;
}

sub check_network_conditions()
{
    $voip_unreg = &check_asterisk_register();
    if($TROUBLE_LEVEL > 0)
    {
        `/bin/sh -c '/usr/sbin/tcpdump -vv -n not net 192.168.80.0/24 &' >> /var/log/asterisk/tcpdumps.log`;
    }
    if(($voip_unreg != 0) && (keys(%VOIP_SERVERS) > 0))
    {
        foreach my $prov (keys %{$VOIP_SERVERS{'sip'}})
        {
            unless($VOIP_SERVERS{'sip'}{$prov} =~ /^Registered/)
            {
                my ($host,$port) = split(/[:]/,$prov);
                if($VOIP_SERVERS{'sip'}{$prov} =~ /^Rejected/)
                {
                    $TROUBLE_LEVEL=7;
                    &write_log("    HOST $host rejects you, please check voip account details: username/password/payments/etc");
                }
                if(&check_network_ping($host) != 0)
                {
                    &write_log("    HOST $host unreachable by regular ping");
                }
                &check_traceroute($host);
                &write_log("    ---");
            }
        }
        foreach my $prov (keys %{$VOIP_SERVERS{'iax'}})
        {
            unless($VOIP_SERVERS{'iax'}{$prov} =~ /^Registered/)
            {
                my ($host,$port) = split(/[:]/,$prov);
                if($VOIP_SERVERS{'iax'}{$prov} =~ /^Rejected/)
                {
                    $TROUBLE_LEVEL=7;                
                    &write_log("    HOST $host rejects you, please check voip account details: username/password/payments/etc");
                }
                if(&check_iax_ping($host,$port) != 0)
                {
                    &write_log("    HOST $host unreachable by iax ping, maybe a firewall ?");
                    if(&check_network_ping($host) != 0)
                    {
                        &write_log("    HOST $host unreachable by regular ping");
                    }
                    &check_traceroute($host);
                }
                &write_log("    ---");
            }
        }
    }
    if($TROUBLE_LEVEL > 0)
    {
        sleep(5);
        `killall tcpdump`;
        `echo -n -e "\n\n\n" >> /var/log/asterisk/tcpdumps.log`;
    }
}

sub send_message()
{
    my $msg = shift;
    `/usr/pluto/bin/MessageSend dcerouter -targetType template 0 34 2 59 13 \"$msg\" 30 $TROUBLE_LEVEL`;
    &write_log("Will send message \n$msg\n\n");
}
