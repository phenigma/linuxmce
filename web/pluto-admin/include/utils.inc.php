<?php 
/* @var $dbADO ADOConnection */

function eval_buffer($string) {
   ob_start();
   eval("$string[2];");
   $return = ob_get_contents();
   ob_end_clean();
   return $return;
}

function eval_print_buffer($string) {
   ob_start();
   eval("print $string[2];");
   $return = ob_get_contents();
   ob_end_clean();
   return $return;
}

function showSectionContentWithPhp($string) {
	
   $string = preg_replace_callback("/(<\?=)(.*?)\?>/si",
                                   "eval_print_buffer",$content);
   $string=preg_replace_callback("/(<\?php|<\?)(.*?)\?>/si",
                                 "eval_buffer",$string);
   return stripslashes($string);                                 	
} 

// display array nicely formated - used for debug only
function print_array($arr)
{
	print '<pre>';
	print_r($arr);
	print '</pre>';
}

//---------------------------------------------------------------------------------------
//customized data format
//---------------------------------------------------------------------------------------
function formatDate($strData)
{
  $result = trim( $strData);
  $result = str_replace( "  ", " ", $result );
  //list( $aMonth, $aDay, $aYear, $theRest ) = explode( " ", $result, 4 );
  $result = date( "j M, Y",strtotime($result));
  return $result;
}
function formatTime($strData)
{
  $result = trim( $strData);
  $result = str_replace( "  ", " ", $result );
  //list( $aMonth, $aDay, $aYear, $theRest ) = explode(" ", $result, 4);
  $result = date( "G:i:s A", strtotime($result));
  return $result;
}
function formatNumber($nr) {return number_format($nr,0,',','.');}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//listing functions
//---------------------------------------------------------------------------------------
function getPagesMenu($query,$start,$items,$orderBy,$orderType,$url)
{
 $pages_menu = '';
 $urlStr = $GLOBALS['vRoot'].$url;
 $_SESSION['itemsPerPage'] = $items;

 if(($orderBy!='')&&(($orderType!=''))) {
   $_SESSION['sortOrder'] = $orderBy;
   $_SESSION['sortType']  = $orderType;
 }
 
 $res = @mysql_query($query);
 $rows = @(int)mysql_num_rows($res);
 if( ($rows>0) && ($rows>$items) ){
   
     $pages = $rows/$items;
     
     $prev = $start - $items;
     $crt_page = $start / $items;
     $first_page = $crt_page - $GLOBALS["prevNo"] + 1;
     $last_page  = $crt_page + $GLOBALS["nextNo"];
     
     //initialize if necessary
     if ($prev < 0) {$prev = 0;}
     if ($first_page < 1) {$first_page= 1;}
     if ($last_page > $pages) {$last_page = $pages;}
     
     if ($start!=0) {$pages_menu .= '<a class="smallb" href="'.$urlStr.'start='.$prev.'&orderBy='.$orderBy.'&orderType='.$orderType.'">&#171;</a>';}
     for ($i=$first_page; $i<=(int)$last_page; $i++)
     {
      if (($i-1)*$items==$start) {//crt page
       $crt_pos = $i;
       $pages_menu.= ' [<a class="smallnote" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'"><b>'.$i.'</b></a>]';
      } 
      else {$pages_menu.= ' [<a class="small" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'">'.$i.'</a>]';}
     }
     //last items page
     if (($items*($i-1)) != $rows)
      {
       if (($i-1)*$items==$start){
          $pages_menu.= ' [<a class="smallnote" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'"><b>'.$i.'</b></a>]';
       }else{
          $pages_menu.= ' [<a class="small" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'">'.$i.'</a>]';
       }
      }
     
    if (((@$crt_pos * $items) != $rows) && ($start!=($items*($i-1)))) {
      $pages_menu.= ' <a class="smallb" href="'.$urlStr.'start='.$items*(@$crt_pos).'&orderBy='.$orderBy.'&orderType='.$orderType.'">&#187;</a>';}
      
    $pages_menu = '<table cellpadding="3" cellspacing="0" border="0"><tr><td class="small">
                  '.$pages_menu.'</td></tr></table>';
 } 
 
 return $pages_menu;
}

//---------------------------------------------------------------------------------------

function die_admin($output,$message,$sourceScript,$query) {
$out=$message."<br />"."query:".$query;
$output->set_title(ApplicationName."::Error");
$output->set_body($out);
$output->do_output();
die();
}

function die_mesg_public($output,$message,$sourceScript,$query) {
	
	
	if ($GLOBALS["inDebug"]==1) {
		$queryTemp=explode("::",$query);
		if (count($queryTemp)==2) {
				$out="<h2>".$message."</h2><h3>Source:$sourceScript</h3><h4>SQL Error:".$queryTemp[0]."</h4><h4>".$queryTemp[1]."</h4>";
		}else{
				$out="<h2>".$message."</h2><h3>Source: $sourceScript</h3><h4>Error:".$query."</h4>";
		}		
		$output->setBody($out);
		$output->output();
	} else {
		$queryTemp=explode("::",$query);
		if (count($queryTemp)==2) {
				$mailBody=$message."\n\n Source:$sourceScript \n\n SQL Error:".$queryTemp[0]." \n\n ".$queryTemp[1]."";
		}else{
				$mailBody=$message." \n\n Source: $sourceScript \n\n ".$query;
		}	
		if (strlen($GLOBALS['sendErrorsTo'])>2) {
			mail($GLOBALS['sendErrorsTo'],"Error on site:".$GLOBALS['vRoot'],$mailBody);
		}
		$out="An error occured and an email about it was sent to the webadmin.<br />Sorry for the incovenience!";
		$output->setBody($out);
		$output->output();
	}
//$output->do_output();
die();
}


function getPagesNextPrev($pagesNumber,$currentPage) {
$output = "";
        if ($pagesNumber > 1) {
            $output.= "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"> ";
            $output.= "<tr>";
            $output.= "<td class=\"small\" width=\"25%\" nowrap>";
            if ($currentPage-5>1) {
             $startShowPrevNext  = $currentPage - 5;
            } else {
             $startShowPrevNext  = 1;
            }
            if ($currentPage+5<=$pagesNumber) {
             $endShowPrevNext = $currentPage+5;
            } else {
             $endShowPrevNext = $pagesNumber;
            }

            for ($jk=$startShowPrevNext;$jk<=$endShowPrevNext;$jk++) {
                if ($currentPage==$jk) {
                    $output.="[<b>" + $jk + "</b>]";
                } else {
                    $output .= "[<a class=\"small\" href=\"javascript:changePage(".$jk.");\">".$jk."</a>]";
                }
                $output .= "&nbsp;";
            }
            $output .= "</td>";
            $output .= "<td width=\"19%\" class=\"small\" align=\"right\" nowrap>Go to page:
                    <input class=\"small\" type=\"text\" name=\"gotoPageNo\" maxlength=\"3\" size=\"2\">&nbsp;<input type=\"button\" class=\"small\" value=\"Go\" onClick=\"javascript:formSubmit(this.form);\" id=button1 name=button1>
                    </td>
                    <td class=\"small\" align=\"right\" width=\"1%\"><img src=\"images/spacer.gif\" width=\"29\" height=\"8\"></td>
                    </tr>
                    </table>";
        } else {
            $output = "&nbsp;";
        }
        return $output;
} 

function cleanString($string,$len=1000) {
	return substr(strip_tags(stripslashes(trim($string))),0,$len);
}

function cleanStringWithTags2Show($string,$len=1000) {
	return trim($string);
}

function cleanStringWithTags4DB($string,$len=1000) {
	return mysql_escape_string(trim($string));
}

function cleanInteger($number) {
	return (int)$number;
}

function cleanFloat($number) {
	return (float)$number;
}

function cleanArray($array2Parse) {
	$resultArray = array();
	foreach ($array2Parse as $elem) {
		if (cleanString((trim($elem)))!="") {
			$resultArray[]=$elem;			
		}
	}
	return $resultArray;
}



function getChildsNo($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	$childsNo=0;
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$childsNo++;
				$childsNo+=getChildsNo($row['PK_Device'],$dbADO);
		}
	}
	return $childsNo;
}

$GLOBALS['childsArray'] = array();

function getDeviceChildsArray($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsArray'][]=$row['PK_Device'];
				$GLOBALS['childsArray'][]=getDeviceChildsArray($row['PK_Device'],$dbADO);
		}
	}
	//return $childsArray;
}


$GLOBALS['childsDeviceCategoryNo']=0;
function getDeviceCategoryChildsNo($parentID,$dbADO) {
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsDeviceCategoryNo']++;
				$GLOBALS['childsDeviceCategoryNo']+=getDeviceCategoryChildsNo($row['PK_DeviceCategory'],$dbADO);
		}
	}	
}

$GLOBALS['childsDeviceCategoryArray'] = array();

function getDeviceCategoryChildsArray($parentID,$dbADO) {
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsDeviceCategoryArray'][]=$row['PK_DeviceCategory'];				
				$GLOBALS['childsDeviceCategoryArray'][]=getDeviceCategoryChildsArray($row['PK_DeviceCategory'],$dbADO);
		}
	}
}

function getDeviceCategoryChildsOptions($parentID,$parentName,$selectedValue,$notIn,$dbADO) {
	$whereNotIn = '';
	if (strlen($notIn)!='') {
		$whereNotIn=' and PK_DeviceCategory not in ('.$notIn.')';
	}
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = $parentID $whereNotIn order by Description Asc";
	$resGP = $dbADO->Execute($queryGP);
	$options='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
			@$GLOBALS['categoriesArray'][$row['PK_DeviceCategory']]=$parentName.' - '.$row['Description'];
			$options.= '<option '.($selectedValue==$row['PK_DeviceCategory']?' selected="selected" ':'').' value="'.$row['PK_DeviceCategory'].'">'.$parentName.' - '.$row['Description'].' #'.$row['PK_DeviceCategory'].'</option>';
			$options.= getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$parentName.' - '.$row['Description'],$selectedValue,$notIn,$dbADO);
		}
		$resGP->Close();
	}
	return $options;
}


function getEventCategoryChildsArray($parentID,$parentName,$selectedValue,$dbADO) {
	$queryGP = "select PK_EventCategory,Description from EventCategory where  FK_EventCategory_Parent = $parentID order by Description Asc";
	$resGP = $dbADO->Execute($queryGP);
	$EventCategoryOptions = '';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {				
				$EventCategoryOptions.= '<option '.($selectedValue==$row['PK_EventCategory']?' selected="selected" ':'').' value="'.$row['PK_EventCategory'].'">'.$parentName.' - '.$row['Description'].'</option>';
				$EventCategoryOptions.= getEventCategoryChildsArray($row['PK_EventCategory'],$parentName.' - '.$row['Description'],$selectedValue,$dbADO);
		}
	}	
	return $EventCategoryOptions;
}

/*
check if the user has permission for this field
*/

function checkOwner($userIDField,$pkField,$pkValue,$table,$dbADO,$fkField='',$fkValue='') {
	$currentUser = (int)@$_SESSION['userID'];
	
	if ($currentUser!=0) {
		if (trim($fkField)!='') {
			$querySelOwner = 'SELECT '.$userIDField.' FROM '. $table .' WHERE '.$userIDField.'= '.$currentUser." AND $pkField='$pkValue' AND $fkField='$fkValue'";
		} else {
			$querySelOwner = 'SELECT '.$userIDField.' FROM '. $table .' WHERE '.$userIDField.'= '.$currentUser." AND $pkField='$pkValue'";
		}
		
		$resSelOwner = $dbADO->_Execute($querySelOwner);

		if ($resSelOwner && $resSelOwner->RecordCount()==1) {
			return true;
		}
	} else {
		return false;
	}
	
	return false;
}

function getUserCanModifyInstallation($userID,$installationID,$dbADO) {
	$query = "SELECT userCanModifyInstallation from Installation_Users where FK_Installation = ? and FK_Users = ?";
	$res=$dbADO->Execute($query,array($installationID,$userID));
	 if ($res) {
	 	$row = $res->FetchRow();
	 	if ($row['userCanModifyInstallation']==1) {
	 		return true;
	 	}
	 }
	 return false;
}

