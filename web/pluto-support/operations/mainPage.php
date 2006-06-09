<?
/* @var $dbADO ADOConnection */

$out='
	<table border="0">
		<tr>
			<td>
			<h1>Pluto Home Support Site</h1>  
			<p>This support site has all our online documentation in the tree on the left. In the menus above you\'ll find forums, mailing lists, bug reports, change logs, license info, doxygen graphs, etc.</p>  <p>If you\'re new to Pluto, you can visit our <b>consumer home page</b> at <a href="http://plutohome.com">plutohome.com</a> for screen shots, flash presentations, product photos, dealer locator.</p>  <p>If you\'re an <b>open source programmer</b> or technically savvy, visit <a href="http://plutohome.org">plutohome.org</a> for a rapid, get-to-the-facts intro that explains what Pluto is, how it works, and how to download it for free.</p>  
			<p>When you\'re ready to get Pluto, be sure to read the <a href="'.$wikiHost.'index.php/'.wikiLink('Quick Start Guides').'"><b>Quick Start Guide</b></a>--you\'ll be up and going in no time. Also check out the <a href="'.$wikiHost.'index.php/'.wikiLink('What can I do with Pluto?').'">What can I do with Pluto?</a> to see what\'s possible and how to do it.</p>
			</td>
		</tr>
	</table>';


$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>