#!/usr/bin/perl

use DBI;
use Socket;

open(CONF,"/etc/pluto.conf");
@data=<CONF>;
close(CONF);

foreach $line (@data) {
  chomp($line);
  ($option, $eq, $value) = split(/ /,$line);
  if($option eq "MySqlHost") {
    $DBHOST=$value;
    $FILEHOST=$value;
  } elsif ($option eq "MySqlUser") {
    $DBUSER=$value;
  } elsif ($option eq "MySqlPassword") {
    $DBPASSWD=$value;
  } elsif ($option eq "PK_Device") {
    $PKDEV=$value;
  }
}

$db = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Couldn't connect to database: $DBI::errstr\n";
$gw = "";

`rm -f /var/log/pluto/gc100-conf.log >> /dev/null`;

if($ARGV[0] eq "") {
    loggc("No params\n\tFinding GC100 on the network\n");
    $flag = allias_up();
    if(find_gc100() == 1) {
        loggc("GC100 Found\n");
	$mac = get_gc100mac();
	loggc("Finding IP ");
	$ip = find_ip();
	loggc("[$ip]\n");
	loggc("Finding Installation ");
	$install = get_install();
	loggc("[$install]\n");
	loggc("Finding Template ");
	$dev_templ = get_template();
	loggc("[$dev_templ]\n");
	$sa = "select * from Device where FK_DeviceTemplate='$dev_templ'";
	$state = $db->prepare($sa) or die "first";
	$state->execute() or die "second";
	while($row_x = $state->fetchrow_hashref()) {
		$newmac = $row_x->{'MACaddress'};
		$ret = comp_mac($newmac,$mac);
		if($ret == 1) {
			loggc("The device allready exist in the database\n");
			$state->finish();
			$db->disconnect();
			exit(2);
		}
	}
	$state->finish();
	loggc("Creating Device...\n");
	open(F,">temp_exec.sh");
	print F "#!/bin/bash\n";
	print F "./CreateDevice -i $install -d $dev_templ -I $ip -M $mac -C $PKDEV -n > gc100_temp.file\n";
	close(F);
	`chmod +x temp_exec.sh`;
	`./temp_exec.sh`;
	`rm -f temp_exec.sh`;
	loggc("Done\n");
	open(FILE, "gc100_temp.file");
        @data = <FILE>;
        $Device_ID = $data[0];
        chomp($Device_ID);
	print "$Device_ID\n";	
        close(FILE);
	`rm -f gc100_temp.file`;
	system("/usr/pluto/bin/MessageSend localhost 0 0 2 24 26 $Device_ID");
	loggc("Configuring GC100 via Web...");
	configure_webgc($ip);
	loggc("Done\n");
	if($flag == 1) {
	    allias_down();
	}
    } else {
        loggc("Gc100 as default not found\n");
	exit(1);
    }
} else {
exit(0);
    loggc("With params\nFinding GC100 on the network\n");
    if($ARGV[1] eq "") {
        if(find_gc100() == 1) {
            loggc("GC100 found\n");
    	    $flag = allias_up();
	    $mac = get_gc100mac();
	    $ip = find_ip();
	    update_db();
	} else {
	    loggc("Gc100 as default not found\n");
	    exit(1);
	}
    }
    $ip = $ARGV[1];
    $flag = allias_up();
    configure_webgc($ip);
    if($flag == 1) {
        allias_down();
    }
}

$db->disconnect();

sub comp_mac {
	$fst = shift;
	$snd = shift;
	@f1 = split(/\:/,$fst);
	@f2 = split(/\:/,$snd);
	print "@f1\n";
	print "@f2\n---\n";
	if(hex($f1[0]) eq hex($f2[0]) && hex($f1[1]) eq hex($f2[1]) && hex($f1[2]) eq hex($f2[2]) && hex($f1[3]) eq hex($f2[3]) && hex($f1[4]) eq hex($f2[4]) && hex($f1[5]) eq hex($f2[5])) {
		return 1;
	} else {
		return 0;
	}
}

