<?php
 function developer($output,$conn) {
		$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="438"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="155"><a href="index.php?section=pluto_system_overview"><img src="images/buttons/pluto_system_overview_on.gif" width="155" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=how_the_system_works"><img src="images/buttons/how_the_system_works_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="128"><a href="index.php?section=learn_how_to_use_it"><img src="images/buttons/learn_how_to_use_it_off.gif" width="128" height="11" border="0" /></a></td>
        </tr>
    </table></td>
  </tr>
	<tr>
		<td class="insidetable2">
	Pluto heartily embraces the open source community.  All of Pluto\'s software with source code is available for download under GPL or Pluto Licenses.
	Open Source is a key component of our corporate strategy, so we have done everything possible to make Pluto a comfortable, extensible platform for open source developers.
	We have a class generator that you can use to build your own DCE Device or plug-in in a matter of minutes.  It builds a complete, cross-platform project ready to 
	run on both Windows and Linux, including documentation and stubs for you to implement.  Plus we have our own in-house programmers available through our live help to
	provide instant assistance.  Additionally our <a href="http://plutohome.com/support">support site</a> includes forums, bug tracking and extensive documentation both for Pluto\'s own modules, as well as any modules 
	that you may develop that you would like Pluto to host.  We have a sophisticated build system that allows us to target different platforms and operating systems 
	simultaneously.<br><br>
	
	<a href="index.php?section=letter">What is the open source strategy?</a> &nbsp;&nbsp; <a href="support/index.php?section=document&docID=181">Is the Pluto license OSD-approved?</a><br><br>
	
    <p>Pluto Series 2 was a re-written from scratch to make it conducive to an open source project.  All the code is broken into small plug-ins, and the heart - the DCERouter - is 
    now a general purpose message router.  This means that open source programmers are free to not only add to Pluto, but also use replacement versions for all of Pluto\'s modules.
    
    <p>We also now maintain 3 separate repositories of all software so that we can offer a stable retail product and simultaneously an active development community.</p>
    <p><b>Development</b> is the current development version. Any developers are free to make any contributions to this version, which is unrestricted. Once a developer feels that his plug-in, DCE module, or other changes are ready for release to the general public, an in-house maintainer will be assigned, who will confirm that the documentation and basic functionality are adequate. Pluto&rsquo;s in-house developers may offer to provide assistance.</p>
    <p><b>Testing</b> will be the version our in-house quality assurance team and beta testers use.</p>
    <p><b>Release</b> means the software is certified and has past all tests. All the Pluto systems sold will be based on this repository.</p>
    <p>Of course, anyone is free to use any repository, and can mix modules from other modules.</p>
    <p><strong><span class="title">Core projects:</span></strong></p>
    <p class="normaltext">These are the principal programs in the Pluto system.  Note that the Pluto license is essentially the same as GPL, but requires a license when a manufacturer or dealer wants to sell the Pluto platform along with hardware.  For software developers, the same rights and restrictions apply as with GPL.  The full text of the license will be available when version 2 is released.  All DCE devices, libraries and utilities are GPL.</p>
		</td>
	</tr>
</table>';

    $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=1
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql,$conn) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable2" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><span class="title"><B>'.$row->Description.'</B></span></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }

$out.='
    <p><strong><span class="title">Libraries:</span></strong></p>
    <p class="normaltext">These are common libraries which are used throughout all the Pluto projects. We have tried to break them into separate classes so that they are useful to programmers outside the scope of Pluto as well.</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=2
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql,$conn) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 
cellSpacing=0 borderColor=#808EA8 class="insidetable2" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr><td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td><td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td></tr>
         <tr><td align="center" width="25%"><span class="title"><B>'.$row->Description.'</B></span></td><td align="center" width="25%">'.$row->Language.'</td><td align="center" width="25%" >'.$row->Platform.'</td><td align="center" width="25%">'.$row->License.'</td></tr>
         
         <tr><td colspan="4">'.$row->Overview.'</td></tr></table><br><br>';
      
  }
 $out.='<p><strong><span class="title">Utilities:</span></strong></p>
    <p class="normaltext">Various utilities, some specific to Pluto, some general purpose</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=3
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql,$conn) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 cellSpacing=0 borderColor=#808EA8 class="insidetable2" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td>
		</tr>
         <tr>
			<td align="center" width="25%"><span class="title"><B>'.$row->Description.'</B></span></td>
			<td align="center" width="25%">'.$row->Language.'</td>
			<td align="center" width="25%" >'.$row->Platform.'</td>
			<td align="center" width="25%">'.$row->License.'</td>
		</tr>
      	<tr>
			<td colspan="4">'.$row->Overview.'</td>
		</tr>
</table><br><br>';
      
  }
 $out.='<p><strong><span class="title">DCE Devices and Plug-ins:</span></strong></p>
    <p class="normaltext">A DCE device exposes Data, Commands and Events, and is able to connect to the DCE Router,. DCE allows any device to work seamlessly with all other devices, and be controlled from any Orbiter. You can create your own DCE device in as little as 60 seconds! Just visit xxxx, specify what data your device needs, what commands you want to implement, and what events you fire. The wizard will automatically build you a cross-platform project complete with a gcc-compatible Makefile, and a Visual C++ .NET project file. Just compile, run and start adding your own implementation. See the DCE Router project for details.</p>
    <p class="normaltext">Following are the DCE devices that will be ready for release in October. Less commonly used DCE devices will be released later.</p>';
       $sql="SELECT PK_Project, Project.Description AS Description, Overview, HomePage, Platform, Language, License.Description AS License, ProjectLevel.Description AS ProjectLevel
FROM `Project`
INNER JOIN License ON FK_License = PK_License
INNER JOIN ProjectLevel ON FK_ProjectLevel = PK_ProjectLevel
  where FK_ProjectLevel=4
ORDER BY FK_ProjectLevel, Description";
$r=mysql_query($sql,$conn) or die("Can not grab from database ".mysql_error());
 while($row=mysql_fetch_object($r))
  {
$out.='<TABLE width=100% border=1 cellPadding=2 cellSpacing=0 borderColor=#808EA8 class="insidetable2" id=AutoNumber1 style="BORDER-COLLAPSE: collapse">
       <tr>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Name</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Language</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>Platform</b></td>
			<td align="center" width="25%" bgcolor="#F2F3F7"><b>License</b></td>
		</tr>
        <tr>
			<td align="center" width="25%"><span class="title"><B>'.$row->Description.'</B></span></td>
			<td align="center" width="25%">'.$row->Language.'</td>
			<td align="center" width="25%" >'.$row->Platform.'</td>
			<td align="center" width="25%">'.$row->License.'</td>
		</tr>
        <tr>
			<td colspan="4">'.$row->Overview.'</td>
		</tr>
</table><br><br>';
      
  }
	
	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview',"I'm an Open Source Developer"=>"index.php?section=developer")); //not used yet
	$output->setTitle(APPLICATION_NAME."::I'm an Open Source Developer");			//page title
	$output->setBody($out);		
	$output->output(); 
 }
?>
