<?
function secure_payment($output)
{
  $out='';
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
  $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
      <img src="images/submenu_txt/secure_payments_txt.gif" width="233" height="14">
    <br><p>As soon as you receive the email with instructions for turning on Bluetooth and setting the ID in your phone, the recognition features will work immediately.  They do not require any software installed in the phone.  However, the secure payment system does require special software on your mobile phone to handle all the complex cryptography.</p>

<p>At Pluto we require that all our products are completely self-configuring and trouble-free.  So, we will not install the software on a phone until we have thoroughly tested that model to be sure that it works perfectly and will never give you any problems.  Of course, there are many models that our engineers are testing, and we are doing the most popular ones first.  So be sure you tell us what model you have under the \'My Settings\' tab.  </p>

<p>Within a few weeks, we will post on this site the ranking of phones by popularity, and the target release date for each model.  The most popular phones will be going at that time, and the less popular models will take up to 2 months.</p>

<p>As soon as our engineers have tested your particular model and confirmed that everything works perfectly, we will send you an email with a link that will activate your phone.  Once you activate the feature, your phone will beep the next time you enter a PlutoVIP establishment and install the software automatically--you will not need to do anything except press \'ok\'.</p>

<p>In the meantime, another department at PlutoVIP is contacting all businesses nationwide, particularly those you list under \'Refer a business\', to be sure they sign up and install the free software so that when your phone is activated you can start using it all over town.</p>

<p>Check back on this page periodically to see the release date for your model.</p>
  </td></tr></table>';
$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');
       $output->settingsIMG = 'images/home%20page%20img/a_my_settings_on.gif';
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME."::Contact a VIP");			
      $output->setPageID(98); //butonul selectat
  		$output->output();
}
?>