function getValidOrbitersArray($installationID,$dbADO) {
		$orbiterID = $GLOBALS['rootOrbiterID'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($orbiterID,$dbADO);
		$validDeviceCategoryOrbiters=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryOrbiters[]=$orbiterID; //add orbiter root id to array
		
		$getOrbitersFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryOrbiters).')';
		$resOrbitersFromMasterDeviceList = $dbADO->_Execute($getOrbitersFromMasterDeviceList);
		$arrayOrbitersFromMasterDeviceList = array();
		 if ($resOrbitersFromMasterDeviceList) {
		 	while ($rowOrbitersFromMasterDeviceList = $resOrbitersFromMasterDeviceList->FetchRow()) {
		 		$arrayOrbitersFromMasterDeviceList[]=$rowOrbitersFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayOrbitersFromMasterDeviceList=cleanArray($arrayOrbitersFromMasterDeviceList);
		$queryOrbitersFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayOrbitersFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resOrbitersFromDevices = $dbADO->_Execute($queryOrbitersFromDevices);
		
		$orbitersFromDevicesArray = array();
		if ($resOrbitersFromDevices) {
			while ($rowOrbitersFromDevices = $resOrbitersFromDevices->FetchRow()) {
				$orbitersFromDevicesArray[] = $rowOrbitersFromDevices['PK_Device'];
			}
		}
		
		return $orbitersFromDevicesArray;
}

function getValidComputersArray($installationID,$dbADO) {
		$computerID = $GLOBALS['rootComputerID'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($computerID,$dbADO);
		$validDeviceCategoryComputers=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryComputers[]=$computerID; //add Computer root id to array
		
		$getComputersFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryComputers).')';
		$resComputersFromMasterDeviceList = $dbADO->_Execute($getComputersFromMasterDeviceList);
		$arrayComputersFromMasterDeviceList = array();
		 if ($resComputersFromMasterDeviceList) {
		 	while ($rowComputersFromMasterDeviceList = $resComputersFromMasterDeviceList->FetchRow()) {
		 		$arrayComputersFromMasterDeviceList[]=$rowComputersFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayComputersFromMasterDeviceList=cleanArray($arrayComputersFromMasterDeviceList);
		$queryComputersFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayComputersFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resComputersFromDevices = $dbADO->_Execute($queryComputersFromDevices);
		
		$ComputersFromDevicesArray = array();
		if ($resComputersFromDevices) {
			while ($rowComputersFromDevices = $resComputersFromDevices->FetchRow()) {
				$ComputersFromDevicesArray[] = $rowComputersFromDevices['PK_Device'];
			}
		}
		
		return $ComputersFromDevicesArray;
}

function getValidLightingObjectsArray($installationID,$dbADO) {
		$lightingID = $GLOBALS['rootLights'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($lightingID,$dbADO);
		$validDeviceCategoryLighting=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryLighting[]=$lightingID; //add lighting root id to array
		
		$getLightingsFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryLighting).')';
		$resLightingsFromMasterDeviceList = $dbADO->_Execute($getLightingsFromMasterDeviceList);
		$arrayLightingsFromMasterDeviceList = array();
		 if ($resLightingsFromMasterDeviceList) {
		 	while ($rowLightingsFromMasterDeviceList = $resLightingsFromMasterDeviceList->FetchRow()) {
		 		$arrayLightingsFromMasterDeviceList[]=$rowLightingsFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayLightingsFromMasterDeviceList=cleanArray($arrayLightingsFromMasterDeviceList);
		$queryLightingsFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayLightingsFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resLightingsFromDevices = $dbADO->_Execute($queryLightingsFromDevices);
		
		$lightingsFromDevicesArray = array();
		if ($resLightingsFromDevices) {
			while ($rowLightingsFromDevices = $resLightingsFromDevices->FetchRow()) {
				$lightingsFromDevicesArray[] = $rowLightingsFromDevices['PK_Device'];
			}
		}
		
		return $lightingsFromDevicesArray;
}

function getValidSecurityObjectsArray($installationID,$dbADO) {
		$securityID = $GLOBALS['rootSecurity'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($securityID,$dbADO);
		$validDeviceCategorySecurity=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategorySecurity[]=$securityID; //add security root id to array
		
		$getSecuritysFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategorySecurity).')';
		$resSecuritysFromMasterDeviceList = $dbADO->_Execute($getSecuritysFromMasterDeviceList);
		$arraySecuritysFromMasterDeviceList = array();
		 if ($resSecuritysFromMasterDeviceList) {
		 	while ($rowSecuritysFromMasterDeviceList = $resSecuritysFromMasterDeviceList->FetchRow()) {
		 		$arraySecuritysFromMasterDeviceList[]=$rowSecuritysFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arraySecuritysFromMasterDeviceList=cleanArray($arraySecuritysFromMasterDeviceList);
		$querySecuritysFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arraySecuritysFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resSecuritysFromDevices = $dbADO->_Execute($querySecuritysFromDevices);
		
		$securitysFromDevicesArray = array();
		if ($resSecuritysFromDevices) {
			while ($rowSecuritysFromDevices = $resSecuritysFromDevices->FetchRow()) {
				$securitysFromDevicesArray[] = $rowSecuritysFromDevices['PK_Device'];
			}
		}
		
		return $securitysFromDevicesArray;
}

function getValidClimateObjectsArray($installationID,$dbADO) {
		$climateID = $GLOBALS['rootClimate'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($climateID,$dbADO);
		$validDeviceCategoryClimate=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryClimate[]=$climateID; //add climate root id to array
		
		$getClimatesFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryClimate).')';
		$resClimatesFromMasterDeviceList = $dbADO->_Execute($getClimatesFromMasterDeviceList);
		$arrayClimatesFromMasterDeviceList = array();
		 if ($resClimatesFromMasterDeviceList) {
		 	while ($rowClimatesFromMasterDeviceList = $resClimatesFromMasterDeviceList->FetchRow()) {
		 		$arrayClimatesFromMasterDeviceList[]=$rowClimatesFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayClimatesFromMasterDeviceList=cleanArray($arrayClimatesFromMasterDeviceList);
		$queryClimatesFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayClimatesFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resClimatesFromDevices = $dbADO->_Execute($queryClimatesFromDevices);
		
		$climatesFromDevicesArray = array();
		if ($resClimatesFromDevices) {
			while ($rowClimatesFromDevices = $resClimatesFromDevices->FetchRow()) {
				$climatesFromDevicesArray[] = $rowClimatesFromDevices['PK_Device'];
			}
		}
		
		return $climatesFromDevicesArray;
}


$GLOBALS['childsFromAnyCategory']=array();
function getChildsForCategory($tableName,$columnPK,$columnFK,$parentID,$dbADO) {
	
	$selectChildsFromCategories = "SELECT Description,$columnPK FROM $tableName WHERE $columnFK = {$parentID} ORDER BY Description ASC";
		$rs2 = $dbADO->_Execute($selectChildsFromCategories);
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$GLOBALS['childsFromAnyCategory'][] = $row2[$columnPK];
				$jsTree.= '
					auxS'.$row2[$columnPK].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].'", "javascript:manageCategories.categSelected.value='.$row2[$columnPK].';manageCategories.actionX.value=null;manageCategories.submit();"))
					auxS'.$row2[$columnPK].'.xID = '.$row2[$columnPK].';
				';
				$jsTree.=getChildsForCategory($tableName,$columnPK,$columnFK,$row2[$columnPK],$dbADO);
			}

	return $jsTree;
}

function calculatePlutoID($PK_PlutoId)
{
	$str = PRV_plutoid_base35_char($mod = $PK_PlutoId%35);
	while ($PK_PlutoId = ($PK_PlutoId-$mod)/35)
	{
		$str .= PRV_plutoid_base35_char($mod = $PK_PlutoId%35);
	}
	return strrev(str_pad($str,6,'0'));
}

function PRV_plutoid_base35_char($int)
{
	if ($int < 10) return $int;
	if ($int < 24) return chr($int+55); // A-N: 65-78
	return chr($int+56); // P-Z: 80-90
}

function calculatePIN()
{
	$pin='';
	for ($i=0; $i<3; $i++)
	$pin.= PRV_plutoid_base35_char(rand(0,34));
	return $pin;
}

function getTopMenu($website,$dbADO,$package=0) {
		$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website'";
		$resSelectMenu = $dbADO->Execute($selectMenu);
		$menuPages='';	
/*		$menuPages='
				AddMenu("1"  ,  "1"   ,  "Wizard"       ,  ""  ,  ""  , "");
			';
*/		
		while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
			
			$menuPages.='
				AddMenu("'.$rowSelectMenu['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectMenu['Description'].'"       ,  ""  ,  ""  , "'.$rowSelectMenu['pageURL'].'");
			';
			//get childs (ne-recursive method)
			$selectSubMenu1 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectSubMenu1 = $dbADO->Execute($selectSubMenu1);
			while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {
				//get level 2 childs (ne-recursive method)
					$selectSubMenu2 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";
					$resSelectSubMenu2 = $dbADO->Execute($selectSubMenu2);
					
				$menuPages.='
					AddMenu("'.$rowSelectSubMenu1['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectSubMenu1['Description'].($resSelectSubMenu2->RecordCount()>0?" &gt;&gt;":"").'"       ,  ""  ,  ""  , "'.$rowSelectSubMenu1['pageURL'].'");
				';
				
					
					while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
						$menuPages.='
							AddMenu("'.$rowSelectSubMenu2['PK_PageSetup'].'"  ,  "'.$rowSelectSubMenu1['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectSubMenu2['Description'].'"       ,  ""  ,  ""  , "'.$rowSelectSubMenu2['pageURL'].'");
						';
					}
					$resSelectSubMenu2->Close();		
			}
			$resSelectSubMenu1->Close();					
		}
		$resSelectMenu->Close();
return $menuPages;
}

function InheritDeviceData($masterDeviceID,$insertID,$dbADO)
{
	$getDeviceTemplateDeviceData='
		INSERT INTO Device_DeviceData
			(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT 
			PK_Device, DeviceTemplate_DeviceData.FK_DeviceData, DeviceTemplate_DeviceData.IK_DeviceData
		FROM Device
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceTemplate_DeviceData on DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceTemplate_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getDeviceTemplateDeviceData,$insertID);

	$getDeviceCategoryDeviceData='
	INSERT INTO Device_DeviceData
		(FK_Device, FK_DeviceData, IK_DeviceData)
	SELECT 
		PK_Device, DeviceCategory_DeviceData.FK_DeviceData, DeviceCategory_DeviceData.IK_DeviceData
	FROM Device
		INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		INNER JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceTemplate.FK_DeviceCategory
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
	WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getDeviceCategoryDeviceData,$insertID);

	$getParentCategoryDeviceData='
		INSERT INTO Device_DeviceData
			(FK_Device, FK_DeviceData, IK_DeviceData)
		SELECT 
			PK_Device, DeviceCategory_DeviceData.FK_DeviceData, DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
			LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getParentCategoryDeviceData,$insertID);
}


function createChildsForControledViaDeviceTemplate($masterDeviceID,$installationID,$insertID,$dbADO)
{
	// check if DeviceTemplate controll anything
	$queryDeviceTemplate_DeviceTemplate_ControlledVia='SELECT * FROM DeviceTemplate_DeviceTemplate_ControlledVia
				WHERE FK_DeviceTemplate_ControlledVia = ? AND AutoCreateChildren = 1';
	$resDeviceTemplate_DeviceTemplate_ControlledVia=$dbADO->Execute($queryDeviceTemplate_DeviceTemplate_ControlledVia,$masterDeviceID);
	if($resDeviceTemplate_DeviceTemplate_ControlledVia->RecordCount()>0){
		// insert the children
		while($row=$resDeviceTemplate_DeviceTemplate_ControlledVia->FetchRow()){
			$queryDeviceTemplate='SELECT Description FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
			$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,$row['FK_DeviceTemplate']);
			$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
			$queryInsertDevice = "INSERT INTO Device(FK_Installation, Description, FK_Device_ControlledVia, FK_DeviceTemplate)
										values(?,?,?,?)";
			$dbADO->Execute($queryInsertDevice,array($installationID,$rowDeviceTemplate['Description'],$insertID,$row['FK_DeviceTemplate']));
			$insertChildID = $dbADO->Insert_ID();
			InheritDeviceData($row['FK_DeviceTemplate'],$insertChildID,$dbADO);
			setDCERouterNeedConfigure($installationID,$dbADO);
		}
	}
}

function createChildsForControledViaDeviceCategory($masterDeviceID,$installationID,$insertID,$dbADO)
{
	// get Device Template Category
	$queryDeviceTemplate='SELECT FK_DeviceCategory FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,$masterDeviceID);
	$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
	$FK_DeviceCategory=$rowDeviceTemplate['FK_DeviceCategory'];

	// query DeviceTemplate_DeviceCategory_ControlledVia if his category controll anything
	$queryDeviceTemplate_DeviceCategory_ControlledVia='SELECT * FROM DeviceTemplate_DeviceCategory_ControlledVia
				WHERE FK_DeviceCategory = ? AND AutoCreateChildren = 1';
	$res=$dbADO->Execute($queryDeviceTemplate_DeviceCategory_ControlledVia,$FK_DeviceCategory);
	if($res->RecordCount()>0){
		
		while($row=$res->FetchRow()){
			$queryDeviceTemplate='SELECT Description FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
			$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,$row['FK_DeviceTemplate']);
			$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
			$queryInsertDevice = "INSERT INTO Device(FK_Installation, Description, FK_Device_ControlledVia, FK_DeviceTemplate)
										values(?,?,?,?)";
			$dbADO->Execute($queryInsertDevice,array($installationID,$rowDeviceTemplate['Description'],$insertID,$row['FK_DeviceTemplate']));
			$insertChildID = $dbADO->Insert_ID();
			InheritDeviceData($row['FK_DeviceTemplate'],$insertChildID,$dbADO);
			setDCERouterNeedConfigure($installationID,$dbADO);
		}
	}
}

$GLOBALS['DeviceIDControlledVia']=array();
$GLOBALS['DeviceControlledVia']=array();
function GetDeviceControlledVia($deviceID, $dbADO)
{
	$whereClause='';
	if ($deviceID!=0) {
		$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
		$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$_SESSION['installationID']));
		if ($resSelectMasterDeviceForParent) {
			$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
			$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];

			$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
			$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($DeviceTemplateForParent));

			$GLOBALS['childsDeviceCategoryArray'] = array();
			if ($resSelectControlledViaCategory) {
				while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
					$GLOBALS['childsDeviceCategoryArray'][]=$rowSelectControlledVia['FK_DeviceCategory'];
					getDeviceCategoryChildsArray($rowSelectControlledVia['FK_DeviceCategory'],$dbADO);
				}
			}
			$controlledViaCategoryArray = cleanArray($GLOBALS['childsDeviceCategoryArray']);

			$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
			$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

			$childsDeviceTemplateArray = array();
			if ($resSelectControlledViaDeviceTemplate) {
				while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
					$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];
				}
			}



			if (count($controlledViaCategoryArray)>0) {
				$whereClause.=' and (FK_DeviceCategory in ('.join(",",$controlledViaCategoryArray).')';
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
				$whereClause.=')';
			} else {
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
			}

		}
	}

	$queryDeviceTemplate = "SELECT DISTINCT Device.Description,Device.PK_Device
											FROM
											Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
											WHERE 1=1 $whereClause AND FK_Installation='".$_SESSION['installationID']."' order by Device.Description asc";
	$resDeviceTemplate = $dbADO->_Execute($queryDeviceTemplate);
	$DeviceTemplateTxt = '';
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$GLOBALS['DeviceIDControlledVia'][]=$row['PK_Device'];
			$GLOBALS['DeviceControlledVia'][]=$row['Description'];
		}
	}
}

function isMediaDirector($deviceID,$dbADO)
{
	$getTemplate='SELECT FK_DeviceTemplate,FK_Device_ControlledVia FROM Device WHERE PK_Device=?';
	$resTemplate=$dbADO->Execute($getTemplate,$deviceID);
	if($resTemplate->RecordCount()>0){
		$row=$resTemplate->FetchRow();
		$DeviceTemplate=$row['FK_DeviceTemplate'];
	}else
		return false;
	if($DeviceTemplate==$GLOBALS['rootMediaDirectorsID']){
		return true;
	}
	elseif($row['FK_Device_ControlledVia']=='' )
		return false;
	else{
		return isMediaDirector($row['FK_Device_ControlledVia'],$dbADO);
	}
}

function isCore($deviceID,$dbADO)
{
	$getTemplate='SELECT FK_DeviceTemplate,FK_Device_ControlledVia FROM Device WHERE PK_Device=?';
	$resTemplate=$dbADO->Execute($getTemplate,$deviceID);
	if($resTemplate->RecordCount()>0){
		$row=$resTemplate->FetchRow();
		$DeviceTemplate=$row['FK_DeviceTemplate'];
	}else
		return false;
	if($DeviceTemplate==$GLOBALS['rootCoreID']){
		return true;
	}
	return false;
}

function isOrbiter($deviceID,$dbADO)
{
	$orbitersArray=array();
	$orbitersArray=getValidOrbitersArray($_SESSION['installationID'],$dbADO);
	if(in_array($deviceID,$orbitersArray))
		return true;
	return false;
}


function deleteCommandGroup($PK_CommandGroup,$dbADO)
{
	$deleteParameters='
		DELETE CommandGroup_Command_CommandParameter
		FROM CommandGroup_Command_CommandParameter
		JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteParameters,$PK_CommandGroup);
	
	$deleteCommandGroup_Command='
		DELETE FROM CommandGroup_Command
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_Command,$PK_CommandGroup);

	$deleteCommandGroup_EntArea='
		DELETE FROM CommandGroup_EntertainArea
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_EntArea,$PK_CommandGroup);

	$deleteCommandGroup_Room='
		DELETE FROM CommandGroup_Room
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_Room,$PK_CommandGroup);
	
	$deleteCommandGroup='
		DELETE FROM CommandGroup 
		WHERE PK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup,$PK_CommandGroup);
}

function grabDirectory ($path, $depth) {
	if (($d = @opendir ($path)) === false) 
		return;
	else {
		while ($f = readdir ($d)) {
			if ($f != "." && $f != "..") {
				if (@is_dir ($path . "/" . $f)) {
					$GLOBALS['physicalPathsArray'][]=$path . "/" . $f;
					$depth++;
					grabDirectory ($path . "/" . $f, $depth);
					$depth--;
				} 
			}
		}
		closedir ($d);
	}
}


function grabFiles($path,$fileParm='-type f') {
	$filesArray=array();
	// required to read files larger than 2G
	exec('find "'.$path.'" '.$fileParm.' -maxdepth 1',$retArray);
	foreach ($retArray AS $file){
		if($file!=$path)
			$filesArray[]=str_replace($path.'/','',$file);
	}
	return $filesArray;
}

function resizeImage($source, $destination, $new_width, $new_height,$forcedPNG=0)
{
	if(!file_exists($source))
		return 1;	// Source file does not exists

	$arr=getimagesize($source);
	$old_w=$arr[0];
	$old_h=$arr[1];
	if($new_height<=0 || $new_width<=0)
		return 2;	// invalid width or height
	$aspect_ratio=$new_width/$new_height;
	if($old_w/$old_h==$aspect_ratio)
	{
		$src_x=0;
		$src_y=0;
		$src_w=floor($old_w*$aspect_ratio);
		$src_h=floor($old_h*$aspect_ratio);
	}
	elseif($old_w/$old_h>$aspect_ratio)
	{
		$src_x=floor(($old_w-$old_h*$aspect_ratio)/2);
		$src_y=0;
		$src_w=floor($old_h*$new_width/$new_height);
		$src_h=floor($old_h);
	}
	else
	{
		$src_x=0;
		$src_y=floor(($old_h-$old_w/$aspect_ratio)/2);
		$src_w=floor($old_w);
		$src_h=floor($old_w*$new_height/$new_width);
	}
	$new_w=$new_width;
	$new_h=$new_height;	 

	$tmpArray=@getimagesize($source);
	$destinationType=substr($destination,strrpos($destination,'.')+1);
	
	$dst_img=imagecreatetruecolor($new_w,$new_h);
	$src_img=(@$tmpArray[2]==2)?ImageCreateFromJpeg($source):imagecreatefrompng($source);
	if(!@imagecopyresized($dst_img,$src_img,0,0,$src_x,$src_y,$new_w,$new_h,$src_w,$src_h))
		return 3; // resize error
	if($forcedPNG==1 && !@imagepng($dst_img, str_replace('.jpg','.png',$destination), 100)){
		return 5; // writing png thumbnail error
	}else{
		if(($destinationType=='jpg') && !@imagejpeg($dst_img, $destination, 100))
			return 4; // writing jpg thumbnail error
		elseif(@$destinationType=='png' && !@imagepng($dst_img, $destination, 100))
			return 5;	// writing png thumbnail error
	}

	
	return 0;
}

