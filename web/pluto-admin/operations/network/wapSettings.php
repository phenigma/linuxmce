<?
function wapSettings($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */

	$settingsFile=$GLOBALS['wap.conf'];
	if(!file_exists($settingsFile)){
		$_REQUEST['error']='ERROR: The file '.$settingsFile.' does not exist.';
	}else{
		$fileArr=file($settingsFile);
		if($fileArr===false){
			$_REQUEST['error']='ERROR: The file '.$settingsFile.' could not be opened.';
		}else{
			$wapURL=trim(implode('',$fileArr));
		}
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = cleanInteger($_SESSION['installationID']);
	
	
	$out.='<div align="left"><h3>WAP/GPRS settings</h3></div>';
	
	if ($action=='form') {
		if(!isset($wapURL) || $wapURL==''){
			$coreFields=getFieldsAsArray('Device','IPAddress',$dbADO,'WHERE FK_Installation='.$installationID.' AND FK_DeviceTemplate='.$GLOBALS['rootCoreID']);
			$wapURL='http://'.$coreFields['IPAddress'][0].'/pluto-admin/check.wml';
		}

		$out.=setLeftMenu($dbADO).'
		
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div class="err"><B>'.@$_REQUEST['error'].'</B></div>
	<form action="index.php" method="post" name="wapSettings" onSubmit="return validateInput();">
	<input type="hidden" name="section" value="wapSettings">
	<input type="hidden" name="action" value="add">
	<p><B>Instructions:</B>	
	<p>1. You can add to your core a routable ip. When open your browser from the phone and go to <B>http://[ your_ip_address ]/pluto-admin/check.wml</B>.

	<p>2. If you don\'t want to add a routable ip to the core, you will need to use another linux machine which has one. There, you\'ll have to create a file "check.wml" which uses \'curl\' to access the wap page from the core:';
$code='
<?php
Header( "Content-type: text/vnd.wap.wml");
$Message=queryServer($_SERVER[\'QUERY_STRING\'],\'http://<your_core_ip_address>/pluto-admin/check.wml\');

print $Message;

function queryServer($params,$url)
{
$defined_vars = get_defined_vars();
$user_agent = @$defined_vars[\'HTTP_USER_AGENT\'];

$ch = curl_init();
$serverAddress=($_SERVER[\'QUERY_STRING\']!=\'\')?$url.\'?\'.$_SERVER[\'QUERY_STRING\']:$url;

curl_setopt($ch, CURLOPT_URL, $serverAddress);
curl_setopt($ch, CURLOPT_HEADER, 0);
curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);

$result=curl_exec ($ch);
curl_close ($ch);

return $result;
}
?>';
$out.='<br>'.highlight_string($code,true).'<br>';
$out.='
<p>There, you will be able to access the wap site from your phone\'s browser like this: <B>http://[ your_routable_ip_address ]/[ the_path_to_wml_page ]/check.wml</B>.
	<table>
		<tr>
			<td>Please enter WAP/GPRS URL</td>
			<td><input type="text" name="url" value="'.@$wapURL.'" size="50"></td>
		</tr>
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Update"></td>
		</tr>
	</table>
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=wapSettings&error=You are not authorised to change the installation.");
			exit();
		}
		
		if(isset($_POST['save'])){
			$url=$_POST['url'];
			
			$handle = @fopen($settingsFile, 'w');

			if (!@fwrite($handle, $url)) {
				header("Location: index.php?section=wapSettings&error=Cannot write to ".$settingsFile);
				exit();
			}
			fclose($handle);

			header("Location: index.php?section=wapSettings&msg=WAP/GPRS url was updated.");
			exit();
		}

		header("Location: index.php?section=wapSettings");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: WAP/GPRS settings');			
	$output->output();  		
}
?>
