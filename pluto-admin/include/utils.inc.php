<?php 

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
			<td title="'.$row['Path'].'" align="left">'.(($row['Missing']!=0)?'<img src="include/images/missing.gif" align="top"> ':'').'<b>Filename:</b> <a href="index.php?section=mainMediaFilesSync&path='.$row['Path'].'&filename='.urlencode($row['Filename']).'">'.$row['Filename'].'</a><br><B>Path:</B> '.$row['Path'].'</td>
			<td rowspan="2"><a href="#" onClick="self.location=\'index.php?section=mainMediaBrowser&attributeID='.$GLOBALS['attributeID'].'&action=properties&fileID='.$row['PK_File'].'\';">Delete</a></td>
		</tr>
		<tr style="background-color:'.(($art_index%2==0)?'#EEEEEE':'#EBEFF9').';">
			<td align="left">'.$otherAttributes.'</td>
		</tr>
		';
	return $out;
}

?>