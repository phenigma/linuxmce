#!/usr/bin/perl
use DBI;

open(CONF,"/etc/pluto.conf");
@data=<CONF>;
close(CONF);

foreach $line (@data) {
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

while (1 eq 1) {
  $inline = <STDIN>;
  chomp($inline);
  $found = 0;

  @data = split(/ /, $inline);
  if($data[4] ne "localhost") {
    $tag = $data[4];
    $op = $data[5];
    $mac_found = $data[9];
    $ip_sent = $data[7];
  } else {
    $tag = $data[5];
    $op = $data[6];
    $mac_found = $data[10];
    $ip_sent = $data[8];
  }
  if($tag eq "dhcpd:") {
    if($op eq "DHCPOFFER") {

      system("/usr/pluto/bin/convert_mac $mac_found > dhcpd_temp.file");
      open(FILE, "dhcpd_temp.file");
      @data = <FILE>;
      $mac_2_nr = $data[0];
      close(FILE);
      system("rm -f dhcpd_temp.file");
      
      $db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL server\n";

      $sql = "select PK_Device from Device WHERE MACaddress='$mac_found' AND IPaddress='$ip_sent'";
      $st = $db_handle->prepare($sql);
      $st->execute();
      if($row = $st->fetchrow_hashref()) {
	  $found = 0;
      } else {
          $sql = "select PK_DHCPDevice,FK_DeviceTemplate,ConfigureScript,Package_Source.Name FROM DHCPDevice JOIN DeviceTemplate ON DHCPDevice.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate LEFT JOIN Package ON DeviceTemplate.FK_Package=PK_Package LEFT JOIN Package_Source ON Package_Source.FK_Package=Package.PK_Package WHERE Mac_Range_Low<=$mac_2_nr AND Mac_Range_High>=$mac_2_nr";
	  $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
          $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
          while($row_ref = $statement->fetchrow_hashref())
	  {
    	      $found = 1;
	      $dhcpd_device = $row_ref->{PK_DHCPDevice};
              $dev_template = $row_ref->{FK_DeviceTemplate};
              $configure_script = $row_ref->{ConfigureScript};
	      $package_name = $row_ref->{Name};
	      $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
              $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
	      $tmp = "";
    	      $row_ref = $statement->fetchrow_hashref();
              $tmp = $row_ref->{PK_Device};
              if($tmp eq "") {
	        $install = get_install();
                system("/usr/pluto/bin/CreateDevice -i $install -c $dhcpd_device -I $ip_sent -M $mac_found -n > dhcpd_temp.file\n");
	        open(FILE, "dhcpd_temp.file");
	        @data = <FILE>;
                $Device_ID = $data[0];
	        chomp($Device_ID);
                close(FILE);
	        system("/usr/pluto/bin/MessageSend localhost 0 0 2 24 26 $Device_ID");
                system("rm -f dhcpd_temp.file");
	        if($package_name ne "") {
                  system("apt-get install $package_name");
	        }
              } else {
	        $Device_ID = $tmp;
              }
	      chomp($Device_ID);
              system("/usr/pluto/bin/$configure_script -d $Device_ID -i $ip_sent -m $mac_found");
          }
          if($found == 0) {
	    $sql = "select PK_UnknownDevices FROM UnknownDevices WHERE MacAddress='$mac_found'";
            $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
            $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
	    $row_ref = $statement->fetchrow_hashref();
            if($row_ref eq "")
	    {
              $sql = "insert into UnknownDevices(MacAddress,IPAddress) VALUES('$mac_found','$ip_sent')";
	      $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
              $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
	    } else {
              $sql = "update UnknownDevices SET IPAddress='$ip_sent' WHERE MacAddress='$mac_found'";
	      $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
              $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
            }
          }
          $db_handle->disconnect();
        }
     }
  }
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
      $sql = "select FK_Installation from Device WHERE PK_Device='$PKDEV'";
      $st = $db_handle->prepare($sql);
      $st->execute();
      if($row = $st->fetchrow_hashref()) {
        return $row->{'FK_Installation'};
      } else {
        return 1;
      }
    }
  }
}
