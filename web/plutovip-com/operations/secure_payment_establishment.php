<?
function secure_payment_establishment($output)
{
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}

	$queryEstablishment=dbQuery("SELECT * FROM Establishment WHERE FK_MasterUsers='".$_SESSION['userID']."'");
	if(mysql_num_rows($queryEstablishment)!=0)
		$resEstablishment=mysql_fetch_object($queryEstablishment);
	$pmtnetChecked=explode(",",$resEstablishment->pmtnet);
	$gatewayChecked=explode(",",$resEstablishment->gateway);
	
	$out.="		
		<script language=\"javascript\" type=\"text/javascript\">

		function chosetype()
		{
			var selval = document.getElementById('dd_typeproc').value;
			
			document.getElementById('spProc1').style.display = 'none';
			document.getElementById('spProc2').style.display = 'none';
			document.getElementById('spProc3').style.display = 'none';

			var spProc = document.getElementById('spProc' + selval);
			if (spProc) spProc.style.display = '';
			
			if (selval==1 || selval==2 || selval==3)
			{
				document.getElementById('spDial123').style.display = '';
			}
			else document.getElementById('spDial123').style.display = 'none';
		}

		function chosepc()
		{
			document.getElementById('div_otherpc4').style.display = (document.getElementById('dd_typepc').value==4) ? '' : 'none';
		}

		function chosepcproc()
		{
			document.getElementById('div_otherpcproc8').style.display = (document.getElementById('dd_howpcproc').value==8) ? '' : 'none';
		}

		function collectData()
		{
			var i;
			var Result='';
  			Result='';
  			var hasitem = false;
  
  			for(i = 1; i<999; i++)
  			{
    			var o=document.getElementById('cbpn_' + i);
    			if( o && o.checked )
    			{
					if (hasitem)
	    				Result += ',';
					Result += i;
      				hasitem = true;
    			}
  			}

  			if (!hasitem)
  				Result = '*null*';

        document.getElementById('pmtnet').value=Result;

  			hasitem = false;
  
  			Result='';
        for(i = 1; i<999; i++)
  			{
    			var o=document.getElementById('cbgw_' + i);
    			if (o && o.checked)
    			{
					if (hasitem)
	    				Result += ',';
					Result += i;
      				hasitem = true;
    			}
  			}
  			if (!hasitem)
  				Result = '*null*';
  
  			document.getElementById('gateway').value=Result;
		}

	</script>";
	
	$out.='
 	<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
		<tr>
			<td align="left" class="insidetable"><img src="images/submenu_txt/secure_payments_txt.gif" width="233" height="14"></td>
		</tr>
		<tr>
   			<td valign="top" align="left" class="insidetable">
	
		<form action="index.php?section=secure_payment_establishment" method="POST" name="form1">
			<input type="hidden" name="gateway" id="gateway" value="">
			<input type="hidden" name="pmtnet" id="pmtnet" value="">
			<table border="0" cellpadding="2" cellspacing="2" width="100%" align="left" class="maintable">
				<tr>
					<td valign="top" align="left">You can start using the VIP recognition, member card, and loyalty programs immediately.<br><br>
	
			Please tell us how you process credit card payments so that we can confirm the right instructions for your particular combination.  
			We will send you an email with simple instructions for your setup soon.
			Try to be as accurate as possible so that we can give you clear instructions for your configuration. 
			That way it will only take you a few minutes to get setup and running!<br><br></td>
				</tr>
	  			<tr>
	    			<td valign="top" align="left">
	      				How do you process credit card payments?<br>
	      				<select size="1" name="dd_typeproc" onchange="chosetype();">
	      					<option value="0">(please select)</option>
	      					<option value="1" '.(($resEstablishment->dd_typeproc==1)?'selected':'').'>A cash register with credit card processing built-in</option>
	      					<option value="2" '.(($resEstablishment->dd_typeproc==2)?'selected':'').'>A separate "box" not built into my cash register</option>
	      					<option value="3" '.(($resEstablishment->dd_typeproc==3)?'selected':'').'>A pc computer based system</option>
	      					<option value="4" '.(($resEstablishment->dd_typeproc==4)?'selected':'').'>We call in for an authorization manually</option>
	      				</select>
	    			</td>
	  			</tr>
	  			<tr>
	    			<td id="procChoices" valign="top" align="left">
	    				<span id="spProc1" style="'.(($resEstablishment->dd_typeproc==1)?'':'display:none').'">
	    					What is the make and model of your cash register?&nbsp;
	    					<input type="text" name="makereg" value="'.$resEstablishment->makereg.'" size="30">
	    				</span>
	    				<span id="spProc2" style="'.(($resEstablishment->dd_typeproc==2)?'':'display:none').'">
	    					What is the make and model of that "box", or credit card terminal?&nbsp;
	    					<input type="text" name="maketerm" value="'.$resEstablishment->maketerm.'" size="30">
	    				</span>
	    				<span id="spProc3" style="'.(($resEstablishment->dd_typeproc==3)?'':'display:none').'">
	    					What type of PC? 
	    					<select size="1" name="dd_typepc" onchange="chosepc();">
	    						<option value="0">(please select)</option>
	    						<option value="1" '.(($resEstablishment->dd_typepc==1)?'selected':'').'>Windows</option>
	    						<option value="2" '.(($resEstablishment->dd_typepc==2)?'selected':'').'>Linux/Unix</option>
	    						<option value="3" '.(($resEstablishment->dd_typepc==3)?'selected':'').'>DOS</option>
	    						<option value="4" '.(($resEstablishment->dd_typepc==4)?'selected':'').'>Other</option>
	    						<option value="5" '.(($resEstablishment->dd_typepc==5)?'selected':'').'>I don\'t know</option>
	    					</select><br>
	    					<div id="div_otherpc4" style="'.(($resEstablishment->dd_typeproc==4)?'':'display:none').'">
	    						&bull; Please specify what type of PC: <input type="text" name="typepc" value="'.$resEstablishment->typepc.'" size="30">
	    					</div>
	    					What is the make and model of your POS software?
	    					<input type="text" name="makepossoft" size="30" value="'.$resEstablishment->makepossoft.'"><br>
	    					How does it process the credit cards?&nbsp; 
	    					<select size="1" name="dd_howpcproc" onchange="chosepcproc();">
	    						<option value="0">(please select)</option>
	    						<option value="1" '.(($resEstablishment->dd_howpcproc==1)?'selected':'').'>Credit card processing is built-in to the POS software</option>
	    						<option value="2" '.(($resEstablishment->dd_howpcproc==2)?'selected':'').'>ICVerify</option>
	    						<option value="3" '.(($resEstablishment->dd_howpcproc==3)?'selected':'').'>PCAuthorize</option>
	    						<option value="4" '.(($resEstablishment->dd_howpcproc==4)?'selected':'').'>WebAuthorize</option>
	    						<option value="5" '.(($resEstablishment->dd_howpcproc==5)?'selected':'').'>PCCharge</option>
	    						<option value="6" '.(($resEstablishment->dd_howpcproc==6)?'selected':'').'>rita Server</option>
	    						<option value="7" '.(($resEstablishment->dd_howpcproc==7)?'selected':'').'>Java Card</option>
	    						<option value="8" '.(($resEstablishment->dd_howpcproc==8)?'selected':'').'>Other</option>
	    						<option value="9" '.(($resEstablishment->dd_howpcproc==9)?'selected':'').'>I don\'t know</option>
	    					</select><br> 
	    					<div id="div_otherpcproc8">
	    						&bull; Please specify what PC software: <input type="text" name="typepcsoft" value="'.$resEstablishment->typepcsoft.'" size="30">
	    					</div>
	    				</span>
	    			</td>
	  			</tr>
	  			<tr>
	    			<td id="dialChoices" valign="top" align="left">
	    				<span id="spDial123" style="display:none">
	    					How does your system get the credit authorization? 
	    					<select size="1" name="dd_dial">
	    						<option value="0">(please select)</option>
	    						<option value="1" '.(($resEstablishment->dd_dial==1)?'selected':'').'>It dials using a regular phone line</option>
	    						<option value="2" '.(($resEstablishment->dd_dial==2)?'selected':'').'>It does it over the internet</option>
	    						<option value="3" '.(($resEstablishment->dd_dial==3)?'selected':'').'>A special leased-line</option>
	    						<option value="4" '.(($resEstablishment->dd_dial==4)?'selected':'').'>I don\'t know</option>
	    					</select>
	    				</span>
	    			</td>
	  			</tr>
	  			<tr>
	    			<td id="Settlement" valign="top" align="left">How do you handle settement? 
	      				<select size="1" name="dd_settle">
	      					<option value="0">(please select)</option>
	      					<option value="1" '.(($resEstablishment->dd_settle==1)?'selected':'').'>We have to settle (or close-out) each terminal (batch process)</option>
	      					<option value="2" '.(($resEstablishment->dd_settle==2)?'selected':'').'>Our payment processor does this automatically.</option>
	      					<option value="3" '.(($resEstablishment->dd_settle==3)?'selected':'').'>I don\'t know</option>
	      				</select>
					</td>
	  			</tr>
	  			<tr>
	    			<td id="bankingChoices" valign="top" align="left">
						What is the name of the bank where you have your merchant account? <input type="text" name="bankname" size="30" value="'.$resEstablishment->bankname.'"><br>
	    				<br>
	    				What processing network or gateway do you use.&nbsp; Check all that apply.&nbsp; 
	    				Sometimes the same bank uses 
	    				different networks, so we have to ask:<br>
	    				<br>
	    				<input type="hidden" name="cbDontKnowProcessing" >
	      				<input type="checkbox" '.(($resEstablishment->cbDontKnowProcessing==1)?'checked':'').' onclick="document.getElementById(\'cbDontKnowProcessing\').value=this.checked?\'1\':\'0\';" />
	      				I don\'t know, please call my bank and ask.&nbsp; If so, do you have a name and phone number? 
	      				<input type="text" name="bankcontact" value="'.$resEstablishment->bankcontact.'" size="30"><br>
						&nbsp;
						<table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" id="AutoNumber2" width="100%">
	      					<tr>
	        					<td>Processing Networks:</td>
	        					<td>Payment Gateways:</td>
	      					</tr>
	      					<tr>
	        					<td valign="top">';
									
									$queryNetworks=dbQuery("SELECT PKID_ProcessingNetwork,Description FROM ProcessingNetwork");
									while($resNetworks=mysql_fetch_object($queryNetworks)){
										$out.='<input type="checkbox" name="cbpn_'.$resNetworks->PKID_ProcessingNetwork.'" '.((in_array($resNetworks->PKID_ProcessingNetwork,$pmtnetChecked))?'checked':'').'>'.$resNetworks->Description.'<br/>';
									}
								$out.='	Other: <input type="text" name="pmtnet_other" value="'.$resEstablishment->pmtnet_other.'" size="20"><br>
								</td>
	        					<td valign="top">';
								$queryGateways=dbQuery("SELECT PKID_PaymentGateway,Description FROM PaymentGateway");
									while($resGateways=mysql_fetch_object($queryGateways)){
										$out.='<input type="checkbox" name="cbgw_'.$resGateways->PKID_PaymentGateway.'" '.((in_array($resGateways->PKID_PaymentGateway,$gatewayChecked))?'checked':'').'>'.$resGateways->Description.'<br/>';
									}
	
								$out.='Other: <input type="text" name="gateway_other" value="'.$resEstablishment->gateway_other.'" size="20"><br>
	    						</td>
	      					</tr>
	    				</table>		
						<input type="submit" name="saveBtn" value="Update">
	    			</td>
	  			</tr>
			</table>
		</form>

		</td>
	</tr>
