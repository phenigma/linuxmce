<?php
 function how_pluto_vip_works($output) {
		$out = '';
		
$out.='
<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="5%" valign="top" align="left" >
  </td>
   <td width="100%" class="insidetable" align="left">
  <table width="100%" calss="insidetable">
  <p><img src="images/meniuri/how_plutovip_works_txt.gif" width="152" height="13"><br>
  <p>
    			 A business needs the free PlutoVIP software and a bluetooth compatible cash register, or a cheap adapter that works with most 
			    existing ones.  It takes about 10 minutes to setup.&nbsp; As an individual, you only need a mobile phone that supports 
			    Bluetooth--a common feature.&nbsp; Both you and the business need to register at plutovip.com, which takes about 2 minutes.
			    You should provide your name and a photo that you want the business to see whenever you enter.  
			    You can also specify extra information for each type of business that you want displayed when you enter, such as your sizes
			    at clothing stores, or your favorite drink at bars, and so on.&nbsp; When you enter the business, 
          their cash register or computer will automatically detect the unique id that your phone transmits and can then lookup 
          whatever information you would like to share with the business.  Within 10 seconds or so, a little dialog pops up
			    on their register with that information.  The business can link it to their own customer number, so that you are automatically
			    registered the next time you come in, such as at a health club.  You can disable the service at any time just by turning Bluetooth off
          on your mobile phone.  We\'ll tell you how to do that on your phone when you sign up--it only takes a second.
			</p>
			<p>
				 To use the service, you don\'t have to do anything--just carry your mobile phone in your pocket or purse.  The business can then provide more personal and 
			    convenient service.&nbsp; For example, enter a hotel, and the front desk will immediately be alerted and shown your reservation so the 
			    bellhop can bring you your key before you even reach the counter.<br />
			</p></table>
         <table width="100%" calss="insidetable">
         <td><br>
         <img src="images/refund.png" width="176" height="208"><br>
         <img src="images/charge.png" width="176" height="208"><br>
         <img src="images/airline.png" width="176" height="208"></td>
         <td width="2%"></td><td valign="top"><br>
         <p>   On the business\'s register, next to your name and picture that popped up, are buttons they can push to request a variety of transactions.
    		For example, maybe the business wants to request your name and address.
    		If the business clicks on that button, your phone beeps and tells you what the business is asking for.
    		Just press yes or no on the phone.  It really is that simple.  
    		If you want to be able to make payments with the mobile phone, you will need to store your credit card number on the PlutoVIP site.
    		It is stored in a secure, encrypted database.  Nobody--not even the PlutoVIP staff--has access to it.
    		Then, when the business wants you to pay for something, they simply click on your picture, and type in the amount.  
			Your phone will beep, show you the details of the transaction, and, again you just press yes or no and enter your pin number.
    		<br /><br />
    		It\'s faster, easier and safer than any other payment method.  
    		The entire transaction is completely encrypted, so the business will never know your pin number, or your payment information.
			If you approve the payment, then the payment gets processed like normal.  PlutoVIP is not involved in the transaction and does not charge for the 
      service.  The only thing PlutoVIP does is store the credit card information in an encrypted state so the cashier never 
      sees it. 
			And, because the credit card information is not stored in the phone--it is in the encrypted 
			database which can only be unlocked with your pin--you don\'t have to worry about losing the phone.
			Nobody can ever use your phone to make payments.  Wow! That\'s a lot simpler and easier than lugging around a stack of cards, 
			and hoping that nobody is skimming your number.
			And praying that the merchant\'s database is secure so that nobody can get the credit card number.  
			Plus, PlutoVIP is super fast--however long it takes you to punch-in your pin on the phone is how long it takes to do the transaction.
			And you can go to PlutoVIP.com to get a full transaction history.
			<br /><br />
			There are even specialized transactions, and simple development kits to enable all sorts of new transactions, like airport check-in.&nbsp;
			Or ticket-free parking garages: drive in and out without getting out of the car or even rolling down the window, and without having 
			to carry around an easy-to-lose parking ticket.&nbsp; In every case, there is nothing to learn.&nbsp; Your phone will just tell you the nature of the 
			transaction, and you press 1 or 2 buttons to confirm.&nbsp; It is safe, secure, and super simple, and you do not have to do anything but carry your mobile 
    		phone.</p>
           <p>Here\'s something even more impressive.&nbsp; Pluto is actually the most effective tool for protecting your home and family, and turns 
			your mobile phone into the most intelligent, yet simple remote control for everything in your home.&nbsp; 
			<a href="http://www.plutohome.com">Learn more about Pluto Home</a>
			<br /><br />
			<a href="index.php">Back to main page</a></p>
           </td></table>
            </td></table>'; 

// $output->setImgName("businesses.jpg"); //img that needs to be changed
        $output->setPageID(2); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::How Pluto Vip works");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
}
