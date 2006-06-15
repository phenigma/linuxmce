<?
session_name('Pluto');
session_start();

include($_SERVER['DOCUMENT_ROOT'].'/support/include/config/config.inc.php');
require($_SERVER['DOCUMENT_ROOT'].'/support/include/utils.inc.php');
require($_SERVER['DOCUMENT_ROOT'].'/support/include/masterusers.inc.php');

// autologin check: if cookie is set grab the user's data from database
if (@$_SESSION['userIsLogged']!="yes"){
	//print_r($_COOKIE);
	if(isset($_COOKIE['PlutoHomeAutoLogin'])){
		parse_str(base64_decode($_COOKIE['PlutoHomeAutoLogin']));
		$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=&sqlCvsAdmin=');
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
			$_SESSION['sqlCvsAdmin']=@$sqlCvsAdmin;
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
    <td width="99"><a href="'.$PlutoHomeHost.'index.php?section=logout"><img src="'.$imagesPath.'buton_logout.jpg" width="99" height="36" border="0"/></a></td>
  </tr>
</table>';

$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
if($package!=0)
	$menuPages = buildHeaderTopMenu(2,$dbADO);
else
	$menuPages = buildHeaderTopMenu(3,$dbADO);
?>

<link rel='stylesheet' type='text/css' href='<?=$serverPath?>include/styles/plutoSupport.css'>

<html>
<HEAD></HEAD>
<BODY>
<table width="100%" border="1" cellspacing="0" cellpadding="0">
  <tr>
    <td background="<?=$imagesPath?>back_header.jpg"><table width="900"  border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td width="225" valign="top"><table width="225"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="124" valign="top"><img src="<?=$imagesPath?>live_support.jpg" width="124" height="36" /></td>
                <td width="102" valign="top"><script language="JavaScript" src="http://plutohome.com/phplive/js/status_image.php?base_url=http://plutohome.com/phplive&l=radu&x=1&deptid=4&"></script></td>
              </tr>
          </table></td>
          <td width="31" valign="top"><img src="<?=$imagesPath?>header_colt_1.jpg" width="31" height="36" /></td>
          <td><form name="form2" method="post" action="<?=$PlutoHomeHost?>index.php?section=search"><table width="226"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="132" background="<?=$imagesPath?>back_search.jpg"><input type="text" name="cuvant" value="" class="search"></td>
                <td width="94" background="<?=$imagesPath?>back_search.jpg"><input type="image" src="<?=$imagesPath?>buton_search.jpg" name="search""></td></form>
              </tr>
          </table></td>
          <td width="239" valign="top"><?=(@$_SESSION['userIsLogged']!='yes')?$login:$logout;?></td>
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
  	<td><table width="100%" style="background-color:#4e6ca6;z-index:100;" cellpadding="0" cellspacing="0">
		<td height="25">
		<table>
			<tr>
				<td><a href="/wiki/" class="white_link">Documentation</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=mail" class="white_link">Mailing lists</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=mainDownload&package=0" class="white_link">Download</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=mantis" class="white_link">Bug reports</a></td>
				<td class="white_link">|</td>
				<td><a href="../doxygen/" class="white_link">Source Code Docs</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=versions&package=0" class="white_link">Change Logs</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=authors" class="white_link">Authors</a></td>
				<td class="white_link">|</td>
				<td><a href="../index.php?section=license&package=0" class="white_link">License</a></td>
				<td class="white_link">|</td>				
				<td><a href="../index.php?section=help" class="white_link">Help</a></td>
				<td class="white_link">|</td>				
			</tr>
		</table>
		</td>
		<td width="60" align="right">&nbsp;</td>
	  <tr>
	  </table>
	  </td>
	</tr>  
 </table>
 <?
 
 function getHeaderTopMenuOld($website,$dbADO,$package=0) {
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

// this menu use absolute or relative links
function buildHeaderTopMenu($website,$dbADO)
{
	global $relativePath;
	$menuPages='';
		
	// hard coded menu items	
	$menuPages.='menuObject.item0 = "Projects";
	menuObject.item0_0 = "Summary of all projects";
	menuObject.url0_0 = "'.$relativePath.'index.php?section=modules&package=0";
	menuObject.item0_1 = "Pluto Home main page";
	menuObject.url0_1 = "'.$relativePath.'index.php?section=home&package=0";
	menuObject.item0_2 = "Pluto VIP main page";
	menuObject.url0_2 = "'.$relativePath.'index.php?section=home&package=159";
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
			menuObject.url0_'.$firstPos.' = "'.$relativePath.'index.php?section=home&package='.$row['PK_Package'].'";
		';		
		$firstPos++;
	}

	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website='$website' ORDER BY OrderNum ASC";
	$resSelectMenu = $dbADO->Execute($selectMenu);
	$pos=1;	
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='menuObject.item'.$pos.' = "'.ReplaceTokens($rowSelectMenu['Description']).'"
		';
		if($rowSelectMenu['pageURL']!=''){
			$itemUrl=(strpos($rowSelectMenu['pageURL'],'http://')===false)?$relativePath.ReplaceTokens($rowSelectMenu['pageURL']):ReplaceTokens($rowSelectMenu['pageURL']);
			$menuPages.='menuObject.url'.$pos.' = "'.str_replace('\'','\\\\\'',$itemUrl).'"
		';
		}
		$menuPages.=getHeaderSubmenu($website,$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
	}
	return $menuPages;
}

function getHeaderSubmenu($website,$level,$parentID,$dbADO)
{
	global $relativePath;
	$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent =? AND showInTopMenu = 1 AND Website=? ORDER BY OrderNum ASC";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($parentID,$website));
	$menuPages='';
	$pos=0;	
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='
			menuObject.item'.$level.$pos.' = "'.ReplaceTokens($rowSelectMenu['Description']).'"
		';
		if($rowSelectMenu['pageURL']!=''){
			$itemUrl=(strpos($rowSelectMenu['pageURL'],'http://')===false)?$relativePath.ReplaceTokens($rowSelectMenu['pageURL']):ReplaceTokens($rowSelectMenu['pageURL']);
			$menuPages.='menuObject.url'.$level.$pos.' = "'.str_replace('\'','\\\\\'',$itemUrl).'"
		';
		}
		$menuPages.=getHeaderSubmenu($website,$level.$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
		
	}
	return $menuPages;
}

?>
