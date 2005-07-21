<?
function mediaProviders($output,$mediadbADO,$dbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$mtArray=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');
	$mtArray=array_map('cleanMediaType',$mtArray);
	
	if($action=='form'){
		
		$mpArray=getFieldsAsArray('MediaProvider','PK_MediaProvider,Description,EK_MediaType',$mediadbADO,'','ORDER BY Description ASC');
		$mpRows='';
		for($i=0;$i<count(@$mpArray['PK_MediaProvider']);$i++){
			$color=($i%2!=0)?'#F0F3F8':'#FFFFFF';
			$mpRows.='
			<tr bgcolor="'.$color.'">
				<td>'.$mpArray['Description'][$i].'</td>
				<td>'.$mtArray[$mpArray['EK_MediaType'][$i]].'</td>
				<td><a href="javascript:if(confirm(\'Are you sure you want to delete this media provider?\'))self.location=\'index.php?section=mediaProviders&action=del&dmp='.$mpArray['PK_MediaProvider'][$i].'\'">Delete</a></td>
			</tr>		
			';
		}
		if(count(@$mpArray['PK_MediaProvider'][0])==0){
			$mpRows.='
			<tr>
				<td colspan="3" align="center">No media providers.</td>
			</tr>		
			';
		}
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<h3 align="center">Media Providers</h3>
		<form action="index.php" method="POST" name="mediaProviders">
			<input type="hidden" name="section" value="mediaProviders">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" cellspacing="0" align="center">
			<tr bgcolor="lightblue">
				<td colspan="3" align="center"><B>Media Providers</B></td>
			</tr>		
			<tr bgcolor="#F0F3F8">
				<td align="center"><B>Description</B></td>
				<td align="center"><B>Media type</B></td>
				<td align="center"><B>Action</B></td>
			</tr>		
			'.$mpRows.'
			<tr>
				<td colspan="3" align="center">&nbsp;</td>
			</tr>		

			<tr bgcolor="lightblue">
				<td colspan="3" align="center"><B>Add Media Provider</B></td>
			</tr>		
			<tr>
				<td><B>Description</B></td>
				<td colspan="2"><input type="text" name="description" value=""></td>
			</tr>		
			<tr>
				<td><B>Media Type</B></td>
				<td colspan="2">'.pulldownFromArray($mtArray,'type',0).'</td>
			</tr>		
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="add" value="Add Media Provider"></td>
			</tr>		
		</table>
		
		</form>';
	}else{
		if((int)@$_REQUEST['dmp']>0){
			$mediadbADO->Execute('DELETE FROM MediaProvider WHERE PK_MediaProvider=?',(int)$_REQUEST['dmp']);
			header('Location: index.php?section=mediaProviders&msg=Media provider was deleted.');
			exit();
		}
		
		$description=stripslashes($_POST['description']);
		$type=(int)$_POST['type'];
		if($description!='' && $type>0){
			$mediadbADO->Execute('INSERT INTO MediaProvider (Description,EK_MediaType) VALUES (?,?)',array($description,$type));
			if($mediadbADO->Affected_Rows()>0)
				$msg='The media provider was added';
		}
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=mediaProviders'.$suffix);
	}
	
	$output->setNavigationMenu(array("Media providers"=>'index.php?section=mediaProviders'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>