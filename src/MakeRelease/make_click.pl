#!/usr/bin/perl

if($ARGV[0] eq "fm") {
  print "Making click for Fresh Meat for project ";
  open(PRJ,"fmprojects.list");
  @projects = <PRJ>;
  close(PRJ);
} elsif($ARGV[0] eq "sf") {
  print "Making click for Source Forge for project ";
  open(PRJ,"sfprojects.list");
  @projects = <PRJ>;
  close(PRJ);
} else {
  print "Not known\n";
  exit(1);
}	
open(PROXY,"proxy.list");
@proxys = <PROXY>;
close(PROXY);

$proxysize = get_size(@proxys);
$projectsize = get_size(@projects);
$random_proxy = rand($proxysize);
@data = split(/\./,$random_proxy);
$random_proxy = $data[0];
$random_project = rand($projectsize);
@data = split(/\./,$random_project);
$random_project = $data[0];

$prox = $proxys[$random_proxy];
chomp($prox);
$proj = $projects[$random_project];
chomp($proj);

print "$proj ";
system("curl -x $prox --connect-timeout 5 -f http://freshmeat.net/projects/$proj/");
print "[DONE]\n";

sub get_size {
  @in = @_;
  $counter = 0;
  foreach $line (@in) {
    $counter = $counter + 1;
  }
  return $counter;
}