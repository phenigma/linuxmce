#!/usr/bin/perl


use DBI;
use diagnostics;
use strict;

sub showUsage
{
	print "\n$_[0] -d <Device Template> -i <IP> -m <mac address>\n";
	exit(-1);
}

showUsage $0 if ( $#ARGV != 5 || $ARGV[0] ne "-d" || $ARGV[2] ne "-i" || $ARGV[4] ne "-m" );

my $deviceID = $ARGV[1];
my $deviceIP = $ARGV[3];
my $deviceMAC = $ARGV[5];

my $dbConnection;
my $machineID;
my $installationID;

sub connectToDatabase
{
	my $dbHostname;
	my $dbUsername;
	my $dbPassword;
	map 
	{
		if ( /^(\w+)\s+=\s+(.*)$/ )
		{
			my $name = $1;
			my $value = $2;
			
			$dbHostname = $value if ( $name eq "MySqlHost" );
			$dbUsername = $value if ( $name eq "MySqlUser" );
			$dbPassword = $value if ( $name eq "MySqlPassword" );
			$machineID = $value if ( $name eq "PK_Device" );
		}
	} `cat /etc/pluto.conf`;

	$dbConnection = DBI->connect("dbi:mysql:database=pluto_main;host=$dbHostname;user=$dbUsername;password=$dbPassword") or die "Could not connect to MySQL";
}

sub findInstallationID
{
	my $statement = $dbConnection->prepare("select FK_Installation from Device where PK_Device = $machineID");
	
	$statement->execute();
	return $statement->fetchrow_hashref()->{'FK_Installation'};
}

sub updateSqueezeBoxControlledViaField
{
	#
	# Device template with ID 53 is the Slim Server Streamer template.
	#
	my $statement = $dbConnection->prepare("select PK_Device from Device where FK_Installation = $installationID AND FK_DeviceTemplate = 53");
	$statement->execute();
	
	my $slimServerID;
	my $row;
	if ( $row = $statement->fetchrow_hashref() )
	{
		$slimServerID = $row->{"PK_Device"};
		print "00 Found SlimServer Device with ID: $slimServerID\n";
		$statement->finish();
		
		$dbConnection->do( "UPDATE Device set FK_Device_ControlledVia = $slimServerID where PK_Device = $deviceID" );
	} else {
		print "01 No Slim Server devices found in this installation. Something is wrong.";
	}
}

connectToDatabase;

$installationID = findInstallationID;

print "\n\n00 Configuring new squeeze box device: $deviceID, $deviceIP, $deviceMAC\n";
print "00 Connected to database\n";
print "00 Found local machine ID: $machineID with installation ID: $installationID\n";

updateSqueezeBoxControlledViaField
