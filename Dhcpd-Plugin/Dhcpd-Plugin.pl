#!/usr/bin/perl
use DBI;

$DBHOST="10.0.0.150";
$DBUSER="root";
$DBPASSWD="";

while (1 eq 1) {
  $inline = <STDIN>;
  chomp($inline);
  $found = 0;

  @data = split(/ /, $inline);
  if($data[4] eq "dhcpd:") {
    if($data[5] eq "DHCPOFFER") {
      $mac_found = $data[9];
      $ip_sent = $data[7];

      system("convert_mac $mac_found > dhcpd_temp.file");
      open(FILE, "dhcpd_temp.file");
      @data = <FILE>;
      $mac_2_nr = $data[0];
      close(FILE);
      system("rm -f dhcpd_temp.file");
      
      $db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL server\n";
      $sql = "select PK_DHCPDevice,FK_DeviceTemplate,ConfigureScript,Package_Source.Name FROM DHCPDevice JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate LEFT JOIN Package ON DeviceTemplate.FK_Package=PK_Package LEFT JOIN Package_Source ON Package_Source.FK_Package=PK_Package WHERE Mac_Range_Low<=$mac_2_nr AND Mac_Range_High>=$mac_2_nr AND (FK_RepositorySource=2 OR FK_RepositorySource Is Null)";
      $statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
      $statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
      while($row_ref = $statement->fetchrow_hashref())
      {
          $found = 1;
          $dev_template = $row_ref->{FK_DeviceTemplate};
          $configure_script = $row_ref->{ConfigureScript};
          $package_name = $row_ref->{Name};
          system("create_device -d 1 -i $ip_sent -m $mac_found\n");
          if($package_name ne "") {
            system("apt-get install $package_name");
          }
          system("$configure_script -d $dev_temlate -i $ip_sent -m $mac_found");
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