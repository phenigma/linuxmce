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
	<br><br>Pluto heartily embraces the open source community.  All of Pluto\'s software with source code is available for download under GPL or Pluto Licenses.
	Open Source is a key component of our corporate strategy, so we have done everything possible to make Pluto a comfortable, extensible platform for open source developers.
	We have a class generator that you can use to build your own DCE Device or plug-in in a matter of minutes.  It builds a complete, cross-platform project ready to 
	run on both Windows and Linux, including documentation and stubs for you to implement.  Plus we have our own in-house programmers available through our live help to
	provide instant assistance.  Additionally our <a href="http://plutohome.com/support">support site</a> includes forums, bug tracking and extensive documentation both for Pluto\'s own modules, as well as any modules 
	that you may develop that you would like Pluto to host.  We have a sophisticated build system that allows us to target different platforms and operating systems 
	simultaneously.<br><br>
	
	<a href="index.php?section=letter">What is the open source strategy?</a><br><br>
	
    <p>Pluto Series 2 was a re-written from scratch to make it conducive to an open source project.  All the code is broken into small plug-ins, and the heart--the DCERouter--is 
    now a general purpose message router.  This means that open source programmers are free to not only add to Pluto, but also use replacement versions for all of Pluto\'s modules.
    
    <p>We also now maintain 3 separate repositories of all software so that we can offer a stable retail product and simultaneously an active development community.</p>
    <p><b>Development</b> is the current development version. Any developers are free to make any contributions to this version, which is unrestricted. Once a developer feels that his plug-in, DCE module, or other changes are ready for release to the general public, an in-house maintainer will be assigned, who will confirm that the documentation and basic functionality are adequate. Pluto&rsquo;s in-house developers may offer to provide assistance.</p>
    <p><b>Testing</b> will be the version our in-house quality assurance team and beta testers use.</p>
    <p><b>Release</b> means the software is certified and has past all tests. All the Pluto systems sold will be based on this repository.</p>
    <p>Of course, anyone is free to use any repository, and can mix modules from other modules.</p>
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
 		$output->setTitle(APPLICATION_NAME."::I'm an Open Source Developer");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
