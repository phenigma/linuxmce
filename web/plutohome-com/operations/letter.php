<?php
 function letter($output) {
		$out = '';
		
$out.=<<<TEXT

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
<p>Give away a successful product for free as open source?  What?  That seems an odd strategy for a profit-minded company.  
True.  However the benefits are too compelling for the company, our paying customers, the open source community, and our dealers.</p>
<p><b>For the company:</b>  
Potential customers fall into 2 categories.  First, those who don't want to mess with technology.  They buy Pluto as a turnkey appliance they 
take out of the box, plug in, and hit 'on'.  Or, they get professional installation from high-end dealers.  
Second, the more tech-oriented who get their own equipment and install software.</p>
<p>Our customers belong to the first group.  And even though the software is available for free, they are still going to buy a pre-packaged solution
from dealers.  Up until now, we have had no solution for the second group.  We could have made Pluto non-free software to
make money off both groups.  But there are so many benefits to going open source and it makes the product better, so we figured it made
more sense this way.</p>
<p><b>For our paying customers:</b> They just want the best smarthome solution on the market.  By going open source, Pluto can offer more
features and benefits, it becomes more reliable, developers can add specialty device drivers, language translations, skins, and so on.</p>

<p><b>For the open source community:</b> There's the obvious benefit of having a new media + smarthome solution developed by a full-time team.
But there's another benefit to the open source community, which we, humbly and modestly propose:  A great benefit of the open source model is the
<i>potential</i> for a diverse mix of software to work together.  For example, the Xine media player tells the Asterisk phone system to send calls to 
voicemail during the movie.  Or Asterisk tells Xine to lower the volume on your music when a call comes in.  This
<i>should</i> be easier with open source since the Asterisk team can see the Xine team's source and vice-versa.  Yet it seems closed 
source has a lead here.  Most Windows applications do expose COM and OLE Automation interfaces allowing devices to communicate
with each other, and you can, for example, drag and drop all sorts of non-Microsoft content into a MS Word document.  With Pluto's DCE 
framework, however, all sorts of open source project work together as a seamless whole, across multiple platforms and operating systems,
in a way not possible with closed source.  Read <a href="support/index.php?section=document&docID=15">Pluto allows projects to work together</a>
for an example.</p>

<p><b>For our dealers:</b>  By opening the platform, dealers now have a choice of hardware.  They can offer recognized name-brand commercial servers
with 24/7 service contracts.  The price is lower, and international dealers don't need to worry about sending equipment back to the U.S. for repairs.
Plus, a dealer that needs something special, say an interface for a type of swimming pool control only sold in Australia, doesn't need to 
wait for our developers.  He can get a local developer to add whatever customization he needs.</p>

</td>
  </tr>
</table>
	
TEXT;
		

	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview',"Pluto's open source strategy"=>"index.php?section=letter")); //not used yet
	$output->setTitle(APPLICATION_NAME);			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
