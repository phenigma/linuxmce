<?
session_start('Pluto');

require($_SERVER['DOCUMENT_ROOT'].'/support/include/config/config.inc.php');
require($_SERVER['DOCUMENT_ROOT'].'/support/include/utils.inc.php');
require($_SERVER['DOCUMENT_ROOT'].'/support/include/masterusers.inc.php');

// autologin check: if cookie is set grab the user's data from database
if (@$_SESSION['userIsLogged']!="yes"){
	//print_r($_COOKIE);
	if(isset($_COOKIE['PlutoHomeAutoLogin'])){
		parse_str(base64_decode($_COOKIE['PlutoHomeAutoLogin']));
		$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
		if($isMasterUsers[0]){
			parse_str($isMasterUsers[1]);
			$_SESSION['userID'] = $MasterUsersID;
			$_SESSION['PlutoId'] = $PlutoId;
			$_SESSION['Pin'] = $Pin;
			$_SESSION['username'] = $username;
			$_SESSION['userIsLogged']="yes";
			$_SESSION['categ']=$FirstAccount;
			$_SESSION['Email']=$Email;
			$_SESSION['extPassword']=$extPassword;
		}
	}
}
// end autologin check

$serverPath=$PlutoSupportHost;
$imagesPath=$serverPath.'include/images/';
$supportPath=$PlutoSupportHost;
$relativePath=(isset($overridePath))?$overridePath:'../';		// for the directories under support

$login='<table width="239"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="73"><img src="'.$imagesPath.'buton_home.jpg" width="73" height="36" /></td>
                <td width="31"><img src="'.$imagesPath.'header_colt_1.jpg" width="31" height="36" /></td>
                <td width="135"><a href="'.$PlutoHomeHost.'index.php?section=clientLoginForm" target="_top"><img src="'.$imagesPath.'buton_login_register.jpg" width="135" height="36" border="0"/></a></td>
              </tr>
          </table>';

$logout='<table width="214" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="115"><a href="'.$PlutoHomeHost.'index.php?section=myPluto"><img src="'.$imagesPath.'buton_my_pluto.jpg" width="115" height="36" border="0"/></a></td>
    <td width="99"><a href="'.$relativePath.'index.php?section=logout"><img src="'.$imagesPath.'buton_logout.jpg" width="99" height="36" border="0"/></a></td>
  </tr>
</table>';

$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
if($package!=0)
	$menuPages = getHeaderTopMenu(2,$dbADO);
else
	$menuPages = getHeaderTopMenu(3,$dbADO);
$topMenu='
	<SCRIPT LANGUAGE="JavaScript">
		    <!--
			'.
			$menuPages
			.'		    
		    Build();
		    --> 
	</script>';
?>
<link rel='stylesheet' type='text/css' href='<?=$serverPath?>include/styles/plutoSupport.css'>
<script language='javascript' src='<?=$serverPath?>Config.js'></script>
<script language='javascript' src='<?=$serverPath?>Menu.js'></script>

<html>
<HEAD></HEAD>
<BODY>
<table width="100%" border="1" cellspacing="0" cellpadding="0">
  <tr>
    <td background="<?=$imagesPath?>back_header.jpg"><table width="900"  border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td width="225"><table width="225"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="124"><img src="<?=$imagesPath?>live_support.jpg" width="124" height="36" /></td>
                <td width="102" valign="top"><!-- BEGIN PHP Live! code, (c) OSI Codes Inc. -->
