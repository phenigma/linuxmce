#!/usr/bin/perl

open(NAME, "npkg.tmp");
$namepkg = <NAME>;
chomp($namepkg);
$basepath = <NAME>;
chomp($basepath);
close(NAME);

print "Syncing the package $namepkg\n";

open(FILES, "flist.tmp");
@list = <FILES>;
close(FILES);

foreach $line (@list) {
  chomp($line);
}

print "Creting temporary directory ";
system("mkdir SFtemp");
print "[Done]\n";

print "Changing dir into temp ";
chdir("SFtemp");
print "[Done]\n";

print "Making cvs CO\n";
system("cvs -d:ext:plutoinc\@cvs.sourceforge.net:/cvsroot/ checkout -P $namepkg");
print "[Done]\n";

print "Changing dir to $namepkg ";
chdir("$namepkg");
print "[Done]\n";

print "Finding root dir \n";
$counter = 0;
foreach $line (@list) {
  chomp($line);
  $size = length($basepath);
  $rest = substr($line, $size+1);
  $listd[$counter] = $rest;
  $counter = $counter+1;
}
print "[Done]\n";

print "Copying new files ";
$counter = 0;
foreach $line (@list) {
  $var = 0;
  open(TEST, $listd[$counter]) or $var=1;
  if ($var == 1) {
    system("cp -f $line $listd[$counter]");
    system("cvs add $listd[$counter]");
  } else {
    close(TEST);
    system("cp -f $line $listd[$counter]");
#    system("cvs update $listd[$counter]");
  }
  $counter = $counter+1;
}
print "[Done]\n";

$counter=0;
opendir(DIR, ".");
$data=readdir(DIR);
while($data ne "") {
  $subd = 1;
  $data=readdir(DIR);
  if($data ne ".." && $data ne ".") {
    $listo[$counter]=$data;
    opendir(SUBDIR, $data) or $subd=0;
    if($subd == 0) {
      $counter=$counter+1;
    } else {
      $data2=readdir(SUBDIR);
      while($data2 ne "") {
        $data2=readdir(SUBDIR);
        if($data2 ne ".." && $data2 ne ".") {
          $listo[$counter]=$data."/".$data2;
          $counter=$counter+1;
        }
      }
    }
  }
}
closedir DIR;
                                                                                                

print "Removing unwhanted file ";
foreach $line (@listd) {
  $found = 0;
  $counter=0;
  foreach $line2 (@listo) {
    if($line eq $line2) {
      $listo[$counter]="-";
      $found = 1;
    }
    $counter = $counter + 1;
  }
}

foreach $in (@listo) {
  if($in ne "-" && $in ne "") {
    ($dir, $file) = split(/\//,$in);
    if($dir ne "CVS" && $dir ne "CVSROOT" && $dir ne ".") {
      print "[$in]\n";
      ($dir, $file) = split(/\//,$in);
      if($file ne "CVS") {
        opendir(SUBDIR, $dir) or $subd=0;
        if($subd == 0) {
        system("rm -f -r $in");
        system("cvs remove $in");
        system("cvs -d:ext:plutoinc\@cvs.sourceforge.net:/cvsroot/ commit -m 'Automatic Update'");
        } else {
          $data2=readdir(SUBDIR);
          while($data2 ne "") {
            $data2=readdir(SUBDIR);
            if($data2 ne ".." && $data2 ne ".") {
              system("rm -f -r $in");
              system("cvs remove $in");
              system("cvs -d:ext:plutoinc\@cvs.sourceforge.net:/cvsroot/ commit -m 'Automatic Update'");
            }
          }
        }
      }
    }
  }
}
print "[Done]\n";

print "Checking in all ";
system("cvs -d:ext:plutoinc\@cvs.sourceforge.net:/cvsroot/ commit -m 'Automatic Update'");
print "[Done]\n";

chdir("../../");
system("rm -r SFtemp");