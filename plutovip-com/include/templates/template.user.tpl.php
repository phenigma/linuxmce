<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
 <link rel="stylesheet" type="text/css" href="styles/style.css">
<title>{Title}</title>
<script language="JavaScript"><!--
function open_popup(page,w,h) {
window.open(page,'PopupWindow','width='+w+',height='+h+',resizable=no,statusbar=no,scrollbars=yes');
}
//--></script>


		
 {CssFile}
 {JavaScripts}
 
<style type="text/css">
<!--
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #A1A7BD;
}
-->
</style>
{ScriptInHead}
<script language="javascript" type="text/javascript">
		function select_phonemake(pmake)
		{
			pmodel = document.getElementById('phonemodel');
			while (pmodel.length > 1) pmodel.remove(pmodel.length-1); // leave 'other'
			if (pmake.value=='0')
			{
				document.getElementById('phonemakeother').style.visibility = 'visible';
			}
			else
			{
				document.getElementById('phonemakeother').style.visibility = 'hidden';
				if (navigator.appName == 'Microsoft Internet Explorer')
				{
					switch (pmake.value)
					{
													case '2':
								pmodel.add(new Option('cdmaOne C413S',11));pmodel.add(new Option('P800',2));pmodel.add(new Option('P80x',14));pmodel.add(new Option('P900',3));pmodel.add(new Option('P90x',15));pmodel.add(new Option('R520m or R520mc',12));pmodel.add(new Option('T39m or T39mc',13));pmodel.add(new Option('T610',5));pmodel.add(new Option('T616',6));pmodel.add(new Option('T618',17));pmodel.add(new Option('T630',7));pmodel.add(new Option('T68i',4));pmodel.add(new Option('Z600',16));pmodel.add(new Option('Z600',18));								break;
														case '3':
								pmodel.add(new Option(' 8910i',26));pmodel.add(new Option('3600',19));pmodel.add(new Option('3620',20));pmodel.add(new Option('3650',9));pmodel.add(new Option('3660',21));pmodel.add(new Option('6310i',8));pmodel.add(new Option('6600',22));pmodel.add(new Option('6650',23));pmodel.add(new Option('6820',24));pmodel.add(new Option('7600',25));								break;
														case '5':
								pmodel.add(new Option('T280i',27));pmodel.add(new Option('Timeport 270',30));pmodel.add(new Option('Timeport 270',28));pmodel.add(new Option('V880',31));pmodel.add(new Option('Wireless Handset',29));								break;
														case '6':
								pmodel.add(new Option('X70',32));								break;
														case '7':
								pmodel.add(new Option('Bluetooth PCS Phone',33));								break;
														case '8':
								pmodel.add(new Option('M11',34));pmodel.add(new Option('PALDIO 633S',35));								break;
												}
				}
				else
				{
					switch (pmake.value)
					{
													case '2':
								pmodel.add(new Option('cdmaOne C413S',11),null);pmodel.add(new Option('P800',2),null);pmodel.add(new Option('P80x',14),null);pmodel.add(new Option('P900',3),null);pmodel.add(new Option('P90x',15),null);pmodel.add(new Option('R520m or R520mc',12),null);pmodel.add(new Option('T39m or T39mc',13),null);pmodel.add(new Option('T610',5),null);pmodel.add(new Option('T616',6),null);pmodel.add(new Option('T618',17),null);pmodel.add(new Option('T630',7),null);pmodel.add(new Option('T68i',4),null);pmodel.add(new Option('Z600',16),null);pmodel.add(new Option('Z600',18),null);								break;
														case '3':
								pmodel.add(new Option(' 8910i',26),null);pmodel.add(new Option('3600',19),null);pmodel.add(new Option('3620',20),null);pmodel.add(new Option('3650',9),null);pmodel.add(new Option('3660',21),null);pmodel.add(new Option('6310i',8),null);pmodel.add(new Option('6600',22),null);pmodel.add(new Option('6650',23),null);pmodel.add(new Option('6820',24),null);pmodel.add(new Option('7600',25),null);								break;
														case '5':
								pmodel.add(new Option('T280i',27),null);pmodel.add(new Option('Timeport 270',30),null);pmodel.add(new Option('Timeport 270',28),null);pmodel.add(new Option('V880',31),null);pmodel.add(new Option('Wireless Handset',29),null);								break;
														case '6':
								pmodel.add(new Option('X70',32),null);								break;
														case '7':
								pmodel.add(new Option('Bluetooth PCS Phone',33),null);								break;
														case '8':
								pmodel.add(new Option('M11',34),null);pmodel.add(new Option('PALDIO 633S',35),null);								break;
												}
				}
			}
			select_phonemodel(pmodel);
		}
		function select_phonemodel(pmodel)
		{
			document.getElementById('phonemodelother').style.visibility = (pmodel.value=='0') ? 'visible' : 'hidden';
		}

		</script>