</table>';
		
if(isset($_POST["saveBtn"]))
{
	mysql_data_seek($queryNetworks,0);
	$pmtnet=array();
	while($resNetworks=mysql_fetch_object($queryNetworks)){
		if(isset($_POST['cbpn_'.$resNetworks->PKID_ProcessingNetwork]))
			$pmtnet[]=$resNetworks->PKID_ProcessingNetwork;
	}

	mysql_data_seek($queryGateways,0);
	$gateway=array();
	while($resGateways=mysql_fetch_object($queryGateways)){
		if(isset($_POST['cbgw_'.$resGateways->PKID_PaymentGateway]))
			$gateway[]=$resGateways->PKID_PaymentGateway;
	}
	
	
	$upd=dbQuery("UPDATE Establishment SET 
			dd_typeproc='".$_POST['dd_typeproc']."', 
			bankcontact='".$_POST['bankcontact']."',
			makereg='".$_POST['makereg']."', 
			maketerm='".$_POST['maketerm']."',
			typepc='".$_POST['typepc']."', 
			typepcsoft='".$_POST['typepcsoft']."',
			cbDontKnowProcessing='".$_POST["cbDontKnowProcessing"]."',
			dd_dial='".$_POST['dd_dial']."',
			dd_typepc='".$_POST['dd_typepc']."',
			dd_howpcproc='".$_POST['dd_howpcproc']."',
			pmtnet='".implode(",",$pmtnet)."',
			gateway='".implode(",",$gateway)."',
			dd_settle='".$_POST['dd_settle']."', 
			makepossoft='".$_POST[' makepossoft']."', 
			gateway_other='".$_POST['gateway_other']."',
	 		pmtnet_other='".$_POST['pmtnet_other']."',
			bankname='".$_POST['bankname']."'
	WHERE (FK_MasterUsers='".$_SESSION['userID']."')");

	Header("Location: index.php?section=secure_payment_establishment");
	exit(0);
}
  
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
