#!/usr/bin/perl

use DBI;
use Socket;


$db = DBI->connect("dbi:mysql:database=pluto_main;host=localhost;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";
$gw = "";

system("rm -f /var/log/pluto/gc100-conf.log >> /dev/null");

if($ARGV[0] eq "") {
    loggc("Finding GC100 on the network\n");
    $flag = allias_up();
    if(find_gc100() == 1) {
        loggc("GC100 Found\n");
	$mac = get_gc100mac();
	if($flag == 1) {
	    allias_down();
	}
	$ip = find_ip();
	$install = get_install();
	$dev_templ = get_template();
	
	@data = sqlexec("select * from Device WHERE Description='gc100'");
	if($data[0]->{'PK_Device'} ne "") {
	  loggc("The device allready exist in the database\n");
	  $db->disconnect();
	  exit(0);
	}
	system("/usr/pluto/bin/CreateDevice -i $install -d $dev_templ -I $ip -M $mac -n\n");
    } else {
        loggc("Gc100 as default not found\n");
	exit(0);
    }
} else {
    loggc("Finding GC100 on the network\n");
    if($ARGV[1] eq "") {
        if(find_gc100() == 1) {
            loggc("GC100 found\n");
    	    $flag = allias_up();
	    $mac = get_gc100mac();
	    $ip = find_ip();
	    update_db();
	} else {
	    loggc("Gc100 as default not found\n");
	    exit(0);
	}
    }
    $ip = $ARGV[1];
    $flag = allias_up();
    if($flag == 1) {
        allias_down();
    }
    configure_webgc();
}


$db->disconnect();

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
      if($value eq "") {
        return 1;
      } else {
        return $value;
      }
    }
  }
}
#sub get_install {
#  $sql = "select FK_Installation from Device WHERE PK_Device='$PKDEV'";
#  $st = $db->prepare($sql);
#  $st->execute();
#  if($row = $st->fetchrow_hashref()) {
#    return $row->{'FK_Installation'};
#  } else {
#    return 1;
#  }
#}
sub get_template {
  $sql = "select PK_DeviceTemplate from DeviceTemplate WHERE Description='gc100'";
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
    return $row->{'PK_DeviceTemplate'};
  } else {
    return 40;
  }
}

sub get_gc100mac {
  system("curl http://192.168.1.70/Commands.cgi -o gc100mac --silent");
  system("curl http://192.168.1.101/Commands.cgi -o gc100mac --silent");
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
	    system("rm -f gc100mac");
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
	$local_ip = "192.168.1.1";
    }
    
    loggc("Using dev=$local_dev ip=$local_ip\n");
    $gw = $local_ip;
    @frag = split(/\./,$local_ip);
    loggc("Fragments [$frag[0]] [$frag[1]] [$frag[2]] [$frag[3]]\n");
    if($frag[0] ne "192" || $frag[1] ne "168" || $frag[2] ne "1") {
        loggc("Making allias\n");
	system("ifconfig $local_dev:100 192.168.1.1 broadcast 255.255.255.255 netmask 255.255.255.0");
	return 1;
    } else {
	return 0;
    }
}

sub allias_down {
  system("ifconfig $local_dev:100 down");
}

sub find_gc100 {

    $host = shift || '192.168.1.70';
    $port = shift || 80;

    $proto = getprotobyname('tcp');

    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr); 

    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    $var = 0;
    connect(SOCKET, $paddr) or $var=1;
    if($var == 0) {
        close SOCKET;
	return 1;
    }
    
    $host = shift || '192.168.1.101';
    $port = shift || 80;
           
    $proto = getprotobyname('tcp');
                
    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr);
                        
    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    $varr = 0;
    connect(SOCKET, $paddr) or $varr=1;
    if($varr == 0) {
      close SOCKET;
      return 1;
    }
    
    if($var == 1 && $varr == 1) {
      return 0;
    }
}

sub find_ip {
@data = sqlexec("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData='28'");
foreach $row (@data) {
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
    
    @data2 = sqlexec("SELECT IPaddress FROM Device WHERE IPaddress<>0");
    foreach $rowx (@data2) {
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
    $main_ip = $ii + 1;
    $ii = $send + 1;
  }
}
$main_ip = long_2_ip($main_ip);
return $main_ip;
}

sub update_db {
    @data = sqlexec("UPDATE Devices SET IPaddress='$ip', MACaddress='$mac' WHERE PK_Device='$ARGV[0]'");
}

sub configure_webgc {
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.70/commands.cgi?2=$ip&3=255.255.255.0&4=$gw&7=submit\"");
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.101/commands.cgi?2=$ip&3=255.255.255.0&4=$gw&7=submit\"");
}

sub loggc {
    $line = shift;
    open(LOG,">>/var/log/pluto/gc100-conf.log");
    print LOG $line;
    close(LOG);
}