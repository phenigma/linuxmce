<?
function settings($output)
{
	global $updateMasterUserUrl, $states;
  $out='';
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
  
  $queryShareWithVipPN=dbQuery("SELECT PKID_C_Field FROM C_Field WHERE Name='PKID_PhoneNumber'");
  if(mysql_num_rows($queryShareWithVipPN)!=0){
  	$resShareWithVipPN=mysql_fetch_object($queryShareWithVipPN);
  	$PhoneNumberFKID_C_Field = $resShareWithVipPN->PKID_C_Field;
  }
  $out.='
  <script>
  var BinaryVersionArray= new Array();
  function setBinaryVersion()
  {
  	document.getElementById(\'binaryVersion\').innerHTML=BinaryVersionArray[document.form1.Users_FKID_PhoneModel.selectedIndex];
  }
  </script>
<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <form action="index.php?section=settings" method="POST" name="form1" enctype="multipart/form-data">
		<input type="hidden" name="action" value="">


  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
  <img src="images/submenu_txt/settings_txt.gif" width="172" height="14"><br><br>
  <div align="left">
			At a minimum you must put in your name, which the businesses will see when you enter.  
			It does not need to be your real name, however.  You can put in a \'Nick Name\' and leave your real name blank.  
			Business will automatically see whatever information you mark as \'share with businesses\'.  
			If you do not check \'share with businesses\', the business will only be able to see the information if you confirm that request on your mobile phone.  
			Other VIP\'s who are at the same place at around the same time as you will be able to login and see whatever information you mark as \'share with other vip\'s\'.
			If you do not mark any information, then they will never be able to see you.  If you\'re looking to meet new people, it\'s a great way to make yourself more available, but at the same time without revealing any personal information at all.	
		</div>';
   $sql1="select * from Users where FK_MasterUsers='".$_SESSION['userID']."'";
   $r1=mysql_query($sql1) or die("Can not grab from database ".mysql_error());
   $row1=mysql_fetch_object($r1);
   $sql2="select * from PhoneModel where PKID_PhoneModel='".$row1->FKID_PhoneModel."'";
   $r2=mysql_query($sql2) or die("Can not grab from database ".mysql_error());
   $row2=mysql_fetch_object($r2);
   $out.='
   <table align="center" border="0" cellpadding="4" cellspacing="0">
			<tr>
				<td colspan="2">&nbsp;</td>
				<th title="Share with VIPs or Businesses" colspan="2">Share with</td>
			</tr>
			<tr>
				<td align="right"><b>Pluto ID</b><br>Pluto ID is a unique combination and represents your identity for PlutoVIP phone application. You need to provide this ID when you\'ll install the software on your phone.</td>
				<td valign="top">'.(($_SESSION['categ']==0)?'PV':'PE').$_SESSION['PlutoId'].$_SESSION['Pin'].'</td>
				<td><font size="-2">Other VIPs</font></td>
				<td><font size="-2">Businesses</font></td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Username</th>
				<td><input type="text" name="MasterUsers_Username" value="'.$_SESSION['username'].'" /></td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<th align="right">E-mail</th>
				<td>		<style type="text/css">
		.email 
		{
			color:#00A;
		}
		</style>
		<span class="email">'.$_SESSION['Email'].'</span></td>';
      $sh="select * from C_Field where Name='Email'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
      $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
       $share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Birth Date</th>
				<td>
				<select name="b_month"><option value="0">(select)</option><option value="1"'; if($row1->BirthDate[5].$row1->BirthDate[6]=="01") $out.='selected="selected"'; $out.=' >January</option><option value="2" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="02") $out.='selected="selected"'; $out.='>February</option><option value="3" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="03") $out.='selected="selected"'; $out.='>March</option><option value="4"'; if($row1->BirthDate[5].$row1->BirthDate[6]=="04") $out.='selected="selected"'; $out.='>April</option><option value="5" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="05") $out.='selected="selected"'; $out.='>May</option><option value="6" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="06") $out.='selected="selected"'; $out.='>June</option><option value="7" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="07") $out.='selected="selected"'; $out.='>July</option><option value="8" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="08") $out.='selected="selected"'; $out.='>August</option><option value="9"'; if($row1->BirthDate[5].$row1->BirthDate[6]=="09") $out.='selected="selected"'; $out.='>September</option><option value="10" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="10") $out.='selected="selected"'; $out.='>October</option><option value="11" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="11") $out.='selected="selected"'; $out.='>November</option><option value="12" '; if($row1->BirthDate[5].$row1->BirthDate[6]=="12") $out.='selected="selected"'; $out.='>December</option></select>
				<input type="text" name="b_day" value="'.$row1->BirthDate[8].$row1->BirthDate[9].'" size="2" maxlength="2" />, 
				<input type="text" name="b_year" value="'.$row1->BirthDate[0].$row1->BirthDate[1].$row1->BirthDate[2].$row1->BirthDate[3].'" size="4" maxlength="4" />
				</td>';
         $sh="select * from C_Field where Name='BirthDate'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
      $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.='/></th>';
 $share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr>
				<td align="right" width="33%">
					<b>Allow other vips to contact me</b><br />
					<font size="-1">If you answer \'yes\' to this question, then other VIP\'s will be able to send you messages through the system, however your email address will never be revealed unless you check the \'share box\' next to your email address</font>
				</td>
				<td><select name="Users_AllowContactOtherVip"><option value="0">No</option><option value="1" selected="selected">Yes</option></select></td>
				<td>&nbsp;</td><td>&nbsp;</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td align="right"><b>Nick Name</b><br ><font size="-1">Leave blank to use your real name</font></td>
				<td><input type="text" name="Users_Nickname" value="'.$row1->Nickname.'" /></td>';
          $sh="select * from C_Field where Name='Nickname'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
            $share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr>
				<th align="right">First Name</th>
				<td><input type="text" name="Users_FirstName" value="'.$row1->FirstName.'" /></td>';
          $sh="select * from C_Field where Name='FirstName'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
 $share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Last Name</th>
				<td><input type="text" name="Users_LastName" value="'.$row1->LastName.'" /></td>';
 $sh="select * from C_Field where Name='LastName'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
 $share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr>
				<th align="right">Gender</th>
				<td><select name="Users_Gender"><option value="">(select)</option><option value="f"'; if($row1->Gender=="f") $out.='selected="selected"'; $out.='>Female</option><option value="m" '; if($row1->Gender=="m") $out.='selected="selected"'; $out.='>Male</option></select></td>';
 $sh="select * from C_Field where Name='Gender'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]"  value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Phone Model</th>
				<td>	
		Make: 
		<select name="PhoneModel_FKID_PhoneMake" onchange="select_phonemake(this);">';
      $sql_query="select * from PhoneMake";
      $result=mysql_query($sql_query);
      while($rows=mysql_fetch_object($result))
  {
      $out.='<option value="'.$rows->PKID_PhoneMake.'"';if($rows->PKID_PhoneMake==$row2->FKID_PhoneMake) $out.='selected="selected"'; $out.='>'.$rows->Description.'</option>';
  }
  $out.='</select><input type="text" name="Users_PhoneMakeOther" value="" id="phonemakeother" style="visibility:hidden" /><br />
     
      Model: <select name="Users_FKID_PhoneModel" id="phonemodel" onchange="select_phonemodel(this);setBinaryVersion();">';
        

        $sql_r="
        SELECT 
        	PhoneModel.*,
        	BinaryVersion.Description AS Version, BinaryVersion.Revision
        FROM PhoneModel
			LEFT JOIN BinaryVersion ON FKID_BinaryVersion=PKID_BinaryVersion
        WHERE FKID_PhoneMake='".$row2->FKID_PhoneMake."'";
        $r_s=mysql_query($sql_r);
        while($rows_r=mysql_fetch_object($r_s))
    {
    	$out.='<script>BinaryVersionArray[BinaryVersionArray.length]=\''.$rows_r->Version.' '.(($rows_r->Revision!='')?'Rev: ':'').$rows_r->Revision.'\';</script>';
     $out.='<option value="'.$rows_r->PKID_PhoneModel.'"';if($rows_r->PKID_PhoneModel==$row2->PKID_PhoneModel) $out.='selected="selected"'; $out.='>'.$rows_r->Description.'</option>';
    }
  
    $out.='</select> <span id="binaryVersion"></span><input type="text" name="Users_PhoneModelOther" value="" id="phonemodelother" style="visibility:hidden" /></td>';
 $sh="select * from C_Field where Name='FKID_PhoneModel'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.='  /></th>
			</tr>
			<tr>
					<th align="right">Current Picture</th>
					<td>';
           
            $file='client/images/'.$_SESSION['username'].'.jpg';
             
            if(file_exists($file)){
						$out.='<img src="client/images/'.$_SESSION['username'].'.jpg" border="0" width="150" height="150" /><br />';
                  
						$out.='<input type="submit" value="Delete"  name="delete_picture" id="delete_picture" onclick="if(confirm(\'Are you sure you want to delete this your picture?\')){this.form.action.value=\'delete_picture\';this.form.submit();}" />';
            }
            $out.='
					</td>';
