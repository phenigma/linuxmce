<?php
 function business($output) {
		$out = '';
		
$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="10%" valign="top" align="left" >
   <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/meniuri/how_get_plutovip_txt.gif" width="148" height="13"></td>
    </tr>
       </table></td>
  <tr>
    <td class="insidetable" valign="top"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=individual"><img src="images/submeniuri/im_an_individual_off.jpg" width="144" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=business"><img src="images/submeniuri/im_a_business_on.jpg" width="144" height="17" border="0"></a></td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <tr>
        <td><a href="index.php?section=im_manufacturer"><img src="images/submeniuri/im_a_manufacturer_off.jpg" width="144" height="17" border="0"></a></td>
      </tr>
      </tr>
    </table>
  </tr>
  </td>
   <td width="100%" class="insidetable" align="left">
    <p><img src="images/submeniuri/im_an_business_txt.gif" width="103" height="13"><br>
      <p><strong>If you are a business</strong></p>
        <p>
				You will need a Windows-based cash register, point of sale system, or computer that supports 
				Bluetooth.&nbsp; If yours does not already support Bluetooth (most do not), then 
				you can purchase a little adapter for $79 that plugs into the USB port.  Businesses that sign up now will never pay for the software or the service.
			</p>
			<p>
				PlutoVIP is also working with leading cash register manufacturers to have the 
				service built-in, and provide an easy, cheap retrofit.&nbsp; So, even if your 
				system is not Windows-based, be sure to sign up and tell us the make and model of your cash register.&nbsp; We will then 
				contact the manufacturer to determine if there is an easy way to make it work, and 
				we will send you an email when we have a solution.
			</p>
			<p>
				Also, it is recommended, though not mandatory, that you have internet access 
				at your location.&nbsp; When a new PlutoVIP enters your business, the software 
				uses the internet to lookup the persons name and picture so you have nothing to 
				do.&nbsp; If you do not have an internet connection, then the system will popup 
				a message allowing you to type in that persons name, and whatever other 
				information you want to store.&nbsp; A little box will appear on your screen 
				with that information the next time that person enters.
			</p>
			<a href="index.php">Back to main page</a><br />
 </td></table>'; 
 $output->setImgName("businesses.jpg"); //img that needs to be changed
        $output->setPageID(4); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::I'm a Business");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
}