sub ip_2_long {
  $local_ip = shift;
  $local_long = "";
  @frag = split(/\./,$local_ip);
  for($i=0;$i<4;$i=$i+1) {
    if($frag[$i] < 10) {
      $frag[$i] = "00".$frag[$i];
    } elsif($frag[$i] < 100) {
      $frag[$i] = "0".$frag[$i];
    }
  }
  return $frag[0].$frag[1].$frag[2].$frag[3];
}

sub long_2_ip {
  $local_long = shift;
  $start = 0;
  for($i=0;$i<4;$i=$i+1) {
    $dat[$i] = substr($local_long,$start,3);
    $dat[$i] = $dat[$i] / 1;
    $start = $start + 3;
  }
  
  return $dat[0].".".$dat[1].".".$dat[2].".".$dat[3];
}

sub sqlexec {
  $local_line = shift;
  $st = $db->prepare($local_line);
  $st->execute();
  $i=0;
  while($local_row = $st->fetchrow_hashref()) {
    $outdata[$i]=$local_row;
    $i = $i + 1;
  }         
  $st->finish();
  return @outdata;
}

sub get_install {
  open(FILE,"/etc/pluto.conf");
  @data=<FILE>;
  close(FILE);
  foreach $line (@data) {
    chomp($line);
    ($var,$value) = split(/=/,$line);
    @frag = split(/ /,$var);
    $var = $frag[0];
    @frag = split(/ /,$value);
    $value = $frag[1];
    if($var eq "PK_Installation") {
      $sql = "select FK_Installation from Device WHERE PK_Device='$PKDEV'";
      $st = $db->prepare($sql);
      $st->execute();
      if($row = $st->fetchrow_hashref()) {
        return $row->{'FK_Installation'};
      } else {
        return 1;
      }
      $st->finish();
    }
  }
}

sub get_template {
  $sql = "select PK_DeviceTemplate from DeviceTemplate WHERE Description='gc100'";
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
	$st->finish();
	return $row->{'PK_DeviceTemplate'};
  } else {
  	loggc("Invalid Template");
    	exit(4);
  }
}

sub get_gc100mac {
  `curl http://192.168.1.70/Commands.cgi -o gc100mac --silent`;
  `curl http://192.168.1.101/Commands.cgi -o gc100mac --silent`;
  open(FILE,"gc100mac");
  @data = <FILE>;
  close(FILE);
  $line = $data[0];
  @vars = split(/ /,$line);
  $i = 0;
  foreach $line (@vars) {
    if($vars[$i] eq "Address") {
	$ii = $i - 1;
	if($vars[$ii] eq "MAC") {
	    $j = $i + 2;
	    @frag = split(/\</,$vars[$j]);
	    $mac = $frag[0];
	    $mac =~ tr/\-/\:/;
	    loggc("Mac Found $mac\n");
	    system("rm -f gc100mac >> /dev/null");
	    if($mac eq "") {
	      exit(1);
	    }
	    return $mac;
	}
    }
    $i = $i + 1;
  }
}

sub allias_up {
    $sql = "select * from Device_DeviceData where FK_DeviceData='32'";
    $st = $db->prepare($sql);
    $st->execute();
    if($row = $st->fetchrow_hashref()) {
	$data = $row->{'IK_DeviceData'};
	@frag = split(/\|/,$data);
	@frag = split(/\,/,$frag[1]);
	$local_dev = $frag[0];
	($fst, $nd) = split(/\:/,$frag[0]);
	$local_dev = $fst;
	$local_ip = $frag[1];
    } else {
	$local_dev = "eth1"
    }
    $st->finish();
    
    $gw = $local_ip;
    loggc("device:$local_dev ip:$local_ip gw:$gw\n");
    @frag = split(/\./,$local_ip);
    if($frag[0] ne "192" || $frag[1] ne "168" || $frag[2] ne "1") {
        loggc("Making allias\n");
	system("ifconfig $local_dev:100 192.168.1.1 broadcast 255.255.255.255 netmask 255.255.255.0");
	return 1;
    } else {
	return 0;
    }
}

