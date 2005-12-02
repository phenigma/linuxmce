#!/usr/bin/perl

use diagnostics;
use strict;
use DBI;

#need to resubmit "http://dcerouter/pluto-admin/index.php?section=devices&type=phones&action=add&update=Update"

my $EXT_FILE = "extensions_pluto_dial.conf";
my $EXT_BUFFER = "[ext-pluto-custom]\n";

my %USERS = ();
my %PHONES = ();

my $TIMEOUT = 15;

my $CONF_HOST="localhost";
my $CONF_USER="root";
my $CONF_PASSWD="";

my $DB_PL_HANDLE;
my $DB_TC_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;
my $DB_ROW;



&read_pluto_config();
$DB_PL_HANDLE = DBI->connect("dbi:mysql:database=pluto_main;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
$DB_TC_HANDLE = DBI->connect("dbi:mysql:database=pluto_telecom;host=".$CONF_HOST.";user=".$CONF_USER.";password=".$CONF_PASSWD.";") or die "Could not connect to MySQL";
&get_all_users_extensions();
&get_all_phones_extensions();

$EXT_BUFFER .= "\n;Lines\n";
$EXT_BUFFER .= "exten => 100,1,Macro(vm,100)\n";
$EXT_BUFFER .= "exten => 100,2,Hangup\n";

$DB_SQL = "select ID,EK_HouseMode,Routing from Line_HouseMode order by ID,EK_HouseMode";
$DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
my $tmp="";
while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
	my $line = "10".$1 if($DB_ROW->{'ID'} =~ /(\d)$/);
	my $hm = $DB_ROW->{'EK_HouseMode'};
	if($tmp ne $line)
	{
		$EXT_BUFFER .= "exten => $line,1,SetVar(HOUSEMODE=1)\n";
		$EXT_BUFFER .= "exten => $line,2,GotoIf($line-hm\${HOUSEMODE})\n";
		$EXT_BUFFER .= "exten => $line,3,Hangup\n";	
	}
	my $action = "NoOp(\"Do nothing\")";
	if($DB_ROW->{'Routing'} =~ /^ring,((\d+[,])*(\d+))$/)
	{
		my @arr=split(/,/,$1);
		$action = "Dial(";
		foreach my $i (@arr)
		{
			$action .= "Local/".$PHONES{$i}."\@trusted&";
		}
		$action .= ",$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^user,(\d+)$/)
	{
		$action = "Dial(Local/".$USERS{$1}."\@trusted,$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^transfer,(\d+)$/)
	{
		$action = "Dial(Local/".$1."\@trusted,$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^voicemail,(\d+)$/)
	{
		$action = "Macro(vm,".$USERS{$1}.")";
	}
	if($DB_ROW->{'Routing'} =~ /^prompt,(\d+)$/)
	{
		$action = "Goto(voice-menu-pluto-custom,s,1)";
	}
	$EXT_BUFFER .= "exten => $line-hm$hm,1,$action\n";
	$EXT_BUFFER .= "exten => $line-hm$hm,2,Goto($line-hm$hm-\${DIALSTATUS},1)\n";		
	$EXT_BUFFER .= "exten => $line-hm$hm,3,Hangup\n";
	$EXT_BUFFER .= "exten => $line-hm$hm-BUSY,1,Goto(100,1)\n";	
	$EXT_BUFFER .= "exten => $line-hm$hm-NOANSWER,1,Goto(100,1)\n";
	$EXT_BUFFER .= "exten => $line-hm$hm-CONGESTION,1,Goto(100,1)\n";	
	$EXT_BUFFER .= "exten => $line-hm$hm-CHANUNAVAIL,1,Goto(100,1)\n";
	$tmp = $line;
}


$EXT_BUFFER .= "\n;Users\n";
$DB_SQL = "select EK_Users,EK_UserMode,StepOrder,Routing from UserRouting order by EK_Users,EK_UserMode,StepOrder";
$DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
$tmp = "";
while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{{
	next unless(defined $USERS{$DB_ROW->{'EK_Users'}});
	my $user = $USERS{$DB_ROW->{'EK_Users'}};
	my $um = $DB_ROW->{'EK_UserMode'};
	my $try = $DB_ROW->{'StepOrder'};

	if($tmp ne $user)
	{
		$EXT_BUFFER .= "exten => $user,1,SetVar(USERMODE=1)\n";
		$EXT_BUFFER .= "exten => $user,2,GotoIf($user-um\${USERMODE})\n";
		$EXT_BUFFER .= "exten => $user,3,Hangup\n";
	}

	my $action = "NoOp(\"Do nothing\")";
	if($DB_ROW->{'Routing'} =~ /^ring,((\d+[,])*(\d+))$/)
	{
		my @arr=split(/,/,$1);
		$action = "Dial(";
		foreach my $i (@arr)
		{
			$action .= "Local/".$PHONES{$i}."\@trusted&";
		}
		$action .= ",$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^user,(\d+)$/)
	{
		$action = "Dial(Local/".$USERS{$1}."\@trusted,$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^transfer,(\d+)$/)
	{
		$action = "Dial(Local/".$1."\@trusted,$TIMEOUT)";
	}
	if($DB_ROW->{'Routing'} =~ /^voicemail,(\d+)$/)
	{
		$action = "Macro(vm,".$USERS{$1}.")";
	}
	$EXT_BUFFER .= "exten => $user-um$um,1,Goto($user-um$um-try$try,1)\n";	
	$EXT_BUFFER .= "exten => $user-um$um-try$try,1,$action\n";
	$EXT_BUFFER .= "exten => $user-um$um-try$try,2,Goto($user-um$um-try$try-\${DIALSTATUS},1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-try$try,3,Hangup\n";
	$EXT_BUFFER .= "exten => $user-um$um-try$try-BUSY,1,Goto($user-um$um-try".($try+1).",1)\n";	
	$EXT_BUFFER .= "exten => $user-um$um-try$try-NOANSWER,1,Goto($user-um$um-try".($try+1).",1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-try$try-CONGESTION,1,Goto($user-um$um-try".($try+1).",1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-try$try-CHANUNAVAIL,1,Goto($user-um$um-try".($try+1).",1)\n";

#		$EXT_BUFFER .= "exten => $user-um$um-try$try,1,Hangup\n";

}}



$EXT_BUFFER .= "\n\n[voice-menu-pluto-custom]\n\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,2,Wait(1)\n";
$EXT_BUFFER .= "exten => s,3,Background('You reached XXX. To call all extensions in the house press 0, to call user1 press 1, to call user2 press 2, to call user3 press 3, to leave a message press #')\n";
$EXT_BUFFER .= "exten => s,4,DigitTimeout,14\n";
$EXT_BUFFER .= "exten => s,5,ResponseTimeout,15\n";
$EXT_BUFFER .= "exten => t,1,Goto(s,1)\n";

foreach my $user (sort (values(%PHONES)))
{
	$tmp .= "Local/$user\@trusted&";
}

$EXT_BUFFER .= "exten => 0,1,Dial($tmp,$TIMEOUT)\n";
$EXT_BUFFER .= "exten => 0,2,Goto(s,1)\n";
foreach my $user (sort (values(%USERS)))
{
	$EXT_BUFFER .= "exten => $1,1,Dial(Local/$user\@trused,$TIMEOUT)\n" if ($user =~ /(\d)$/);
	$EXT_BUFFER .= "exten => $1,2,Goto(s,1)\n" if ($user =~ /(\d)$/);
}
$EXT_BUFFER .= "exten => #,1,Macro(vm,100)\n";
$EXT_BUFFER .= "exten => #,2,Hangup\n";


print $EXT_BUFFER;

#for each user:
#$EXT_BUFFER .= "exten => 101,1, Dial(Local/200@trusted,Local/201@trusted)"; #ring extensions
#$EXT_BUFFER .= "exten => 101,102, Dial(Local/102@trusted)"; #ring another user
#$EXT_BUFFER .= "exten => 101,303, Dial(Local/901140740649941)"; #ring on cell phone
#$EXT_BUFFER .= "exten => 101,404, Macro(vm,101)"; #Voicemail
#$EXT_BUFFER .= "exten => 101,405, Hangup";


#create voiceboxes
#my $VM_FILE = "/etc/asterisk/voicemail.conf"
#my $VM_BUFFER = "[default]"
#$VM_BUFFER .= "100 => 123456789,101,user@mail.server,,attach=no|saycid=no|envelope=no|delete=no|nextaftercmd=no";







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
	$DB_SQL = "select PK_Users,Extension from Users where `Extension` is not null";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
	{
		$USERS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'Extension'};
	}
	$DB_STATEMENT->finish();
}

sub get_all_phones_extensions()
{
	$DB_SQL = "select FK_Device, IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
	{
		$PHONES{$DB_ROW->{'FK_Device'}} = $DB_ROW->{'IK_DeviceData'};
	}
	$DB_STATEMENT->finish();
}
