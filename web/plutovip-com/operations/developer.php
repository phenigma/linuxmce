<?php
 function developer($output) {
		$out = '';
		
$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/how_to_buy_txt.gif" width="82" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=get_pluto"><img src="images/submenus/How%20to%20buy/pluto_for_my_home_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dealer"><img src="images/submenus/How%20to%20buy/a_dealer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manufacturer"><img src="images/submenus/How%20to%20buy/a_manufacturer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=developer"><img src="images/submenus/How%20to%20buy/open_source_developer_on.gif" width="200" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/open_source_developer_txt.gif" width="213" height="13"><br>
 <br><br>Read the reason behind Pluto\'s decision to release it\'s product as open source <a href="index.php?section=letter">here</a><br><br>On November 22nd, 2004, when Pluto 2 is ready for beta testing, all the software and source code will be available for free under GPL and Pluto licenses.</p>
    <p>Pluto&rsquo;s in-house engineers will continue aggressive development as always, and look forward to working with the open source community. As an open source project, developers will be completely autonomous, free to break-off from the Pluto corporate version. However, as a company actively promoting this product to non-technical, mainstream consumers, it is essential that the released version be completely stable, fully functional, and well documented. Our customers buy Pluto because of the lifestyle, the simplicity Pluto brings. To maintain this reputation, the released version must go through our exhaustive quality assurance process.  If you have questions or require assistance, please fill out <a href="index.php?section=contact">this form</a>.</p>
    <p>Therefore Pluto will maintain 3 repositories of the software.</p>
    <p><b>Development</b> will be the current development version. Any developers are free to make any contributions to this version, which is unrestricted. Once a developer feels that his plug-in, DCE module, or other changes are ready for release to the general public, an in-house maintainer will be assigned, who will confirm that the documentation and basic functionality are adequate. Pluto&rsquo;s in-house developers may offer to provide assistance.</p>
    <p><b>Testing</b> will be the version our in-house quality assurance team and beta testers use.</p>
    <p><b>Release</b> means the software is certified and has past all tests. All the Pluto systems sold will be based on this repository.</p>
    <p>Of course, anyone is free to use any repository, and can mix modules from other modules.</p>
    <p>Version 2 will not significantly change the features or usage, but it required a complete re-write to make Pluto more conducive to an open source project. DCE has been separated from Pluto&rsquo;s proprietary code so that DCE is useful as a stand-alone project. The Pluto Core software is now a series of plug-ins for a generic DCE router. Also, version 2 incorporates a new plug-in architecture so developers can add to Pluto, or replace existing modules with their own. The initial Pluto 2 release, targeted for the end of October, 2004, will include the following projects developed in-house at Pluto. There are many additional DCE devices for various pieces of hardware that will be released as they are converted to the new plug-in architecture.</p>
    <p><strong><font color="#009934">Core projects:</font></strong></p>
    <p>These are the principal programs in the Pluto system.  Note that the Pluto license is essentially the same as GPL, but requires a license when a manufacturer or dealer wants to sell the Pluto platform along with hardware.  For software developers, the same rights and restrictions apply as with GPL.  The full text of the license will be available when version 2 is released.  All DCE devices, libraries and utilities are GPL.</p>';
    $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=1
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><font color="#009934">'.$row->Description.'</font></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }

$out.='
    <p><strong><font color="#009934">Libraries:</font></strong></p>
    <p>These are common libraries which are used throughout all the Pluto projects. We have tried to break them into separate classes so that they are useful to programmers outside the scope of Pluto as well.</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=2
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><font color="#009934">'.$row->Description.'</font></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
         
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }
 $out.='<p><strong><font color="#009934">Utilities:</font></strong></p>
    <p>Various utilities, some specific to Pluto, some general purpose</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=3
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><font color="#009934">'.$row->Description.'</font></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
      
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }
 $out.='<p><strong><font color="#009934">DCE Devices and Plug-ins:</font></strong></p>
    <p>A DCE device exposes Data, Commands and Events, and is able to connect to the DCE Router,. DCE allows any device to work seamlessly with all other devices, and be controlled from any Orbiter. You can create your own DCE device in as little as 60 seconds! Just visit xxxx, specify what data your device needs, what commands you want to implement, and what events you fire. The wizard will automatically build you a cross-platform project complete with a gcc-compatible Makefile, and a Visual C++ .NET project file. Just compile, run and start adding your own implementation. See the DCE Router project for details.</p>
    <p>Following are the DCE devices that will be ready for release in October. Less commonly used DCE devices will be released later.</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=4
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><font color="#009934">'.$row->Description.'</font></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
         
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }
  $out.='</tr>
</table>';
	

		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(5); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::I'm a Open Source Developer");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