sub allias_down {
  	loggc("Bringing down the allias...");
  `ifconfig $local_dev:100 down`;
  	loggc("Down\n");
}

sub find_gc100 {
	loggc("Finding...\n");
    $host = shift || '192.168.1.70';
    $port = shift || 80;

    $proto = getprotobyname('tcp');

    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr); 
	loggc("Creating socket...");
    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    	loggc("Done\n");
    $var = 0;
	loggc("Trying first ip...");
    connect(SOCKET, $paddr) or $var=1;
    if($var == 0) {
        close SOCKET;
	loggc("Found on first default ip\n");
	return 1;
    }
    loggc("Not found on first ip\n");
    
    $host = shift || '192.168.1.101';
    $port = shift || 80;
           
    $proto = getprotobyname('tcp');
                
    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr);
                        
    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    $varr = 0;
	loggc("Trying second ip...");
    connect(SOCKET, $paddr) or $varr=1;
    if($varr == 0) {
    	loggc("Found on second ip\n");
      close SOCKET;
      return 1;
    }
    	loggc("Not found on second ip\n");
    
    if($var == 1 && $varr == 1) {
      return 0;
    }
}

sub find_ip {
$sql = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData='28'";
$stx = $db->prepare($sql) or die "MySQL sux";
$stx->execute() or die "It sux";
while($row = $stx->fetchrow_hashref()) {
  $line = $row->{'IK_DeviceData'};
  ($staticrange, $dinamicrange) = split(/\,/,$line);
  $sl = length($staticrange);
  $dl = length($dinamicrange);
  if($staticrange ne "" && $dinamicrange ne "" && $sl > 16 && $dl > 16) {
    ($sstart,$send) = split(/-/,$staticrange);
    ($dstart,$dend) = split(/-/,$dinamicrange);
    $sstart = ip_2_long($sstart);
    $send = ip_2_long($send);
    $dstart = ip_2_long($dstart);
    $dend = ip_2_long($dend);
    
    $sql = "SELECT IPaddress FROM Device WHERE IPaddress<>0";
    $st2 = $db->prepare($sql) or die "y";
    $st2->execute() or die "x";
    while($rowx = $st2->fetchrow_hashref()) {
      $fip = ip_2_long($rowx->{'IPaddress'});
      if($fip >= $sstart && $fip <= $send) {
        $flag=0;
        @oldips = split(/\,/,$ips);
        foreach $oldip (@oldips) {
          if($oldip eq $fip) {
            $flag=1;
          }
        }
        if($flag == 0) {
          $ips = $ips.",".$fip;
        }
      }
    }
    $st2->finish();
  }
}
@oldip = split(/\,/,$ips);
foreach $ipeu (@oldip) {
	$x = ip_2_long($ipeu);
	$y = ip_2_long($send);
	if($x > $y) {
		$send = $ipeu;
	}
}
for($ii=$sstart;$ii<=$send;$ii=$ii+1) {
  @oldip = split(/\,/,$ips);
  $flag = 0;
  foreach $line (@oldip) {
    if($ii == $line) {
      $flag = 1;
    }
  }
  if($flag == 0) {
    $main_ip = $ii;
    $ii = $send + 1;
  }
}
$main_ip = long_2_ip($main_ip);
return $main_ip;
}

sub update_db {
    $sql = "UPDATE Devices SET IPaddress='$ip', MACaddress='$mac' WHERE PK_Device='$ARGV[0]'";
    $st = $db->prepare($sql);
    $st->execute();
    $st->finish();
}

sub configure_webgc {
    $local_ip = shift;
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.70/commands.cgi?2=$local_ip&3=255.255.255.0&4=$gw&7=submit\"");
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.101/commands.cgi?2=$local_ip&3=255.255.255.0&4=$gw&7=submit\"");
}

sub loggc {
    $line = shift;
    open(LOG,">>/var/log/pluto/gc100-conf.log");
    print LOG $line;
    print $line;
    close(LOG);
}