<?
function referral_summary ($output)
{
	global $checkReferrerUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}

	$refSummary=getReferrerSummary($_SESSION['userID'],$checkReferrerUrl);
	
	$out.='
  <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  	<tr>
	     <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
		'.$refSummary.'	
	  </td>
	 </tr>
  </table>';

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->referIMG = 'images/home%20page%20img/a_refer_business_on.gif';
	$output->setImgName("img.jpg");
	$output->setTitle(APPLICATION_NAME."::Refer for business");
	$output->setPageID(98); //butonul selectat
	$output->output();
}
?>