function multi_page($query, $params,$url, $page_no, $art_pagina,$dbADO)
{
	$res=$dbADO->Execute($query,$params);
	$total=$res->RecordCount();
	$max_pages = $total/$art_pagina;
	if( $max_pages - floor($max_pages) >= 0 )
		$max_pages = floor($max_pages) + 1;
	else
		$max_pages = floor( $max_pages );

	if($total!=0){
		$output= '<div align="center">';
		for($i=0;$i<$max_pages;$i++){
			$output.=($page_no==$i)?'<font color="red">'.($i+1).'</font> ':'<a href='.$url.'&page_no='.($i+1).'>[ '.($i+1).' ]</a> ';
		}
		$output.='</div>';
	}
	if($total==0){
		$output.='No records';
	}
	else{
		$output.='<table>
					<tr bgcolor="lightblue">
						<td align="center"><B>Files</B></td>
						<td align="center">&nbsp;</td>
					</tr>';
		$art_index=0;
		$start = $page_no * $art_pagina;
		$res=$dbADO->Execute($query." LIMIT  $start,$art_pagina",$params);
	    while($row = $res->FetchRow()){
			$art_index++;
			if($art_index-1 == $art_pagina)
		        break;
			$output.=multi_page_format($row, $art_index+$start,$dbADO);
		}
	}
	$output.='Found: '.$total.'. ';
	if($total!=0)
		$output.= 'Page '.($page_no+1).' from '.$max_pages.'.<br>';
	return $output;
}
function multi_page_format($row, $art_index,$mediadbADO)
{
	$queryOtherAttributes='
		SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name,FirstName
		FROM File_Attribute
			INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
			INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
		WHERE FK_File=? AND FK_AttributeType!=? ORDER BY AttributeType.Description ASC
		';

	$otherAttributes='';
	$resOtherAttributes=$mediadbADO->Execute($queryOtherAttributes,array($row['FK_File'],$GLOBALS['attrType']));
	while($rowOtherAttributes=$resOtherAttributes->FetchRow()){
		$otherAttributes.='<b>'.$rowOtherAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowOtherAttributes['PK_Attribute'].'&action=properties" target="_self">'.$rowOtherAttributes['Name'].(($rowOtherAttributes['FirstName']!='')?', '.$rowOtherAttributes['FirstName']:'').'</a> ';
	}

	$out='
		<tr style="background-color:'.(($art_index%2==0)?'#EEEEEE':'#EBEFF9').';">
			<td title="'.$row['Path'].'" align="left">'.(($row['Missing']!=0)?'<img src="include/images/missing.gif" align="top"> ':'').'<b>Filename:</b> <a href="index.php?section=editMediaFile&fileID='.$row['PK_File'].'">'.$row['Filename'].'</a><br><B>Path:</B> <a href="index.php?section=mainMediaFilesSync&path='.$row['Path'].'&filename='.urlencode($row['Filename']).'">'.$row['Path'].'</a></td>
			<td rowspan="2"><a href="#" onClick="if(confirm(\'Are you sure you want to delete the media file? The physical file will be deleted too if exist!\'))self.location=\'index.php?section=mainMediaBrowser&attributeID='.$GLOBALS['attributeID'].'&action=properties&fileID='.$row['PK_File'].'\';">Delete</a></td>
		</tr>
		<tr style="background-color:'.(($art_index%2==0)?'#EEEEEE':'#EBEFF9').';">
			<td align="left">'.$otherAttributes.'</td>
		</tr>
		';
	return $out;
}

function getDevicesArrayFromCategory($categoryID,$dbADO)
{
	$GLOBALS['childsDeviceCategoryArray']=array();
	getDeviceCategoryChildsArray($categoryID,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;

	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$DTArray=array();
	$DTIDArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$DTArray[]=$rowDeviceCategory['Description'];
		$DTIDArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}

	$devicesList=array();
	$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
	$joinArray[]=0;
	$queryDevice='
		SELECT 
			Device.* FROM Device 
		WHERE
			FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
	$resDevice=$dbADO->Execute($queryDevice,(int)$_SESSION['installationID']);
	while($rowD=$resDevice->FetchRow()){
		$devicesList[$rowD['PK_Device']]=$rowD['Description'];
	}
	unset($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray']=array();
	return $devicesList;
}

function deleteDevice($PK_Device,$dbADO)
{
	getDeviceChildsArray($PK_Device,$dbADO);
	
	$toDelete = cleanArray($GLOBALS['childsArray']);
	$toDelete[]=$PK_Device;
	if (!is_array($toDelete)) {
		$toDelete=array();
	}
	foreach ($toDelete as $elem) {
	
		$arrayFKDeviceTables=array('CommandGroup_Command','Device_Command','Device_CommandGroup','Device_DeviceData','Device_DeviceGroup','Device_Device_Related','Device_EntertainArea','Device_HouseMode','Device_Orbiter','Device_StartupScript','Device_Users');
		foreach($arrayFKDeviceTables AS $tablename){	
			$queryDelFromTable='DELETE FROM '.$tablename.' WHERE FK_Device='.$elem;
			$dbADO->Execute($queryDelFromTable);
		}		
		$queryDelDevice = 'DELETE FROM Device WHERE PK_Device = '.$elem;
		$dbADO->_Execute($queryDelDevice);
	}

}

function addDeviceToEntertainArea($deviceID,$entArea,$dbADO)
{
	$queryDE='SELECT * FROM Device_EntertainArea WHERE FK_Device=? AND FK_EntertainArea=?';
	$resDE=$dbADO->Execute($queryDE,array($deviceID,$entArea));
	if($resDE->RecordCount()==0){
		$insertDeviceEntertainArea='INSERT INTO Device_EntertainArea (FK_Device, FK_EntertainArea) VALUES (?,?)';
		$dbADO->Execute($insertDeviceEntertainArea,array($deviceID,$entArea));
	}
					
	if(isOrbiter($deviceID,$dbADO) || isMediaDirector($deviceID,$dbADO)){
		$queryChilds='SELECT * FROM Device WHERE FK_Device_ControlledVia=? AND FK_Installation=?';
		$resChilds=$dbADO->Execute($queryChilds,array($deviceID,$_SESSION['installationID']));
		if($resChilds->RecordCount()>0){
			while($rowChilds=$resChilds->FetchRow()){
				addDeviceToEntertainArea($rowChilds['PK_Device'],$entArea,$dbADO);
			}
		}
	}
}

function getChilds($parentID,$dbADO) {
	
	$selectDevicesFromCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent = {$parentID} ORDER BY Description";
		$rs2 = $dbADO->_Execute($selectDevicesFromCategories);
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$jsTree.= '
					auxS'.$row2['PK_DeviceCategory'].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].' #'.$row2['PK_DeviceCategory'].'", "javascript:document.forms[0].deviceCategSelected.value='.$parentID.';document.forms[0].deviceSelected.value='.$row2['PK_DeviceCategory'].';document.forms[0].actionX.value=null;document.forms[0].submit();"))
					auxS'.$row2['PK_DeviceCategory'].'.xID = '.$row2['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row2['PK_DeviceCategory'],$dbADO);
			}

	return $jsTree;
}

