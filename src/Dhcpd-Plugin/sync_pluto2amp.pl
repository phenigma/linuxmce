#!/usr/bin/perl

use strict;
#use diagnostics;
use DBI;

#declare vars (it's safer this way)
my $DEVICE_ID=0;
my $DEVICE_EXT = 0;
my $DEVICE_TYPE = ""; #default is SIP phone
my $DEVICE_PORT = 0;  #default for SIP

my $CONF_HOST="localhost";
my $CONF_USER="root";
my $CONF_PASSWD="";

my $DB_PL_HANDLE;
my $DB_AS_HANDLE;

my $STATUS = 0;

#check params
&read_pluto_config();

#fix permissions on each run
`chmod g+w /etc/asterisk/*`;

#connect to databases
$DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
$DB_AS_HANDLE = DBI->connect("dbi:mysql:database=asterisk;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";

my @DEVICES = ();

if (defined($ARGV[0]))
{
    @DEVICES = @ARGV;
}
else
{
    &set_embed_phones();
}

&check_and_fix_duplicates();

foreach my $I (@DEVICES)
{
    $DEVICE_ID = $I;
    $DEVICE_EXT = 0;
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
        print "Processing #$DEVICE_ID ($DEVICE_EXT)\n";
        $DEVICE_PORT = 4569 if($DEVICE_TYPE =~ /^iax/i);
        $DEVICE_PORT = 5060 if($DEVICE_TYPE =~ /^sip/i);
        &update_asterisk_db();
        #export the extension for other scripts
        `echo $DEVICE_EXT > /tmp/phone${DEVICE_ID}extension`;
    }
}

$DB_PL_HANDLE->disconnect();
$DB_AS_HANDLE->disconnect();

#run AMP's scripts to generate asterisk's config
`/var/www/pluto-admin/amp/admin/retrieve_iax_conf_from_mysql.pl`;
`/var/www/pluto-admin/amp/admin/retrieve_sip_conf_from_mysql.pl`;
`/var/www/pluto-admin/amp/admin/retrieve_zap_conf_from_mysql.pl`;

#reload asterisk
`/usr/sbin/asterisk -r -x reload`;

#helpers
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

sub check_and_fix_duplicates()
{
    my $DB_SQL;
    my $DB_STATEMENT;
    my $DB_ROW;
    my %TEMPL_MOD = ();
    my %DEV_TEMPL = ();
    my %DEV_EXTEN = ();


    $DB_SQL = "select FK_DeviceTemplate, AllowedToModify from DeviceTemplate_DeviceData where FK_DeviceData=31";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $TEMPL_MOD{$DB_ROW->{'FK_DeviceTemplate'}}=$DB_ROW->{'AllowedToModify'};
    }
    $DB_STATEMENT->finish();
    $DB_SQL = "select FK_Device,FK_DeviceTemplate,IK_DeviceData from Device_DeviceData,Device where FK_DeviceData=31 and FK_Device=PK_Device";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $DEV_EXTEN{$DB_ROW->{'FK_Device'}} = $DB_ROW->{'IK_DeviceData'};
        $DEV_TEMPL{$DB_ROW->{'FK_Device'}} = $DB_ROW->{'FK_DeviceTemplate'};
    }
    $DB_STATEMENT->finish();

    foreach my $i (keys %DEV_EXTEN)
    {
        foreach my $j (keys %DEV_EXTEN)
        {
            if(($i ne $j) && ($DEV_EXTEN{$i} eq $DEV_EXTEN{$j}) && ($DEV_EXTEN{$i} ne "0") && (length($DEV_EXTEN{$i})>0))
            {
                print "Conflict for extension ".$DEV_EXTEN{$i}." between #$i and #$j\n";
                if($TEMPL_MOD{$DEV_TEMPL{$i}} == 1)
                {
                    $DEV_EXTEN{$i} = 0;
                }
                elsif($TEMPL_MOD{$DEV_TEMPL{$j}} == 1)
                {
                    $DEV_EXTEN{$j} = 0;
                }
                else
                {
                    print "Conflict can't be solved\n";
                    exit;
                }
            }
        }
    }
    foreach my $i (keys %DEV_EXTEN)
    {
        $DEVICE_ID = $i;
        &read_pluto_device_data();
        $DEVICE_EXT = $DEV_EXTEN{$i};
        &update_device_data();
    }
}

