<?php
 function schedule($output) {
	$sch=$_REQUEST['sch'];

	$leftMenu='
		<div style="position:relative; top=0px; float:left; width=250px;">
		';
		include('dealer/dealerMenu.php');
		$leftMenu.=dealerMenu();
		$leftMenu.='
		</div>';
	
	
	switch ($sch){
		case 1:
			$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Schedule 1'=>'index.php?section=schedule&sch=1'));
		$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer.gif"></td>
      </tr>
    </table></td>
  </tr>
		<tr class="insidetable2">
			<td class="title" align="center"><B>SCHEDULE 1 – Pluto Program Requirements</B></td>
		</tr>
		<tr>
		  <td width="100%" class="insidetable2" align="left">
			'.$leftMenu.'
			<p>Dealer is expected to meet, at a minimum, the following program requirements in order to be considered for authorization by Pluto Home as a Dealer of Pluto Products:
			<ul>
				<li>Authorized Agreement with Pluto Home Co. in good standing</li>
				<li>Notify Pluto of any changes to support or return policies</li>
			</ul>
			<p><B>The following requirements are for Pluto Pro dealers only</B>:
			<ul>
				<li>Get a demo system up and running within 90 days of acceptance.  This does not require purchasing anything from Pluto Home.  The dealer is free to use either Pluto Home’s equipment, or compatible off-the-shelf equipment.  However, the demo system must consist of a minimum of: 1 Core, 1 Media Director, 1 Orbiter, 1 Mobile Orbiter, and 1 Telephone Extension.  It must interface with A/V equipment, and it is highly recommended that it interface with lighting, security and surveillance systems also.</li>
				<li>Pluto must verify that the demo system is properly setup and configured.  This can be done remotely, however, Pluto also offers on-site training.  Pluto will not begin making referrals until this has been completed.</li>
				<li>It is important that potential clients be able to see the product to understand its capabilities.  Dealer must maintain the capability to demonstrate the product to potential clients.  This can be accomplished in a dealer show room, or in a demo home, or in the home of a client who is willing to allow reasonable demonstrations for serious clients.</li>
				<li>Provide first level technical support pre and post sales to regional customer base</li>
				<li>Perform warranty and support contract entitlement checking</li>
				<li>Provide configuration and trouble shooting assistance to customers</li>
			</ul>
			<p><B>Pluto Program Entitlements</B>:
			<ul>
				<li>Opportunity to participate in market development activities for new and existing products</li>
				<li>Pluto Dealer logo usage</li>
				<li>Participation in the Pluto Marketing Program (reimbursement of marketing funds after specific sales volumes are reached)</li>
				<li>Access to Pluto Level II technical support personnel</li>
			</ul>
			<p><B>Pluto Sales Volume and Accreditation Levels</B>:

			<B>Pluto Authorized Dealer</B>- Planet Club: 0-5 complete installations per year<br>
			<B>Pluto Authorized Dealer</B>- Galaxy Club: 5-20 complete installations per year, or a minimum of $6,000 in licensing fees if dealer uses non-Pluto equipment.  Eligible for Marketing reimbursement, up to $2,000 per year<br>
			<B>Pluto Authorized Dealer</B>- Universe Club: 20+ complete installations per year, or a minimum of $30,000 in licensing fees if dealer uses non-Pluto equipment.  Eligible for Marketing reimbursement, up to $10,000 per year

		  </td>
		</tr>
		</table>
	';
		break;
		case 2:
			$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Schedule 2'=>'index.php?section=schedule&sch=2'));
		$out='
		<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
		<tr class="insidetable2">
			<td class="title" align="center"><B>SCHEDULE 2 – Pluto Platform Marketing Program</B></td>
		</tr>
		<tr>
		  <td width="100%" class="insidetable2" align="left">
		'.$leftMenu.'		
		
			<p>The Pluto Platform Marketing Program provides marketing support to Dealers on a case-by-case basis and at the sole discretion of Pluto Home. 
			<p>As a <I>Pluto Authorized Dealer</I>, marketing support will be based on the volume of purchases of Pluto Products.  Marketing reimbursements may be up to 1/3 of your spending with Pluto Home, as explained in Schedule 1. Pluto Home marketing funds may be used for a variety of marketing activities that are pre-approved and meet the requirements of these Guidelines.

			<p><B>A wide range of activities qualify</B>

			<p>Pluto Platform Marketing Program support and marketing funds may be used for specified and pre-approved activities, i.e.:
			<ul>
				<li>Print advertising—Yellow Pages, newspaper and magazine ads</li>
				<li>Radio and television advertising</li>
				<li>Direct mail pieces</li>
				<li>Outdoor advertising</li>
				<li>Tradeshows, seminars, road shows</li>
				<li>Telemarketing programs</li>
				<li>Web advertising</li>
				<li>Pluto literature</li>
				<li>Pluto Promotional items</li>
				<li>Pluto training</li>
				<li>Pluto Demo equipment</li>
			</ul>
			<p>Pluto Home reserves the right to exercise discretion in approving funds and marketing activities.
			<p><B>Simple approval and reimbursement process</B>
			<p>The approval and reimbursement process is simple, but both steps are essential. Following the procedure exactly reduces the chance of confusion when you request funding upon completion of a marketing activity.

			<p><B>1. Obtain prior approval</B>
			<p>All marketing activities must be approved for funding by Pluto Home Marketing before they begin. Requests for funding must be submitted four weeks before the activity’s initiation. Complete the upper portion of the Marketing Funds Request Form and send by facsimile or e-mail to Pluto Home Marketing Manager.

			<p>Be sure to include the required back-up documentation as outlined in the Frequently Asked Questions section. Pluto Home Marketing will try to respond to your request within five business days.
			<p><B>2. Apply for reimbursement</B>
			<p>Once your pre-approved marketing activity is complete, you may apply for reimbursement. Complete the Marketing Reimbursement Claim Form and send it along with all relevant paid invoices from your supplier(s) and back-up documentation (see the Frequently Asked Questions section below) to your Pluto Home Marketing Manager:
			<p>Allow four to six weeks for receipt of your reimbursement check.

			<p><B>Frequently Asked Questions:</B>
			<p><B><I>What types of documentation should be submitted with my Request Form?</I></B>
			<ul>
				<li>Print advertising – Include a copy of the ad to be placed and a schedule of ad-run dates and publications.</li>
				<li>Broadcast advertising – Include a copy of the proposed radio or television script (and storyboard for television) with a run schedule and/or list of stations you plan to use. As a general rule, the Pluto name should be mentioned at least twice in any ad copy.</li>
				<li>Outdoor advertising – Include a drawing or picture of the proposed billboard, transit sign, etc. The Pluto logo must be prominently displayed.</li>
				<li>Direct-mail programs – Include a copy of each component of the mailing, and an estimate or exact figure of postal costs. If the pieces are being developed by a marketing/advertising agency with Pluto as the featured product, it is advisable to involve Pluto Home Marketing during the design stage.</li>
				<li>Trade shows, seminars, and events – Include a diagram of the layout of the booth or room indicating Pluto coverage, a detailed plan of the event including target audience, and samples of materials. Provide details of other vendors being represented.</li>
				<li>Telemarketing – Include a statement of the purpose of your telemarketing campaign with a description of the target market, and a copy of the proposed script with a completed call information sheet. If an outside vendor is conducting the telemarketing campaign, please attach copies of all documentation received from that vendor.</li>
				<li>Web advertising – Include a printout of the proposed ad and the URL where it will be posted.</li>
				<li>Promotional items/premiums – Include a sample of each item, and a sketch showing the placement of the Pluto logo and Dealer logos.</li>
			</ul>
			<p><B><I>What type of documentation should be submitted with my Claim Form for reimbursement?</I></B>

			<p>All Claim Forms MUST be accompanied by copies of paid invoices from your supplier(s). You must also include the following:
			<ul>
				<li>Print advertising – Submit an original full-page tear sheet with the ad and the date of publication.</li>
				<li>Broadcast advertising – Submit a copy of the script with notarized affidavit of performance from the radio or television station, along with the ad on audio or videotape. A sales representative at the radio or television station can provide these items at your request.</li>
				<li>Telemarketing – Submit detailed call records.</li>
			</ul>
			<p><B>What activities are not eligible for reimbursement?</B>
			<ul>
				<li>The following expenses or activities are not reimbursable:</li>
				<li>Pluto System product purchases</li>
				<li>Customer entertainment</li>
				<li>Golf tournaments</li>
				<li>Claims without proper back-up documentation</li>
				<li>Agency hourly fees and placement fees</li>
				<li>Travel, lodging and related expenses</li>
				<li>Alcoholic beverages</li>
				<li>Advertising that features a competitive company* or product logo along with the Pluto logo.</li>
			</ul>

		<p>*A company or product is considered competitive if it competes with any product of Pluto Home including the Products in question, or if the company manufacturing the product has other products that compete with any or all those by Pluto Home 
		  </td>
		</tr>
		</table>
	';
		
		break;
		case 3:
			$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Schedule 3'=>'index.php?section=schedule&sch=3'));
		$out='
		<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
		<tr class="insidetable2">
			<td class="title" align="center"><B>SCHEDULE 3 - Dealer Support Services Roles & Responsibilities – <I>Pluto Pro Only</I></B></td>
		</tr>
		<tr>
		  <td width="100%" class="insidetable2" align="left">
			'.$leftMenu.'		
			<p>The Pluto Platform Marketing Program provides marketing support to Dealers on a case-by-case basis and at the sole discretion of Pluto Home. 
			<p><B>Support Roles and Responsibilities</B>
			<p>This document describes the responsibilities of Pluto Home and the Pluto Pro Authorized Dealer for the provision of support services for the Pluto System Products.
			<p>The Authorized Dealer is responsible for organizing first line support for their customers, including corporate entities and individual end users, for all Pluto Products and associated Dealer products, if applicable. 
			<p>To demonstrate these capabilities, the dealer is required to setup a demo system as outlined earlier within 90 days of accreditation.
 			<p>Pluto Home is responsible for organizing second line support for the Dealer only.

			<p><B>1. Pluto Second-Line Support Specification</B>
			<p>The Authorized Dealer is responsible for reporting the deliveries of all Pluto products and the amounts and the configurations of the Pluto installation.  This process can be fully automated.  

			<p><B>2. Dealer Responsibilities</B>
			<ul>
				<li>Authorized Dealer shall use reasonable efforts to carry out the recommended first line operation and maintenance of the Pluto System and seek to remedy all faults that can be reasonably remedied. When expert assistance is needed from Pluto Home the Authorized Dealer will still act as the point of contact for exchange of information to the end customer.</li>
				<li>Authorized Dealer shall keep an operational logbook and record of faults.</li>
				<li>Authorized Dealer shall use reasonable efforts to ensure that their maintenance personnel are sufficient in number and are of an adequate level of competence. Pluto Home may recommend necessary training to ensure this competence.</li>
				<li>Authorized Dealer shall coordinate reasonable access by Pluto Home engineers to any location experiencing difficulty if the problem cannot be resolved by the dealer.  This can be electronic access, such as an incoming internet connection, or physical access if necessary.  It is essential for both Pluto Home and dealer that any customer complaints or issues be resolved as quickly as possible.</li>
				<li>Authorized Dealer is responsible for ensuring that its customers are responsible for the use of the Products, including operating procedures, audit controls, accuracy and security of input and output data, restart and recovery routines and other procedures necessary for their intended use of the Products.</li>
			</ul>
			<p>In addition to the service and support requirements, there are a number of ‘bench marks’ that <I>Pluto Authorized Dealer</I> should be working towards:
			<ul>
				<li>In-house problem resolution rate of 80%</li>
				<li>Maintain customer problem tracking system to identify customers, product serial numbers, support coverage, warranty status</li>
				<li>Track fault and support calls, escalate and document problems and resolutions</li>
				<li>Maintain adequate lab systems to simulate customer problems</li>
				<li>Maintain adequate parts sparing to fulfill critical customer needs</li>
			</ul>
		  </td>
		</tr>
		</table>
	';
		
		break;
		
	}
	$output->setPressedButton(5);
	$output->setTitle(APPLICATION_NAME."::Dealer Program Guidelines");			//page title
	$output->setBody($out);
	$output->output();
 }
?>