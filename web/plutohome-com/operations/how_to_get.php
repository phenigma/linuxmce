<?php
 function how_to_get($output,$conn) {
		$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/powerd_by_pluto.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2"><p><strong>Powered By Pluto</strong> &ndash; Pluto version 2 is available as an embedded solution for incorporating into other consumer electronics products. For example, Powered by Pluto TV&rsquo;s have all the functionality of a Pluto Media Director built-in. Imagine buying a TV, plugging in only 1 cable, and your mobile phone turns into the remote, the TV recognizes you, knowing all your favorite shows, can play your entire collection of music and movies, and works seamlessly with all the devices in the house&mdash;regardless of brand. Or an in-car entertainment systems that automatically synchronizes with your Pluto Core so your entire media collection is always available, and the phone book, navigation system, playlists, and other preferences are automatically synchronized. You even receive instant notifications on your mobile phone if the car&rsquo;s alarm is set off&mdash;just like your Pluto Home system. Sign up for our newsletter to be notified as these revolutionary products are released.</p>
	</td>
  </tr>
</table>
';
	include('operations/contactForm.php');
	$out.=contactForm('get_pluto',$conn);	

		
	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','Powered By Pluto'=>'index.php?section=how_to_get'));
	$output->setTitle(APPLICATION_NAME." :: Powered By Pluto");			
	$output->setBody($out);
	$output->output();
 }
?>
