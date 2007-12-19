<?
function vdr($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/vdr.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$files=grabFiles($GLOBALS['vdr_to_path']);		
	$vdr_plugin=getDeviceFromDT($installationID,$GLOBALS['VDR_Plugin'],$dbADO);	
	$selectedProvider=getDeviceData($vdr_plugin,$GLOBALS['DD_provider'],$dbADO);

	if($action=='form'){
		$installationData=getFieldsAsArray('Installation','Installation.FK_Country AS FK_Country,FK_City,FK_Region',$dbADO,'INNER JOIN City ON FK_City=PK_City WHERE PK_Installation='.$installationID);

		$country=$installationData['FK_Country'][0];
		$region=$installationData['FK_Region'][0];
		$city=$installationData['FK_City'][0];
		
		if($selectedProvider==0){
			$service=(int)@$_REQUEST['service'];			
		}else{
			$providerData=getFields('pluto_vdr.Provider','PK_Provider,Description,filename,FK_ProviderType,FTA,Radio,CA',$dbADO,'WHERE PK_Provider='.$selectedProvider);
			$service=$providerData[0]['FK_ProviderType'];
		}	

		$service=(isset($_REQUEST['service']))?(int)@$_REQUEST['service']:$service;
		$country=(isset($_REQUEST['country']))?(int)@$_REQUEST['country']:$country;
		$region=(isset($_REQUEST['country']))?(int)@$_REQUEST['region']:$region;
		$city=(isset($_REQUEST['country']))?(int)@$_REQUEST['city']:$city;
			
		$services=getAssocArray('pluto_vdr.ProviderType','PK_ProviderType','Description',$dbADO);
		$countries=getAssocArray('Country','PK_Country','Description',$dbADO,'','ORDER BY Description ASC');
		$regions=($city!=0)?getAssocArray('Region','PK_Region','Region',$dbADO,'WHERE FK_Country='.$country,'ORDER BY Region ASC'):array(0=>'Please select country');
		$cities=($region!=0)?getAssocArray('City','PK_City','City',$dbADO,'WHERE FK_Country='.$country,'ORDER BY City ASC'):array(0=>'Please select region');
		if(isset($_REQUEST['service'])){
			$fta_flag=(int)@$_REQUEST['fta_flag'];
			$radio_flag=(int)@$_REQUEST['radio_flag'];
			$ca_flag=(int)@$_REQUEST['ca_flag'];
		}elseif($selectedProvider!=0){
			$fta_flag=$providerData[0]['FTA'];
			$radio_flag=$providerData[0]['Radio'];
			$ca_flag=$providerData[0]['CA'];
		}else{
			$fta_flag=1;
			$radio_flag=1;
			$ca_flag=1;
		}
				
		$out.='
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="vdr">
			<input type="hidden" name="section" value="vdr">
			<input type="hidden" name="action" value="add">
		
			<table>
				<tr>
					<td><b>'.$TEXT_COUNTRY_CONST.'</b></td>
					<td>'.pulldownFromArray($countries,'country',$country,'onChange="document.vdr.action.value=\'form\';document.vdr.submit();"').'</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_REGION_CONST.'</b></td>
					<td>'.pulldownFromArray($regions,'region',$region,'onChange="document.vdr.action.value=\'form\';document.vdr.submit();"').'</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_CITY_CONST.'</b></td>
					<td>'.pulldownFromArray($cities,'city',$city,'onChange="document.vdr.action.value=\'form\';document.vdr.submit();"').'</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_CHOOSE_TV_SERVICE_CONST.'</b></td>
					<td>'.pulldownFromArray($services,'service',$service,'onChange="document.vdr.action.value=\'form\';document.vdr.submit();"').'</td>
				</tr>
				<tr>
					<td colspan="2">
						<input type="checkbox" name="fta_flag" value="1" '.(($fta_flag==1)?'checked':'').' title="'.htmlentities($TEXT_FTA_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> FTA  
						<input type="checkbox" name="radio_flag" value="1" '.(($radio_flag==1)?'checked':'').' title="'.htmlentities($TEXT_RADIO_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> Radio 
						<input type="checkbox" name="ca_flag" value="1" '.(($ca_flag==1)?'checked':'').' title="'.htmlentities($TEXT_CA_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> CA</td>
				</tr>				
				';
			if($service!=0 && $country!=0){
				$where="INNER JOIN pluto_vdr.ProviderCountries ON FK_Provider=PK_Provider WHERE FK_ProviderType=$service AND FK_Country=$country";
				$where.=" AND FTA=$fta_flag AND Radio=$radio_flag AND CA=$ca_flag";
				$where.=" GROUP BY FK_Provider";
				$providers=getFields('pluto_vdr.Provider','PK_Provider,Description,filename',$dbADO,$where);
				
				if(count($providers)>0){
					$out.='
					<tr>
						<td colspan="2">
						<table width="100%" cellpadding="1" cellspacing="1">
							<tr>
								<td colspan="3"  style="background:#FFFFFF;"><b>'.$TEXT_PROVIDERS_CONST.'</b></td>
							</tr>';
							for($i=0;$i<count($providers);$i++){
								$color=($providers[$i]['PK_Provider']==$selectedProvider)?'#DEDEDE':'#FFFFFF';
								$out.='
								<tr>
									<td  style="background:'.$color.';">'.$providers[$i]['Description'].'</td>
									<td  style="background:'.$color.';">'.$providers[$i]['filename'].'</td>
									<td  style="background:'.$color.';" align="right"><a href="index.php?section=vdr&action=pick&providerID='.$providers[$i]['PK_Provider'].'">Pick</a></td>
								</tr>						
								';
							}
						$out.='</td>
							</tr>';	
				}else{
					$out.='
					<tr>
						<td>'.$TEXT_NO_RECORDS_CONST.'</td>
						<td></td>
					</tr>';
					
				}				
			}
			$out.='
			</table>
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=vdr&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		$providerID=(int)@$_REQUEST['providerID'];
		if($providerID!=0){
			$providerData=getFields('pluto_vdr.Provider','PK_Provider,Description,filename',$dbADO,'WHERE PK_Provider='.$providerID);
			if(count($providerData)==0){
				header("Location: index.php?section=vdr&error=".urlencode('Provider not found'));
				exit();
			}else{
				// ********************* tmp code **************************
				// todo: remove php move
				// move selected file in target directory and rename it as channels.conf
				
				$cmd='mv "'.$GLOBALS['vdr_from_path'].$providerData[0]['filename'].'" "'.$GLOBALS['vdr_to_path'].'channels.conf"';
				exec_batch_command($cmd);
				
				// ************************** end tmp code *****************
				
				$cmd='sudo -u root /usr/pluto/bin/setVDR.sh '.$providerData[0]['filename'];
				exec_batch_command($cmd);
				
				set_device_data($vdr_plugin,$GLOBALS['DD_provider'],$providerID,$dbADO);
				
				header("Location: index.php?section=vdr&msg=".urlencode('Provider updated.'));
				exit();
			}
		}
		
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_VDR_SETUP_CONST);
	
	$output->setNavigationMenu(array($TEXT_VDR_SETUP_CONST=>'index.php?section=vdr'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_VDR_SETUP_CONST);
	$output->output();
}
?>