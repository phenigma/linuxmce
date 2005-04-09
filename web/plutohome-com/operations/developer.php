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
	
	<a href="support/index.php?section=document&docID=194">What are the benefits of using Pluto\'s platform for my software project?</a><br><br>
	<a href="support/index.php?section=document&docID=15">Programmer\'s guide</a>&nbsp;&nbsp; <a href="support/index.php?section=document&docID=181">Is the Pluto license OSD-approved?</a><br><br>
	<a href="index.php?section=letter">What is Pluto\'s open source strategy?</a> 
';
      
	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview',"I'm an Open Source Developer"=>"index.php?section=developer")); //not used yet
	$output->setTitle(APPLICATION_NAME."::I'm an Open Source Developer");			//page title
	$output->setBody($out);		
	$output->output(); 
 }
?>