function pickDeviceTemplate($categoryID, $boolManufacturer,$boolCategory,$boolDeviceTemplate,$returnValue,$defaultAll,$section,$firstColText,$dbADO,$useframes=0,$genericSerialDevicesOnly=0)
{
	global $dbPlutoMainDatabase;
	$out='
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>';
	
	$selectedManufacturer = (int)@$_REQUEST['manufacturers'];
	$selectedDeviceCateg= (int)@$_REQUEST['deviceCategSelected'];
	$selectedDevice= (int)@$_REQUEST['deviceSelected'];
	$selectedModel= (int)@$_REQUEST['model'];
	$justAddedNode = (int)@$_GET['justAddedNode'];

	$actionX = (isset($_REQUEST['actionX']) && cleanString($_REQUEST['actionX'])!='null')?cleanString($_REQUEST['actionX']):'form';
	
	$selectManufacturersTxt = '';
	$queryManufacturers = "SELECT * FROM Manufacturer ORDER BY Description";
	$rs = $dbADO->_Execute($queryManufacturers);
		while ($row = $rs->FetchRow()) {
			$selectManufacturersTxt.="<option ".($selectedManufacturer==$row['PK_Manufacturer']?" selected ":"")." value='{$row['PK_Manufacturer']}'>{$row['Description']}</option>";
		}
	$rs->Close();
	
	if ($actionX=='form') {
	
	
		$jsTree = '';
		if(is_null($categoryID))
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent IS NULL ORDER BY Description";
		else 
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent ='$categoryID' ORDER BY Description";
		$rs = $dbADO->_Execute($selectDeviceCategories);
			while ($row = $rs->FetchRow()) {		
				$jsTree.='
				auxS'.$row['PK_DeviceCategory'].' = insFld(foldersTree, gFld("'.$row['Description'].' #'.$row['PK_DeviceCategory'].'", "javascript:document.forms[0].deviceCategSelected.value=0;document.forms[0].deviceSelected.value='.$row['PK_DeviceCategory'].';document.forms[0].actionX.value=null;document.forms[0].submit();"));
				auxS'.$row['PK_DeviceCategory'].'.xID = '.$row['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row['PK_DeviceCategory'],$dbADO);
			}
		$rs->Close();
		
		
		$selectModels = '';
		$modelsJsArray = '
				modelsArray = new Array();
		';
		$where = " AND 1=1 ";
		$manufOrDevice = 0;
		if ($selectedManufacturer!=0) {	
			$where.=" AND FK_Manufacturer = '$selectedManufacturer'";
			$manufOrDevice = 1;
		}

		if ($selectedDevice!=0) {
			if ($selectedDeviceCateg==0) {
				$GLOBALS['childsDeviceCategoryArray'] = array();
				
				getDeviceCategoryChildsArray($selectedDevice,$dbADO);
				$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
				$GLOBALS['childsDeviceCategoryArray'][]=$selectedDevice;
				
				$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
				
				$where.=" and DeviceCategory.PK_DeviceCategory in ($childArray)";
			} else {		
				$where .= " and DeviceCategory.PK_DeviceCategory = '$selectedDevice'";	
			}
			$manufOrDevice = 1;
		}elseif(!is_null($categoryID)){
			$GLOBALS['childsDeviceCategoryArray'] = array();
				
			getDeviceCategoryChildsArray($categoryID,$dbADO);
			$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
			$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;
			
			$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
			$where.=" and DeviceCategory.PK_DeviceCategory in ($childArray)";
		}

		$where.=(($genericSerialDevicesOnly==1)?' AND CommandLine=\''.$GLOBALS['GenericSerialDeviceCommandLine'].'\'':'');
		
		if ($manufOrDevice || $defaultAll==1)	{
		
			$queryModels = "SELECT 
								DeviceTemplate.*,
								DeviceCategory.Description as deviceDescription,
								Manufacturer.Description as manufacturerDescription 
							FROM DeviceTemplate 
							INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
							INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer
							WHERE 1=1 $where ORDER BY Description";	
			$rs = $dbADO->_Execute($queryModels);
			
			$arJsPos=0;

			// if call come from addMyDevice, aply controlled by filter
			$allowedDevices=array();
			if(ereg('parentID',@$_SESSION['from']) && @$_SESSION['parentID']!=0){
				$allowedDevices=getDeviceTemplatesControlledBy($_SESSION['parentID'],$dbADO);
			}

			while ($row = $rs->FetchRow()) {
			
				if(count($allowedDevices)==0 || (count($allowedDevices)>0 && in_array($row['PK_DeviceTemplate'],$allowedDevices)))			
					$selectModels.="<option ".($selectedModel==$row['PK_DeviceTemplate']?" selected ":"")." value='{$row['PK_DeviceTemplate']}' ".(($row['IsPlugAndPlay']==1)?'style="background-color:lightgreen;"':'').">{$row['Description']}</option>";
					
					if ($row['PK_DeviceTemplate']>0) {
					$modelsJsArray.='
					
					model'.$row['PK_DeviceTemplate'].' = new Array();
						model'.$row['PK_DeviceTemplate'].'.manufacturerName = \''.$row['manufacturerDescription'].'\';
						model'.$row['PK_DeviceTemplate'].'.deviceDescription = \''.$row['deviceDescription'].'\';
					    model'.$row['PK_DeviceTemplate'].'.userID = \''.@$_SESSION['userID'].'\';
					
					modelsArray['.$row['PK_DeviceTemplate'].'] = model'.$row['PK_DeviceTemplate'].';			
					
					';
					}
					$arJsPos++;
				}
			$rs->Close();
		}
		
			
		
		$scriptInHead = "
		
	
		<!-- As in a client-side built tree, all the tree infrastructure is put in place
		     within the HEAD block, but the actual tree rendering is trigered within the
		     BODY -->
		
		<!-- Code for browser detection -->
		<script src=\"scripts/treeview/ua.js\"></script>
		
		<!-- Infrastructure code for the tree -->
		<script src=\"scripts/treeview/ftiens4.js\"></script>
		
		<!-- Execution of the code that actually builds the specific tree.
		     The variable foldersTree creates its structure with calls to
			 gFld, insFld, and insDoc -->
		<script>
		
		USETEXTLINKS = 1
		STARTALLOPEN = 0
		USEFRAMES = ".$useframes."
		USEICONS = 0
		WRAPTEXT = 1
		PRESERVESTATE = 1
		ICONPATH = 'scripts/treeview/' 
		HIGHLIGHT = 1
		
		";
		if(is_null($categoryID)){
			$scriptInHead.="
				foldersTree = gFld('<b>Device Category</b>', \"javascript:document.forms[0].deviceSelected.value=0;document.forms[0].deviceCategSelected.value=0;document.forms[0].actionX.value=null;document.forms[0].submit();\");";
		}else{
			$queryCategoryName='SELECT Description FROM DeviceCategory WHERE PK_DeviceCategory=?';
			$resCategoryName=$dbADO->Execute($queryCategoryName,$categoryID);
			$rowCategoryName=$resCategoryName->FetchRow();
			$scriptInHead.="
				foldersTree = gFld('<b>".$rowCategoryName['Description']."</b>', \"javascript:document.forms[0].deviceSelected.value=0;document.forms[0].deviceCategSelected.value=".$categoryID.";document.forms[0].actionX.value=null;document.forms[0].submit();\");";
		}
		$scriptInHead.="
		foldersTree.xID = 1001635872;
		$jsTree
		
		foldersTree.treeID = 't2'
		
		function getObj(obj) {
				if (document.layers) {
					if (typeof obj == 'string') {
						return document.layers[obj];
					} else 	{
						return obj;
					}
				}
				if (document.all) {
					if (typeof obj == 'string') {
						return document.all(obj);
					} else {
						return obj;
					}
				}
				if (getObj) {
					if (typeof obj == 'string') {
						return getObj(obj);
					} else {
						return obj;
					}
				}
				return null;	
			} 

			function showHideObject(obj) {
				obj = getObj(obj);					
				if(document.all || getObj) {  			
						if (obj.style.visibility == \"visible\") {
							obj.style.visibility = \"hidden\";
							obj.style.display = \"none\";
						} else {
						    obj.style.visibility = \"visible\";
							obj.style.display = \"block\";
						}
				} else if (document.layers) {
						if (obj.style.visibility == \"visible\") {
							obj.visibility = \"hide\";	
						} else {
						obj.visibility = \"show\";	
						}
				}
			} 
		$modelsJsArray
		
		
		</script>
		";
		$out.=$scriptInHead;
		$out.='
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>	
		<form action="index.php" method="POST" name="'.$section.'">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="actionX" value="add">
		<input type="hidden" name="deviceCategSelected" value="'.$selectedDeviceCateg.'">
		<input type="hidden" name="deviceSelected" value="'.$selectedDevice.'">
		<input type="hidden" name="modelSelected" value="'.$selectedModel.'">
		<input type="hidden" name="allowAdd" value="'.(int)@$_REQUEST['allowAdd'].'">
		<input type="hidden" name="parmToKeep" value="'.@$_REQUEST['parmToKeep'].'">
		
		<table cellpadding="5" cellspacing="0" border="0" align="center">
				<input type="hidden" name="selectedField" value="" />
					<tr>
						<th width="25%">Manufacturers</th>
						<th width="25%">Device Category</th>
						<th width="25%">
						Models ';
						if(ereg('parentID',@$_SESSION['from']) && $_SESSION['parentID']!=0){
							$parentFields=getProperties($_SESSION['parentID'],'Device','FK_DeviceTemplate','PK_Device',$dbADO);
							$dtFields=getProperties($parentFields['FK_DeviceTemplate'],'DeviceTemplate','Description','PK_DeviceTemplate',$dbADO);
							$out.=' controlled by '.$dtFields['Description'];
						}
						$out.='
						</th>				
					</tr>
					<tr valign="top">
						<td width="25%" align="center"  valign="top">
							<select name="manufacturers" id="select_Manufacturer" onchange="this.form.selectedField.value=\'manufacturers\';this.form.actionX.value=\'form\';this.form.submit();" size="10" style="z-index:1;">
								<option value="" selected="selected">All</option>
								'.$selectManufacturersTxt.'						
							</select><br /><br />';
					if($boolManufacturer==1){
						$out.='
							<input type="text" name="Manufacturer_Description" size="15" />
							<input type="submit" class="button" name="addManufacturer" value="Add"  />';
					}
					$out.=$firstColText.'
						</td>
						<td width="25%" align="center" valign="top">
							<span style="display:none;"><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></span>
							<span>
							<script>
								initializeDocument();						
							</script>
							<noscript>
							A tree for site navigation will open here if you enable JavaScript in your browser.
							</noscript>
							</span>
							<br /><br />';
						if($boolCategory==1){
							$out.='
							<input type="text" name="DeviceCategory_Description" size="15" />
							<input type="submit" class="button" name="addDeviceCategory"   value="Add '.($selectedDevice==0?' Top Level Child':' Child').'" />
							'.($selectedDevice!=0?'<input type="button" class="button" name="editDeviceCategory" value="Edit" onClick="javascript: windowOpen(\'index.php?section=editDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=600,height=250,toolbars=true\');" />':'<input   type="submit" class="button" name="editDeviceCategory" value="Edit" disabled="disabled" />').'';
							getDeviceCategoryChildsNo($selectedDevice,$dbADO);					
							$childsToDelete = $GLOBALS['childsDeviceCategoryNo'];
							$out.='
							&nbsp;&nbsp;'.($selectedDevice!=0?'<input type="button" class="button" name="deteleDeviceCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) windowOpen(\'index.php?section=deleteDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=100,height=100,toolbars=0\');" />':'<input type="submit" class="button" name="deleteDeviceCategory"   value="Delete" disabled="disabled" />');
						}
						$out.='
						</td>
						<td width="25%" align="center"  valign="top">
							<script>
								function checkEdit(objF) {
										if(document.'.$section.'.model.selectedIndex!=0){							
											objF.section.value=\'editMasterDevice\';
											objF.submit();
										}
								}
		
								function updateModelDetail(objF) {
									if (typeof modelsArray[objF.model[objF.model.selectedIndex].value] != "undefined") {
										getObj(\'modelManuf\').innerHTML = \'<br />\'+modelsArray[objF.model[objF.model.selectedIndex].value].manufacturerName;
										getObj(\'modelDeviceDescription\').innerHTML = \'<br />\'+modelsArray[objF.model[objF.model.selectedIndex].value].deviceDescription;
									} else {
										getObj(\'modelManuf\').innerHTML = \'\';
										getObj(\'modelDeviceDescription\').innerHTML = \'\';
									} 					
								}
								
							</script>
							<select name="model" id="model" 
									onChange="updateModelDetail(this.form);"
									 size="10">
									<option value="" selected="selected">'.(($section=='deviceTemplatePicker')?'- Please select -':'All').'</option>
									'.$selectModels.'	
							</select>';
							if($returnValue==0){
								$out.='<input type="button" class="button" name="edit_DeviceTemplate" value="Edit" onClick="javascript:checkEdit(this.form);" />';
							}else{
								$out.='<br><input type="button" class="button" name="pickDT" value="Add device" onClick="opener.location=\'index.php?section='.$_SESSION['from'].'&deviceTemplate=\'+document.'.$section.'.model[document.'.$section.'.model.selectedIndex].value+\'&action=add&add=1&'.@$_REQUEST['parmToKeep'].'\';self.close();" />';
							}
							$out.='
							<hr />
						<b><span id="modelManuf"></span><span id="modelDeviceDescription"></span></b><br />
							
							';
							
							if ($selectedManufacturer!=0 && $selectedDevice!=0) {
								$disabledAdd=' ';
							} else {
								$disabledAdd=' disabled="disabled" ';
							}
					
							$disabled_str = "disabled";
				
				$out.='	<b>Device templates highlighted are plug and play.';
			if($boolDeviceTemplate==1){
					$out.='
						<br><br>If your model is not listed, pick the manufacturer and the category and then type the model here: <input type="text" name="DeviceTemplate_Description" size="15"'.$disabledAdd.' />
							<input type="submit" class="button" name="addDeviceTemplate" value="Add"'. $disabledAdd .'  />';
				}
					
				
				$out.='
						</td>				
					</tr>';
				if($returnValue!=0){
					$out.='
					<tr>
						<td colspan="3">To add a device, choose the manufacturer and the category to see all models for you selection. Pick the model you want and click <B>"Add device"</B>.</td>
					</tr>';
				}
				if($categoryID==$GLOBALS['rootAVEquipment']){
					$out.='
					<tr>
						<td colspan="3">After you add the device you\'ll to choose the A/V properties button and then I/R codes.</td>
					</tr>';
				}
				$out.='
				</table>
							
		</form>
		';
	} else {
		
		 $addDeviceCategory = @cleanString($_POST['addDeviceCategory']);
		 $deviceCategoryDesc = cleanString(@$_POST['DeviceCategory_Description']);
		 
		 $addDeviceTemplate = @cleanString($_POST['addDeviceTemplate']);
		 $DeviceTemplate_Desc = @cleanString($_POST['DeviceTemplate_Description']);
	
		 $addManufacturer = @cleanString($_POST['addManufacturer']);
		 $Manufacturer_Description = @cleanString($_POST['Manufacturer_Description']);	 
		
		 $deviceCategSelected = (int)$_POST['deviceCategSelected'];
		 $deviceSelected = (int)$_POST['deviceSelected'];
		 $manufacturerSelected = (int)$_POST['manufacturers'];
	

		 // process form only if user is logged
		 if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
	
			 // add new category
			 $justAddedNode = 0;
			 if (strstr($addDeviceCategory,'Add')) {
			 	if ($deviceCategoryDesc!='') {
			 		$queryInsertCategoryDesc = "insert into DeviceCategory(FK_DeviceCategory_Parent,Description,IsAV)
			 				values(?,?,0)";
			 		$deviceCategParent=($deviceSelected==0)?$categoryID:$deviceSelected;
			 		$dbADO->Execute($queryInsertCategoryDesc,array($deviceCategParent,$deviceCategoryDesc));	 			
			 		$justAddedNode = $deviceSelected;
			 		header("Location: index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");	 			 		
				}
			 }	
			 	 
			 //add new master device list
			 if (strstr($addDeviceTemplate,'Add')) {
			 	
			 	if ($DeviceTemplate_Desc!='') {
			 		if ($deviceSelected!=0 && $manufacturerSelected!=0) {	 			
			 			$queryInsertMasterDevice = 'INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer) values(?,?,?)';
			 			$res = $dbADO->Execute($queryInsertMasterDevice,array($DeviceTemplate_Desc,$deviceSelected,$manufacturerSelected));	 			
			 			$dtID=$dbADO->Insert_ID();

			 			if(in_array($deviceSelected,$GLOBALS['TVdevicesArray'])){
							$openTunerConfig='windowOpen(\'index.php?section=tunerConfig&categoryID='.$deviceSelected.'&dtID='.$dtID.'\',\'width=600,height=400,toolbars=true,scrollbars=1,resizable=1\')';
						}else
							$openTunerConfig='';
			 			
						/*
			 			if($categoryID==$GLOBALS['rootAVEquipment']){
			 				$templateID=$dbADO->Insert_ID();
							$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$templateID.' -i '.$_SESSION['installationID']);	
							setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
							$out.='
								<script>
									'.$openTunerConfig.'
									opener.location="index.php?section=avWizard&type=avEquipment#deviceLink_'.$insertID.'";
									self.close();
								</script>';
			 			}else{
			 			}
			 			*/
		 				$out.='
							<script>
								'.$openTunerConfig.'
								self.location="index.php?section='.$section.'&manufacturers='.$manufacturerSelected.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'&allowAdd='.$boolDeviceTemplate.'&justAddedNode='.$justAddedNode.'";
							</script>';
		 			
			 		}	 			 		
			 	}
			 }
			 
			 // add new manufacturer
			 if (strstr($addManufacturer,'Add')){
			 	if ($Manufacturer_Description!='') {
		 			$queryInsertManufacturer = 'INSERT INTO Manufacturer (Description) values(?)';
		 			$res = $dbADO->Execute($queryInsertManufacturer,array($Manufacturer_Description));	 			
		 			header("Location: index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");
		 			exit();
		 		}	 			 		
			 }
			$out.="
				<script>
					self.location='index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=".$justAddedNode.'&'.$_REQUEST['parmToKeep']."';
				</script>";
		}else{
			$out.="
				<script>
					self.location='index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&error=Please login if you want to change device templates.';
				</script>";
		}
	}
	return $out;
}

function builtTopMenu($website,$dbADO)
{
	$selectMenu = "
		SELECT DISTINCT PageSetup.* FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website' AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND FK_Installation=?))
		ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu,$_SESSION['installationID']);
	$menuPages='';
	$pos=0;	
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='menuObject.item'.$pos.' = "'.$rowSelectMenu['Description'].'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$pos.' = "'.str_replace('\'','\\\\\'',$rowSelectMenu['pageURL']).'"
		';
		$menuPages.=getSubmenu($website,$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
	}
	return $menuPages;
}

function getSubmenu($website,$level,$parentID,$dbADO)
{
	$selectMenu = "
		SELECT PageSetup.* 
		FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_PageSetup_Parent =? AND showInTopMenu = 1 AND Website=? AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND FK_Installation=?))
		ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($parentID,$website,$_SESSION['installationID']));
	$menuPages='';
	$pos=0;	
	if($resSelectMenu->RecordCount()>0)
		$menuPages.='menuObject.menu_items_background_color_roll'.$level.' = "#FFFFFF"
		';
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='
			menuObject.item'.$level.$pos.' = "'.$rowSelectMenu['Description'].'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$level.$pos.' = "'.str_replace('\'','\\\\\'',$rowSelectMenu['pageURL']).'"
		';
		$menuPages.=getSubmenu($website,$level.$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
		
	}
	return $menuPages;
}

function generatePullDown($name,$tableName,$valueField,$labelField,$selectedValue,$dbADO,$filterTable='',$jsEvents='')
{
	$pullDown='
		<select name="'.$name.'" '.$jsEvents.'>	
			<option value="0">- Please select -</option>';
	$query="SELECT $tableName.$valueField,$tableName.$labelField FROM $tableName $filterTable ORDER BY $labelField ASC";
	$res=$dbADO->Execute($query);
	while($row=$res->FetchRow()){
		$pullDown.='<option value="'.$row[$valueField].'" '.(($row[$valueField]==$selectedValue)?'selected':'').'>'.$row[$labelField].'</option>';
	}
	$pullDown.='</select>';
	return $pullDown;
}

function getDeviceTemplatesFromCategory($categoryID,$dbADO)
{
	$GLOBALS['childsDeviceCategoryArray']=array();
	getDeviceCategoryChildsArray($categoryID,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;
	
	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$DTArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$DTArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}
	return $DTArray;
}

// convert a Y-m-d date in a custom format
function formatMySQLDate($mySQLformat,$customFormat)
{
	$dataParts=explode('-',$mySQLformat);
	return date($customFormat,mktime(0,0,0,$dataParts[1],$dataParts[2],$dataParts[0]));
}

function displayCriteria($FK_CriteriaParmNesting,$eventHandlerID,$installationID,$dbADO)
{
	$out='';
	$cplArray=array();
	$resCPL=$dbADO->Execute('SELECT * FROM CriteriaParmList ORDER BY Description ASC');
	while($rowCPL=$resCPL->FetchRow()){
		$cplArray[$rowCPL['PK_CriteriaParmList']]=$rowCPL['Description'];
	}
	
	$resCPN=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?',$FK_CriteriaParmNesting);
	$rowCPN=$resCPN->FetchRow();
	$out.='
		<table border="0" width="100%">
			<tr bgcolor="#E7E7E7">
				<td><B>(</B></td>
				<td colspan="2"><input type="checkbox" name="nestingNot_'.$FK_CriteriaParmNesting.'" value="1" '.(($rowCPN['IsNot']==1)?'checked':'').' onClick="self.location=\'index.php?section=editCriteria&action=update&ehID='.$eventHandlerID.'&chCPN='.$FK_CriteriaParmNesting.'&setNot='.(($rowCPN['IsNot']==1)?'0':'1').'\'"> Not <input type="checkbox" name="nestingAnd_'.$FK_CriteriaParmNesting.'" value="1" '.(($rowCPN['IsAnd']==1)?'checked':'').' onClick="self.location=\'index.php?section=editCriteria&action=update&ehID='.$eventHandlerID.'&chCPN='.$FK_CriteriaParmNesting.'&setAnd='.(($rowCPN['IsAnd']==1)?'0':'1').'\'"> And </td>
				<td colspan="3" align="right"><a href="index.php?section=editCriteria&ehID='.$eventHandlerID.'&fkCPN='.$FK_CriteriaParmNesting.'&action=addSegment">Add segment</a> &nbsp;&nbsp;&nbsp;<a href="index.php?section=editCriteria&ehID='.$eventHandlerID.'&parentCPN='.$FK_CriteriaParmNesting.'&action=addNesting">Add nesting</a> &nbsp;&nbsp;<a href="#" onClick="if(confirm(\'Are you sure you want to delete this nesting?\'))self.location=\'index.php?section=editCriteria&ehID='.$eventHandlerID.'&delCPN='.$FK_CriteriaParmNesting.'&action=delNesting\'">Delete</a></td>
			</tr>';
	$queryCP='
			SELECT CriteriaParm.*, CriteriaParmList.Description AS CPL_Description
			FROM CriteriaParm
			LEFT JOIN CriteriaParmList ON FK_CriteriaParmList=PK_CriteriaParmList
			WHERE FK_CriteriaParmNesting=?';
	$resCP=$dbADO->Execute($queryCP,$FK_CriteriaParmNesting);
	while($rowCP=$resCP->FetchRow()){
		$GLOBALS['displayedCriteriaParms'][]=$rowCP['PK_CriteriaParm'];
		$out.='
			<tr>
				<td>&nbsp;&nbsp;';
			$out.='
				<select name="CriteriaParmID_'.$rowCP['PK_CriteriaParm'].'" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
					<option value="0">- Please select -</option>';
				foreach ($cplArray AS $cplID=>$cplDescription){
					$out.='<option value="'.$cplID.'" '.(($rowCP['FK_CriteriaParmList']==$cplID)?'selected':'').'>'.$cplDescription.'</option>';
				}
				$out.='</select></td>
				<td colspan="3">';
			// exception: FK_CriteriaParmList=24 EventParameter
			if($rowCP['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$resEventParameter=$dbADO->Execute('SELECT * FROM EventParameter ORDER BY Description ASC');
					$out.='Event parameter: <select name="CriteriaParmParm_'.$rowCP['PK_CriteriaParm'].'" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
						<option value="0" ></option>';
					while($rowEP=$resEventParameter->FetchRow()){
						if(isset($_POST['CriteriaParmParm_'.$rowCP['PK_CriteriaParm']])){
							$isSelectedParm=($rowEP['PK_EventParameter']==$_POST['CriteriaParmParm_'.$rowCriteria['PK_CriteriaParm']])?'selected':'';
						}
						else{	
							$isSelectedParm=($rowCP['Parm']==$rowEP['PK_EventParameter'])?'selected':'';	// check old value
						}
						$out.='<option value="'.$rowEP['PK_EventParameter'].'" '.$isSelectedParm.'>'.$rowEP['Description'].'</option>';						
						if($isSelectedParm=='selected')				
							$selectedEventParameter=$rowEP['Description'];
					}
					$out.='</select>';
				}
				$out.=' <select name="CriteriaParmOperator_'.$rowCP['PK_CriteriaParm'].'">
						<option value="1" '.(($rowCP['Operator']==1)?'selected':'').'>=</option>
						<option value="2" '.(($rowCP['Operator']==2)?'selected':'').'>&lt;&gt;</option>
				</select> ';
				if($rowCP['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$out.='Value: ';
					switch(@$selectedEventParameter){
						case 'PK_Device':
						case 'PK_Room':
						$tableName=substr($selectedEventParameter,3);
						$out.=generatePullDown('CriteriaParmValue_'.$rowCP['PK_CriteriaParm'],$tableName,'PK_'.$tableName,'Description',$rowCP['Value'],$dbADO,"WHERE FK_Installation='$installationID'");
						break;
						default:
						$out.='<input type="textbox" name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'" value="'.$rowCP['Parm'].'">';
						break;
					}
				}else{
					if(substr($rowCP['CPL_Description'],0,3)=='PK_'){
						$out.='<select name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'">
										<option value="0" ></option>';
						$tableName=substr($rowCP['CPL_Description'],3);
						$filterTable=($tableName=='Device' || $tableName=='Room')?" WHERE FK_Installation='".$installationID."' ":'';
						$queryTable="SELECT * FROM $tableName $filterTable ORDER BY Description ASC";
						$resTable=$dbADO->Execute($queryTable);
						while($rowTable=$resTable->FetchRow()){
							$out.='<option value="'.$rowTable[$rowCP['CPL_Description']].'" '.(($rowCP['Value']==$rowTable[$rowCP['CPL_Description']])?'selected':'').'>'.$rowTable['Description'].'</option>';
						}
						$out.='</select>';
					}elseif($rowCP['CPL_Description']=='State'){
						$stateArray=array(5=>'Afternoon',1=>'Daylight',6=>'Evening',3=>'Morning',7=>'Night',2=>'Not Daylight',9=>'Weekday',8=>'Weekend');
						$out.='<select name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'">
										<option value=""></option>';
						foreach($stateArray AS $key => $value){
							$out.='<option value="'.$key.'" '.(($rowCP['Value']==$key)?'selected':'').'>'.$value.'</option>';
						}
						$out.='
									</select>';
					}
					else{
						$out.='<input type="text" name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'" value="'.$rowCP['Value'].'">';
					}
				}
				$out.='
				</td>
				<td align="center">'.(($rowCPN['IsAnd']==1)?'And':'Or').'</td>
				<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this criteria parm?\'))self.location=\'index.php?section=editCriteria&action=delete&ehID='.$eventHandlerID.'&dcpID='.$rowCP['PK_CriteriaParm'].'\'">Delete</a></td>			
			</tr>';
	}
	$resNestingChilds=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE FK_CriteriaParmNesting_Parent=?',$FK_CriteriaParmNesting);
	if($resNestingChilds->RecordCount()!=0){
		$out.='<tr>
				<td colspan="6"><table width="100%" border="0">
					<tr>
						<td width="20">&nbsp;</td>
						<td>';
		while($rowNestingChilds=$resNestingChilds->FetchRow()){
			$out.=displayCriteria($rowNestingChilds['PK_CriteriaParmNesting'],$eventHandlerID,$installationID,$dbADO);
		}
				$out.='</td>
					</tr>
				</table>
				</td>
			</tr>';
	}
	$out.='
			<tr bgcolor="#E7E7E7">
				<td colspan="6"><B>)</B></td>
			</tr>
	</table>';
	$out.='<input type="hidden" name="displayedCriteriaParm" value="'.join(',',$GLOBALS['displayedCriteriaParms']).'">';
	return $out;
}

function deleteCriteriaParmNesting($cpnID,$dbADO)
{
	$dbADO->Execute('DELETE FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?',$cpnID);
	$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=?',$cpnID);
	
	$resChilds=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE FK_CriteriaParmNesting_Parent=?',$cpnID);
	while($rowChilds=$resChilds->FetchRow()){
		deleteCriteriaParmNesting($rowChilds['PK_CriteriaParmNesting'],$dbADO);
	}
}

function setOrbitersNeedConfigure($installationID,$dbADO)
{
	$orbitersArray=getValidOrbitersArray($installationID,$dbADO);
	if(count($orbitersArray)!=0){
		$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device IN ('.join(',',$orbitersArray).') AND FK_Installation=?',$installationID);
		$dbADO->Execute('UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter IN ('.join(',',$orbitersArray).')'); 
	}
}

function setDCERouterNeedConfigure($installationID,$dbADO)
{
	$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['rootDCERouter'],$installationID));
}

