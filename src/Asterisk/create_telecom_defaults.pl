#!/usr/bin/perl

#use diagnostics;
use strict;
use DBI;

my %USERS = ();
my %LINES = ();
my $PHONES = "";

my $CONF_HOST="localhost";
my $CONF_USER="root";
my $CONF_PASSWD="";

my $DB_PL_HANDLE;
my $DB_TC_HANDLE;
my $DB_AS_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;
my $DB_ROW;

&read_pluto_config();
$DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
$DB_TC_HANDLE = DBI->connect("dbi:mysql:database=pluto_telecom;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
$DB_AS_HANDLE = DBI->connect("dbi:mysql:database=asterisk;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
&get_all_users_extensions();
&get_all_lines_extensions();
&get_all_phones_extensions();

foreach my $key (keys (%USERS))
{
    unless($USERS{$key} eq '0')
    {
        print "Add defaults for user $key\n";
		
        $DB_SQL = "insert into UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('".$key."','1','0','1','ring,".$PHONES."');";
        $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->finish();
        $DB_SQL = "insert into UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('".$key."','1','1','1','ring,".$PHONES."');";
        $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->finish();

        for(my $i = 2; $i <= 4; $i++)
        {
            $DB_SQL = "insert into UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('".$key."','".$i."','0','1','voicemail,".$USERS{$key}."');";
            $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->finish();
            $DB_SQL = "insert into UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('".$key."','".$i."','1','1','voicemail,".$USERS{$key}."');";
            $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->finish();
        }
    }
}

foreach my $key (keys (%LINES))
{
    unless($LINES{$key} eq '0')
    {
        print "Add defaults for line $key\n";
        foreach my $i (1,3,5)
        {
            $DB_SQL = "insert into Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('".$key."','".$LINES{$key}."','".$i."','ring,".$PHONES."');";
            $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->finish();
        }
        foreach my $i (2,4,6)
        {
            $DB_SQL = "insert into Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('".$key."','".$LINES{$key}."','".$i."','prompt');";
            $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
            $DB_STATEMENT->finish();
        }
    }
}

sub read_pluto_config()
{
    open(CONF,"/etc/pluto.conf") or die "Could not open pluto config";
    while(<CONF>)
    {
        chomp;
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

sub get_all_users_extensions()
{
    $DB_SQL = "select PK_Users,Extension from Users where `Extension` like '30_'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $USERS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'Extension'};
    }
    $DB_STATEMENT->finish();
    $DB_SQL = "select EK_Users from UserRouting";
    $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $USERS{$DB_ROW->{'EK_Users'}} = "0";
    }
    $DB_STATEMENT->finish();
}

sub get_all_lines_extensions()
{
    $DB_SQL = "select id, data from sip where keyword='account' and id like '9999_'";
    $DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $LINES{$DB_ROW->{'id'}} = "SIP";
    }
    $DB_STATEMENT->finish();
    $DB_SQL = "select id, data from iax where keyword='account' and id like '9999_'";
    $DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $LINES{$DB_ROW->{'id'}} = "IAX";
    }
    $DB_STATEMENT->finish();
    $DB_SQL = "select ID from Line_HouseMode";
    $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $LINES{$DB_ROW->{'ID'}} = "0";
    }
    $DB_STATEMENT->finish();

}

sub get_all_phones_extensions()
{
    $DB_SQL = "select FK_Device from Device_DeviceData where FK_DeviceData=31";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $PHONES.= $DB_ROW->{'FK_Device'}.",";
    }
    $PHONES =~ s/[,]$//gm;
    $DB_STATEMENT->finish();
}
