<?php
 function installation($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_on.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
      <p><img src="images/submenus/installation_information_tx.gif" width="187" height="13"></p>
      <p><strong>Wiring considerations</strong><br>
        Wiring is often the biggest hurdle in any whole house system because most such products use their own, proprietary wiring. That means tearing open walls to run their special cabling, which is often very thick and difficult to manage. With Pluto, on the other hand, everything is IP based. That means it plugs into an Ethernet network--the same network that powers the Internet and virtually all office and home networks. An Ethernet network runs on Cat5 (or cat5e/cat6) cable. This cable is commonly installed in homes already, since it is used for most phone systems as well as a home network. If your home does not already have cat 5 cabling, cat 5 is definitely one of the smaller, easier wires to run. What's more, because home networks are so desirable for internet access and file/printer sharing, installing cat5 cabling in a house is well worth the cost and will increase the resale value, even without a Pluto system. This is another advantage of Pluto's use of the industry standard cable. The cable by itself, even without the Pluto system, is very useful to have. When you have an Ethernet/cat5 jack, you can not only plug in Pluto equipment, you can plug in any personal computer, a printer, or a wide variety of other IP based devices.</p>
      <p>So the only wiring requirement is that you have Ethernet (cat5) going to every location where you want to either put a piece of Pluto equipment (a Media Director, a Pluto Phone, etc.) or any third party device that you want Pluto to control (your lighting control system, alarm panel, pool controls, surveillance cameras, etc.). All Pluto devices will plug directly into the jack. And there are 3 primary ways that Pluto will control your existing, non-Pluto equipment: </p>
      <p>Ethernet (aka IP based) -- this is the best, and most flexible way to control equipment. This means the device already uses the same system Pluto uses, and you can plug it into any jack. Chances are the Pluto core will be able to configure it automatically and there will be nothing for you to do.</p>
      <p>RS232 -- common with non-a/v equipment (lighting systems, pool controls, etc), and some high-end a/v equipment. If your a/v equipment has an RS232 port, this is definitely preferable over Infrared.</p>
      <p>Infrared -- most common with a/v equipment, it sends infrared codes just like a remote control. If you are going to be buying new a/v equipment, try to get devices that have an RS232 port. You will find that RS232 is much faster and more reliable. If you decide to buy a device that does not have RS232 and will use Infrared to control it, be sure to only buy a/v equipment that supports discrete codes. If you buy a/v equipment that does not support discrete codes, Pluto will still control it, and control it better than your typical home automation system, however you will find the devices will be slow to respond, and may not be as reliable.</p>
      <p>Even for the Infrared and RS232 devices, they will still ultimately plug into the Ethernet jack. There will just be a device that sits in between them and the Ethernet network. In the case of infrared, that devices is an interface module. You plug the interface module into the Ethernet network, and little i/r emitters into the interface module. The i/r emitters send infrared codes just like your remote control. In the case of RS232 devices, you can plug them either into an interface module, or into the core, or into any of your Pluto Media Directors.</p>
      <blockquote>
        <p><strong><a href="javascript:open_popup('operations/wiring.html#can','500','400')">Can pluto run wireless? </a><br>
            <a href="javascript:open_popup('operations/wiring.html#what','500','400')">What if my home doesn't have cat 5?</a><br> 
            <a href="javascript:open_popup('operations/wiring.html#what_is','500','400')">What is proper cat5 cabling?</a></strong></p>
      </blockquote>      
      <strong>Choosing a location</strong><br>
      The Media Directors are easy. They plug into the tv's, so you will put them near the tv, just like you would a DVD player or a VCR. The Media Directors do have computers inside, so you cannot put them in an enclosed cabinet unless you put some ventilation fans in the back. The ideal placement is on an open shelf. Fortunately the Media Directors are small and attractive, made from high quality brushed aluminum, so they will look good anywhere you put them.
      <p>The Pluto Core requires a bit more consideration. Firstly, remember that your phone lines and your internet connection (dsl/cable modem, etc.) will need to plug into the Core.</p>
      <p>Normally all the cat5 cabling in your house will terminate into a central wiring closet. This is typically where your internet connection and phone lines terminate as well as your alarm panel, lighting control system, etc. In such cases, this is the logical place to put the core. Also, you will get better performance if the Core is placed where all the cat 5 cable terminates. The reason is that video, primarily dvd quality video, requires a lot of data. Each cat 5 cable is able to run about 7 simultaneous dvd movies. The Pluto Core uses a very high-performance network, referred to as Gigabit Ethernet. This uses a more advanced, high speed cable, referred to as cat 5e or cat 6. It looks the same, but inside it's a little different. With cat 5e/6 you can have closer to 100 simultaneous dvd quality movies streaming on a single cable. Therefore, we recommend that you put the Pluto Core where all the cat 5 cable terminates so you plug all the cat 5 cables into a Gigabit switch (Pluto can provide this for you). The Pluto Core will connect to the switch via a cat 6 cable (included). This way the Pluto Core can serve 100 simultaneous dvd movies into the switch, and on each cat 5 cable running through your house, a total of 7 dvd movies can be running at the same time. Since each Media Director will likely be on its own cat 5 cable, and each Media Director will only be playing 1 movie at a time anyway, that means you'll have plenty of bandwidth. Even if you have an enormous mansion, you can still have dozens and dozens of people all watching different movies at the same time, and you will never have a bandwidth problem.</p>
      <p>On the other hand, if the Pluto Core is not located close to the place where all your cat 5 cables are located, then the Pluto Core will be connected into the switch using whatever cable you have running through your walls. If this is the high-speed cat5e/6, this is not a problem--the Core will still be able to feed 100 simultaneous movies into your switch. However, cat 5e/6 is not very common in homes. Normal cat 5 is nearly always used. Therefore, if the Pluto Core is connected to the switch using a cat 5 cable, the Pluto Core will only be able to feed about 7 simultaneous movies at one time, because that is the limit of the cable. Keep this in mind when deciding where you put the Core. You do not need to worry about running out of bandwidth for audio and phone calls. Those take up very small amounts, so even if the Pluto Core is connected with only cat 5 cabling, you will still be able to have hundreds of simultaneous phone calls, and dozens of different music throughout the house.</p>
      <p>The other consideration when choosing the location of the Pluto Core is that it needs to stay cool. It is a powerful, multi-processor commercial server. Be sure to put it somewhere where the temperature will not exceed 75 F/24 C. In fact, the cooler the better for all electronics. Your other electronic devices, such as lighting systems, and so on, will also run more reliably when they are in a cool place.</p>
      <p>The Orbiters are easy. Put them wherever you want to control the system. They are wireless and battery operated. They are available with either a table-top cradle, or a wall-mount cradle. Either way, the cradle needs only electrical power so it can charge the Orbiters batteries.</p>
      <p><strong>Deciding on an Internet connection</strong><br>
        The Pluto Core relies heavily upon a high-speed internet connection, such as DSL or cable modem. The Core uses the internet to do it's self-updating, and when your Pluto system arrives, it will use the internet to configure itself. It includes a voice-over-ip telephone system that can make and receive calls over the internet. Video calls always go over the internet. Further, if your Core runs into difficulty with any of it's subsystems, it will automatically send our technicians a message using the internet so we can automatically send you replacement parts if needed. Therefore, an always-on internet connection is required. These are available in most homes for anywhere from $19/month-39/month. We do recommend that you get a reasonably fast and connection, though, if you want to use the voice-over-ip phone system and the video conferencing. 512kbs down and 256kbs up are recommended minimums (your dsl or cable modem provider will know what that means). If you have any questions, please contact us.</p>
      <p>Deciding what equipment to use with your Pluto system<br>
        Pluto works with all sorts of devices. Everything from security cameras to sprinkler systems. Pick any of the devices on the compatibility list. When picking your audio/video equipment (tv's, amplifiers, etc.), just remember to avoid TOAD devices.</p>
      <p><strong>Final thoughts</strong><br>
      Remember, we are here to help you. We want you to get the most enjoyment out of your Pluto system so that you will enthusiastically refer us to all your friends. If you're a do-it-yourselfer, we're here to help every step of the way. If you would prefer that we do everything for you, including choosing lighting systems, working with local dealers, negotiating the best price, on-site supervision, and so on, we offer project management services. In fact, we even include 8 hours of project management with the purchase of each Pluto system at no charge. </p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(4); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Installation");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>