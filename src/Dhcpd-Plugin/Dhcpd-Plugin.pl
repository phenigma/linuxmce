#!/usr/bin/perl
use DBI;

log_plugin("\n\n\n\nDhcp-Plugin Process Started","log");
open(CONF,"/etc/pluto.conf");
@data=<CONF>;
close(CONF);

foreach $line (@data)
{
	($option, $eq, $value) = split(/ /,$line);

	if($option eq "MySqlHost")
	{
		$DBHOST=$value;
		$FILEHOST=$value;
	}
	elsif ($option eq "MySqlUser")
	{
		$DBUSER=$value;
	}
	elsif ($option eq "MySqlPassword")
	{
		$DBPASSWD=$value;
	}
	elsif ($option eq "PK_Device")
	{
		$PKDEV=$value;
	}
}

log_plugin("Dhcp-Plugin Started","log");
while (1 eq 1)
{
	$inline = <STDIN>;
	chomp($inline);
	$found = 0;

	$line = "";
	$line = parse($inline);
	@data = split(/\,/,$line);
	$tag = $data[5];
	$op = $data[6];
	$mac_found = $data[10];
	$ip_sent = $data[8];

	if($tag eq "dhcpd:")
	{
		if($op eq "DHCPOFFER")
		{
			log_plugin("We have received a DHCP offer a network device","log");
			system("/usr/pluto/bin/convert_mac $mac_found > dhcpd_temp.file");
			open(FILE, "dhcpd_temp.file");
			@data = <FILE>;
			$mac_2_nr = $data[0];
			close(FILE);
			system("rm -f dhcpd_temp.file");

			$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL server\n";
			$sql = "select PK_Device from Device WHERE MACaddress='$mac_found' AND IPaddress='$ip_sent'";

			log_plugin("Searching device in the database","log");
			$st = $db_handle->prepare($sql);
			$st->execute();
			if($row = $st->fetchrow_hashref())
			{
				log_plugin("Device allready exists","log");
				$Device_ID = $row->{'PK_Device'};
				$found = 0;
				$sql = "select FK_DeviceTemplate from Device where MACaddress='$mac_found'   and IPaddress='$ip_sent'";
				$st2 = $db_handle->prepare($sql);
				$st2->execute();
				if($ndrow = $st2->fetchrow_hashref())
				{
					$template_dev = $ndrow->{'FK_DeviceTemplate'};
					$sql = "select IsPlugAndPlay, ConfigureScript from DeviceTemplate where  PK_DeviceTemplate='$template_dev'";
					$st3 = $db_handle->prepare($sql);
					$st3->execute();
					if($trdrow = $st3->fetchrow_hashref())
					{
						$pnp = $trdrow->{'IsPlugAndPlay'};
						$cfg = $trdrow->{'ConfigureScript'};

						if($pnp eq "1")
						{
							log_plugin("Running Configure Script for the Pnp Device","log");
							system("/usr/pluto/bin/$cfg -d $Device_ID -i $ip_sent -m $mac_found");
							log_plugin("/usr/pluto/bin/$cfg -d $Device_ID -i $ip_sent -m $mac_found");
						}
						else
						{
							log_plugin("The Device is not PNP so we will not configure","log");
						}
					}
					$st3->finish();
				}
				else
				{
					log_plugin("Cannot find Device template for the Device Found","err");
				}

				$st2->finish();
			}
			else
			{
				log_plugin("Device not found in the database, checking if it is pnp","log");
				$sql = "select PK_DHCPDevice,FK_DeviceTemplate,ConfigureScript,Package_Source.Name FROM DHCPDevice JOIN DeviceTemplate ON DHCPDevice.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate LEFT JOIN Package ON DeviceTemplate.FK_Package=PK_Package LEFT JOIN Package_Source ON Package_Source.FK_Package=Package.PK_Package WHERE Mac_Range_Low<=$mac_2_nr AND Mac_Range_High>=$mac_2_nr";
				$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
				$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";

				while($row_ref = $statement->fetchrow_hashref())
				{
					log_plugin("Device is pnp. Adding...","log");
					$found = 1;
					$tmp = "";
					$dhcpd_device = $row_ref->{PK_DHCPDevice};
					$dev_template = $row_ref->{FK_DeviceTemplate};
					$configure_script = $row_ref->{ConfigureScript};
					$package_name = $row_ref->{Name};
					$tmp = $row_ref->{PK_Device};
# 					$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
# 					$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
# 					$row_ref = $statement->fetchrow_hashref();
					if($tmp eq "")
					{
						$install = get_install();

						log_plugin("Creating Device","log");
						system("/usr/pluto/bin/CreateDevice -i $install -c $dhcpd_device -I $ip_sent -M $mac_found -n > dhcpd_temp.file\n");
						open(FILE, "dhcpd_temp.file");
						@data = <FILE>;
						$Device_ID = $data[0];
						chomp($Device_ID);
						close(FILE);

# 						We need to run the configuration before anouncing the system because of the stupid way the pluto_main caching works.
 						chomp($Device_ID);
						log_plugin("Running configuration script","log");
						log_plugin("/usr/pluto/bin/$configure_script -d $Device_ID -i $ip_sent -m $mac_found","log");
						open SHELL, "/usr/pluto/bin/$configure_script -d $Device_ID -i $ip_sent -m $mac_found 2>&1|";
						while( <SHELL>)
						{
							chomp;
							log_plugin("CONF: $_", "log");
						}
						close (SHELL);

						log_plugin("We anounce the system that we have found a PnP device","log");
						system("/usr/pluto/bin/MessageSend localhost 0 0 2 24 26 $Device_ID");
						system("rm -f dhcpd_temp.file");
						log_plugin("Installing the package that the device require","log");

						if($package_name ne "")
						{
							system("apt-get install $package_name");
						}
					}
					else
					{
						log_plugin("The device is allready","err");
						$Device_ID = $tmp;

						chomp($Device_ID);
						log_plugin("Running configuration script","log");
						log_plugin("/usr/pluto/bin/$configure_script -d $Device_ID -i $ip_sent -m $mac_found","log");
						open SHELL, "/usr/pluto/bin/$configure_script -d $Device_ID -i $ip_sent -m $mac_found 2>&1|";
						while( <SHELL>)
						{
							chomp;
							log_plugin("CONF: $_", "log");
						}
						close (SHELL);
					}
				}
			    if($found == 0)
				{
					log_plugin("We declare this device as unkown for PnP","log");
					$sql = "select PK_UnknownDevices FROM UnknownDevices WHERE MacAddress='$mac_found'";
					$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
					$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
					$row_ref = $statement->fetchrow_hashref();

					if($row_ref eq "")
	    			{
						log_plugin("Is the first time when we found this device. Adding...","log");
              			$sql = "insert into UnknownDevices(MacAddress,IPAddress) VALUES('$mac_found','$ip_sent')";
	      				$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
              			$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
	    			}
					else
					{
						log_plugin("The device is known allready as unkown. Updating...","log");
              			$sql = "update UnknownDevices SET IPAddress='$ip_sent' WHERE MacAddress='$mac_found'";
	      				$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
              			$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
            		}
          		}
        	}
			$st->finish();
			$db_handle->disconnect();
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
    $value = $frag[1];
    if($var eq "PK_Installation") {
      $sql = "select FK_Installation from Device WHERE PK_Device='$PKDEV'";
      $st = $db_handle->prepare($sql);
      $st->execute();
      if($row = $st->fetchrow_hashref()) {
    	$st->finish();
        return $row->{'FK_Installation'};
      } else {
        return 1;
      }
    }
  }
}

sub log_plugin {
	$data = shift;
	$type = shift;
	if($type eq "err") {
		$type = "01";
	} elsif($type eq "log") {
		$type = "00";
	}
	system("echo \"$type `date` $data\" >> /var/log/pluto/dhcp_pnp.newlog");
}

sub parse {
	$line = shift;
	@st = split(/ /,$line);
	$ret = "";
	foreach $el (@st) {
		if($el ne "") {
			$ret = $ret.",".$el;
		}
	}
	return $ret;
}