function addScenariosToRoom($roomID, $installationID, $dbADO)
{
	$insertTelecomScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) SELECT '.$GLOBALS['ArrayIDCommunicationScenarios'].','.$installationID.',\'Phones Scenario\','.$GLOBALS['TelecomScenariosTemplate'].',Description FROM Room WHERE PK_Room=?';
	$dbADO->Execute($insertTelecomScenario,$roomID);
	$cgID=$dbADO->Insert_ID();

	$insertCG_R='INSERT INTO CommandGroup_Room (FK_Room, FK_CommandGroup,Sort) VALUES (?,?,?)';
	$dbADO->Execute($insertCG_R,array($roomID,$cgID,$cgID));

	$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) VALUES(?,?,?)";
	$dbADO->Execute($queryInsertCommandGroup_Command,array($cgID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));
	$CG_C_insertID=$dbADO->Insert_ID();

	$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
	$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamID'],''));
	$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$GLOBALS['TelecomMenu']));
	$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamDesignObjCurrentScreen'],''));
	$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],''));
	$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamStoreVariables'],''));

	$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup, FK_Room,Sort) VALUES (?,?,?)';
	$insertCG_C='
		INSERT INTO CommandGroup_Command 
			(FK_CommandGroup,FK_Command,TurnOff,OrderNum,FK_Device)
		VALUES
			(?,?,?,?,?)';
	$insertCG_C_CP='
		INSERT INTO CommandGroup_Command_CommandParameter 
			(FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter)
		VALUES
			(?,?,?)';
	
	$selectArmDisarm='SELECT * FROM CommandGroup WHERE FK_Template=? AND FK_Installation=?';
	$resArmDisarm=$dbADO->Execute($selectArmDisarm,array($GLOBALS['SecurityArmDisarmTemplate'],$installationID));
	if($resArmDisarm->RecordCount()){
		$rowArmDisarm=$resArmDisarm->FetchRow();
		$armDisarmCG=$rowArmDisarm['PK_CommandGroup'];
	}else{
		$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'Arm Disarm',0,0,0,$GLOBALS['SecurityArmDisarmTemplate']));
		$armDisarmCG=$dbADO->Insert_ID();

		$dbADO->Execute($insertCG_C,array($armDisarmCG,$GLOBALS['commandGotoScreen'],0,0,$GLOBALS['localOrbiter']));
		$cg_cID=$dbADO->Insert_ID();
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
	}
	$dbADO->Execute($insertCG_Room,array($armDisarmCG,$roomID,$armDisarmCG));
	
	$selectSOS='SELECT * FROM CommandGroup WHERE FK_Template=? AND FK_Installation=?';
	$resSOS=$dbADO->Execute($selectSOS,array($GLOBALS['SecuritySOSTemplate'], $installationID));
	if($resSOS->RecordCount()){
		$rowSOS=$resSOS->FetchRow();
		$sosCG=$rowSOS['PK_CommandGroup'];
	}else{
		$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'*SOS*',0,0,0,$GLOBALS['SecuritySOSTemplate']));
		$sosCG=$dbADO->Insert_ID();

		$dbADO->Execute($insertCG_C,array($sosCG,$GLOBALS['commandGotoScreen'],0,0,$GLOBALS['localOrbiter']));
		$cg_cID=$dbADO->Insert_ID();
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
	}
	$dbADO->Execute($insertCG_Room,array($sosCG,$roomID,$sosCG));
}

function getMediaPluginID($installationID,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['rootMediaPlugin'],$installationID));
	$row=$res->FetchRow();
	return $row['PK_Device'];
}

function pulldownFromArray($valuesArray,$name,$selectedValue,$extra='',$valueKey='key',$zeroValueDescription='- Please select -')
{
//	if(count($valuesArray)==0)
//		return null;
	$out='<select name="'.$name.'" "'.$extra.'">';
	if($zeroValueDescription!='')
		$out.='<option value="0">'.$zeroValueDescription.'</option>';
	foreach ($valuesArray AS $key=>$value){
		$optionValue=($valueKey=='key')?$key:$value;
		$out.='<option value="'.$optionValue.'" '.(($optionValue==$selectedValue)?'selected':'').'>'.$value.'</option>';
	}
	$out.='</select>';
	return $out;
}

$GLOBALS['wizardChilds']=array();
function getChildsOfWizard($page,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM PageSetup WHERE FK_PageSetup_Parent=? ORDER BY OrderNum ASC',$page);
	while($row=$res->FetchRow()){
		$GLOBALS['wizardChilds'][$row['PK_PageSetup']]=$row['pageURL'];
		getChildsOfWizard($row['PK_PageSetup'],$dbADO);
	}
	return $GLOBALS['wizardChilds'];
}

