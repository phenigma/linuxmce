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
		//echo $resSelOwner->sql;
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
				if (is_dir ($path . "/" . $f)) {
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


function grabFiles($path) {
	$filesArray=array();
	if (($d = @opendir ($path)) === false) 
		return $filesArray;
	else {
		while ($f = readdir ($d)) {
			if ($f != "." && $f != "..") {
				if (is_file ($path . "/" . $f)) {
					$filesArray[]= $f;
				} 
			}
		}
		closedir ($d);
	}
	return $filesArray;
}


function resizeImage($source, $destination, $new_width, $new_height)
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

	$dst_img=imagecreatetruecolor($new_w,$new_h);
	$src_img=ImageCreateFromJpeg($source);
	if(!@imagecopyresized($dst_img,$src_img,0,0,$src_x,$src_y,$new_w,$new_h,$src_w,$src_h))
		return 3; // resize error
	if(!@imagejpeg($dst_img, $destination, 100))
		return 4; // writing thumbnail error
	else
		return true;
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
	
		$arrayFKDeviceTables=array('CommandGroup_Command','Device_Command','Device_CommandGroup','Device_DeviceData','Device_DeviceGroup','Device_Device_Related','Device_EntertainArea','Device_HouseMode','Device_Orbiter','Device_StartupScript','Device_Users','InfraredGroup_Command');
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

function pickDeviceTemplate($categoryID, $boolManufacturer,$boolCategory,$boolDeviceTemplate,$returnValue,$defaultAll,$section,$firstColText,$dbADO,$useframes=0)
{
	$out='';
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

		if ($manufOrDevice || $defaultAll==1)	{
		
			$queryModels = "SELECT DeviceTemplate.*,DeviceCategory.Description as deviceDescription,
									Manufacturer.Description as manufacturerDescription 
							FROM DeviceTemplate INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
								  				  INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer
							WHERE 1=1 $where ORDER BY Description";	

			$rs = $dbADO->_Execute($queryModels);
			
			$arJsPos=0;
				while ($row = $rs->FetchRow()) {
					
					$selectModels.="<option ".($selectedModel==$row['PK_DeviceTemplate']?" selected ":"")." value='{$row['PK_DeviceTemplate']}'>{$row['Description']}</option>";
					
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
		
		<script>
		function windowOpen(locationA,attributes) {
			window.open(locationA,'',attributes);
		}
		</script>
		
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
		<form action="index" method="POST" name="'.$section.'">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="actionX" value="add">
		<input type="hidden" name="deviceCategSelected" value="'.$selectedDeviceCateg.'">
		<input type="hidden" name="deviceSelected" value="'.$selectedDevice.'">
		<input type="hidden" name="modelSelected" value="'.$selectedModel.'">
		
		<table cellpadding="5" cellspacing="0" border="0" align="center">
				<input type="hidden" name="selectedField" value="" />
					<tr>
						<th width="25%">Manufacturers</th>
						<th width="25%">Device Category</th>
						<th width="25%">
						Models
						
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
							<input type="submit" name="addManufacturer" value="Add" />';
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
							<input type="submit" name="addDeviceCategory" value="Add '.($selectedDevice==0?' Top Level Child':' Child').'" />
							'.($selectedDevice!=0?'<input type="button" name="editDeviceCategory" value="Edit" onClick="javascript: windowOpen(\'index.php?section=editDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=600,height=250,toolbars=true\');" />':'<input type="submit" name="editDeviceCategory" value="Edit" disabled="disabled" />').'';
							getDeviceCategoryChildsNo($selectedDevice,$dbADO);					
							$childsToDelete = $GLOBALS['childsDeviceCategoryNo'];
							$out.='
							&nbsp;&nbsp;'.($selectedDevice!=0?'<input type="button" name="deteleDeviceCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) windowOpen(\'index.php?section=deleteDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=100,height=100,toolbars=0\');" />':'<input type="submit" name="deleteDeviceCategory" value="Delete" disabled="disabled" />');
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
									<option value="" selected="selected">All</option>
									'.$selectModels.'	
							</select>';
							if($returnValue==0){
								$out.='<input type="button" name="edit_DeviceTemplate" value="Edit" onClick="javascript:checkEdit(this.form);" />';
							}else{
								$out.='<br><input type="button" name="pickDT" value="Pick Device Template" onClick="opener.location=\'index.php?section='.$_SESSION['from'].'&deviceTemplate=\'+document.'.$section.'.model[document.'.$section.'.model.selectedIndex].value+\'&action=add&add=1\';self.close();" />';
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
				if($boolDeviceTemplate==1){
					
					$out.='	<b>New:</b>
							<input type="text" name="DeviceTemplate_Description" size="15"'.$disabledAdd.' />
							<input type="submit" name="addDeviceTemplate" value="Add"'. $disabledAdd .' />';
				}
				$out.='
						</td>				
					</tr>
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
			 			 			 		
				}
			 }	
			 	 
			 //add new master device list
			 if (strstr($addDeviceTemplate,'Add')) {
			 	
			 	if ($DeviceTemplate_Desc!='') {
				 		
			 		if ($deviceSelected!=0 && $manufacturerSelected!=0) {	 			
			 			$queryInsertMasterDevice = 'INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer) values(?,?,?)';
			 			$res = $dbADO->Execute($queryInsertMasterDevice,array($DeviceTemplate_Desc,$deviceSelected,$manufacturerSelected));	 			
			 		}	 			 		
			 	}
			 }
			 
			 // add new manufacturer
			 if (strstr($addManufacturer,'Add')){
			 	if ($Manufacturer_Description!='') {
		 			$queryInsertManufacturer = 'INSERT INTO Manufacturer (Description) values(?)';
		 			$res = $dbADO->Execute($queryInsertManufacturer,array($Manufacturer_Description));	 			
		 		}	 			 		
			 }
			 header("Location: index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&justAddedNode=$justAddedNode");
		}else{
			header("Location: index.php?section=$section&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&error=Please login if you want to change device templates.");
		}
	}
	return $out;
}

function builtTopMenu($website,$dbADO)
{
	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website' ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu);
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
	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent =? AND showInTopMenu = 1 AND Website=? ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($parentID,$website));
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

function generatePullDown($name,$tableName,$valueField,$labelField,$selectedValue,$dbADO,$filterTable='')
{
	$pullDown='
		<select name="'.$name.'">	
			<option value="0">- Please select -</option>';
	$query="SELECT $valueField,$labelField FROM $tableName $filterTable ORDER BY $labelField ASC";
	$res=$dbADO->Execute($query);
	while($row=$res->FetchRow()){
		$pullDown.='<option value="'.$row[$valueField].'" '.(($row[$valueField]==$selectedValue)?'selected':'').'>'.$row[$labelField].'</option>';
	}
	$pullDown.='</select>';
	return $pullDown;
}

function getDeviceTemplatesFromCategory($categoryID,$dbADO)
{
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
?>