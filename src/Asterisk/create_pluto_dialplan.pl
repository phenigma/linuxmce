#!/usr/bin/perl

use diagnostics;
use strict;
use DBI;

my $EXT_FILE = "/etc/asterisk/extensions_pluto_dial.conf";
my $EXT_BUFFER = "[from-pluto-custom]\n";

my %USERS = ();
my %MAILS = ();
my %NAMES = ();
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
$EXT_BUFFER .= "exten => 100,1,VoiceMail(100)\n";
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
		$EXT_BUFFER .= "exten => $line,2,Goto($line-hm\${HOUSEMODE},1)\n";
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
		$action =~ s/[&]$//;		
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
$DB_SQL = "select EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing from UserRouting order by EK_Users,IsPriorityCaller,EK_UserMode,StepOrder";
$DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
$tmp = "";
while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{{
	next unless(defined $USERS{$DB_ROW->{'EK_Users'}});
	my $user = $USERS{$DB_ROW->{'EK_Users'}};
	my $um = $DB_ROW->{'EK_UserMode'};
	my $try = $DB_ROW->{'StepOrder'};
	my $pri = $DB_ROW->{'IsPriorityCaller'};
	unless($tmp =~ /^$user[-]/)
	{
		$EXT_BUFFER .= "exten => $user,1,SetVar(USERMODE=1)\n";
		$EXT_BUFFER .= "exten => $user,2,AGI(pluto-prioritycaller.agi,\${CALLERIDNUM})\n";
		$EXT_BUFFER .= "exten => $user,3,Goto($user-um\${USERMODE}-pri\${PRIORITYCALLER},1)\n";
		$EXT_BUFFER .= "exten => $user,4,Hangup\n";
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
		$action =~ s/[&]$//;		
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
	if($tmp ne $user."-".$pri)
	{
		$EXT_BUFFER .= "exten => $user-um$um-pri$pri,1,Goto($user-um$um-pri$pri-try$try,1)\n";
	}
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,1,$action\n";
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,2,Goto($user-um$um-pri$pri-try$try-\${DIALSTATUS},1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,3,Hangup\n";
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-BUSY,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";	
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-NOANSWER,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-CONGESTION,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
	$EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-CHANUNAVAIL,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
	$tmp = $user."-".$pri;
#	$EXT_BUFFER .= "exten => $user-um$um-try$try,1,Hangup\n";

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
$tmp =~ s/[&]$//;
$EXT_BUFFER .= "exten => 0,1,Dial($tmp,$TIMEOUT)\n";
$EXT_BUFFER .= "exten => 0,2,Goto(s,1)\n";
foreach my $user (sort (values(%USERS)))
{
	$EXT_BUFFER .= "exten => $1,1,Dial(Local/$user\@trused,$TIMEOUT)\n" if ($user =~ /(\d)$/);
	$EXT_BUFFER .= "exten => $1,2,Goto(s,1)\n" if ($user =~ /(\d)$/);
}
$EXT_BUFFER .= "exten => #,1,Macro(vm,100)\n";
$EXT_BUFFER .= "exten => #,2,Hangup\n";

open(FILE,"> $EXT_FILE") or die "Could not open '$EXT_FILE'";
print FILE $EXT_BUFFER;
close(FILE);


#create voiceboxes
my $VM_FILE = "/etc/asterisk/voicemail.conf";
my $VM_BUFFER = "";

open(FILE,"< $VM_FILE") or die "Could not open '$VM_FILE'";

while(<FILE>)
{
	chomp;
	my $line = $_;
	if($line =~ /^\[default\]$/)
	{
		$VM_BUFFER .= $line."\n";
		$line = <FILE>;
		chomp $line;
		my $founduser= ",";
		my $user;
		my $key;
		while($line =~ /[^\[]/)
		{
			$VM_BUFFER .= $line."\n";
			foreach $key (keys (%USERS))
			{
				$user = $USERS{$key};
				if($line =~ /^$user\s[=][>]/)
				{
					$founduser .= $user.",";
				}
			}
			if($line =~ /^100\s[=][>]/)
			{
				$founduser .= "100,";
			}
			
			$line = <FILE>;
			chomp $line;			
		}
		foreach $key (keys (%USERS))
		{
			$user = $USERS{$key};
			unless($founduser =~ /,$user,/)
			{
				$VM_BUFFER .= "$user => $user,".$NAMES{$key}.",".$MAILS{$key}.",,attach=yes|saycid=no|envelope=yes|delete=no|nextaftercmd=no|operator=no\n";
			}
		}
		unless($founduser =~ /,100,/)
		{
			$VM_BUFFER .= "100 => 100,Default,,,attach=no|saycid=no|envelope=no|delete=no|nextaftercmd=no|operator=no\n";
		}
		
		$VM_BUFFER .= $line."\n";		
	}
	else
	{
		$VM_BUFFER .= $line."\n";
	}
}
close(FILE);

open(FILE,"> $VM_FILE") or die "Could not open '$VM_FILE'";
print FILE $VM_BUFFER;
close(FILE);

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
	$DB_SQL = "select PK_Users,UserName,Extension,ForwardEmail from Users where `Extension` like '30%'";
	$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
	$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
	while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
	{
		$USERS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'Extension'};
		$MAILS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'ForwardEmail'};
		$NAMES{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'UserName'};
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
