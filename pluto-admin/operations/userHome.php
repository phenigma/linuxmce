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
<p>If this is a new installation, just choose the Wizard menu and complete each of the steps.  
Depending on the complexity of your system, the whole process may take up to an hours or so.
Once you've finished, your Pluto system should be ready to go.  You can revisit the wizard
pages at any time to change something.</p>
<p>If you're an advanced user, you can choose "Advanced/My Devices" from the menu to see all the 
devices in this installation and edit every setting manually.</p>
<p>To change the boot sequence that your Core and Media Director's go through, choose "Advanced/Boot Sequence."</p>
<p>If you're a developer, you can choose "Advanced/Device Templates" to see all the templates,
or definitions, for Pluto's devices and create your own in a matter of minutes.</p>
<p>You can also visit the <a href="/support/index.php?section=document&docID=1" target='top'>online documentation</a> to learn all there is to know.
This is a local copy of the same online documentation on the plutohome.com's support web site.
Your local copy will still have links to all the forums and bug reporting tools on plutohome.com.</p>




KOL;

$output->setHelpSrc('/support/index.php?section=document&docID=35');
$output->setBody($out);
$output->setScriptInHead($scriptInHead);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>