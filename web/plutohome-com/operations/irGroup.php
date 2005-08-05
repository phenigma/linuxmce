<?
function irGroup($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$publicADO->debug=false;
	$installationID = 0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
   $action1 = isset($_REQUEST['action1'])?cleanString($_REQUEST['action1']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$label=(isset($_REQUEST['label']))?$_REQUEST['label']:'infrared';
	$GLOBALS['label']=$label;
	//$action1=isset($_POST['action1'])?$_POST['action1']:'';
	$irgID = (int)$_REQUEST['irgID'];
   $deviceID=isset($deviceID)?$deviceID:'';
   $dtID=isset($dtID)?$dtID:'';
   $oldIRGroup=isset($_POST['oldIRGroup'])?$_POST['oldIRGroup']:'';
	$userID = isset($_SESSION['userID'])?(int)@$_SESSION['userID']:'';
	if ($irgID==0) {
		header("Location: index.php?section=login");
	}

	$GLOBALS['displayedIRGC']=array();
	$GLOBALS['preferredIGC']=array();
	$GLOBALS['igcPrefered']=array();	

	if ($action=='form') { 
		$irgData=getAssocArray('InfraredGroup','PK_InfraredGroup','Description',$publicADO,'WHERE PK_InfraredGroup='.$irgID);
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
         function clipboard(text)
     {
           texta=document.irGroup.text.value;
		window.setInterval("window.clipboardData.setData(\'Text\',\'[The clipboard is temporarily disabled. To restore normal function, simply close the web browser currently viewing a full size image at {site_name}]\')",20); 
     }
		</script>	
      	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		<form action="index.php" method="POST" name="irGroup">
			<input type="hidden" name="section" value="irGroup">
			<input type="hidden" name="action" value="form">
         <input type="hidden" name="action1" value="">
			<input type="hidden" name="irgID" value="'.$irgID.'">
			<input type="hidden" name="irgroup_command" value="">
			<input type="hidden" name="label" value="'.$label.'">';

		$out.='
		<h3>Edit IR Group #'.$irgID.' '.$irgData[$irgID].'</h3>
		<p class="normaltext"><a href="index.php?section=irg_models&irgID='.$irgID.'">Show models using this group</a>
		<table border="0" width="100%" class="normaltext">
		';

      if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){  
			$irg_c=(int)$_POST['irgroup_command'];
			if($action1!='delete' && $action1!='add' && $action1!='edit'){ 
				$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.','.$dtID.',IRData,'.$_SESSION['userID'].' FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&msg=Custom code added.&label=".$GLOBALS['label']);
				exit();
			}elseif( $action1!='add' && $action1!='edit'){
				$publicADO->Execute('
					DELETE InfraredGroup_Command_Preferred 
					FROM InfraredGroup_Command_Preferred
					INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
					WHERE PK_InfraredGroup_Command=?',$irg_c);
				
				$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
				$publicADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=?',$irg_c);
				
				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&irgID=$irgID&msg=Custom code deleted.&label=".$GLOBALS['label']);
				exit();
			}
      
		}
     
      if($action1=='add'){  
           $customCodesNoArray=explode(',',@$_POST['commandsDisplayed']);
			foreach ($customCodesNoArray as $ig_c){
				$irData1=(isset($_POST['isHidden_'.$ig_c]))?addslashes(urldecode(@$_POST['irData1_'.$ig_c])):addslashes(@$_POST['irData1_'.$ig_c]);
          if($irData1!='')
           { //echo $irData1; exit;
          $publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command, FK_Device, FK_DeviceTemplate, FK_Users,IRData) VALUES("'.$irgID.'","'.$irg_c.'","'.$deviceID.'","'.$dtID.'","'.$_SESSION['userID'].'","'.$irData1.'")');
			$igcID=$publicADO->Insert_ID();
           }
			
			}
				$publicADO->Execute('INSERT INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($igcID,$installationID));		
				
         
				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&irgID=$irgID&msg=Custom code has been added.&label=".$GLOBALS['label']);
				exit();
         }
		   if($action1=='edit'){  
           $customCodesNoArray=explode(',',@$_POST['commandsDisplayed']);
			foreach ($customCodesNoArray as $ig_c){
           if($ig_c==$_POST['irgroup_command'])
           {
				$irData=(isset($_POST['isHidden_'.$ig_c]))?stripslashes(urldecode(@$_POST['ir_'.$ig_c])):stripslashes(@$_POST['ir_'.$ig_c]);
            //echo $irData.'<br>'.$ig_c;
				$publicADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
           }
			}
		
		header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&irgID=$irgID&msg=Custom code has been edited.&label=".$GLOBALS['label']);
				exit();
         }
         if($action1=='addnew'){  
           $text=1; die($action1);
         }
         else $text=0;
		$ad='';
		$commandsDisplayed=array();
		$commandID=0;$group1='';
			$queryStandardCode='select PK_InfraredGroup_Command,FK_CommandCategory,PK_Command,FK_Command,FK_Users,FK_DeviceTemplate,FK_Device,InfraredGroup_Command.psc_user,IRData,Command.Description,
IF(PK_Command=192 OR PK_Command=193 OR PK_Command=194,1,
IF(FK_CommandCategory=22,2,
IF(FK_CommandCategory=27,3,
IF(FK_CommandCategory=21,4,5)
)
  )
) AS GroupOrder,
IF( InfraredGroup_Command.psc_user=?,2,
   IF( FK_DeviceTemplate IS NULL AND FK_Device IS NULL,1,3)
) As DefaultOrder,
CommandCategory.Description AS CommandCategory
FROM InfraredGroup_Command
JOIN Command ON FK_Command=PK_Command
JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory
where FK_InfraredGroup=? 
ORDER BY GroupOrder,CommandCategory.Description,Description,DefaultOrder';
			//$resUserCode=$publicADO->Execute($queryUserCode,array($dtID));
		$out.='<table width="100%" cellpadding="3" cellspacing="2">
				<tr bgcolor="lightblue">
					<td align="center" width="100"><B>Command</B></td>
					<td width="25"><b>DT</b></td>
					<td><B>IR Code</B></td>
					<td align="center" width="100"></td>
				</tr>';
		$resCommand=$publicADO->Execute($queryStandardCode,array($userID,$irgID));
		$commandsToShow=array();
		$existingCodes=$resCommand->RecordCount()+$resCommand->RecordCount();
		while($rowStandardCode=$resCommand->FetchRow()){
        $power=array('192','193','194');
        $input=22;
        $dspmode=21;
        $outputf=27;
        $edit='';
        $read='READONLY';
        if(in_array($rowStandardCode['FK_Command'],$power)) $group='POWER';
        elseif($rowStandardCode['FK_CommandCategory']==$input) $group='INPUT';
        elseif($rowStandardCode['FK_CommandCategory']==$dspmode) $group='DSPMODE';
        elseif($rowStandardCode['FK_CommandCategory']==$outputf) $group='OUTPUT';
        else 
        {
          $queryStandardCategory='select Description FROM CommandCategory where PK_CommandCategory=?';
          $resCommandCategory=$publicADO->Execute($queryStandardCategory,array($rowStandardCode['FK_CommandCategory']));
          $rowStandardCategory=$resCommandCategory->FetchRow();
          $group=$rowStandardCategory['Description'];
        }
        $commandsToShow[]=$rowStandardCode['FK_Command'];
		  $commandsDisplayed[]=$rowStandardCode['PK_InfraredGroup_Command'];
        if($rowStandardCode['FK_Users']==$userID)
        {
          $RowColor="yellow";
          $edit='<input type="button" name="learnCode" value="Edit code" onClick="windowOpen(\'index.php?section=editIRGCode&deviceID='.$rowStandardCode['FK_DeviceTemplate'].'&dtID='.$dtID.'&infraredGroupID='.$rowStandardCode['PK_InfraredGroup_Command'].'&commandID='.$rowStandardCode['PK_Command'].'&action=sendCommand1\',\''.(($GLOBALS['label']=='infrared')?'width=750,height=310':'width=600,height=650').',toolbars=true,scrollbars=1,resizable=1\');" >
          <br><input type="button" class="button" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.irGroup.action1.value=\'delete\';document.irGroup.irgroup_command.value='.$rowStandardCode['PK_InfraredGroup_Command'].';document.irGroup.submit();}">';
          $read='READONLY';
        }
        elseif($rowStandardCode['FK_DeviceTemplate']=='')
        {
          $RowColor='lightblue';
       
          
        }
        elseif($rowStandardCode['FK_Device']=='')
        {
          $edit='';
          $RowColor='lightyellow';
          $read='READONLY';
        }
        else $RowColor='lightyellow';
			if(is_null($deviceID)){
				$GLOBALS['displayedIRGC'][]=$rowStandardCode['PK_InfraredGroup_Command'];
			}
			if(is_null($deviceID)){
				$textAreaElem='<textarea name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%">'.$rowStandardCode['IRData'].'</textarea>';
            $textname="irData_".$rowStandardCode['PK_InfraredGroup_Command'];
			}else{
				$textAreaElem='
					<textarea name="ir_'.$rowStandardCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%" '.$read.'>'.$rowStandardCode['IRData'].'</textarea>
					<input type="hidden" name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="'.urlencode(addslashes($rowStandardCode['IRData'])).'">
					<input type="hidden" name="isHidden_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="1">
               <input type="hidden" name="idCommand" value="'.$rowStandardCode['FK_Command'].'">
                 ';
                $textname="ir_".$rowStandardCode['PK_InfraredGroup_Command'];
			}
         if(isset($_SESSION['userID']))
     {    
           if($rowStandardCode['FK_DeviceTemplate']!='')
           $ad='<input type="button" class="button" name="learnCode" value="New code" onClick="windowOpen(\'index.php?section=newIRGCode&deviceID='.$rowStandardCode['FK_DeviceTemplate'].'&dtID='.$rowStandardCode['FK_DeviceTemplate'].'&infraredGroupID='.$irgID.'&commandID='.$rowStandardCode['PK_Command'].'&action=sendCommand\',\''.(($GLOBALS['label']=='infrared')?'width=750,height=310':'width=600,height=650').',toolbars=true,scrollbars=1,resizable=1\');">';
           else $ad='<input type="button" class="button" name="learnCode" value="New code" onClick="windowOpen(\'index.php?section=newIRGCode&deviceID='.$rowStandardCode['FK_DeviceTemplate'].'&dtID='.$dtID.'&infraredGroupID='.$irgID.'&commandID='.$rowStandardCode['PK_Command'].'&action=sendCommand1\',\''.(($GLOBALS['label']=='infrared')?'width=750,height=310':'width=600,height=650').',toolbars=true,scrollbars=1,resizable=1\');">';
     }
     if($text==1)
        {
        $texta='<textarea name="irData1_'.$rowStandardCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%"></textarea>';
        }
        else $texta='';
         if($group1!=$group)
         $out.='
				<tr >
					<td align="center"  colspan="4"><B>'.$group.'</B></td>
				</tr>';
            if($rowStandardCode['FK_DeviceTemplate']!=0 && $rowStandardCode['FK_DeviceTemplate']!='')
              $devicet=$rowStandardCode['FK_DeviceTemplate'];
            else $devicet='';
            $code=addslashes(urlencode($rowStandardCode['IRData']));
			$out.='
				<tr bgcolor="'.$RowColor.'">
					<td align="center" width="100"><B>'.$rowStandardCode['Description'].'</B><br> User: '.$rowStandardCode['FK_Users'].'</td>
					<td width="25"><a href="index.php?section=editAVDevice&deviceID='.$rowStandardCode['FK_DeviceTemplate'].'">'.$devicet.'</a></td>
					<td>'.((is_null($rowStandardCode['PK_InfraredGroup_Command']))?'<B> Not in database</B>':$textAreaElem).'<br>'.$texta.'</td>
					<td align="left" width="100"><input type="button" name="select" value="Copy To Cliboard" onClick="javascript:this.form.'.$textname.'.focus();this.form.'.$textname.'.select();window.clipboardData.setData(\'Text\',\''.$code.'\'); ">'.$edit.$ad.'</td>
				</tr>';
            $group1=$group;
            
		}
         
   
		 $out.='
		</table>
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
			<input type="hidden" name="commandsDisplayed" value="'.join(',',$commandsDisplayed).'">
		</form>';
	}
  
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: IR Codes');
	$output->output();
}


?>
