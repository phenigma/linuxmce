#!/usr/bin/perl

use DBI;

$db = DBI->connect("dbi:mysql:database=pluto_main;host=localhost;user=root;password=") or die "Couldn't connect to database: $DBI::errstr\n";

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
    $main_ip = $ii;
    $ii = $send + 1;
  }
}
$main_ip = long_2_ip($main_ip);
$installation = get_install();
print "IP=$main_ip on Installation=$installation\n";

#call CREATE DEVICE

@data = sqlexec("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=32");
$row = $data[0];
$devices = $row->{'IK_DeviceData'};
($first,$second) = split(/\|/,$devices);
@frag = split(/\,/,$first);
$liface = $frag[0];
@frag = split(/\,/,$second);
$oiface = $frag[0];
print "$liface-$oiface\n";



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
    $value = $frag[0];
    if($var eq "PK_Installation") {
      if($value eq "") {
        return 1;
      } else {
        return $value;
      }
    }
  }
}