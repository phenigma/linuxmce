<?
$_SESSION['package']=(isset($_SESSION['package']))?$_SESSION['package']:0;		// this should be the hard-coded value
$_SESSION['package']=(isset($_GET['package']))?$_GET['package']:$_SESSION['package'];

$_SESSION['editVar']=(isset($_SESSION['editVar']))?$_SESSION['editVar']:0;		// this should be the hard-coded value
$_SESSION['editVar']=(isset($_GET['edit']))?$_GET['edit']:$_SESSION['editVar'];

$package=(isset($_REQUEST['package']))?$_REQUEST['package']:$_SESSION['package'];

if($package!=0){
	$getPackage='SELECT * FROM Package WHERE PK_Package=?';
	$resPackage=$dbADO->Execute($getPackage,$package);
	$rowPackage=$resPackage->FetchRow();
	$_SESSION['packageName']=$rowPackage['Description'];
}
else 
	$_SESSION['packageName']="Pluto General Support";
?>