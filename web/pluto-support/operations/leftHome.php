<?
$out='
<p>Useful shortcuts for
**package**</p>

<p><a href="index.php?section=mainDownload" target="_top">Download software</a></p>
<p><a href="index.php?section=document&docID=1" target="_top">Documentation</a></p>
<p><a href="index.php?section=forum" target="_top">Forum</a></p>
<p><a href="index.php?section=faq" target="_top">FAQ</a></p>
<p><a href="index.php?section=mail" target="_top">Mailing List</a></p>
<br><br>
<p><b><a href="index.php?section=home&package=0" target="_top">Pluto\'s Main Support Page</a></b></p>
';

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>