$sh="select * from C_Field where Name='Picture'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
					<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
					<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
				</tr>
			<tr>
				<th align="right">Upload New Picture</th><td><input type="file" name="pictureu" onchange="this.form.action.value=\'insert_picture\';this.form.submit();"/></td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Address</th>';
            $sql_q="select * from Address  where FK_MasterUsers='".$_SESSION['userID']."'";
            $rs=mysql_query($sql_q);
            $res=mysql_fetch_object($rs);
            $s_q="select * from City where PKID_City='".$res->FKID_City."'";
            $r_q=mysql_query($s_q);
            $res_q=mysql_fetch_object($r_q);
            $out.='
				<td><input type="text" name="Address_Address" value="'.$res->Address.'" /></td>';
$sh="select * from C_Field where Name='PKID_Address'";
      $rh=mysql_query($sh);
      $roeh=mysql_fetch_object($rh);
       $share="select * from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Country, State, City, Zip</th>
				<td>
<select name="country" onchange="this.form.action.value=\'update\';this.form.submit();">';
            $sql_country="select * from Country order by Name asc";
            $r_country=mysql_query($sql_country);
            while($row_country=mysql_fetch_object($r_country))
  {
              $out.='<option value="'.$row_country->PKID_Country.'"'; if(isset($_POST['country'])&&$_POST['country']==$row_country->PKID_Country) { $out.='selected="selected"'; } else  { if(!isset($_POST['country'])&&$row_country->PKID_Country==$res->FKID_Country) $out.='selected="selected"';} $out.='>'.$row_country->Name.'</option>';
  }

