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
<p>To setup your Pluto system, click the 'Wizard' button on the menu.  All the steps in the Wizard will appear in the box to the left.
Go through each page, and just skip over any that do not apply.  If you ever get stuck, click the 'help' button.  It is context sensitive so it will give you 
detailed instructions on whatever page you are at.</p>
<p>Note that after making changes to your setup, you should reset your core by choosing the last option in the Wizard.</p>
<p>The "Advanced" menu gives you a lot more advanced options.  You can choose "Advanced/My Devices" to see all the 
devices in this installation and edit every setting manually.  "Advanced/My Scenarios" also gives
you low-level control over all the scenarios.</p>
<p>To change the boot sequence that your Core and Media Director's go through, choose "Advanced/Boot Sequence."</p>
<p>If you're a developer, you can choose "Advanced/Device Templates" to see all the templates,
or definitions, for Pluto's devices and create your own in a matter of minutes.</p>
<p>You can also visit the <a href="/support/index.php?section=document&docID=1" target='top'>online documentation</a> to learn all there is to know.
This is a local copy of the same online documentation on the plutohome.com's support web site.</p>
<br>
<p><b>Remember, if you ever have a question</b>, just click the HELP button.  If that doesn't answer your question,
you can visit the forums at plutohome.com or request live chat and technical support.</p>



KOL;

$output->setHelpSrc('/support/index.php?section=document&docID=35');
$output->setBody($out);
$output->setScriptInHead($scriptInHead);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>