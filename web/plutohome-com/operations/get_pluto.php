<?
function get_pluto($output,$conn){
	global $PlutoSupportHost,$PlutoHomeHost;
	
	$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/get_pluto.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table border="0">
		<tr>
			<td width="15">&nbsp;</td>
			<td width="100%">&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Build a Pluto system for FREE!</span></td>
		</tr>
		<tr>
			<td colspan="2"><p>To install Pluto on your own PC:<br><br>

 

1.  Download the latest kick-start CD:<br><br>

	&nbsp;&nbsp;&nbsp;&nbsp;Version ----KICK_START_NAME----<br>

    &nbsp;&nbsp;&nbsp;&nbsp;MD5: ----MD5SUM----<br>

	&nbsp;&nbsp;&nbsp;&nbsp;<a href="'.$PlutoHomeHost.'download/cds/pldebsrg.iso">HTTP download</a><br>

	&nbsp;&nbsp;&nbsp;&nbsp;<a href="ftp://ftp.plutohome.com/pub/----KICK_START_NAME----">FTP download</a><br><br>

 

   2. Burn it using any CD burning software that can burn an .ISO image.  Virtually all cd burning software does this.<br><br>

 

   3. Read the <a href="'.$PlutoSupportHost.'index.php?section=document&docID=212">Quick Start Guide</a>.  Part 3 explains how to boot off the kick-start cd to make your PC a Pluto Core or hybrid.  Note that by default the CD will erase the contents of the hard drive.<br><br>

 

If you really know what you’re doing, click the ‘Download’ link on our <a href="'.$PlutoSupportHost.'index.php?section=mainDownload&package=0">support site</a> to download individual modules.  Note there are hundreds of modules, and it may be quite difficult to get it to work this way.  Or get the source code from anonymous svn like this: svn co http://svn.plutohome.com/pluto/trunk/</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
	
		<tr>
			<td>&nbsp;</td>
			<td>Get useful information:<br>
			<a href="index.php?section=dealer">I\'m a dealer </a><br>
			<a href="index.php?section=manufacturer">I\'m a manufacturer</a><br>
			</td>
		</tr>	
	
	</table>
    </td>
  </tr>
</table>	
';

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Get Pluto');
	$output->output();
}
?>