$out.='</select>';
if(isset($_POST['country'])&&$_POST['country']!="1")
  $out.='<input type="text" name="City_FKID_C_State" value="'.$res_q->Other_state.'">';
else
  if(!isset($_POST['country'])&&$res->FKID_Country!=1)  $out.='<input type="text" name="City_FKID_C_State" value="'.$res_q->Other_state.'">';
  else
  {
  $out.='<select name="City_FKID_C_State"> <option value="">(select)</option>';
             $s="select * from C_State";
             $rq=mysql_query($s);
             while($state=mysql_fetch_object($rq))
  {

            $out.=' <option value="'.$state->PKID_C_State.'"'; if($state->PKID_C_State==$res_q->FKID_C_State) $out.='selected="selected"'; $out.='>'.$state->Name.'</option>';
  }
         $out.='   </select>';
  }
        $out.=' <input type="text" name="City_Name" value="'.$res_q->Name.'" maxlength="50" size="15" />, <input type="hidden" name="FKID_City" value="'.$res_q->PKID_City.'"> <input type="text" name="Address_ZipCode" value="'.$res->ZipCode.'" maxlength="9" size="9" /></td>
				<td>&nbsp;</td><td>&nbsp;</td>
			</tr>
			<tr>
				<th align="right">Add Phone Numbers</th>
				<td colspan="3">
										<table border="0" cellpadding="2" cellspacing="0" width="100%">
						<tr><td align="center">
							<select name="PhoneNumber_NEW_FKID_PhoneNumberCategory"><option value=""></option>';
            $ph="select * from PhoneNumber where FK_MasterUsers='".$_SESSION['userId']."'";
            $r_p=mysql_query($ph);
            $phone=mysql_fetch_object($r_p);
            $s_n="select * from PhoneNumberCategory";
            $r_n=mysql_query($s_n);
            $PhoneCategoriesPK=array();
            $PhoneCategoriesDescription=array();
            while($number_c=mysql_fetch_object($r_n))
  			{
  				$PhoneCategoriesPK[]=$number_c->PKID_PhoneNumberCategory;
            	$PhoneCategoriesDescription[]=$number_c->Description;
              	$out.='<option value="'.$number_c->PKID_PhoneNumberCategory.'"';$out.='>'.$number_c->Description.'</option>';
  			}
              $out.='  </select> ( <input type="text" name="PhoneNumber_NEW_AreaCode" value="" maxlength="3" size="3" /> ) <input type="text" name="PhoneNumber_NEW_Number" value="" maxlength="9" size="9" /> ext.<input type="text" name="PhoneNumber_NEW_Extension" value="" size="4" />						</td>
						<td><font size="-2">Other VIPs</font></td>
						<td><font size="-2">Businesses</font></td>
						<td>&nbsp;</td></tr></table>';
              $reqPhoneNumbers=mysql_query("SELECT PhoneNumber.*, PhoneNumberCategory.Description from PhoneNumber 
              		INNER JOIN PhoneNumberCategory ON PKID_PhoneNumberCategory = FKID_PhoneNumberCategory 
              		WHERE FK_MasterUsers='".$_SESSION['userID']."'");
                  if(mysql_num_rows($reqPhoneNumbers)!=0)
  {
                    $i=1;
               $out.='<tr>
				<th align="right"> Phone Numbers</th>
				<td colspan="3">
										<table border="0" cellpadding="2" cellspacing="0" width="100%">';
  }
            while($resPhoneNumbers=mysql_fetch_object($reqPhoneNumbers)){
 				$out.='
	 				<tr bgcolor="#DDDDDD">
	 					<td align="center">
						<select name="PhoneNumber_'.$resPhoneNumbers->PKID_PhoneNumber.'"><option value=""></option>';
	                  	for($i=0;$i<count($PhoneCategoriesPK);$i++){
	              			$out.='<option value="'.$PhoneCategoriesPK[$i].'"';if($PhoneCategoriesPK[$i]==$resPhoneNumbers->FKID_PhoneNumberCategory) $out.='selected="selected"';$out.='>'.$PhoneCategoriesDescription[$i].'</option>';
	  					} 
              			
	                $out.=' </select> ( <input type="text" name="AreaCode_'.$resPhoneNumbers->PKID_PhoneNumber.'" value="'.$resPhoneNumbers->AreaCode.'" maxlength="3" size="3" /> ) <input type="text" name="Number_'.$resPhoneNumbers->PKID_PhoneNumber.'" value="'.$resPhoneNumbers->Number.'" maxlength="9" size="9" /> ext.<input type="text" name="Extension_'.$resPhoneNumbers->PKID_PhoneNumber.'" value="'.$resPhoneNumbers->Extension.'" size="4" />
	                 	</td>';
	           $reqShareVip=mysql_query("SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND FKID_C_Field='".$PhoneNumberFKID_C_Field."' AND FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."'" ); 
	           if(mysql_num_rows($reqShareVip)!=0)
	           		$shareWithVipChecked='checked';
	           else 
	           		$shareWithVipChecked='';
	           $reqShareEstablishment=mysql_query("SELECT * FROM ShareWithEstablishment WHERE FK_MasterUsers='".$_SESSION['userID']."' AND FKID_C_Field='".$PhoneNumberFKID_C_Field."' AND FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."'" ); 
	           if(mysql_num_rows($reqShareEstablishment)!=0)
	           		$shareWithEstablishmentsChecked='checked';
	           else 
	           		$shareWithEstablishmentsChecked='';

           		$out.='	<td><input type="checkbox" name="ShareVip_'.$resPhoneNumbers->PKID_PhoneNumber.'"  value="" '.$shareWithVipChecked.'></td>
						<td><input type="checkbox" name="ShareEst_'.$resPhoneNumbers->PKID_PhoneNumber.'"  value="" '.$shareWithEstablishmentsChecked.'></td>
						<td><input type="submit" value="Delete"  name="deletephone_'.$resPhoneNumbers->PKID_PhoneNumber.'" onclick="if(confirm(\'Are you sure you want to delete this phone number ?\')){this.form.action.value=\'deletephone_'.$resPhoneNumbers->PKID_PhoneNumber.'\';this.form.submit();}" /></td>
							</tr> ';       
            }
				$out.='</tr>';
            if(mysql_num_rows($reqPhoneNumbers)!=0)
   $out.='</table>';
				$out.='</td>
			</tr>
			<tr><td colspan="4" align="center"><input type="submit" name="update" value="update">
			</td></tr>
		</table>
		</td></tr></table></form>
        ';

	if(isset($_POST['update']) || (isset($_POST['action']) && $_POST['action']=="update"))
  { 

       $bdate=$_POST['b_year'].'-'.$_POST['b_month'].'-'.$_POST['b_day'];
       
       $sql="select * from Users where FK_MasterUsers='".$_SESSION['userID']."'";
       $r=mysql_query($sql);
       if(mysql_num_rows($r)!=0)
    {
       $sql="update Users set BirthDate='".$bdate."', AllowContactOtherVip='".$_POST['Users_AllowContactOtherVip']."', Nickname='".$_POST['Users_Nickname']."', FirstName='".$_POST['Users_FirstName']."', LastName='".$_POST['Users_LastName']."', Gender='".$_POST['Users_Gender']."',FKID_PhoneModel='".$_POST['Users_FKID_PhoneModel']."' where FK_MasterUsers='".$_SESSION['userID']."'";
       mysql_query($sql) or die("Can not update Users table".mysql_error());
    }
    else
    {
      $sql="insert into Users (FKID_PhoneModel , Nickname,  Gender, BirthDate,AllowContactOtherVip, FirstName ,LastName) values('".$_POST['Users_FKID_PhoneModel']."','".$_SESSION['username']."','".$_POST['Users_Gender']."','".$bdate."','".$_POST['Users_AllowContactOtherVip']."','".$_POST['Users_FirstName']."','".$_POST['Users_LastName']."')";
      mysql_query($sql) or die("Can not insert into Users ".mysql_error());
    }
    
     $sql4="select * from City where Name='".$_POST['City_Name']."'";
        $r4=mysql_query($sql4);
        $sql_co="select * from Address where FK_MasterUsers='".$_SESSION['userID']."'";
        $r_co=mysql_query($sql_co);
        $row_co=mysql_fetch_object($r_co);
        if(mysql_num_rows($r4)!=0)
    {
         
          if($_POST['country']!=1)
      {     // die($_POST['City_FKID_C_State']);
             $sql3="update City set FKID_C_State='0', Other_state='".$_POST['City_FKID_C_State']."' where Name='".$_POST['City_Name']."'";
             mysql_query($sql3) or die("Can not update City table".mysql_error());
      }
      else
      {
       // die($_POST['City_FKID_C_State']);
        $sql3="update City set FKID_C_State='".$_POST['City_FKID_C_State']."', Other_state='' where Name='".$_POST['City_Name']."'";
        mysql_query($sql3) or die("Can not update City table".mysql_error());
      }
    }
    else 
    {
      if($_POST['country']!=1)
      {
      $sql3="insert into City(FKID_C_State,Name,Other_state) values('0','".$_POST['City_Name']."','".$_POST['City_FKID_C_State']."')";
      mysql_query($sql3) or die("Can not update City table".mysql_error());
      }
      else
      {
      $sql3="insert into City(FKID_C_State,Name) values('".$_POST['City_FKID_C_State']."','".$_POST['City_Name']."')";
      mysql_query($sql3) or die("Can not update City table".mysql_error());
      }
    }
    $sql_c="select * from City where Name='".$_POST['City_Name']."'";
    $r_c=mysql_query($sql_c);
    $row_c=mysql_fetch_object($r_c);
     $sql="select * from Address where FK_MasterUsers='".$_SESSION['userID']."'";
       $r=mysql_query($sql);
       if(mysql_num_rows($r)!=0)
    {
        $sql2="update Address set Address='".$_POST['Address_Address']."', FKID_City='".$row_c->PKID_City."', ZipCode='".$_POST['Address_ZipCode']."',FKID_Country='".$_POST['country']."' where FK_MasterUsers='".$_SESSION['userID']."'";
        mysql_query($sql2) or die("Can not update Address table".mysql_error());
    }
    else
    {
      $sql="insert into Address (FK_MasterUsers , Address, FKID_City ,ZipCode,FKID_Country) values('".$_SESSION['userID']."','".$_POST['Address_Address']."','".$row_c->PKID_City."','".$_POST['Address_ZipCode']."','".$_POST['country']."')";
      mysql_query($sql) or die("Can not insert into Address ".mysql_error());
    }
       
   
	  
     
    $j=0;
 $Vip=$_POST['ShareVip'];
    for($i=0;$i<count($Vip);$i++)
  {
     $sql1="select * from  ShareWithVip where FKID_C_Field='".$Vip[$i]."' and FK_MasterUsers='".$_SESSION['userID']."'";
     $r1=mysql_query($sql1);
     if(mysql_num_rows($r1)==0)
    {
     $out.=$_SESSION['userID'];
    $sql="insert into  ShareWithVip (FKID_C_Field ,  FK_MasterUsers,FK_FKID_C_Field ) values('".$Vip[$i]."','".$_SESSION['userID']."','".$_SESSION['userID']."')";
    mysql_query($sql) or die("Can not insert into ShareWithVip !!".mysql_error());
    }
  
  }
   $sql_e="select * from ShareWithVip where  FK_MasterUsers='".$_SESSION['userID']."'";
   $r_e=mysql_query($sql_e) or die("Can not grab from database".mysql_error());

   while($row_e=mysql_fetch_array($r_e))
  {   $j=0;
      for($i=0;$i<count($Vip);$i++)
    {
      if($row_e['FKID_C_Field']!=$Vip[$i]) $j++;
      
    }
      
      if($j==count($Vip)) $del[]= $row_e['FKID_C_Field'];
  }
    for($i=0;$i<count($del);$i++)
    {
      
      $sql="delete from ShareWithVip where FKID_C_Field='".$del[$i]."' and FK_MasterUsers='".$_SESSION['userID']."'";
      $r=mysql_query($sql) or die("Can not delete from ShareWithVip ".mysql_error());
    }
  
  
  $j=0;
    $Est=$_POST['ShareEst'];
     for($i=0;$i<count($Est);$i++)
  {
       $sql1="select * from  ShareWithEstablishment where FKID_C_Field='".$Est[$i]."' and FK_MasterUsers='".$_SESSION['userID']."'";
     $r1=mysql_query($sql1);
     if(mysql_num_rows($r1)==0)
    {
      $sql="insert into  ShareWithEstablishment (FKID_C_Field ,  FK_MasterUsers,FK_FKID_C_Field  ) values('".$Est[$i]."','".$_SESSION['userID']."','".$_SESSION['userID']."')";
     mysql_query($sql) or die("Can not insert into ShareWithEstablishment ".mysql_error());
    }
    
  }
 
   $sql_e="select * from ShareWithEstablishment where  FK_MasterUsers='".$_SESSION['userID']."'";
   $r_e=mysql_query($sql_e) or die("Can not grab from database".mysql_error());
   while($row_e=mysql_fetch_object($r_e))
  {
      $j=0;
      for($i=0;$i<count($Est);$i++)
    {
      if($row_e->FKID_C_Field!=$Est[$i]) $j++;
       
    }
      if($j==count($Est)) $del1[]= $row_e->FKID_C_Field;
  }
    for($i=0;$i<count($del1);$i++)
    {
      $sql="delete from ShareWithEstablishment where FKID_C_Field='".$del1[$i]."' and FK_MasterUsers='".$_SESSION['userID']."'";
      $r=mysql_query($sql) or die("Can not delete from ShareWithEstablishment ".mysql_error());
    }
 
  

  	// update MasterUser username if posible
    $updMasterUsers=updateMasterUsers($_SESSION['userID'],'&username='.$_POST['MasterUsers_Username'],$updateMasterUserUrl);
	if($updMasterUsers)
		$_SESSION['username']=$_POST['MasterUsers_Username'];

  
    // update existing phone numbers
    $reqPhoneNumbers=mysql_query("SELECT * FROM PhoneNumber WHERE FK_MasterUsers='".$_SESSION['userID']."'");
    while($resPhoneNumbers=mysql_fetch_object($reqPhoneNumbers)){
    	$sql="UPDATE PhoneNumber SET FKID_PhoneNumberCategory='".$_POST['PhoneNumber_'.$resPhoneNumbers->PKID_PhoneNumber]."', AreaCode='".$_POST['AreaCode_'.$resPhoneNumbers->PKID_PhoneNumber]."',Number='".$_POST['Number_'.$resPhoneNumbers->PKID_PhoneNumber]."', Extension='".$_POST['Extension_'.$resPhoneNumbers->PKID_PhoneNumber]."' WHERE FK_MasterUsers='".$_SESSION['userID']."' AND PKID_PhoneNumber ='".$resPhoneNumbers->PKID_PhoneNumber."'";
    	mysql_query($sql) or die("Can not update PhoneNumber ".mysql_error());
    	
    	// update ShareWithVIP
    	$query="SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."'";
    	$qid=mysql_query($query) or die("Can not select from ShareWithVip".mysql_error());
    	
    	if(mysql_num_rows($qid)==0){
    		if(isset($_POST["ShareVip_".$resPhoneNumbers->PKID_PhoneNumber])){
    			$sql="INSERT INTO ShareWithVip (FKID_C_Field, FK_FKID_C_Field, FK_MasterUsers) VALUES ('".$PhoneNumberFKID_C_Field."', '".$resPhoneNumbers->PKID_PhoneNumber."', '".$_SESSION['userID']."')";
    			mysql_query($sql) or die("Can not insert into ShareWithVip ".mysql_error());
    		}
    	}else{
    		if(isset($_POST["ShareVip_".$resPhoneNumbers->PKID_PhoneNumber])){
    			$sql="UPDATE ShareWithVip SET FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."' WHERE FK_MasterUsers=".$_SESSION['userID']." AND FK_FKID_C_Field='".$PhoneNumberFKID_C_Field."'";
    			mysql_query($sql) or die("Can not update ShareWithVip ".mysql_error());
    		}else{
    			$sql="DELETE FROM ShareWithVip WHERE FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."' AND FK_MasterUsers=".$_SESSION['userID']." AND FK_FKID_C_Field='".$PhoneNumberFKID_C_Field."'";
    			mysql_query($sql) or die("Can not delete from ShareWithVip ".mysql_error());
    		}
    	}

    	// update ShareWithEstablishment
    	$query="SELECT * FROM  ShareWithEstablishment WHERE FK_MasterUsers='".$_SESSION['userID']."' AND FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."'";
    	$qid=mysql_query($query) or die("Can not select from  ShareWithEstablishment".mysql_error());
    	
    	if(mysql_num_rows($qid)==0){
    		if(isset($_POST["ShareEst_".$resPhoneNumbers->PKID_PhoneNumber])){
    			$sql="INSERT INTO  ShareWithEstablishment (FKID_C_Field, FK_FKID_C_Field, FK_MasterUsers) VALUES ('".$PhoneNumberFKID_C_Field."', '".$resPhoneNumbers->PKID_PhoneNumber."', '".$_SESSION['userID']."')";
    			mysql_query($sql) or die("Can not insert into  ShareWithEstablishment ".mysql_error());
    		}
    	}else{
    		if(isset($_POST["ShareEst_".$resPhoneNumbers->PKID_PhoneNumber])){
    			$sql="UPDATE  ShareWithEstablishment SET FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."' WHERE FK_MasterUsers=".$_SESSION['userID']." AND FK_FKID_C_Field='".$PhoneNumberFKID_C_Field."'";
    			mysql_query($sql) or die("Can not update  ShareWithEstablishment ".mysql_error());
    		}else{
    			$sql="DELETE FROM  ShareWithEstablishment WHERE FK_FKID_C_Field='".$resPhoneNumbers->PKID_PhoneNumber."' AND FK_MasterUsers=".$_SESSION['userID']." AND FK_FKID_C_Field='".$PhoneNumberFKID_C_Field."'";
    			mysql_query($sql) or die("Can not delete from  ShareWithEstablishment ".mysql_error());
    		}
         
    	}
      
    }

    // insert new phone number
    if($_POST['PhoneNumber_NEW_FKID_PhoneNumberCategory']!=''){
    	$sql="insert into PhoneNumber (FK_MasterUsers,FKID_PhoneNumberCategory,AreaCode,Number,Extension) values('".(int)$_SESSION['userID']."','".(int)$_POST['PhoneNumber_NEW_FKID_PhoneNumberCategory']."','".(int)$_POST['PhoneNumber_NEW_AreaCode']."','".(int)$_POST['PhoneNumber_NEW_Number']."','".(int)$_POST['PhoneNumber_NEW_Extension']."')";
      	mysql_query($sql) or die("Can not insert into PhoneNumber ".mysql_error());
    }

   header("Location: index.php?section=settings");
  }

// delete phone numbers and their related entries from ShareWithVip and ShareWithEstablishment
if(isset($_POST['action']) && ereg("deletephone_",$_POST['action'])){
	$numberToDelete=substr($_POST['action'],12);
	
	$sql="delete from PhoneNumber where FK_MasterUsers='".$_SESSION['userID']."' AND PKID_PhoneNumber ='".$numberToDelete."'";
	mysql_query($sql) or die("Can not delete from Phone Number".mysql_error());
	// delete from ShareWithVip
	$sql="delete from ShareWithVip where FK_MasterUsers='".$_SESSION['userID']."' AND FK_FKID_C_Field ='".$numberToDelete."'";
	mysql_query($sql) or die("Can not delete from Phone Number".mysql_error());

	// delete from ShareWithEstablishment
	$sql="delete from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' AND FK_FKID_C_Field ='".$numberToDelete."'";
	mysql_query($sql) or die("Can not delete from Phone Number".mysql_error());
	
	header("Location: index.php?section=settings");
  
}
if(isset($_POST['action']) && $_POST['action']=="delete_picture")
  {
   
     $file="client/images/".$_SESSION['username'].".jpg";
    
    unlink($file);
    header("Location: index.php?section=settings");
  }

 if(isset($_POST['action']) && $_POST['action']=="insert_picture")
  {
    $file='client/images/'.$_SESSION['username'].'.jpg';
    if(file_exists($file)) unlink($file);
    $id_user=$_SESSION['username'];
    $file_realname = trim($HTTP_POST_FILES['pictureu']['$id_user']); 
    $uploaddir="client/images/";
    $uploaddir.=$id_user.".jpg";
    move_uploaded_file($_FILES['pictureu']['tmp_name'], $uploaddir.$file_realname); 
    $out='<meta http-equiv="refresh" content="0.000001;url="index.php?section=settings">';
    //header("Location: index.php?section=settings");
  }
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->settingsIMG = 'images/home%20page%20img/a_my_settings_on.gif';
	
	$output->setBody($out);
	$output->setImgName("img.jpg");
	$output->setTitle(APPLICATION_NAME."::Settings");
	$output->setPageID(1); //butonul selectat
	
	$output->output();
}
?>