</head>
<body {ScriptInBody}>


<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" bgcolor="#5C6C93"><table width="900"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="100"><a href="index.php?section=settings"><img src="{settingsIMG}" width="100" height="30" border="0"></a></td>
        <td width="119"><a href="index.php?section=place"><img src="{placesIMG}" width="119" height="30" border="0"></a></td>
        <td width="105"><a href="index.php?section=contact_vip"><img src="{contactIMG}" width="105" height="30" border="0"></a></td>
        <td width="128"><a href="index.php?section=refer_a_business"><img src="{referIMG}" width="128" height="30" border="0"></a></td>
        <td width="105"><a href="index.php?section=instructions"><img src="{instructionsIMG}" width="105" height="30" border="0"></a></td>
        <td>&nbsp;</td>
        <td width="61"><a href="index.php?section=userLogout"><img src="images/home%20page%20img/a_logout.gif" width="61" height="30" border="0"></a></td>
        <td width="85"><a href="index.php?section=news1"><img src="images/home%20page%20img/a_home.gif" width="85" height="30" border="0"></a></td>
      </tr>
    </table></td>
  </tr>
  
  <tr>
    <td align="center" background="images/home page img/a_back.jpg"><table width="900"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="68"><a href="index.php?section=settings"><img src="images/home%20page%20img/a_settings.jpg" width="68" height="21" border="0"></a></td>
        <td width="128"><a href="index.php?section=info_business"><img src="images/home%20page%20img/a_info_business.jpg" width="128" height="21" border="0"></a></td>
        <td width="111"><a href="index.php?section=secure_payment"><img src="images/home%20page%20img/a_secure_payments.jpg" width="111" height="21" border="0"></td>
        <td width="117"><a href="index.php?section=change_pass"><img src="images/home%20page%20img/a_change_pass.jpg" width="117" height="21" border="0"></a></td>
        <td width="94"><a href="index.php?section=change_email"><img src="images/home%20page%20img/a_change_email.jpg" width="94" height="21" border="0"></a></td>
        <td width="108"><a href="index.php?section=credit"><img src="images/home%20page%20img/a_my_credit_card.jpg" width="108" height="21" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
    </table></td>
  </tr>
</table></td>
  </tr>
  <tr>
    <td align="center"><table width="900" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td bgcolor="#FFFFFF"><table width="900" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="149"><a href="index.php"><img src="images/logo.jpg" width="149" height="228" border="0"></td>
            <td><object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="751" height="229">
              <param name="movie" value="flash/default_home.swf">
              <param name="quality" value="high">
              <embed src="flash/default_home.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="751" height="229"></embed>
            </object></td>
          </tr>
        </table></td>
      </tr>
      <tr>
	  <td>{Body}</td>
      </tr>
      <tr>
        <td bgcolor="#FFFFFF">&nbsp;</td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="center" background="images/back_footer.jpg"><table width="900" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="124"><img src="images/copyright.gif" width="142" height="34"></td>
        <td width="88"><a href="index.php?section=privacy_policy"><img src="images/privacy.gif" width="86" height="34" border="0"></a></td>
        <td width="61"><a href="index.php?section=contact"><img src="images/contact.gif" width="71" height="34" border="0"></a></td>
        <td width="71"><img src="images/tel.gif" width="86" height="34"></td>
        <td>&nbsp;</td>
        <td width="115"><a href="http://www.newflavorstudio.com" target=_blank alt="New Flavor Studio"><img src="images/nfs.gif" width="114" height="34" border="0"></a></td>
      </tr>
    </table></td>
  </tr>
</table>
<map name="Map">
  <area shape="rect" coords="20,3,88,30" href="index.php">
</map>
</body>
</html>