<script language="JavaScript" src="http://plutohome.com/phplive/js/status_image.php?base_url=http://plutohome.com/phplive&l=radu&x=1&deptid=0&btn=1"><a href=""></a></script>
<!-- END PHP Live! code : (c) OSI Codes Inc. --></td>
              </tr>
          </table></td>
          <td width="31"><img src="<?=$imagesPath?>header_colt_1.jpg" width="31" height="36" /></td>
          <td><form name="form2" method="post" action="<?=$PlutoHomeHost?>index.php?section=search"><table width="226"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="132" background="<?=$imagesPath?>back_search.jpg"><input type="text" name="cuvant" value="" class="search"></td>
                <td width="94" background="<?=$imagesPath?>back_search.jpg"><input type="image" src="<?=$imagesPath?>buton_search.jpg" name="search""></td></form>
              </tr>
          </table></td>
          <td width="239"><?=(@$_SESSION['userIsLogged']!='yes')?$login:$logout;?></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td background="<?=$imagesPath?>back_project_1.jpg"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td width="400">
          <table width="400" border="0" cellpadding="0" cellspacing="0" background="<?=$imagesPath?>back_project_2.jpg">
              <tr>
                <td width="5"><img src="<?=$imagesPath?>back_project_spacer.jpg" width="5" height="100" /></td>
                <td valign="bottom" class="project_name"><?=@$_SESSION['packageName']?></td>
              </tr>
          </table></td>
          <td>&nbsp;</td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td style="background-image:url(<?=$imagesPath?>back_on.jpg);" valign="top" align="left" height="30"><table width="100%" cellpadding="0" cellspacing="0"><tr><td><span style="position:relative;">&nbsp;<?=$topMenu?></span></td><td align="right"><img src="<?=$imagesPath?>help.jpg" border="0"></td></tr></table>
  </tr>  
 </table>
 <?
 
 function getHeaderTopMenu($website,$dbADO,$package=0) {
 	global $relativePath;
	$dbADO->debug=false;
	$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	if($package!=0){
		$getPackage='SELECT * FROM Package WHERE PK_Package=?';
		$resPackage=$dbADO->Execute($getPackage,$package);
		$rowPackage=$resPackage->FetchRow();
		$_SESSION['packageName']=$rowPackage['Description'];
	}
	
		$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website'";
		$resSelectMenu = $dbADO->Execute($selectMenu);
		$menuPages='';	
/*		$menuPages='
				AddMenu("1"  ,  "1"   ,  "Wizard"       ,  ""  ,  ""  , "");
			';
*/		
		while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
			$menuUrl=(strpos($rowSelectMenu['pageURL'],'http://')===false)?$relativePath.ReplaceTokens($rowSelectMenu['pageURL']):ReplaceTokens($rowSelectMenu['pageURL']);
			$menuPages.='
				AddMenu("'.$rowSelectMenu['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.ReplaceTokens($rowSelectMenu['Description']).'"       ,  ""  ,  ""  , "'.$menuUrl.'");
			';
			//get childs (ne-recursive method)
			$selectSubMenu1 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectSubMenu1 = $dbADO->Execute($selectSubMenu1);
			while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {
				//get level 2 childs (ne-recursive method)
					$menuUrl=(strpos($rowSelectSubMenu1['pageURL'],'http://')===false)?$relativePath.ReplaceTokens($rowSelectSubMenu1['pageURL']):ReplaceTokens($rowSelectSubMenu1['pageURL']);
					$selectSubMenu2 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";
					$resSelectSubMenu2 = $dbADO->Execute($selectSubMenu2);
					
				$menuPages.='
					AddMenu("'.$rowSelectSubMenu1['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.ReplaceTokens($rowSelectSubMenu1['Description']).($resSelectSubMenu2->RecordCount()>0?" &gt;&gt;":"").'"       ,  ""  ,  ""  , "'.$menuUrl.'");
				';
				
					
					while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
						$menuUrl=(strpos($rowSelectSubMenu2['pageURL'],'http://')===false)?$relativePath.ReplaceTokens($rowSelectSubMenu2['pageURL']):ReplaceTokens($rowSelectSubMenu2['pageURL']);
						$menuPages.='
							AddMenu("'.$rowSelectSubMenu2['PK_PageSetup'].'"  ,  "'.$rowSelectSubMenu1['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.ReplaceTokens($rowSelectSubMenu2['Description']).'"       ,  ""  ,  ""  , "'.$menuUrl.'");
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

 ?>
