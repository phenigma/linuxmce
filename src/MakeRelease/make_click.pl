#!/usr/bin/perl

use DBI;
use Socket;

$db = DBI->connect("dbi:mysql:database=Simulate_Clicks;host=10.0.0.150;user=root;password=") or die "Could not conect to Database";

do {
  $prox = get_proxy();
} while(check_proxy($prox) == 0);
print "Proxy Found : $prox\n";

$proj = "";

$proj = get_project(1);
if($proj ne "") {
  print "FM project found : $proj\n";
  system("curl -x $prox --connect-timeout 5 -f http://freshmeat.net/projects/$proj/");
}

$proj = "";

$proj = get_project(2);
if($proj ne "") {
  print "SF project found : $proj\n";
  system("curl -x $prox --connect-timeout 5 -f http://sourceforge.net/projects/$proj/");
}
 
$db->disconnect();

print "[DONE]\n";

sub get_project() {
  $type = shift;
  if($type == 1) {
    $sql = "SELECT MAX(Id) FROM FMProjects";
  } elsif($type == 2) {
    $sql = "SELECT MAX(Id) FROM SFProjects";
  }
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
    $projectsize = $row->{'MAX(Id)'};
  }
  $st->finish();
  
  $ran = rand($projectsize);
  ($st,$nd) = split(/\./,$ran);
  if($type == 1) {
    $sql = "SELECT * FROM FMProjects WHERE Id>=$st LIMIT 1";
  } elsif($type == 2) {
    $sql = "SELECT * FROM SFProjects WHERE Id>=$st LIMIT 1";
  }
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
    $project = $row->{'Project'};
  }
  $st->finish();
  return $project;
}

sub check_proxy() {
  $proxy = shift;
  ($ip, $port) = split(/:/,$proxy);
  print "Checking $ip:$port\n";
  $proto = getprotobyname('tcp');
  $iaddr = inet_aton($ip);
  $paddr = sockaddr_in($port, $iaddr);
  
  socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
  $var = 0;
  connect(SOCKET, $paddr) or $var=1;
  if($var == 1) {
    print "Proxy Unavailable [Deleting $ip:$port]\n";
    $sql = "DELETE FROM Proxy WHERE Ip='$ip'";
    $st = $db->prepare($sql);
    $st->execute();
    $st->finish();
    return 0;
  } else {
    close SOCKET;
    return 1;
  }
}

sub get_proxy {
  $sql = "SELECT MAX(Id) FROM Proxy";
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
    $proxsize = $row->{"MAX(Id)"};
  } else {
    print "No proxy found\n";
    exit(1);
  }
  $st->finish();
      
  $ran = rand($proxsize);
  ($st,$nd) = split(/\./,$ran);
  $sql = "SELECT * FROM Proxy WHERE Id>=$st LIMIT 1";
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
    $proxip = $row->{'Ip'};
    $proxport = $row->{'Port'};
  }
  $st->finish();
  return $proxip.":".$proxport;
}