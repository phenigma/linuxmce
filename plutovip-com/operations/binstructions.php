<?
function binstructions($output)
{
  
  $out='';
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
   $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="center" class="insidetable">
    <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="100%" id="AutoNumber1">
      <tr>
      <td align="left">
      <img src="images/submenu_txt/instructions_txt.gif" width="96" height="14"><br><br></td>
      </tr>
  <tr>
    <td align="left" valign="top">
	<p>You will need a cash register or computer that supports Bluetooth in order to use the system.</p>

<p>
   We are quickly working with cash register manufacturers to be sure that there is an effortless and inexpensive way 
   to get PlutoVIP on all the popular models.  At this moment, the easiest and cheapest solution is a USB to Bluetooth 
   dongle.  We tested many models, and found one that worked particularly well and was quite inexpensive.  You can click
   \'Buy Dongle\' to get it for $79.  It will work with all cash registers and computers that have a USB port and that 
   are based on Windows 98SE or later.  A CD is included with the dongle that will auto-install all the drivers and the 
   PlutoVIP software--it only takes a couple minutes and you\'ll be ready to go.  If you already have a Bluetooth-enabled
   register, please send an email to: <a href="mailto:support@plutovip.com">support@plutovip.com</a> indicating the model,
    and if it uses a Bluetooth dongle, please indicate what type of dongle you are using.  Our testers will confirm that 
   it is compatible with the PlutoVIP software, and send you the CD with the PlutoVIP software at no charge.</p>

<p>We realize that there are thousands of other cash registers out there that do not have USB ports and do not run Windows.
   Our engineers are working as quickly as possible to offer a solution for each model.  Please be sure you tell us what 
   make and model you have under \'Settings\'.  That will help us prioritize all the models based on their popularity. 
   We\'ll send you an email when we have news on your model.  Rest assured that we thoroughly test everything that leaves
   our doors to be sure it is rock-solid and completely foolproof.  When we have a solution for your model, that means it 
   will work right the first time without fuss.</p>

<p>Even if we do not have a solution for your cash register just yet, by signing up now, PlutoVIP will always be free for
  life for your business.  We will never charge you for the software or the service, as our way of saying \'thanks\' for 
   being one of the first.</p>

<p>Be sure to also visit the \'Secure Payments\' page.  You will see it when you click on Settings.  This is where you 
   tell us what type of credit card processing system you have.  When one of your customers wants to pay with the mobile 
   phone, your store still charges the customer directly.  It still goes through your merchant account, just like normal,
    and you still print the usual receipt that the customer signs.  PlutoVIP does not get involved in the transaction, 
   and does not charge anything to use the service.  What PlutoVIP does is pass the credit card information to your credit
    card processor in such a way that the number is not stored in your register.  This is really better for both your 
   business and your customer.  Of course, it makes your customers more comfortable since they do not have to worry that 
   you might have a dishonest employee who uses their credit card number fraudulently.  And it is better for you for two 
   reasons: Firstly, you don\'t have the liability that someone may copy credit card numbers from your system, or that 
   your employees may be skimming cards.  Secondly, PlutoVIP verifies the identity of each member individually before 
   they are allowed to pay with the phone, and they must enter a PIN number on their mobile phone to process the 
   transaction.  So when you take a PlutoVIP payment, you have an extra triple protection that this is a valid transaction:
    (1) you see the card holder\'s picture on your register, (2) the card holder must enter the correct PIN, and (3) 
   PlutoVIP corporate already verified the card holder\'s identity.</p>

<p>Our ultimate goal is to substantially reduce the number of charge-backs and fraudulent transactions.  This costs 
   businesses like yours many billions each year, and directly relates to the percentage you must pay the credit card 
   companies.  Once PlutoVIP has a sufficient number of users, we will negotiate with the with the credit card companies 
   a reduced rate for PlutoVIP payments since the potential for fraud is drastically reduced.  This will be the catalyst 
   that makes our recognition and payment technology a standard feature on cash registers and POS systems.</p>

<p>The way PlutoVIP will make money is by licensing our patent-pending bluetooth recognition system to the cash 
   register/POS manufacturers.  However, we know that in order to get to that point, PlutoVIP must gain critical mass,
    becoming a common system in use everywhere.  That is why we are giving away everything for free to the early adopters 
   like yourself to gain the necessary momentum.</p>

<p>We hope that you enjoy the service.  If you have any questions, please e-mail us at 
   <a href="mailto:support@plutovip.com">support@plutovip.com</a>.</p>
    
    </div></td>
  </tr>
</table>
      </td></tr></table>';
  
       $output->setScriptCalendar('null');
		$output->setScriptTRColor('null');
      $output->instructionsIMG = 'images/home%20page%20img/a_instructions_on.gif';
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME."::Contact a VIP");			
      $output->setPageID(98); //butonul selectat
  		$output->output();
}
?>