sub read_pluto_device_data()
{
    my $DB_SQL;
    my $DB_STATEMENT;

    $DB_SQL = "select FK_Device, IK_DeviceData, FK_DeviceData from Device_DeviceData where FK_Device=$DEVICE_ID and (FK_DeviceData=31 or FK_DeviceData=29)";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $STATUS = -1;
    while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        if($DB_ROW->{'FK_DeviceData'} == 31)
        {
            $DEVICE_EXT = $DB_ROW->{'IK_DeviceData'};
        }
        elsif($DB_ROW->{'FK_DeviceData'} == 29)
        {
            $DEVICE_TYPE = $DB_ROW->{'IK_DeviceData'};
        }
        $STATUS=0;
    }
    $DB_STATEMENT->finish();
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

    $DB_SQL = "select user from devices where (description like 'pl_".$DEVICE_ID."')";
    $DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        my $old_ext = $DB_ROW->{'user'};
        `curl 'http://localhost/pluto-admin/amp/admin/config.php?display=extensions&extdisplay=$old_ext&action=del' > /dev/null`;
    }
}

sub add_to_asterisk_db()
{
    my %EXT_VARS = ();
    my $EXT_DATA = "";

    $DEVICE_TYPE = 'iax2' if($DEVICE_TYPE =~ /^iax/i);
    $DEVICE_TYPE = 'sip' if($DEVICE_TYPE =~ /^sip/i);
    $DEVICE_TYPE = 'zap' if($DEVICE_TYPE =~ /^zap/i);
    $DEVICE_TYPE = 'custom' if($DEVICE_TYPE =~ /^sccp/i);

    unless($DEVICE_TYPE =~/^(iax2|sip|zap|custom)$/)
    {
        print "Unsupported device type : \"$DEVICE_TYPE\"\n";
        return;
    }

    $EXT_VARS{'display'}="extensions";
    $EXT_VARS{'extdisplay'}="";
    $EXT_VARS{'action'}="add";
    $EXT_VARS{'tech'}=$DEVICE_TYPE;
    $EXT_VARS{'extension'}=$DEVICE_EXT;
    $EXT_VARS{'name'}="pl_".$DEVICE_ID;
    $EXT_VARS{'devicetype'}="fixed";
    $EXT_VARS{'deviceuser'}="same";
    $EXT_VARS{'password'}="";
    $EXT_VARS{'outboundcid'}="\"pl_".$DEVICE_ID."\" <$DEVICE_EXT>";
    $EXT_VARS{'qualify'}="yes" if ($DEVICE_TYPE eq 'sip');
    $EXT_VARS{'vm'}="disabled";
    $EXT_VARS{'secret'}=$DEVICE_EXT if ($DEVICE_TYPE ne 'custom');
    $EXT_VARS{'dial'}="SCCP/".$DEVICE_EXT if ($DEVICE_TYPE eq 'custom');
    foreach my $var (keys %EXT_VARS)
    {
        my $str = $EXT_VARS{$var};
        $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
        $EXT_DATA .=$var."=".$str."&";
    }
    `curl -d '$EXT_DATA' 'http://localhost/pluto-admin/amp/admin/config.php?display=extensions' > /dev/null`;
}

sub set_embed_phones()
{
    my $DB_SQL;
    my $DB_STATEMENT;

    $DB_SQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=1759";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        push @DEVICES, $DB_ROW->{'PK_Device'};
    }
}
