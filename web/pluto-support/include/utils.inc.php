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

$GLOBALS['childsArray'] = array();

function getDocumentChildsArray($parentID,$dbADO) {
	$queryGP = "SELECT * FROM Document WHERE  FK_Document_Parent = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsArray'][]=$row['PK_Document'];
				$GLOBALS['childsArray'][]=getDocumentChildsArray($row['PK_Document'],$dbADO);
		}
	}
	//return $childsArray;
}


$GLOBALS['childsDocumentNo']=0;
function getDocumentChildsNo($parentID,$dbADO) {
	$queryGP = "select * from Document where FK_Document_Parent = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsDocumentNo']++;
				$GLOBALS['childsDocumentNo']+=getDocumentChildsNo($row['PK_Document'],$dbADO);
		}
	}	
}

function ReplaceTokens($input) {
	$input = str_replace('**package**',@$_SESSION['packageName'],$input);
	$input = str_replace('**pkg_id**',@$_SESSION['package'],$input);
	$input = str_replace('**un**',@$_SESSION['username'],$input);
	$input = str_replace('**pw**',@$_SESSION['extPassword'],$input);
	return $input;
}

function getTopMenu($website,$dbADO,$package=0) {
	$dbADO->debug=false;
	
		$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website'";
		$resSelectMenu = $dbADO->Execute($selectMenu);
		$menuPages='';	
/*		$menuPages='
				AddMenu("1"  ,  "1"   ,  "Wizard"       ,  ""  ,  ""  , "");
			';
*/		
		while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
			$menuUrl=(strpos($rowSelectMenu['pageURL'],'http://')===false)?ReplaceTokens($rowSelectMenu['pageURL']):ReplaceTokens($rowSelectMenu['pageURL']);
			$menuPages.='
				AddMenu("'.$rowSelectMenu['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "'.ReplaceTokens($rowSelectMenu['Description']).'"       ,  ""  ,  ""  , "'.$menuUrl.'");
			';
			//get childs (ne-recursive method)
			$selectSubMenu1 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectSubMenu1 = $dbADO->Execute($selectSubMenu1);
			while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {
				//get level 2 childs (ne-recursive method)
					$selectSubMenu2 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";
					$resSelectSubMenu2 = $dbADO->Execute($selectSubMenu2);
				$menuUrl=(strpos($rowSelectSubMenu1['pageURL'],'http://')===false)?ReplaceTokens($rowSelectSubMenu1['pageURL']):ReplaceTokens($rowSelectSubMenu1['pageURL']);	
				$menuPages.='
					AddMenu("'.$rowSelectSubMenu1['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "'.ReplaceTokens($rowSelectSubMenu1['Description']).($resSelectSubMenu2->RecordCount()>0?" &gt;&gt;":"").'"       ,  ""  ,  ""  , "'.$menuUrl.'");
				';
				
					
					while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
						$menuUrl=(strpos($rowSelectSubMenu2['pageURL'],'http://')===false)?ReplaceTokens($rowSelectSubMenu2['pageURL']):ReplaceTokens($rowSelectSubMenu2['pageURL']);	
						$menuPages.='
							AddMenu("'.$rowSelectSubMenu2['PK_PageSetup'].'"  ,  "'.$rowSelectSubMenu1['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.ReplaceTokens($rowSelectSubMenu2['Description']).'"       ,  ""  ,  ""  , "'.ReplaceTokens($rowSelectSubMenu2['pageURL']).'");
						';
					}
					$resSelectSubMenu2->Close();		
			}
			$resSelectSubMenu1->Close();					
		}
		$resSelectMenu->Close();
		$dbADO->debug=false;
return $menuPages;
}



function dbQuery($query,$error='MySQL error')
{
	$res=mysql_query($query) or die($error.": ".mysql_error());
	return $res;
}

function userIsAdmin($userID)
{
	if(count($GLOBALS['userIsAdmin'])==0 || in_array($userID,$GLOBALS['userIsAdmin']))
		return true;
	return false;
}


$GLOBALS['parentsArray'] = array();

function getDocumentParentsArray($docID,$dbADO) {
	if($docID!=0){
		$queryParent='
			SELECT * FROM Document WHERE PK_Document=?';
		$resParent=$dbADO->Execute($queryParent,$docID);
		if($resParent){
			while($rowParent=$resParent->FetchRow()){
				$GLOBALS['parentsArray'][]=$rowParent['FK_Document_Parent'];
				$GLOBALS['parentsArray'][]=getDocumentParentsArray($rowParent['FK_Document_Parent'],$dbADO);
			}
		}
	}
}

function forumJump($redirectAddress)
{
	if(isset($_SESSION['userID']))
		return "index.php?section=forum&redirect=".str_replace('?','&',$redirectAddress);
	else
		return "phpbb2/$redirectAddress";
}

function builtTopMenu($website,$dbADO)
{
	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website' ORDER BY OrderNum ASC";
	$resSelectMenu = $dbADO->Execute($selectMenu);
	$menuPages='';
	$pos=1;
	// hard coded menu items	
	$menuPages.='menuObject.item0 = "Projects";
	menuObject.item0_0 = "Summary of all projects";
	menuObject.url0_0 = "index.php?section=modules&package=0";
	menuObject.item0_1 = "Pluto Home main page";
	menuObject.url0_1 = "index.php?section=home&package=0";
	menuObject.item0_2 = "Pluto VIP main page";
	menuObject.url0_2 = "index.php?section=home&package=159";
	menuObject.item0_3 = "<B>Pluto Core Programs</B>";
	';
	$queryModules = '
		SELECT PK_Package, Package.Description AS Pack, Comments, PackageType.Description as Type,FK_PackageType
		FROM Package
		LEFT JOIN PackageType ON FK_PackageType = PK_PackageType
		WHERE HomePage IS NOT NULL AND isSource=0 AND PK_Package!=159
		ORDER BY Type';
	$res=$dbADO->Execute($queryModules);
	$firstPos=4;
	$firstCateg=1;
	while($row=$res->FetchRow()){
		if($row['FK_PackageType']!=$firstCateg){
			$menuPages.='menuObject.item0_'.$firstPos.' = "<B>'.$row['Type'].'</B>";
			';
			$firstCateg=$row['FK_PackageType'];
			$firstPos++;
		}
		$menuPages.='
			menuObject.item0_'.$firstPos.' = "'.$row['Pack'].'";
			menuObject.url0_'.$firstPos.' = "index.php?section=home&package='.$row['PK_Package'].'";
		';		
		$firstPos++;
	}
	
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='menuObject.item'.$pos.' = "'.ReplaceTokens($rowSelectMenu['Description']).'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$pos.' = "'.str_replace('\'','\\\\\'',ReplaceTokens($rowSelectMenu['pageURL'])).'"
		';
		$menuPages.=getSubmenu($website,$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
	}
	return $menuPages;
}

function getSubmenu($website,$level,$parentID,$dbADO)
{
	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent =? AND showInTopMenu = 1 AND Website=? ORDER By OrderNum ASC";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($parentID,$website));
	$menuPages='';
	$pos=0;	
	if($resSelectMenu->RecordCount()>0)
		$menuPages.='menuObject.menu_items_background_color_roll'.$level.' = "#FFFFFF"
		';
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='
			menuObject.item'.$level.$pos.' = "'.ReplaceTokens($rowSelectMenu['Description']).'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$level.$pos.' = "'.str_replace('\'','\\\\\'',ReplaceTokens($rowSelectMenu['pageURL'])).'"
		';
		$menuPages.=getSubmenu($website,$level.$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
		
	}
	return $menuPages;
}
?>