#!/usr/bin/perl
use DBI;

# A request from a device already configured for the network, even if it's a new device for pluto system

#Dec 20 05:42:03 dcerouter dhcpd: DHCPREQUEST for 192.168.80.253 from 00:0f:23:fc:a4:ea via eth1
#Dec 20 05:42:03 dcerouter dhcpd: DHCPACK on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1

# This is a REAL dhcp offer 
#Dec 20 05:44:43 dcerouter dhcpd: DHCPDISCOVER from 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPOFFER on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPREQUEST for 192.168.80.253 (192.168.80.1) from 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPACK on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1

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
		chop ($PKDEV);
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
		if($op eq "DHCPACK")
		{
			log_plugin("DHCP : MAC $mac_found IP $ip_sent","log");

			$db_handle = DBI->connect("dbi:mysql:database=pluto_main;host=$DBHOST;user=$DBUSER;password=$DBPASSWD") or die "Could not connect to MySQL server\n";
			$sql = "SELECT PK_Device FROM Device WHERE MACaddress='$mac_found' AND IPaddress='$ip_sent'";

			$st = $db_handle->prepare($sql);
			$st->execute();
			if($row = $st->fetchrow_hashref())
			{
				log_plugin("Device already exists","log");
				$st->finish();
			}
			else
			{
				$st->finish();
				$sql = "SELECT PK_UnknownDevices FROM UnknownDevices WHERE MACaddress='$mac_found'";
				$st = $db_handle->prepare($sql);
				$st->execute();
				if($row = $st->fetchrow_hashref())
				{
					log_plugin("Device already marked as unknown, ignore","log");
					$st->finish();
				}
				else
				{
					$st->finish();
					log_plugin("NEW DEVICE !!!","log");
					for ($i=0;$i<100;$i++)
					{
						system("/usr/pluto/bin/MessageSend localhost $PKDEV -1001 2 65 53 2 5 \"$mac_found\" 28 \"$ip_sent\"");
						$errcode = system("echo $? 1>/dev/null");

						if ( $errcode eq "0" ) {
							last;
						} else {
							sleep 10;
						}
					}
				}
			}
			$db_handle->disconnect();
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
	system("echo \"$type `date` $data\" >> /var/log/pluto/dhcp_pnp.log");
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
