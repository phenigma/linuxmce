<?php
 function public_license($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/public_license_txt.gif" width="148" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><p>The Pluto Public License, Version 1.0<br>
      Copyright (C) 2004 Pluto, Inc., Florida, USA</p>
      <strong>Preface
      </strong>      <p>This software was developed by Pluto, Inc., which continues to actively develop, maintain and support it. Until November 2004, it was a proprietary, closed source product, sold both as a software distribution, and also pre-installed on hardware.</p>
      <p>Pluto is a firm supporter of the open source movement, and believes the open source model results in a superior product. Pluto also suspects its software will be of interest to the open source community.</p>
      <p>Pluto has decided to sacrifice sales from software distribution, making all the software available for free under an open source model, while focusing exclusively on generating revenue through hardware distribution and licensing to hardware manufacturers. Ultimately Pluto&rsquo;s revenues will come from end consumers with the means and desire to purchase a complete, hardware+software solution with Pluto&rsquo;s software pre-installed. All others who wish to use Pluto&rsquo;s software can use it on their existing equipment under this open source license at no charge.</p>
      <p>The goal of this license is to foster this mutually beneficial arrangement by splitting the licensing into &ldquo;software distribution&rdquo; and &ldquo;hardware distribution&rdquo;. When this software is part of a software distribution, it is provided under the terms of this &ldquo;Pluto Public License&rdquo;. In such case, this license intends to grant the same freedoms and restrictions as the popular GPL license, and comply with the currently accepted Open Source Definition, including allowing any party to make modifications and sell or give away this software as part of an aggregate software distribution without requiring royalties. However, if this software is offered as part of a hardware distribution, this license cannot be used and a separate license must be obtained from Pluto, Inc.</p>
      <strong>Definitions
      </strong>      <p><strong> &ldquo;Software distribution&rdquo;</strong> refers to distributing software in an electronic form with no tangible goods other than traditional storage media, such as a CD-ROM, packaging and documentation.</p>
      <p>Examples of &ldquo;software distribution&rdquo; where this free, open source license apply:</p>
      <ul>
        <li>1.  An individual obtains this software and installs it on an existing computer.</li>
        <li>2.  A company sells a software product which includes this software, either alone or with other software, for individuals to install on their existing computers.</li>
        <li>3.  A company or individual charges to deliver, install or configure this software on the end purchaser&rsquo;s existing equipment.</li>
      </ul>
      <p><strong> &ldquo;Hardware distribution&rdquo;</strong> refers to selling some tangible hard good, such as a computer, television or automobile, which includes this software pre-installed or installed by the selling party or one of its agents, or if the hard good is marketed with reference to the software or its features.</p>
      <p>Examples of &ldquo;Hardware distribution&rdquo; which require a separate license:</p>
      <ul>
        <li>4.  A company offers tangible merchandise that comes with the Pluto system pre-installed or bundled with the system, such as a personal computer, a television, an automobile, a point of sale system, etc.</li>
        <li>5.  A reseller, such as a smart home dealer, offers hard goods which are clearly intended to ultimately run this software, and are marketed as such.</li>
        <li>6.  A manufacturer offers a product which automatically installs this software, or includes instructions on how to do so.</li>
        <li>7.  A dealer offers a product based on Pluto, but directs his clients to purchase the required hardware from a third party.</li>
      </ul>
      <p>The purpose of examples #5, #6 and #7 are to prevent a hard goods manufacturer or dealer from attempting to circumvent the intent of this license by marketing a product which clearly benefits from this software,
	  but is not technically delivered with this software pre-installed, perhaps booting up to a &ldquo;click-here to install the software prompt&rdquo;. 
	  If a hard goods manufacturer wishes to offer a product which includes this software, or a dealer offers a turnkey solution based on this software, a separate license must be obtained.  Pluto's business model allows the company to offer
	  a commercial-grade product for free, as open source, to the open source community and any do-it-yourselfers.  This model is sustained by charging
	  a licensing fee to those who offer a commercial product based on Pluto.</p>
      <p> <strong>&ldquo;Software&rdquo;</strong> refers to any software distributed under this license, including modified versions, and third-party application programs based on software distributed under this license.</p>
      <strong>Granted rights
      </strong>      <p>1. Subject to the terms of this License, you are hereby granted a royalty-free, non-exclusive, world-wide license provided you comply with the conditions of this license. Distribution of the software, or third-party software that links with the software, in any form signifies acceptance of this license.</p>
      <p>2. You are freed of any patents claims by Pluto, Inc. or its affiliates infringed by the making, using or selling of the software provided you comply with the terms of this license.</p>
      <p>3. You may distribute the software with or without modifications provided that the entire package includes the initial developer&rsquo;s disclaimers, copyright and trademark notices. Your modifications must not alter or remove these notices.</p>
      <p>4. If you distribute modified versions of this software, you must identify your modifications and make the source code publicly available without charge for a minimum of 12 months after initial distribution. The initial developer is granted a royalty-free, non-exclusive right to distribute these modifications in future versions of the software under this license or other licenses of the initial developer.</p>
      <p>5. If you distribute this software in a machine-executable form, the distribution must include a conspicuous reference to this license and prominent instructions on where to obtain the complete source code upon which the distribution was made.</p>
      <p>6. If you develop or distribute applications which link to this software, those applications must be distributed under this license, or a license approved by the Open Source Initiative (see <a href="http://www.opensource.org/">http://www.opensource.org</a>) or the Free Software Foundation (see <a href="http://www.gnu.org/">http://www.gnu.org</a>). Otherwise written permission must be obtained from the original author.</p>
      <strong>Limitation of liability
      </strong>      <p>In no event shall the initial developers or copyright holders be liable for any damages whatsoever, even if they have been advised of the possibility of such damages, to the extent provided by law.</p>
      <strong>No warranty
      </strong>      <p>The Software and this license document are provided &ldquo;as is&rdquo; with no warranty of any kind, either express or implied, including warranties that the software is free of defects, merchantable, fit for a particular purpose or non-infringing.</p>
      <strong>Choice of law
    </strong>      <p>This license is governed by the laws of the State of Florida, USA. If any part of this license is found void and unenforceable, it will not affect the validity of the balance of the license, which shall remain valid and enforceable according to its terms.</p></td>
  </tr>
</table>
	
TEXT;

	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview','Accesories'=>'index.php?section=accesories'));
		
 	$output->setTitle(APPLICATION_NAME." :: Public License");			//page title
	$output->setBody($out);		
  	$output->output(); 
 }
?>