function deviceForScenariosSelector($name,$selectedValue,$dbADO,$allowNoValue=1,$extra='')
{
	$out='
		<select name="'.$name.'" '.$extra.'>';
	if($allowNoValue==1){
		$out.='<option value="0">-Please select-</option>';
	}
	$out.='
			<option value="-300" '.(($selectedValue=='-300')?'selected':'').'>[Local Orbiter]</option>';
							
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia IS NULL
		ORDER BY FK_Room ASC,Description ASC';
	$resTopDevices=$dbADO->Execute($query,array($_SESSION['installationID']));
							
	if ($resTopDevices) {
		while ($rowTopDevice = $resTopDevices->FetchRow()) {
			$roomName=($rowTopDevice['RoomName']!='')?stripslashes($rowTopDevice['RoomName']):'No Room';
			$out.='<option '.($rowTopDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowTopDevice['PK_Device'].'" title="Device template #'.$rowTopDevice['FK_DeviceTemplate'].': '.$rowTopDevice['Template'].'">'.$rowTopDevice['Description'].' ['.$roomName.']</option>';
			$out.=pulldownChildsDevice($rowTopDevice['PK_Device'],$selectedValue,$dbADO,1);
		}
	}
						
	$out.='
		</select>';
	return $out;
}

function getDeviceNameForScenarios($deviceID,$dbADO)
{
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE PK_Device = ? ';
	$resDevice=$dbADO->Execute($query,$deviceID);
							
	if ($resDevice->RecordCount()>0) {
		$row = $resDevice->FetchRow();
		return '<span title="Device template #'.$row['FK_DeviceTemplate'].' : '.$row['Template'].'"><B>'.$row['Description'].'</B> ['.(($row['RoomName']!='')?$row['RoomName']:'None').']</span>';
	}else
		return '';
	
}

function pulldownChildsDevice($parentDevice,$selectedValue,$dbADO,$depth,$filter='')
{
	$out='';
	$query = '
		SELECT Device.*, Room.Description AS RoomName,DeviceTemplate.Description AS Template
		FROM Device 
		LEFT JOIN Room ON FK_Room=PK_Room
		INNER JOIN DeviceTemplate ON FK_DEviceTemplate=PK_DeviceTemplate
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia =? '.$filter.'
		ORDER BY FK_Room ASC,Description ASC';
	$resDevices=$dbADO->Execute($query,array($_SESSION['installationID'],$parentDevice));
	while($rowDevice=$resDevices->FetchRow()){
		$roomName=($rowDevice['RoomName']!='')?stripslashes($rowDevice['RoomName']):'No Room';
		$prefix='&nbsp;&nbsp;&nbsp;|';
		for($i=0;$i<$depth;$i++)
			$prefix.='---';
		$out.='<option '.($rowDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowDevice['PK_Device'].'" title="Device template #'.$rowDevice['FK_DeviceTemplate'].': '.$rowDevice['Template'].'">'.$prefix.' '.$rowDevice['Description'].' ['.$roomName.']</option>';
		$out.=pulldownChildsDevice($rowDevice['PK_Device'],$selectedValue,$dbADO,($depth+1));
	}
	return $out;
}

function getInstallWizardDeviceTemplates($step,$dbADO,$device='',$distro=0,$operatingSystem=0)
{
	if($distro!=0){
		$queryDistro='SELECT * FROM Distro WHERE PK_Distro=?';
		$resDistro=$dbADO->Execute($queryDistro,$distro);
		if($resDistro->RecordCount()>0){
			$rowDistro=$resDistro->FetchRow();
			$distroName=$rowDistro['Description'];
			$operatingSystem=$rowDistro['FK_OperatingSystem'];
		}else
			$distroName='';
	}
	
	if($operatingSystem!=0){
		$queryOperatingSystem='SELECT * FROM OperatingSystem WHERE PK_OperatingSystem=?';
		$resOperatingSystem=$dbADO->Execute($queryOperatingSystem,$operatingSystem);
		if($resOperatingSystem->RecordCount()>0){
			$rowOperatingSystem=$resOperatingSystem->FetchRow();
			$operatingSystemName=$rowOperatingSystem['Description'];
		}else
			$operatingSystemName='';
	}
			
	$out='<table>';
	$queryInstallWizard='
		SELECT InstallWizard.*,
			DeviceCategory.Description AS Category,
			DeviceTemplate.Description AS Template
		FROM InstallWizard
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE step=?';
	$resInstallWizard=$dbADO->Execute($queryInstallWizard,$step);
	if($resInstallWizard->RecordCount()==0){
		$out.='
			<tr>
				<td>&nbsp;</td>
			</tr>';
	}else{
		$out.='
			<tr class="normaltext">
				<td colspan="5"><b>What software modules do you want on this device?</b> &nbsp; The most common are selected by default.</td>
			</tr>';
	}
	$oldCategory='';
	$displayedTemplatesRequired=array();
	while($row=$resInstallWizard->FetchRow()){
		if($row['Category']!=$oldCategory){
			$oldCategory=$row['Category'];
			$displayCategory=$row['Category'];
		}else 
			$displayCategory='-';
		// check if the device actually exist to display actual entries
		if($device!=''){
			if($device!=@$_SESSION['CoreDCERouter']){
				$queryDevice='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
				$resDevice=$dbADO->Execute($queryDevice,array($row['FK_DeviceTemplate'],$device));
			}else{
				// if device is DCE Router, check also childs of Core
				$queryDevice='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND (FK_Device_ControlledVia=? OR FK_Device_ControlledVia=?)';
				$resDevice=$dbADO->Execute($queryDevice,array($row['FK_DeviceTemplate'],$_SESSION['CoreDCERouter'],$_SESSION['CoreID']));
			}
			$deviceTemplateChecked=($resDevice->RecordCount()==0)?'':'checked';
			$rowDevice=$resDevice->FetchRow();
			$oldDevice=$rowDevice['PK_Device'];
		}

		$out.='
			<tr class="normaltext">
				<td align="center"><b>'.$displayCategory.'</b></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;'.$row['Template'].'</td>';
		$query='
			SELECT 
				InstallWizard_Distro.*,
				InstallWizard.Default
			FROM InstallWizard_Distro
				INNER JOIN InstallWizard ON FK_InstallWizard=PK_InstallWizard
			WHERE (FK_Distro IS NULL OR FK_Distro=?) AND (FK_OperatingSystem=? OR FK_OperatingSystem IS NULL) AND FK_InstallWizard=?';
		$res=$dbADO->Execute($query,array($distro,$operatingSystem,$row['PK_InstallWizard']));
		$rowIWD=$res->FetchRow();
		$templateIsChecked=((isset($deviceTemplateChecked) && @$isCoreFirstTime!=1)?$deviceTemplateChecked:(($rowIWD['Default']==1)?'checked':''));
		$out.='<td><input type="checkbox" '.(($res->RecordCount()==0)?'disabled':'').' '.$templateIsChecked.' name="device_'.$device.'_requiredTemplate_'.$row['FK_DeviceTemplate'].'" value="1"> 
			<input type="hidden" name="templateName_'.$row['FK_DeviceTemplate'].'" value="'.$row['Template'].'">
			<input type="hidden" name="oldDevice_'.$device.'_requiredTemplate_'.$row['FK_DeviceTemplate'].'" value="'.@$oldDevice.'">
		';
		if($res->RecordCount()==0)
			$out.='Not available for '.(($distro!=0)?$distroName:'').(($operatingSystem!=0)?' / '.$operatingSystemName:'');
		else{
			$displayedTemplatesRequired[]=$row['FK_DeviceTemplate'];
			$out.=$rowIWD['Comments'];
		}
		$out.='
				</td>
			</tr>
		';
	}

	$out.='	<input type="hidden" name="displayedTemplatesRequired_'.$device.'" value="'.(join(',',$displayedTemplatesRequired)).'">
	</table>';
	return $out;
}

function getMediaDirectorOrbiterChild($MD_PK_Device,$dbADO)
{
	$DTArray=getDeviceTemplatesFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
	if(count($DTArray)==0)
		return null;
	$getOrbiterChild='SELECT * FROM Device WHERE FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Device_ControlledVia=?';
	$resOrbiterChild=$dbADO->Execute($getOrbiterChild,array($MD_PK_Device));
	if($resOrbiterChild->RecordCount()!=0){
		$rowOrbiterChild=$resOrbiterChild->FetchRow();
		return $rowOrbiterChild['PK_Device'];
	}
	return null;
}

function getDevicesFromCategories($categoriesArray,$dbADO)
{
	unset($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray']=array();

	$devicesFullArray=array();
	foreach ($categoriesArray AS $categoryID){
		$devicesFullArray=$devicesFullArray+getDevicesArrayFromCategory($categoryID,$dbADO);
	}
	return $devicesFullArray;
}

function commandPulldownForDevice($deviceID,$dbADO)
{
	$out='';
	if ($deviceID!=0) {
		if(cleanInteger(@$_REQUEST['newDevice']!='-300')){
			$query = "
				SELECT PK_Command,Command.Description
				FROM Device 
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
				INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				WHERE PK_Device=?";
			$resNewCommand = $dbADO->Execute($query,$deviceID);
		}else{
			$query = "
				SELECT PK_Command,Command.Description
				FROM Command 
				INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				WHERE FK_DeviceTemplate=?
				ORDER BY Command.Description ASC";
			$resNewCommand = $dbADO->Execute($query,$GLOBALS['deviceTemplateOrbiter']);
		}
		if ($resNewCommand) {
			$out.='<select name="addNewDeviceCommand">
					<option value="0">-please select-</option>';
			while ($rowNewCommand = $resNewCommand->FetchRow()) {
				$out.='<option value="'.$rowNewCommand['PK_Command'].'">'.$rowNewCommand['Description'].'</option>';
			}
			if ($resNewCommand->RecordCount()==0) {
				$out.='<option value="0">-no command-</option>';
			}
			$resNewCommand->Close();
			$out.='</select>';
		}
	}else{
		return '';
	}
	return $out;
}

function lightingDevicesTable($cgID,$dbADO)
{
	$out='
		<table cellpadding="2">
			<tr bgcolor="#DDDDDD">
				<td align="center"><B>Device / Room</B></td>
				<td align="center"><B>Type</B></td>
				<td align="center"><B>Unchanged</B></td>
				<td align="center"><B>On</B></td>
				<td align="center"><B>Off</B></td>
				<td align="center"><B>Set level</B></td>
			</tr>';

	$lightingDevicesArray=getValidLightingObjectsArray($_SESSION['installationID'],$dbADO);
	if(count($lightingDevicesArray)==0)
	$lightingDevicesArray[]=0;
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE PK_Device IN ('.join(',',$lightingDevicesArray).')
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice);
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="6">No lighting devices available in this room.</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$lightingCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['genericSetLevelCommand'];
		$queryCGCommands='
				SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
				FROM Command 
				INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
				INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
				LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
				WHERE FK_Device =? AND PK_Command IN ('.$lightingCommands.') AND FK_CommandGroup=?
				ORDER BY Description ASC';

		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$dimValue='';
		if($resCGCommands->RecordCount()==0)
		$selectedCommand=1;
		else{
			$rowCGCommands=$resCGCommands->FetchRow();
			switch($rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
				$selectedCommand=2;
				break;
				case $GLOBALS['genericOFFCommand']:
				$selectedCommand=3;
				break;
				case $GLOBALS['genericSetLevelCommand']:
				$selectedCommand=4;
				$dimValue=($rowCGCommands['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCommands['IK_CommandParameter']:'';
				break;
			}
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.(($rowGetRoomsDevice['RoomName']=='')?'None':$rowGetRoomsDevice['RoomName']).'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'> <input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.scenarioWizard.command_'.$rowGetRoomsDevice['PK_Device'].'[3].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="updateDevices" value="Update"  ></td>
			</tr>
		';
	}
	$out.='</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
		<script>
		function validateForm()
		{
			devicesNo='.count($displayedDevices).';
			devicesArray=new Array(devicesNo);';
			$posStart=0;
			foreach($displayedDevices as $Device){
				$out.='devicesArray['.$posStart.']='.$Device.';';
				$posStart++;
			}
		$out.='
			for(i=0;i<devicesNo;i++){
				eval("isDim=document.scenarioWizard.command_"+devicesArray[i]+"[3].checked");
				if(isDim==true){
					eval("dimVal=parseInt(document.scenarioWizard.dimValue_"+devicesArray[i]+".value)");
					if(dimVal<0 || dimVal>100 || isNaN(dimVal)){
						alert("Please enter a value between 0 and 100.");
						eval("document.scenarioWizard.dimValue_"+devicesArray[i]+".focus()");
						return false;
					}
				}
			}
			return true;
		}
		</script>';
	
	return $out;
}

function climateDevicesTable($cgID,$dbADO)
{
	$out='
		<table>
			<tr bgcolor="#DDDDDD">
				<td align="center"><B>Device / Room</B></td>
				<td align="center"><B>Type</B></td>
				<td align="center"><B>Unchanged</B></td>
				<td align="center"><B>Auto</B></td>
				<td align="center"><B>Heat</B></td>
				<td align="center"><B>Cool</B></td>
				<td align="center"><B>Off</B></td>
				<td align="center"><B>Set temperature</B></td>
			</tr>';

	$climateDevicesArray=getValidClimateObjectsArray($_SESSION['installationID'],$dbADO);
	if(count($climateDevicesArray)==0)
	$climateDevicesArray[]=0;
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			LEFT JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE PK_Device IN ('.join(',',$climateDevicesArray).') AND FK_DeviceCommandGroup = ?
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice,$GLOBALS['ThermostatCommands']);
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="9">No climate devices available in this room.</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$climateCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['setCoolHeat'].','.$GLOBALS['setCoolHeat'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['genericSetLevelCommand'];
		$queryCGCommands='
			SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
			FROM Command 
			INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
			INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
			LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
			WHERE FK_Device =? AND PK_Command IN ('.$climateCommands.') AND FK_CommandGroup=?
			ORDER BY Description ASC';

		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$dimValue='';
		if($resCGCommands->RecordCount()==0)
			$selectedCommand=1;
		else{
			while($rowCGC=$resCGCommands->FetchRow()){
				if($rowCGC['FK_Command']!=$GLOBALS['genericSetLevelCommand'])
					$rowCGCommands=$rowCGC;
				else
					$rowCGCSetLevel=$rowCGC;
			}
			switch(@$rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
					$selectedCommand=2;
				break;
				case $GLOBALS['setCoolHeat']:
					if($rowCGCommands['IK_CommandParameter']=='H')
						$selectedCommand=3;
					else
						$selectedCommand=4;
				break;
				case $GLOBALS['genericOFFCommand']:
					$selectedCommand=5;
				break;
				default:
					$selectedCommand=0;
				break;
			}
			$dimValue=(@$rowCGCSetLevel['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCSetLevel['IK_CommandParameter']:'';
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.(($rowGetRoomsDevice['RoomName']=='')?'None':$rowGetRoomsDevice['RoomName']).'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="5" '.(($selectedCommand==5)?'checked':'').'></td>
				<td align="center"><input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.climateScenarios.command_'.$rowGetRoomsDevice['PK_Device'].'[5].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
				<tr>
					<td colspan="9" align="center"><input type="submit" class="button" name="updateDevices" value="Update"  ></td>
				</tr>';
	}
	$out.='</table>
			<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
	return $out;
}

function advancedCommandGroupCommandsTable($cgID,$section,$dbADO)
{
	$selectCommandsAssigned = "
		SELECT CommandGroup_Command.*
		FROM CommandGroup_Command				
		LEFT JOIN Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
		WHERE (Device.FK_Installation = ? OR Device.FK_Installation IS NULL) AND CommandGroup_Command.FK_CommandGroup = ?
		ORDER BY PK_CommandGroup_Command ASC";
	$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($_SESSION['installationID'],$cgID));
	$out='
		<input type="hidden" name="commandToTest" value="">
		<input type="hidden" name="cgc" value="">
		<table border="0" cellpadding="2" cellspacing="0">';

	if($resCommandAssigned->RecordCount()>0){
		$lineCount=0;
		while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {
			$lineCount++;
			$out.='
				<tr bgcolor="'.(($lineCount%2==1)?'#DDDDDD':'').'">
					<td valign="top">
						<a name="hook_'.$rowCommandAssigned['PK_CommandGroup_Command'].'"></a>
						<input type="hidden" name="device_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" value="'.$rowCommandAssigned['FK_Device'].'">'.getDeviceNameForScenarios($rowCommandAssigned['FK_Device'],$dbADO).'
					</td>
				<td>';
			if($rowCommandAssigned['FK_Device']!='-300'){
				$query = "SELECT PK_Command,Command.Description
								FROM Device 
									INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
									INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
									INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
							   WHERE PK_Device=?";
				$resNewCommand = $dbADO->Execute($query,array($rowCommandAssigned['FK_Device']));
			}else{
				$query = "
							SELECT PK_Command,Command.Description
								FROM Command 
									INNER JOIN DeviceCommandGroup_Command ON 
										DeviceCommandGroup_Command.FK_Command = Command.PK_Command
									INNER JOIN DeviceTemplate_DeviceCommandGroup ON 
										DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
							   WHERE FK_DeviceTemplate=?
									ORDER BY Command.Description ASC";
				$resNewCommand = $dbADO->Execute($query,array(cleanInteger($GLOBALS['deviceTemplateOrbiter'])));
			}
			if ($resNewCommand) {
				$out.='<select name="deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">';
				while ($rowNewCommand = $resNewCommand->FetchRow()) {
					$out.='<option '.($rowCommandAssigned['FK_Command']==$rowNewCommand['PK_Command']?'selected="selected"':'').' value="'.$rowNewCommand['PK_Command'].'">'.$rowNewCommand['Description'].'</option>';
				}
				if ($resNewCommand->RecordCount()==0) {
					$out.='<option value="0">-no command-</option>';
				}
				$out.='</select>';
			}


			$query = "SELECT Command_CommandParameter.FK_CommandParameter,Command_CommandParameter.Description as C_CP_Description,CommandParameter.Description as CP_Description,
										CommandGroup_Command_CommandParameter.IK_CommandParameter,
										ParameterType.Description as PT_Description
											FROM Command_CommandParameter 
												INNER JOIN Command on FK_Command = PK_Command
												INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
												INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
												LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
									  WHERE FK_Command = ? AND FK_CommandGroup_Command =?
									  ORDER BY CommandParameter.Description asc
								";
			$resSelectParameters = $dbADO->Execute($query,array($rowCommandAssigned['FK_Command'],$rowCommandAssigned['PK_CommandGroup_Command']));

			if ($resSelectParameters) {
				$out.='<table>';
				while ($rowSelectParameters=$resSelectParameters->FetchRow()) {
					$out.="<tr ".(strlen(trim($rowSelectParameters['CP_Description']))==0?" bgColor='lightgreen' ":"").">
												<td>#{$rowSelectParameters['FK_CommandParameter']} <span title=\"{$rowSelectParameters['C_CP_Description']}\">{$rowSelectParameters['CP_Description']}</span> ({$rowSelectParameters['PT_Description']})</td>
												<td><input type='text' name=\"CommandParameterValue_{$rowCommandAssigned['PK_CommandGroup_Command']}_{$rowSelectParameters['FK_CommandParameter']}\" value=\"{$rowSelectParameters['IK_CommandParameter']}\" >".'</td></tr>';
				}
				$out.='</table>';
			}


			$out.='
					
							</td>
						<td valign="top">
						<input type="button" class="button" name="editA" value="Remove" onClick="document.'.$section.'.cgc.value='.$rowCommandAssigned['PK_CommandGroup_Command'].';document.'.$section.'.submit();">
						</td>						
					</tr>
					<tr bgcolor="'.(($lineCount%2==1)?'#DDDDDD':'').'">
						<td align="center" colspan="3"><input type="button" class="button" name="testCommand" value="Test command" onClick="document.'.$section.'.commandToTest.value='.$rowCommandAssigned['PK_CommandGroup_Command'].';document.'.$section.'.submit();"></td>
					</tr>
					';
		}

		$out.='
					<tr>
						<td colspan="3" align="center"><input type="submit" class="button" name="addNewDeviceButton" value="Update"  ></td>
					</tr>';
	}
	$out.='<tr>
						<td colspan="2">
							<a name="hook_0"></a>
					 		<B>Device:</B>'.deviceForScenariosSelector('addNewDevice',cleanInteger(@$_REQUEST['addNewDevice']),$dbADO,1,'onChange="document.'.$section.'.oldWizard.value=\'-1\';this.form.submit();"');
	$out.=(((int)@$_REQUEST['addNewDevice']!=0)?' Command: ':'').commandPulldownForDevice(cleanInteger(@$_REQUEST['addNewDevice']),$dbADO).'
						<input type="submit" class="button" name="addNewDeviceButton" value="Add"  ></td>
					</tr>
				</table>';
	return $out;
}

function processLightingScenario($cgID,$dbADO)
{
	$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
	foreach($displayedDevicesArray AS $elem){
		$deviceCommand=(int)@$_POST['command_'.$elem];
		$dimValue=@$_POST['dimValue_'.$elem];
		$oldCommand=@$_POST['oldCommand_'.$elem];
		$oldDimValue=@$_POST['oldDimValue_'.$elem];
		$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
		$deleteParameters='
			DELETE CommandGroup_Command_CommandParameter
			FROM CommandGroup_Command_CommandParameter
			JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
			WHERE FK_CommandGroup=? AND FK_Device=?';

		if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldDimValue!=$dimValue))	{
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
			$dbADO->Execute($deleteParameters,array($cgID,$elem));

			switch($deviceCommand){
				case 1:
					// do nothing, delete is done above
				break;
				case 2:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
				break;
				case 3:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				break;
				case 4:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
					$cgcInsertID=$dbADO->Insert_ID();

					$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParamAbsoluteLevel'],$dimValue));
				break;
			}
		}
	}
}

function processClimateScenario($cgID,$dbADO)
{
	$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
	foreach($displayedDevicesArray AS $elem){
		$deviceCommand=(int)@$_POST['command_'.$elem];
		$dimValue=@$_POST['dimValue_'.$elem];
		$oldCommand=@$_POST['oldCommand_'.$elem];
		$oldDimValue=@$_POST['oldDimValue_'.$elem];
		$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
		$deleteParameters='
			DELETE CommandGroup_Command_CommandParameter
			FROM CommandGroup_Command_CommandParameter
			JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
			WHERE FK_CommandGroup=? AND FK_Device=?';
		if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldDimValue!=$dimValue))	{
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericHeatCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericCoolCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['setCoolHeat']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
			$dbADO->Execute($deleteParameters,array($cgID,$elem));

			switch($deviceCommand){
				case 1:
					// do nothing, delete is done above
				break;
				case 2:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
				break;
				case 3:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['setCoolHeat']));
					$cgcInsertID=$dbADO->Insert_ID();
	
					$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParameterOnOff'],'H'));
				break;
				case 4:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['setCoolHeat']));
					$cgcInsertID=$dbADO->Insert_ID();
	
					$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParameterOnOff'],'C'));
				break;
				case 5:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				break;
			}

			if($dimValue!=''){
				$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
				$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
				$cgcInsertID=$dbADO->Insert_ID();

				$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
				$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParamAbsoluteLevel'],$dimValue));
			}

		}
	}
}

