<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
header("Content-type: text/plain");

// port destination
$portsDestination=array('SSH_22', 'Web_80', 'nomon_SSH_22');

$portNo=count($portsDestination);

/*
for($i=10000;$i<20000;$i++){
	$res=dbQuery("INSERT INTO RemoteAssistance (Port) VALUES ($i)",$conn);
}
die('?');
*/

$installationID= (int)@$_GET["installationID"];
if($installationID==0){
	die('ERROR: Installation ID not specified');
}

if(isset($_GET['action']) && $_GET['action']=='del'){
	dbQuery("
		UPDATE RemoteAssistance 
		SET 
		EK_Installation=NULL,
		Description=NULL,
		DateAllocated=NULL
		WHERE EK_Installation=".$installationID,$conn);	
	die("Success\n");
}

dbQuery("BEGIN",$conn);
$res=dbQuery("SELECT PK_Port,Port FROM RemoteAssistance WHERE EK_Installation=$installationID",$conn);

// if the installation had already requested ports or we are asked to just display the allocated ports, return corresponding ports
if(mysql_num_rows($res)!=0 || (isset($_GET['action']) && $_GET['action']=='display')){
	$pos=0;
	$out=array();
	while($row=mysql_fetch_assoc($res)){
		$out[]=$portsDestination[$pos].'='.$row['Port'];
		$pos++;
	}
	die("#Ports:\n".join("\n",$out)."\n#END\n");
}

// if not, return free ports
$stamp=time();
$res=dbQuery("SELECT PK_Port,Port FROM RemoteAssistance WHERE EK_Installation IS NULL AND DateAllocated IS NULL LIMIT 0,$portNo",$conn);
if(mysql_num_rows($res)!=$portNo){
	die('ERROR: Not enough ports');
}

$pos=0;
$out=array();
while($row=mysql_fetch_assoc($res)){
	dbQuery("
		UPDATE RemoteAssistance 
		SET 
		EK_Installation=$installationID,
		Description='".$portsDestination[$pos]."',
		DateAllocated='$stamp'
		WHERE PK_Port=".$row['PK_Port'],$conn);
	$out[]=$portsDestination[$pos].'='.$row['Port'];
	$pos++;
}
dbQuery("COMMIT",$conn);
die("#Ports:\n".join("\n",$out)."\n#END\n");
?>
