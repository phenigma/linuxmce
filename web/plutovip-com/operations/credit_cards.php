<?
function credit_cards($output)
{
   $out='';
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
   $out.='
  <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" " valign="top" align="center" class="insidetable">
      <p>The credit cards settings will be available soon!</p>
      </td>
      </tr>
      </table>';
     
   $output->setScriptCalendar('null');
		$output->setScriptTRColor('null');	
     $output->settingsIMG = 'images/home%20page%20img/a_my_settings_on.gif';
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME.":: My Credit Cards");			
      $output->setPageID(98); //butonul selectat
  		$output->output();
}
?>