<?
$out='';
$dbADO->debug=false;
$reloadTree = @$_REQUEST['reloadTree']=='true'?true:false;
$docID=(isset($_GET['docID']))?$_GET['docID']:1;

$scriptInHead='
<script>
	'.($reloadTree==true?'
	if (parent.frames[\'treeframe\']) {
		top.location=\'index.php?section=document&docID='.$docID.'\';
	}':'').'
</script>
';

$out.='Index page. <br>';


$output->setBody($out);
$output->setScriptInHead($scriptInHead);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		

?>