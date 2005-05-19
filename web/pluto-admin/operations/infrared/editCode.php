<?php
function editCode($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$ircode = (int)@$_REQUEST['ircode'];
	if($ircode==0){
		die('Invalid ID.');
	}
	
	if ($action=='form') {		
		$res=$dbADO->Execute('SELECT IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$ircode);
		if($res->RecordCount()!=0){
			$row=$res->FetchRow();
			$oldData=$row['IRData'];
		}else{
			$_GET['error']='IR/GSD code doesn\'t exists.';
		}
		
		$out.='
		<script language="JavaScript" type="text/javascript" src="scripts/wysiwyg/richtext.js"></script>
		<script language="JavaScript" type="text/javascript">
		<!--
		//Usage: initRTE(imagesPath, includesPath, cssFile)
		initRTE("scripts/wysiwyg/images/", "scripts/wysiwyg/", "scripts/wysiwyg/");
		//-->
		</script>		
		
		<div align="center" style="height:90%; width:99%;">
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm" align="center"><B>'.stripslashes(@$_GET['msg']).'</B></div>
		
		
		<form action="index.php" method="post" name="editCode" onSubmit="updateRTEs();">
		<input type="hidden" name="section" value="editCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="ircode" value="'.$ircode.'">		
		<B>IR/GSD code</B><br>
<script language="JavaScript" type="text/javascript">
<!--
//Usage: writeRichText(fieldname, html, width, height, buttons, readOnly)
writeRichText(\'irdata\', \''.addslashes(str_replace(array("\r\n"),'<br>',@$oldData)).'\', 770, 460, true, false);
//-->
</script>
		<input type="submit" class="button" name="save" value="Update"> <input type="button" class="button" name="close" value="Close" onclick="self.close();">
		</form>
		</div>
		';
		
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editCode&ircode=$ircode&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$irData=stripslashes(str_replace("\r\n","",$_POST['irdata']));
		$irData=str_replace("<br>","\r\n",$irData);
		$irData=unhtmlentities($irData);
		
		
		$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array(trim($irData),$ircode));
		$out.="
			<script>
			    opener.location.reload();
				self.location='index.php?section=editCode&ircode=$ircode&msg=The code was updated.';
			</script>
				";			
		
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit IR/GSD code');			
	$output->output();
}

function unhtmlentities($chaineHtml)
{ 
	$tmp = get_html_translation_table(HTML_ENTITIES);
	$tmp = array_flip ($tmp);
	$chaineTmp = strtr ($chaineHtml, $tmp);
	return $chaineTmp;
}
?>
