<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function manufacturer_area($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$RecordsPerPage=20;
	$page=(isset($_REQUEST['page']))?((int)$_REQUEST['page']-1):0;
	$start=$page*$RecordsPerPage;

	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	$manufacturerUser=getFieldsAsArray('Manufacturer_Users','CanEditProfile,EK_Manufacturer,EK_Users,Validated',$manufacturerADO,'WHERE EK_Users='.$FK_Users);	
	
	if(isset($manufacturerUser['Validated'][0]) && $manufacturerUser['Validated'][0]!=1){
		header('Location: index.php?section=manufacturer_signin');
		exit();
	}else{
		$manufacturerID=$manufacturerUser['EK_Manufacturer'][0];
	}	
	$dtArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_Manufacturer='.$manufacturerID);
	$manufDT=getFieldsAsArray('Manufacturer_DeviceTemplate','EK_Manufacturer,EK_DeviceTemplate,Comments',$manufacturerADO,'WHERE EK_Manufacturer='.$manufacturerID);

if($action=='form'){
	
	$pages=(int)(count($manufDT['EK_DeviceTemplate'])/$RecordsPerPage);
	$dtTable='<div align="center">';
	for($i=0;$i<$pages;$i++){
		if($i==$page){
			$dtTable.=' '.($i+1).' ';
		}else{
			$dtTable.='<a href="index.php?section=manufacturer_area&page='.($i+1).'">[ '.($i+1).' ]</a> ';
		}
	}
	$dtTable.='</div>';
	
	$dtTable.='<table cellpadding="3" cellspacing="0">
		<tr bgcolor="lightblue">
			<td><B># </B></td>
			<td><B>Device Template</B></td>
			<td><B>Action</B></td>
		</tr>';	
	for($i=$start;($i<($start+$RecordsPerPage) && $i<count($manufDT['EK_DeviceTemplate']));$i++){
		$color=($i%2==0)?'#F0F3F8':'#FFFFFF';
		$dtTable.='
			<tr bgcolor="'.$color.'">
				<td>'.$manufDT['EK_DeviceTemplate'][$i].'</td>
				<td>'.$dtArray[$manufDT['EK_DeviceTemplate'][$i]].'</td>
				<td><input type="submit" name="del_'.$manufDT['EK_DeviceTemplate'][$i].'" value="Delete" onClick="if(!confirm(\'Are you sure you want to delete this device template?\'))return false;"></td>
			</tr>
			<tr bgcolor="'.$color.'">
				<td colspan="3"> '.nl2br($manufDT['Comments'][$i]).'</td>
			</tr>
		';
	}
	
	if(isset($_POST['dt'])){
		$extraRow='
		<tr>
			<td colspan="2" align="center">';
		if(!isset($dtArray[(int)$_POST['dt']])){
			$extraRow.='<span class="err">Device template does not exists.</span>';
		}else{
			$extraRow.='<span class="confirm">'.$dtArray[(int)$_POST['dt']].'</span>';
			if(in_array((int)$_POST['dt'],$manufDT['EK_DeviceTemplate']))
				$extraRow.=' <B>[Already added]</B>';
			else{
				$showSubmit=1;
			}
		}
		$extraRow.='</td>
		</tr>';
	}
	$dtTable.='
		<tr>
			<td colspan="2">&nbsp;</td>
		</tr>	
		<tr>
			<td colspan="2">Add device template <input type="text" name="dt" value="'.@$_POST['dt'].'" size="2"> <input type="button" name="checkDT" value="Check" onClick="document.manufacturer_area.action.value=\'form\';document.manufacturer_area.submit();"></td>
		</tr>
		'.@$extraRow;	
	if(@$showSubmit==1){
		$dtTable.='
		<tr>
			<td>Comments</td>
			<td align="left"><textarea name="comments"></textarea></td>
		</tr>		
		<tr>
			<td colspan="2" align="center"><input type="submit" name="add" value="Submit"></td>
		</tr>';
	}	
	$dtTable.='	
	</table>';

	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="manufacturer_area">
			<input type="hidden" name="section" value="manufacturer_area">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>Manufacturer area</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="right">&nbsp;'.(($manufacturerUser['CanEditProfile'][0]==1)?'<a href="index.php?section=edit_manufacturer_profile">Edit my profile</a>':'').'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Device templates</b></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center">'.$dtTable.'</td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center"></td>
      			</tr>	
      			<tr>
      				<td width="33%" class="normaltext" align="center">&nbsp;</td>
      			</tr>	
				<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Add device template</b></td>
      			</tr>	
			</table>
		</form>
      	';
}else{
	
	$dt=$_POST['dt'];
	$comments=stripslashes(@$_POST['comments']);

	if(isset($dtArray[$dt]) && !in_array((int)$_POST['dt'],$manufDT['EK_DeviceTemplate'])){
		$manufacturerADO->Execute('INSERT INTO Manufacturer_DeviceTemplate (EK_Manufacturer,EK_DeviceTemplate,Comments) VALUES (?,?,?)',array($manufacturerID,$dt,$comments));
		$msg='Device template '.$dtArray[$dt].' was added.';
	}elseif(isset($_POST['add'])){
		$err='Device template already added or does not exist.';
	}
	
	for($i=0;$i<count($manufDT['EK_DeviceTemplate']);$i++){
		if(isset($_POST['del_'.$manufDT['EK_DeviceTemplate'][$i]])){
			$manufacturerADO->Execute('DELETE FROM Manufacturer_DeviceTemplate WHERE EK_Manufacturer=? AND EK_DeviceTemplate=?',array($manufacturerID,$manufDT['EK_DeviceTemplate'][$i]));
			$msg='The device template '.$dtArray[$manufDT['EK_DeviceTemplate'][$i]].'was deleted.';
		}
	}
	
	$sufix=(@$err!='')?'&err='.$err:'&msg='.@$msg;
	header('Location: index.php?section=manufacturer_area'.$sufix);
	exit();
}


	$output->setNavigationMenu(array("My Pluto"=>"index.php?section=myPluto","Manufacturer Area"=>"index.php?section=manufacturer_area"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Dealer area");
	$output->output();
}
?>