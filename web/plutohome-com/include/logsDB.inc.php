<?
	/* @var $logsADO ADOConnection */
	

  	$LogsDSN = $dbLogsType.'://'.urlencode($dbLogsUser).':'.urlencode($dbLogsPass).'@'.$dbLogsServer.'/'.urlencode($dbLogsDatabase); 
  	$logsADO = &ADONewConnection($LogsDSN);
  	
  	$parms=($_SERVER['QUERY_STRING']!='')?$_SERVER['QUERY_STRING']:'POST';
  	$logsADO->Execute("INSERT INTO SiteLog (Data,FK_MasterUser,Section,Params) VALUES (NOW(),?,?,?)",array(@$_SESSION['userID'],@$_REQUEST['section'],$parms));
  	$insertID=$logsADO->Insert_ID();
  	
  	
	if(@$_REQUEST['section']=='wizard' && isset($_REQUEST['action']) && $_REQUEST['action']!='form'){
		$step=$_REQUEST['step'];
		switch ($step){
			case 1:
				$stepActions=(int)$_POST['sollutionType'];
			break;
			case 2:
				$stepActions=(int)$_POST['dedicated'];
			break;
			case 3:
				$doNothing=1;
			break;
			case 4:
				$doNothing=1;
			break;
			case 5:
				$stepActions='('.$_SESSION['installationID'].')'.(isset($_POST['installSourceCode']))?'source,':'';
				$stepActions.=((int)@$_POST['enableDHCP']==1)?'dhcp,':'';
				foreach($_POST AS $key=>$value){
					if(ereg('device_',$key) && ereg('_requiredTemplate_',$key) && $value==1){
						$pattern=(isset($_SESSION['CoreDCERouter']))?'device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_':'device__requiredTemplate_';
						$stepActions.=str_replace($pattern,'',$key).',';
					}
				}
				$stepActions=substr($stepActions,0,-1);
			break;
			case 6:
				$stepActions='('.$_SESSION['installationID'].')'.((isset($_POST['disklessBoot_']))?'net,':'');
				foreach($_POST AS $key=>$value){
					if(ereg('device__requiredTemplate_',$key) && $value==1){
						$stepActions.=str_replace('device__requiredTemplate_','',$key).',';
					}
				}
				$stepActions=substr($stepActions,0,-1);
			break;
			case 7:
				$doNothing=1;
			break;
			case 8:
				$stepActions=$_REQUEST['action'];
			break;
		}
		
		if(@$doNothing!=1)
			$logsADO->Execute("INSERT INTO WizardLog (FK_SiteLog,Step,Actions) VALUES (?,?,?)",array($insertID,$step,@$stepActions));
	}
	$logsADO->Close();
?>