function processAdvancedScenarios($cgID,$section,$dbADO)
{
	$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:0;
	$from=@$_REQUEST['from'];
	$roomID=(int)@$_REQUEST['roomID'];
	$ehID=@$_REQUEST['ehID'];
	$CommandIsTested=(isset($_REQUEST['commandToTest']) && (int)$_REQUEST['commandToTest']>0)?1:0;
	$returnHook=null;
	
	if(isset($_REQUEST['cgc']) && (int)$_REQUEST['cgc']>0){
		$toDel=(int)$_REQUEST['cgc'];
		$commandsDeleted='';
		$parametersDeleted='';
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$query = $dbADO->Execute($deleteObjFromDevice,array($toDel));
		//delete saved values
		if ($dbADO->Affected_Rows()>0) {
			$commandsDeleted = "Command removed from this command group!";
			$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
			$query = $dbADO->Execute($deleteParamValues,array($toDel));
			if ($dbADO->Affected_Rows()>0) {
				$parametersDeleted = 'Parameter values also deleted!';
			} else {
				$parametersDeleted = 'No parameter values deleted!';
			}

		} else {
			$commandsDeleted = "Command not removed from this command group!";
		}
		if($section=='scenarioWizard'){
			header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=$commandsDeleted $parametersDeleted");
		}
		else{
			header("Location: index.php?section=$section&ehID=$ehID&wizard=$wizard&msg=$commandsDeleted $parametersDeleted");
		}
		exit();
	}

	// process commands and params

	$x=cleanInteger(@$_POST['addNewDevice']);
	$y=cleanInteger(@$_POST['addNewDeviceCommand']);

	if ($y!=0 && $x!=0) {
		$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) VALUES (?,?,?)";
		$insertRs = $dbADO->Execute($queryInsertCommandGroup_Command,array($cgID,$y,$x));
	}

	$GLOBALS['isModified'] = 0;
	if ($dbADO->Affected_Rows()>0) {
		$GLOBALS['isModified']=1;
	}
	$selectCommandsAssigned = "
				SELECT CommandGroup_Command.* 
				FROM CommandGroup_Command				
				LEFT JOIN Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
				WHERE (Device.FK_Installation = ? OR Device.FK_Installation IS NULL) AND CommandGroup_Command.FK_CommandGroup = ?";								

	$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($_SESSION['installationID'],$cgID));
	$GLOBALS['parametersUpdatedAlert'] = 'Parameters not updated!';

	if ($resCommandAssigned) {
		while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {

			$deviceSelected = isset($_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$x;
			$commandSelected = isset($_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$y;

			$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Device = ? WHERE PK_CommandGroup_Command = ? ';
			$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($deviceSelected,$rowCommandAssigned['PK_CommandGroup_Command']));

			if ($dbADO->Affected_Rows()==1) {//enter here only if the Device is changed
				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = NULL WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($rowCommandAssigned['PK_CommandGroup_Command']));
				//delete old parameters
				$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? ';
				$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));

			} elseif($commandSelected!=0) {
				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = ? WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($commandSelected,$rowCommandAssigned['PK_CommandGroup_Command']));
				if ($dbADO->Affected_Rows()==1) {//if we have changed the command, delete old values
					$returnHook=$rowCommandAssigned['PK_CommandGroup_Command'];
					$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? ';
					$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));
				}

			}
			//command parameters

			$query = "
				SELECT 
					Command_CommandParameter.FK_CommandParameter,
					Command_CommandParameter.Description as C_CP_Description,
					CommandParameter.Description as CP_Description,
					CommandGroup_Command_CommandParameter.IK_CommandParameter,
					ParameterType.Description as PT_Description
				FROM Command_CommandParameter 
				INNER JOIN Command on FK_Command = PK_Command
				INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
				INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
				LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
				WHERE FK_Command = ?";

			$resSelectParameters = $dbADO->Execute($query,array($commandSelected));
			if ($resSelectParameters) {
				while ($rowSelectParameters=$resSelectParameters->fetchRow()) {
					$value=cleanString(@$_POST['CommandParameterValue_'.$rowCommandAssigned['PK_CommandGroup_Command'].'_'.$rowSelectParameters['FK_CommandParameter']]);
					//see if we need to update or to insert
					$checkExists = "SELECT * FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter = ?";
					$resExists=$dbADO->Execute($checkExists,array($rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					if ($resExists->RecordCount()==1) {
						$update = "UPDATE CommandGroup_Command_CommandParameter  SET  IK_CommandParameter  = ? WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter =? ";
						$dbADO->Execute($update,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					} else {
						$insert = "INSERT INTO CommandGroup_Command_CommandParameter (IK_CommandParameter,FK_CommandGroup_Command,FK_CommandParameter) values(?,?,?)";
						$dbADO->Execute($insert,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					}
					if ($dbADO->Affected_Rows()==1) {
						$GLOBALS['parametersUpdatedAlert']='Parameters updated!';
					}
				}
			}
		}
	}

	
	if($CommandIsTested==1){

		$commandToTest=$_REQUEST['commandToTest'];
		$queryCommands='
			SELECT CommandGroup_Command_CommandParameter.*, FK_Command,FK_Device 
			FROM CommandGroup_Command
			LEFT JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command 
			WHERE PK_CommandGroup_Command=?';

		$resCommands=$dbADO->Execute($queryCommands,$commandToTest);
		$commandParmsArray=array();
		while($rowCommands=$resCommands->FetchRow()){
			$commandParmsArray[]=$rowCommands['FK_CommandParameter'].' "'.$rowCommands['IK_CommandParameter'].'"';
			$FK_Command=$rowCommands['FK_Command'];
			$FK_Device=$rowCommands['FK_Device'];
		}
		$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$FK_Device.' 1 '.$FK_Command.' '.join(' ',$commandParmsArray);
		exec($commandToSend);
		if($section=='scenarioWizard'){
			header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=The command with parameters was sent: $commandToSend#hook_$commandToTest");
		}else{
			header("Location: index.php?section=$section&ehID=$ehID&wizard=$wizard&msg=The command with parameters was sent");
		}
		exit();
	}
	
	return $returnHook;
}

function getDeviceNames($dbADO,$filter='')
{
	$devices=array();
	$resDevices=$dbADO->Execute('SELECT * FROM Device WHERE FK_Installation=? '.$filter,$_SESSION['installationID']);
	while($row=$resDevices->fetchRow()){
		$devices[$row['PK_Device']]=$row['Description'];
	}
	return $devices;
}

function setLeftMenu($dbADO)
{
	$jsRedirect='';
	$wizardPagesArray=getChildsOfWizard(1,$dbADO);
	$toCompare='index.php?'.$_SERVER['QUERY_STRING'];
	if(in_array($toCompare,$wizardPagesArray)){
		$pageID=array_search($toCompare,$wizardPagesArray);
		$isWizard=1;
		$redirectLink='index.php?section=wizard&pageSetup='.$pageID.'&senderID=0';
		$jsRedirect='<script>
			function getQueryVariable(query,variable) {
			  var vars = query.split("&");
			  for (var i=0;i<vars.length;i++) {
			    var pair = vars[i].split("=");
			    if (pair[0] == variable) {
			      return pair[1];
			    }
			  } 
			}

			try{
				var queryStr=top.treeframe.location.search.substring(9,top.treeframe.location.search.length);
				if(getQueryVariable(queryStr,"pageSetup")!='.$pageID.'){
					top.treeframe.location=\''.$redirectLink.'\';
				}
			}catch(e){
				// no frame
			}
		</script>';
	}
	return $jsRedirect;
}

function CheckValidCode($code,$dbADO)
{
	list($device, $pin) = explode("-", $code);
	$sql = "SELECT ActivationCode
			FROM Device
	 			INNER JOIN Installation ON Device.FK_Installation = Installation.PK_Installation
			WHERE Device.PK_Device = '$device' LIMIT 1";
	$res = $dbADO->Execute($sql);
	if ($res->RecordCount() == 0)
		return false;
	$row = $res->FetchRow();
	if ($row['ActivationCode'] !== $pin)
		return false;
	return true;
}

// return an array of packages
function GetActivationSh($code,$param='install',$otherParam='')
{
	global $dbPlutoMainUser;
	global $dbPlutoMainPass;
	global $dbPlutoMainServer;


	list($device, $pin) = explode("-", $code);
#	echo("/usr/pluto/bin/ConfirmDependencies -h ".$dbPlutoMainServer." -u ".$dbPlutoMainUser." -p ".$dbPlutoMainPass." -d $device install");
	exec("/usr/pluto/bin/ConfirmDependencies -D pluto_main -h $dbPlutoMainServer -u $dbPlutoMainUser -p '$dbPlutoMainPass' -d $device $otherParam $param", $result, $retcode);
	// || die("ERROR. Can't generate answer: $device:$pin");

	if ($retcode == 0)
		$return = "OK\n" . implode("\n", (array)$result) . "\n#EOF";
	else
		$return = "ERROR. Something went terribly wrong while generating the activation script. The installation can not continue. Please contact Plutohome to solve this problem. Thank you.";
	return $return;
}

function htmlPulldown($contentArray,$name,$selectedValue='None',$unselectedLabel)
{
	if(!is_array($contentArray))
		return '';	// error
	$out='<select name="'.$name.'">
		<option value="0">'.$unselectedLabel.'</option>';
	foreach ($contentArray AS $key=>$label){
		$out.='<option value="'.$key.'" '.(($key==$selectedValue)?'selected':'').'>'.$label.'</option>';
	}
	$out.='</select>';
	return $out;
}

function getAssocArray($table,$keyField,$labelField,$dbADO,$whereClause='',$orderClause='')
{
	$retArray=array();
	$res=$dbADO->Execute("SELECT $keyField,$labelField FROM $table $whereClause $orderClause");
	while($row=$res->FetchRow()){
		$retArray[$row[$keyField]]=$row[$labelField];
	}
	return $retArray;
}

function createDevice($FK_DeviceTemplate,$FK_Installation,$controlledBy,$roomID,$dbADO,$childOfMD=0)
{
	global $dbPlutoMainDatabase;
	
	$orbiterID=getMediaDirectorOrbiterChild($controlledBy,$dbADO);
	
	$parentID=($childOfMD==0)?$orbiterID:$controlledBy;
	$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$FK_DeviceTemplate.' -i '.$FK_Installation);
	$dbADO->Execute('UPDATE Device SET FK_Device_ControlledVia=?,FK_Room=? WHERE PK_Device=?',array($parentID,$roomID,$insertID));
}

function controlledViaPullDown($pulldownName,$deviceID,$dtID,$deviceCategory,$controlledVia,$dbADO,$zeroOption='0,- Please Select -',$jsEvent='')
{
	$optionsArray=getControlledViaDeviceTemplates($deviceID,$dtID,$deviceCategory,$dbADO);
	
	$selectOptions='';
	foreach ($optionsArray AS $key=>$value){
		$selectOptions.='<option value="'.$key.'" '.(($key==$controlledVia)?'selected':'').'>'.$value.'</option>';
	}
	
	$zeroValues=explode(',',$zeroOption);
	$out='
	<select name="'.$pulldownName.'">
		<option value="'.@$zeroValues[0].'">'.$zeroValues[1].'</option>
		'.$selectOptions.'
	</select>';
	
	return $out;
}

function getControlledViaDeviceTemplates($deviceID,$dtID,$deviceCategory,$dbADO)
{
	
	$installationID=(int)$_SESSION['installationID'];
	
	$whereClause='';
	if ($deviceID!=0) {
		$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
		$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$installationID));
		if ($resSelectMasterDeviceForParent) {
			$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
			$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];

			$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
			$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($DeviceTemplateForParent));

			$GLOBALS['childsDeviceCategoryArray'] = array();
			if ($resSelectControlledViaCategory) {
				while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
					$GLOBALS['childsDeviceCategoryArray'][]=$rowSelectControlledVia['FK_DeviceCategory'];
					getDeviceCategoryChildsArray($rowSelectControlledVia['FK_DeviceCategory'],$dbADO);
				}
			}
			$controlledViaCategoryArray = cleanArray($GLOBALS['childsDeviceCategoryArray']);

			$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
			$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

			$childsDeviceTemplateArray = array();
			if ($resSelectControlledViaDeviceTemplate) {
				while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
					$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];
				}
			}



			if (count($controlledViaCategoryArray)>0) {
				$whereClause.=' and (FK_DeviceCategory in ('.join(",",$controlledViaCategoryArray).')';
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
				$whereClause.=')';
			} else {
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
			}

		}
	}
	$whereClause.=" AND PK_Device!='$deviceID'";
	$queryDeviceTemplate = "
		SELECT DISTINCT Device.Description,Device.PK_Device
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE FK_Installation=? $whereClause order by Device.Description asc";
	$resDeviceTemplate = $dbADO->Execute($queryDeviceTemplate,$installationID);
	$optionsArray=array();
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$optionsArray[$row['PK_Device']]=$row['Description'];
		}
	}
	
	// if device is AV with UsesIR=1, override default controlled_via based on category and device template
	$controlledByIfIR=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
	$controlledByIfNotIR=getDevicesFromCategories(array($GLOBALS['rootComputerID']),$dbADO);
	
	// get selected category Device Templates
	$avArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
	if(in_array($dtID,$avArray)){
		$queryDevice='
			SELECT 
				Device.*, DeviceTemplate_AV.UsesIR
			FROM Device 
				INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
				LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate
			WHERE PK_Device=?';	
		$resDevice=$dbADO->Execute($queryDevice,$deviceID);
		$rowD=$resDevice->FetchRow();

		$tmpArray=array();
		$tmpArray[$deviceID]=$rowD['Description'];
		$devicesAllowedToControll=($rowD['UsesIR']==1)?array_diff($controlledByIfIR,$tmpArray):array_diff($controlledByIfNotIR,$tmpArray);

		foreach($devicesAllowedToControll as $key => $value){
			$optionsArray[$key]=$value;
			//$selectOptions.='<option value="'.$key.'" '.(($rowD['FK_Device_ControlledVia']==$key)?'selected':'').'>'.$value.'</option>';
		}
	}
	asort ($optionsArray, SORT_STRING);

	return $optionsArray;
}


function PortForHumans($device,$deviceNames)
{
	$devname = substr(strrchr($device, '/'), 1);

	if (strstr($devname, "ttyS") === $devname)
	{
		$devid = substr($devname, 4);
		if ($devid{0} === "_")
		{
			# it's a virtual serial port
			list($PK_Device, $number) = split("_", substr($devid, 1));
			$number++;
			# TODO: translate $PK_Device to actual device name
			return "Port $number on 'device ".@$deviceNames[$PK_Device]."'";
		}
		else
		{
			# it's a hardware serial port on the Core
			$number = $devid + 1;
			return "$devname | COM$number";
		}
	}
	
	return "Unknown: $devname";
}

function serialPortsPulldown($name,$selectedPort,$allowedToModify,$topParentIP)
{
	if($topParentIP==0){
		return 'Error: top parent IP not found';
	}
	
	$serial_ports=array();
	exec("sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '$topParentIP' /usr/pluto/bin/ListSerialPorts.sh", $serial_ports);

	$extra=((isset($allowedToModify) && $allowedToModify==0)?'disabled':'');
	$out=pulldownFromArray($serial_ports,$name,$selectedPort,$extra,'value');
	
	return $out;
}

// return the deviceID of the orbiter plugin from current installation
function getInfraredPlugin($installationID,$dbADO)
{
	$res=$dbADO->Execute('SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['InfraredPlugIn'],$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->Fetchrow();
		return $row['PK_Device'];
	}
}

function isInfrared($deviceTemplate,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM DeviceTemplate_AV WHERE FK_DeviceTemplate=? AND UsesIR=1',$deviceTemplate);
	if($res->RecordCount()>0){
		return 1;
	}else{
		return 0;
	}
}

function getCheckedDeviceCommandGroup($deviceTemplate,$deviceCategory,$dbADO)
{
	$querySelCheckedCommandGroups = 'SELECT FK_DeviceCommandGroup FROM DeviceTemplate_DeviceCommandGroup WHERE  FK_DeviceTemplate = ?';
	$resSelCheckedCommandGroups = $dbADO->Execute($querySelCheckedCommandGroups,$deviceTemplate);
	$selCheckedCommandsGroups = array();
	if ($resSelCheckedCommandGroups) {
		while ($rowSelCheckedCommandGroups = $resSelCheckedCommandGroups->FetchRow()) {
			$selCheckedCommandsGroups[]=$rowSelCheckedCommandGroups['FK_DeviceCommandGroup'];
		}
	}

	if (!is_array($selCheckedCommandsGroups)) {
		$selCheckedCommandsGroups=array();
		$selCheckedCommandsGroups[]=0;
	}
	if (count($selCheckedCommandsGroups)==0) {
		$selCheckedCommandsGroups[]=0;
	}

	$query = "
		SELECT  ".$dbADO->IfNull('FK_DeviceCategory_Parent','0')." AS parent 
		FROM DeviceCategory
		WHERE PK_DeviceCategory = ?";
	$deviceParent=0;
	$res = $dbADO->Execute($query,array($deviceCategory));
	if ($res) {
		while ($row = $res->FetchRow()) {
			$deviceParent=$row['parent'];
		}
	}

	$deviceParent=(int)$deviceParent;
	$deviceCG = array();
	$query = "
		SELECT DeviceCommandGroup.* 
		FROM DeviceCommandGroup
		INNER JOIN DeviceCategory on FK_DeviceCategory = PK_DeviceCategory
		WHERE 
			FK_DeviceCategory in ($deviceParent,$deviceCategory) OR
			PK_DeviceCommandGroup in (".join(",",$selCheckedCommandsGroups).") 
		ORDER BY DeviceCommandGroup.Description Asc	";
	$resCommands = $dbADO->_Execute($query);
				
	$groups = array();
	if ($resCommands) {
		while ($row = $resCommands->FetchRow()) {
			$groups[$row['PK_DeviceCommandGroup']]['checked']=(in_array($row['PK_DeviceCommandGroup'],$selCheckedCommandsGroups))?1:0;
			$groups[$row['PK_DeviceCommandGroup']]['Description']=$row['Description'];
		}
	}
	
	$resN=$dbADO->Execute('SELECT * FROM DeviceCommandGroup WHERE FK_DeviceCategory IS NULL');
	while($rowN=$resN->FetchRow()){
		$groups[$rowN['PK_DeviceCommandGroup']]['checked']=(in_array($rowN['PK_DeviceCommandGroup'],$selCheckedCommandsGroups))?1:0;
		$groups[$rowN['PK_DeviceCommandGroup']]['Description']=$rowN['Description'];
	}
	
	return $groups;
}

