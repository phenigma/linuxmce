<?
function userHome($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
$dbADO->debug=false;
$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

$scriptInHead='
<script>
	'.($reloadTree==true?'
	if (parent.frames[\'treeframe\']) {
		top.frames[\'treeframe\'].location=\'index.php?section=leftMenu\';
	}':'').'
</script>
';

$out.=<<<KOL

<p><b>Welcome to Pluto Admin.</b></p>
<p>***NOTE*** The wizard isn't yet complete.  Some steps can only be done with the Advanced pages.</p>
<p>Setting up your Pluto system is pretty simple.  There are 4 basic parts which the wizard will guide you through:</p>
<p>1.  You need to provide some <b>basic information</b> about your house, like the rooms and the entertainment areas.</p>
<p>2.  You will add the <b>devices</b> that you want to use, like
your security system, climate system, stereo equipment, etc., and you will use the floorplan editor to show where these
devices are in the house.</p>
<p>3.  Next, you will specify your <b>scenarios</b>.  A scenario is basically a command,
or group of commands.  For example, you could create a lighting scenario called "Entertaining" which
sets up your lights, and maybe does a few other things.  Maybe a media scenario called "TV" which 
turns on the TV and prepares the room.  Every scenario will become a button on the orbiter that you 
touch to execute the scenario.</p>
<p>4.  Lastly, you may want to create some <b>event handlers</b>.  This is where
you say that when something happens (an event) under certain conditions (a criteria), 
you want Pluto to do something (a command).  An example is:  When the motion detector by the 
front door is triggered (the event), and it is nighttime (the criteria), you want Pluto to turn
on the porch light (the command).  These are optional--you don't need to specify any events.</p>
<br>
<p>The easiest and fastest way to complete these 4 parts is to just click on the "Wizard" button 
and complete each page.  Everything you need to do will be handled by the wizard, and when you 
have finished, your Pluto system will be ready to go.  You can revisit the wizard
pages at any time to change something.</p>
<p>Note that after making changes to your setup, you should reset your core.  This will cause
it to reload the new configuration, and also regenerate all the graphics that appear on the 
orbiters.</p>
<p>If you're an advanced user, you can choose "Advanced/My Devices" from the menu to see all the 
devices in this installation and edit every setting manually.  "Advanced/My Scenarios" also gives
you low-level control over all the scenarios.</p>
<p>To change the boot sequence that your Core and Media Director's go through, choose "Advanced/Boot Sequence."</p>
<p>If you're a developer, you can choose "Advanced/Device Templates" to see all the templates,
or definitions, for Pluto's devices and create your own in a matter of minutes.</p>
<p>You can also visit the <a href="/support/index.php?section=document&docID=1" target='top'>online documentation</a> to learn all there is to know.
This is a local copy of the same online documentation on the plutohome.com's support web site.
Your local copy will still have links to all the forums and bug reporting tools on plutohome.com.</p>
<br>
<p><b>If you ever have a question</b>, just click the HELP button.  It is context sensitive and will 
take you to the appropriate page in the documentation to explain what you see on screen.  If 
you still have questions, there will be links to Pluto's forums, live chat, and tech support.</p>



KOL;

$output->setHelpSrc('/support/index.php?section=document&docID=35');
$output->setBody($out);
$output->setScriptInHead($scriptInHead);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>