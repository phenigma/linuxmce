<?
function instructions($output)
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
    <td align="left" valign="top"><br>Thank you for signing up. Be sure you tell us what make/model
    phone you have under the \'Settings\' page so we can email you instructions for 
    turning on the Bluetooth recognition feature for your particular model.<br><br>Remember when our grandparents spoke of the 
    good ol\' days when they would go around town and people knew their names and 
    offered personal service? The boy at the ice cream store always knew your favorite flavor.
    It was a time of innocence when you were more than a 
    number and weren\'t scrutinizing the cashier to be sure he didn\'t skim your 
    credit card or tear your receipt into little bits to defend against identity 
    theft. We probably assumed that era was nothing more than a nostalgic 
    memory, never to return.<p>While PlutoVIP certainly can\'t roll back the clock, 
    at least it promises to bring back a little touch of 
    that golden age: the comfort and convenience of personal service and the 
    calm from knowing your financial data won\'t be used against you. But, <b>we 
    need your help!</b> Scores of people need to start using the service so that it 
    gains critical mass and becomes a standard feature in cash registers. To 
    reward those like yourself who sign up early, the service will be free for 
    life. The activation fee is waived, and you will never pay 1 penny for the 
    service. Of course, we can\'t do this forever. So be sure to tell all your 
    friends and family to sign up <b>now</b>. Even if they don\'t have a Bluetooth phone 
    yet, they can still sign up now while it\'s free for life and activate their 
    phone at a later time.</p>
    <p>Click \'Refer a Business\' to list the businesses you frequent.  We\'ll contact them and ask them to sign up.  After all, it\'s free for life for them too if they sign up now, so there\'s no reason for them not to sign up and start giving you a little VIP treatment.  We\'ve also created some cards you can print out and give your favorite businesses as a friendly reminder .
    </p>
    <p>The only thing you need to do now is click on \'My Settings\' and specify, 
    at a minimum, what name you want businesses to see when you enter. Then we 
    also recommend that you put some information in the \'Info for Businesses\' 
    page (you will see it when you click on \'My Settings\'). There you specify 
    extra information you want each type of business to know about you, such as 
    your favorite drink at a bar, or maybe your sizes at clothing stores.</p>
    <p>While you\'re there, check out the \'Secure Payments\' page. This allows you 
    to pay the bill at PlutoVIP businesses securely, using your mobile phone and 
    protected with a pin code. It is the easiest and safest way to pay wherever 
    you go. Note that when you pay using your mobile phone, you are still paying 
    the business directly with a credit card just like before--PlutoVIP is not 
    involved in the transaction, and never charges you anything. The only 
    difference is that when you pay using the mobile phone, the payment cannot 
    be completed without your pin code, and even when the payment does go 
    through, the merchant cannot see your real credit card number--everything is 
    encrypted. That way your credit card number is not being stored in their 
    system like it normally is when you hand them the card directly. Nothing is 
    ever stored in the phone, so it is harmless to lose it--unlike the physical 
    credit card. For more information on how this works and the security 
    measures PlutoVIP has taken to protect you against fraud, </p>
    <p>Thank you again, and enjoy the service!</p></td>
    <td width="396" valign="top">
    <a href="http://www.rockwellsite.com" target="_blank"><br><img border="0" src="images/rockwell.jpg" width="390" height="487"></a><br>image available at <a href="http://www.rockwellsite.com" target="_blank">www.rockwellsite.com</a></td>
  </tr>
  <tr>
    <td width="100%" colspan="2">&nbsp;</td>
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