function getDeviceInformation($deviceID,$dbADO)
{
	$sql='
		SELECT 
			Device.*,
			DeviceTemplate.Description AS DeviceTemplate,
			DeviceCategory.Description AS Category,
			Manufacturer.Description AS Manufacturer
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		WHERE PK_Device=?
		';
	$res=$dbADO->Execute($sql,$deviceID);
	$row=array();
	if($res){
		$row=$res->FetchRow();
	}
	return $row;
}

function getTopLevelParent($deviceID,$dbADO)
{
	$topParent=0;
	if($deviceID!=0){
		$res=$dbADO->Execute('SELECT FK_DeviceTemplate,FK_Device_ControlledVia,IPaddress FROM Device WHERE PK_Device=?',$deviceID);
		$row=$res->FetchRow();
		if((int)$row['FK_Device_ControlledVia']!=0){
			$topParent=getTopLevelParent($row['FK_Device_ControlledVia'],$dbADO);
		}else{
			if($row['FK_DeviceTemplate']==$GLOBALS['rootCoreID'] || $row['FK_DeviceTemplate']==$GLOBALS['rootMediaDirectorsID']){
				$topParent=($row['FK_DeviceTemplate']==$GLOBALS['rootCoreID'] && $row['IPaddress']=='')?'127.0.0.1':$row['IPaddress'];
			}else{
				$topParent=0;
			}
		}
	}
	
	return $topParent;
}

function getDeviceFromDT($installationID,$DeviceTemplate,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($DeviceTemplate,$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}
}

function getDeviceData($deviceID,$deviceData,$dbADO)
{
	$req=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($deviceID,$deviceData));
	if($req->RecordCount()==0){
		return null;
	}else{
		$row=$req->FetchRow();
		return $row['IK_DeviceData'];
	}
}

function devicesTree($name,$selectedValue,$dbADO,$filter,$extra='')
{
	$out='
		<select name="'.$name.'" '.$extra.'>';
	$out.='<option value="0">-Please select-</option>';
							
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia IS NULL '.$filter.'
		ORDER BY FK_Room ASC,Description ASC';
	$resTopDevices=$dbADO->Execute($query,array($_SESSION['installationID']));
							
	if ($resTopDevices) {
		while ($rowTopDevice = $resTopDevices->FetchRow()) {
			$roomName=($rowTopDevice['RoomName']!='')?stripslashes($rowTopDevice['RoomName']):'No Room';
			$out.='<option '.($rowTopDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowTopDevice['PK_Device'].'" title="Device template #'.$rowTopDevice['FK_DeviceTemplate'].': '.$rowTopDevice['Template'].'">'.$rowTopDevice['Description'].' ['.$roomName.']</option>';
			$out.=pulldownChildsDevice($rowTopDevice['PK_Device'],$selectedValue,$dbADO,1,$filter);
		}
	}
						
	$out.='
		</select>';
	return $out;
}

function getDeviceGroups($installationID, $dbADO)
{
	$req=$dbADO->Execute('SELECT PK_DeviceGroup,Description FROM DeviceGroup WHERE FK_Installation=? ORDER BY Description ASC',$installationID);
	$dg=array();
	while($row=$req->Fetchrow()){
		$dg[$row['PK_DeviceGroup']]=$row['Description'];
	}
	return $dg;
}

function getDeviceTemplatesControlledBy($parentID,$dbADO)
{
	$whereClause='';
	$selectMasterDeviceForParent = '
		SELECT Device.FK_DeviceTemplate,FK_DeviceCategory,FK_DeviceCategory_Parent 
		FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE PK_Device = ?';
	$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($parentID));
	if ($resSelectMasterDeviceForParent) {
		$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
		$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];
		$DeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory'];
		$ParentDeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory_Parent'];

		$queryDTControlledByCategory ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE FK_DeviceCategory = ? OR FK_DeviceCategory = ?';
		$resDTControlledByCategory = $dbADO->Execute($queryDTControlledByCategory,array($DeviceCategoryForParent,$ParentDeviceCategoryForParent));

		$childsDeviceTemplateArray = array();
		if ($resDTControlledByCategory) {
			while ($rowDTControlledByCategory= $resDTControlledByCategory->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowDTControlledByCategory['FK_DeviceTemplate'];
			}
		}

		$querySelectControlledViaDeviceTemplate ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate_ControlledVia = ?';
		$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

		
		if ($resSelectControlledViaDeviceTemplate) {
			while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate'];
			}
		}

		if (count($childsDeviceTemplateArray)) {
			$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
		}
	}



	$queryDeviceTemplate = "select PK_DeviceTemplate ,Description from DeviceTemplate where 1=1 $whereClause order by Description asc";
	$resDeviceTemplate = $dbADO->_Execute($queryDeviceTemplate);
	$DeviceTemplates = array();
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$DeviceTemplates[].=$row['PK_DeviceTemplate'];
		}
	}
	
	return $DeviceTemplates;
}

function getProperties($primaryKey,$table,$properties,$field,$dbADO)
{
	$propArray=explode(',',$properties);
	$res=$dbADO->Execute("SELECT $properties FROM $table WHERE $field='$primaryKey'");
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row;
	}else{
		return null;
	}
}

function displayRemotes($mdID,$dbADO)
{
	$out='<B>Infrared remote controls you will use:</B> ';
	$remotes=array();
	
	$DTarray=getDeviceTemplatesFromCategory($GLOBALS['RemoteControlls'],$dbADO);
	if(count($DTarray==0)){
		$DTarray[]=0;
	}
	
	$res=$dbADO->Execute('SELECT PK_Device,Description FROM Device WHERE FK_Device_ControlledVia=? AND FK_DeviceTemplate IN ('.join(',',$DTarray).')',array($mdID));
	while($row=$res->fetchRow()){
		$remotes[$row['PK_Device']]=$row['Description'];
	}
	$delLinks='';
	foreach ($remotes AS $rid=>$description){
		$delLinks.='<a href="javascript:if(confirm(\'Are you sure you want to delete this remote?\'))self.location=\'index.php?section=avWizard&type=media_directors&action=del&delRemote='.$rid.'\';">'.$description.'</a>, ';
	}
	$out.=substr($delLinks,0,-2).' <input type="button" class="button" name="button" value="Add Remote" onClick="document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=mediaDirectors&categoryID='.$GLOBALS['RemoteControlls'].'&parmToKeep='.urlencode('mdID='.$mdID).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');">';
	
	return $out;
}

function processRemotes($dbADO)
{
	global $dbPlutoMainDatabase;
	$installationID=(int)$_SESSION['installationID'];
	$mdID=(int)@$_REQUEST['mdID'];
	
	unset($_SESSION['from']);
	$deviceTemplate=(int)@$_REQUEST['deviceTemplate'];
	if($deviceTemplate!=0 && $mdID!=0){
		$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID.' -C '.$mdID,$ret);
		setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
		$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
		exec($commandToSend);
	}
	
	if(isset($_REQUEST['delRemote']) && (int)$_REQUEST['delRemote']!=0){
		deleteDevice((int)$_REQUEST['delRemote'],$dbADO);
	}
}

function getArrayFromTable($tableName,$primary,$secundary,$dbADO,$filter='',$orderBy='')
{
	$res=$dbADO->execute("SELECT $primary,$secundary FROM $tableName $filter $orderBy");
	$result=array();
	while($row=$res->Fetchrow()){
		$result[$row[$primary]]=$row[$secundary];
	}
	
	return $result;
}

function getFieldsAsArray($tableName,$fields,$dbADO,$filter='',$orderBy='')
{
	$fieldsArray=explode(',',$fields);
	$res=$dbADO->execute("SELECT $fields FROM $tableName $filter $orderBy");
	$result=array();
	while($row=$res->Fetchrow()){
		foreach ($fieldsArray AS $field){
			$result[$field][]=$row[$field];
		}
	}
	
	return $result;	
}

function displayReceivers($mdID,$dbADO)
{
	$out='<B>Infrared Receivers</B> ';

	unset($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray']=array();
	getDeviceCategoryChildsArray($GLOBALS['InfraredReceivers'],$dbADO);
	$categArray=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$categArray[]=$GLOBALS['InfraredReceivers'];
	
	$templateArray=getArrayFromTable('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_DeviceCategory IN ('.join(',',$categArray).')','ORDER BY Description ASC');
	
	if(count($templateArray)==0)
		$templateArray[]=0;
	$receiverArray=getFieldsAsArray('Device','PK_Device,FK_DeviceTemplate',$dbADO,'WHERE FK_Device_ControlledVia='.$mdID.' AND FK_DeviceTemplate IN ('.join(',',array_keys($templateArray)).')');
	if(count($receiverArray)==0)
		$selectedReceiver=0;
	else
		$selectedReceiver=$receiverArray['PK_Device'][0];
	$out.='
		<input type="hidden" name="oldReceiverDT_'.$mdID.'" value="'.@$receiverArray['FK_DeviceTemplate'][0].'">
		<input type="hidden" name="oldReceiver_'.$mdID.'" value="'.$selectedReceiver.'">';
		
	$out.=pulldownFromArray($templateArray,'receiver_'.$mdID,@$receiverArray['FK_DeviceTemplate'][0]);
	
	return $out;
}

function processReceiver($mdID,$dbADO)
{
	$oldReceiver=(int)@$_POST['oldReceiver_'.$mdID];
	$oldReceiverDT=(int)@$_POST['oldReceiverDT_'.$mdID];
	$newReceiver=(int)@$_POST['receiver_'.$mdID];
	if($oldReceiver==0){
		if($newReceiver!=0){
			createDevice($newReceiver,$_SESSION['installationID'],$mdID,NULL,$dbADO,1);
		}
	}else{
		if($newReceiver==0){
			deleteDevice($oldReceiver,$dbADO);
		}elseif($oldReceiverDT!==$newReceiver){
			deleteDevice($oldReceiver,$dbADO);
			createDevice($newReceiver,$_SESSION['installationID'],$mdID,NULL,$dbADO,1);
		}
	}
}

function getLogName($deviceID,$dtID,$dtDescription)
{
	$logName=$deviceID.'_'.str_replace(array(' ','\''),array('_','_'),$dtDescription);

	$patern=array("/[ :+=()<]/","/[->*?\$\.%\/]/","/#/","/__/","/^_*/","/_*$/");
	$replacement =array("_","","","","","");
	$logName=preg_replace($patern,$replacement,$logName);

	// override log namimg rules for DCE router and orbiters
	$logName=($dtID==$GLOBALS['rootDCERouter'])?'DCERouter.newlog':$logName.'.newlog';

	$logName='/var/log/pluto/'.$logName;
	
	return $logName;
}

function processAudioSettings($deviceID,$dbADO)
{
	if(isset($_POST['audioSettings_'.$deviceID]) && $_POST['audioSettings_'.$deviceID]!='0'){
		$newAS=$_POST['audioSettings_'.$deviceID].@$_POST['ac3_'.$deviceID];
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newAS,$deviceID,$GLOBALS['AudioSettings']));
	}
}

function processVideoSettings($deviceID,$dbADO)
{
	$dbADO->debug=true;
	if(isset($_POST['videoSettings_'.$deviceID]) && $_POST['videoSettings_'.$deviceID]!='0'){
		$newVS=$_POST['videoSettings_'.$deviceID].'/'.$_POST['refresh_'.$deviceID];
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newVS,$deviceID,$GLOBALS['VideoSettings']));
	}
	$dbADO->debug=false;
}

function isCritical($deviceID)
{
	if(in_array($deviceID,$GLOBALS['CriticalDeviceTemplates'])){
		return true;
	}
	else{
		return false;
	}
}

function formatDeviceData($deviceID,$DeviceDataArray,$dbADO,$isIPBased=0)
{
	$deviceDataBox='';

	foreach ($DeviceDataArray AS $rowDDforDevice){
		
		if(!in_array($rowDDforDevice['FK_DeviceData'],$GLOBALS['DeviceDataToDisplay']))
			$GLOBALS['DeviceDataToDisplay'][]=$rowDDforDevice['FK_DeviceData'];

		$ddValue=$rowDDforDevice['IK_DeviceData'];

		if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='')){
			$deviceDataBox.='<b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$rowDDforDevice['dd_Description']).'</b> '.((@$rowDDforDevice['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowDDforDevice['Tooltip'].'" border="0" align="middle"> ':'');
			switch($rowDDforDevice['typeParam']){
				case 'int':
				if(in_array($rowDDforDevice['dd_Description'],$GLOBALS['DeviceDataLinkedToTables'])){
					$tableName=str_replace('PK_','',$rowDDforDevice['dd_Description']);
					$filterQuery='';
					switch($tableName){
						case 'Device':
						$filterQuery=" WHERE FK_Installation='".$installationID."'";
						break;
						case 'FloorplanObjectType':
						$filterQuery=" WHERE FK_FloorplanType='".@$specificFloorplanType."'";
					}

					$queryTable="SELECT * FROM $tableName $filterQuery ORDER BY Description ASC";
					$resTable=$dbADO->Execute($queryTable);
					$deviceDataBox.='<select name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
												<option value="0"></option>';
					while($rowTable=$resTable->FetchRow()){
						$itemStyle=($tableName=='FloorplanObjectType' && is_null(@$rowTable['FK_DesignObj_Control']))?' style="background-color:red;"':'';
						$deviceDataBox.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').' '.$itemStyle.'>'.$rowTable['dd_Description'].'</option>';
					}
					$deviceDataBox.='</select>';
				}
				else
					$deviceDataBox.='<input type="text" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
				break;
				case 'bool':
					$deviceDataBox.='<input type="checkbox" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
				break;
				default:
				if($rowDDforDevice['FK_DeviceData']==$GLOBALS['Port']){
					$deviceDataBox.=serialPortsPulldown('deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'],@$ddValue,$rowDDforDevice['AllowedToModify'],getTopLevelParent($deviceID,$dbADO));
				}else{
					$deviceDataBox.='<input type="text" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
				}
			}

			$deviceDataBox.='
							<input type="hidden" name="oldDeviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.$ddValue.'">';					
			unset($ddValue);
			$deviceDataBox.='<br>';
		}
	}
	if($isIPBased==1){
		$deviceDataBox.='<B>IP</B> <input type="text" name="ip_'.$deviceID.'" value="'.$rowD['IPaddress'].'"><br>
								<B>MAC</B> <input type="text" name="mac_'.$deviceID.'" value="'.$rowD['MACaddress'].'">';
	}
	
	return $deviceDataBox;
}

function getPipes($deviceID,$dbADO){
	$pipes=array();
	$res=$dbADO->Execute('
		SELECT 
			Device_Device_Pipe.*,
			Device.Description,
			Cin.Description AS input,
			Cout.Description AS output
		FROM Device_Device_Pipe
		INNER JOIN Device ON FK_Device_To=PK_Device 
		LEFT JOIN Command Cin ON Device_Device_Pipe.FK_Command_Input=Cin.PK_Command
		LEFT JOIN Command Cout ON Device_Device_Pipe.FK_Command_Output=Cout.PK_Command
		WHERE FK_Device_From=?',$deviceID);
	while($row=$res->FetchRow()){
		$pipes[$row['FK_Pipe']]['to']=$row['Description'];
		$pipes[$row['FK_Pipe']]['input']=($row['input']!='')?$row['input']:'none';
		$pipes[$row['FK_Pipe']]['output']=($row['output']!='')?$row['output']:'none';
	}
	
	return $pipes;
}

function getmicrotime() 
{ 
    list($usec, $sec) = explode(" ", microtime()); 
    return ((float)$usec + (float)$sec); 
}
?>