#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

#declare vars (it's safer this way)
my $DEVICE_ID;
my $DEVICE_EXT = 0;
my $DEVICE_TYPE = "SIP"; #default is SIP phone
my $DEVICE_PORT = 5060;  #default for SIP

my $CONF_HOST;
my $CONF_USER;
my $CONF_PASSWD;

my $DB_PL_HANDLE;
my $DB_AS_HANDLE;

my $STATUS = 0;

#check params
unless (defined($ARGV[0]))
{
    print "USAGE :$0 <DEVICE_ID>\n";
    exit(-1);
}
&read_pluto_config();

#connect to databases
$DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=$CONF_HOST;user=$CONF_USER;password=$CONF_PASSWD") or die "Could not connect to MySQL";
$DB_AS_HANDLE = DBI->connect("dbi:mysql:database=aserisk;host=$CONF_HOST;user=$CONF_USER;password=$CONF_PASSWD") or die "Could not connect to MySQL";

foreach $DEVICE_ID (@ARGV)
{
	&read_pluto_device_data();
	if($STATUS == -1)
	{
		&remove_from_asterisk_db();
	}
	else
	{
		if($DEVICE_EXT == 0)
		{
			&find_next_extension();
			&update_device_data();
		}
		$DEVICE_PORT = 4569 if($DEVICE_TYPE =~ /^iax/i);
		&update_asterisk_db();
	}
}

$DB_PL_HANDLE->disconnect();
$DB_AS_HANDLE->disconnect();

#run AMP's scripts to generate asterisk's config
`/var/www/pluto-admin/amp/admin/retrieve_sip_conf_from_mysql.pl`;

#reload asterisk
`asterisk -r -x reload`;

return 0;

#helpers
sub read_pluto_config()
{
	open(CONF,"/etc/pluto.conf");
	while(<CONF>)
	{
    	my ($option, $eq, $value) = split(/ /,$_);
    	if($option eq "MySqlHost")
    	{
        	$CONF_HOST=$value;
    	}
    	elsif ($option eq "MySqlUser")
    	{
        	$CONF_USER=$value;
    	}
    	elsif ($option eq "MySqlPassword")
    	{
        	$CONF_PASSWD=$value;
    	}
	}
	close(CONF);
}

sub read_pluto_device_data()
{
	my $DB_SQL;
	my $DB_STATEMENT;
	my $DB_ROW;

	$DB_SQL = "select FK_Device, IK_DeviceData, FK_DeviceData from Device_DeviceData where FK_Device=$DEVICE_ID and (FK_DeviceData=31 or FK_DeviceData=29)";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
	{
		if($DB_ROW->{'FK_DeviceData'} == 31)
		{
			$DEVICE_EXT = $DB_ROW->{'IK_DeviceData'};
		}
		elsif($DB_ROW->{'FK_DeviceData'} == 29)
		{
			$DEVICE_TYPE = $DB_ROW->{'IK_DeviceData'};
		}
	}
	$STATUS = -1 unless($DB_ROW); #device was deleted from pluto_main
}

sub find_next_extension()
{
	my $DB_SQL;
	my $DB_STATEMENT;
	my $DB_ROW;

	$DB_SQL = "select IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
	{
    	my $extention = $DB_ROW->{'IK_DeviceData'};
    	if($extention > $DEVICE_EXT)
    	{
        	$DEVICE_EXT = $extention;
    	}
	}
	if($DEVICE_EXT == 0)
	{
    	$DEVICE_EXT = 200; #200 is default for AMP
	}
	else
	{
    	$DEVICE_EXT = $DEVICE_EXT+1;
	}
}

sub update_device_data()
{
	my $DB_SQL;
	my $DB_STATEMENT;
	my $DB_ROW;

	$DB_SQL = "update Device_DeviceData SET IK_DeviceData='$DEVICE_EXT' WHERE FK_Device='$DEVICE_ID' AND FK_DeviceData='31'";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

	$DB_SQL = "update Device_DeviceData SET IK_DeviceData='$DEVICE_TYPE' WHERE FK_Device='$DEVICE_ID' AND FK_DeviceData='29'";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

}

sub update_asterisk_db()
{
	&remove_from_asterisk_db();
	&add_to_asterisk_db();
}

sub remove_from_asterisk_db()
{
	my $DB_SQL;
	my $DB_STATEMENT;
	my $DB_ROW;
	my $TBL;

	foreach $TBL ('iax','sip','zap')
	{
		$DB_SQL = "select id from $TBL where keyword='pl:".$DEVICE_ID."%'";
		$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
		$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
		if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
		{
			$DB_SQL = "delete from $TBL where id='".$DB_ROW->{IK_DeviceData}."'";
			$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
			$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
			return
		}
	}
}

sub add_to_asterisk_db()
{
	my $DB_SQL;
	my $DB_STATEMENT;
	my $DB_ROW;
	
	$DEVICE_TYPE = 'iax' if($DEVICE_TYPE =~ /^iax/);
	$DEVICE_TYPE = 'sip' if($DEVICE_TYPE =~ /^sip/);
	$DEVICE_TYPE = 'zap' if($DEVICE_TYPE =~ /^zap/);	
	
	return unless($DEVICE_TYPE =~/^(iax|sip|zap)$/);
	
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'account', '$DEVICE_EXT', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'accountcode', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'secret', '$DEVICE_EXT', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'canreinvite', 'no', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'context', 'from-internal', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'dtmfmode', 'rfc2833', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'host', 'dynamic', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'type', 'friend',0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'mailbox', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'username', '$DEVICE_EXT', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'nat', 'no', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'port', '$DEVICE_PORT', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'qualify', 'no', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'callgroup', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'pickupgroup', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'disallow', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'allow', '', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'record_in', 'On-Demand', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'record_out', 'On-Demand', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	$DB_SQL = "insert into $DEVICE_EXT values ($DEVICE_EXT, 'callerid', 'pl:$DEVICE_ID <$DEVICE_EXT>', 0);";
	$DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

}
