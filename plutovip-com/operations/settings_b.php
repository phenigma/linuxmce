<?
function settings_b($output){
	global $updateMasterUserUrl;
	
	$queryShareWithVipPN=dbQuery("SELECT PKID_C_Field FROM C_Field WHERE Name='PKID_PhoneNumber'");
	if(mysql_num_rows($queryShareWithVipPN)!=0){
		$resShareWithVipPN=mysql_fetch_object($queryShareWithVipPN);
		$PhoneNumberFKID_C_Field = $resShareWithVipPN->PKID_C_Field;
	}

  $out='';
  $out.='
  <style type="text/css">
		.email 
		{
			color:#00A;
		}
		</style>
  <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
   
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
    <form action="index.php?section=settings_b" method="POST" name="form1">	
  		<input type="hidden" name="action" value="">
    <table>
      <tr><td align="left"><img src="images/submenu_txt/info_businesses_txt.gif" width="253" height="14"></td></tr><form action="index.php?section=settitngs_b" method="POST" name="form1">		
      <tr><td width="340"></td><td align="center"><br><b >My Settings</b><br>
		<br><b>Click on <a href="index.php?section=binstructions">Instructions</a> if you have any questions</b></td></tr></table>
		<table align="left" border="0" cellpadding="4" width="100%" cellspacing="0">
  			<tr>
				<td colspan="2">&nbsp;</td>
				<th title="Share with VIPs or Businesses" colspan="2">Share with</td>
			</tr>
			<tr>
  				<td align="right" width="50%"><b>Pluto ID</b></td>
  				<td width="50%">'.(($_SESSION['categ']==0)?'PV':'PE').$_SESSION['PlutoId'].$_SESSION['Pin'].'</td>
  				<td><font size="-2">Other VIPs</font></td>
				<td><font size="-2">Businesses</font></td>
  			</tr>
			<tr>
  				<td align="center" width="50%" colspan="2">Pluto ID is a unique combination and represents your identity for PlutoVIP  application. You need to provide this ID when you\'ll install the software on your Windows-based cash register, point of sale system, or computer.</td>
  			</tr>
			<tr bgcolor="#EEEEEE">
  				<th align="right">Username</th>
  				<td colspan="3"><input type="text" name="MasterUsers_Username" value="'.$_SESSION['username'].'" /></td>
  			</tr>
			<tr>
  				<th align="right">E-mail</th>
  				<td>'.$_SESSION['Email'].'
		<input type="hidden" name="verify_email" value="'.$_SESSION['Email'].'" />';
		    $sql1="SELECT * FROM EmailConfirmationLog WHERE FK_MasterUsers='".$_SESSION['userID']."'";
	    	$r1=mysql_query($sql1) or die("Can not grab from database ".mysql_error());
	      	$row1=mysql_fetch_object($r1);
	      	if(mysql_num_rows($r1)==0 or $row1->Accepted!=1){
				$out.='<br />(unverified) 
				<br /><a href="index.php?section=settings_b&action=verification&email='.$_SESSION['Email'].'" >send verification e-mail</a>';
	  		}
	       $sql2="SELECT PKID_EstablishmentCategory,Description  FROM EstablishmentCategory ORDER BY PKID_EstablishmentCategory asc";
	       $r2=mysql_query($sql2) or die("Can not grab from database ".mysql_error());
	       $sql2_e="select * from Establishment where FK_MasterUsers='".$_SESSION['userID']."'";
	       $r2_e=mysql_query($sql2_e) or die("Can not grab from database ".mysql_error());
	       $row2_e=mysql_fetch_object($r2_e);
			$out.='</td>';
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
				<th align="right">Category</th>
				<td><select name="FKID_EstablishmentCategory">';
         while($row2=mysql_fetch_object($r2))
  {
         $out.='<option value="'.$row2->PKID_EstablishmentCategory.'" ';if($row2->PKID_EstablishmentCategory==$row2_e->FKID_EstablishmentCategory) $out.='selected="selected"';$out.=' >'.$row2->Description.'
         </option>';
  }
			$out.=' </select> <input type="text" name="CategoryOther" value="'.$row2_e->CategoryOther.'" /></td>';
			$sh="SELECT * FROM C_Field WHERE Name='FKID_EstablishmentCategory'";
      		$rh=mysql_query($sh);
      		$roeh=mysql_fetch_object($rh);
       		$share="SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND  FKID_C_Field='".$roeh->PKID_C_Field."'";
		    $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
			$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      		$rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>			
			</tr>			
			</tr>
			<tr>
				<th align="right">Business Name</th>
				<td><input type="text" name="Name" value="'.$row2_e->Name.'" /></td>';
			$sh="SELECT * FROM C_Field WHERE Name='Name'";
      		$rh=mysql_query($sh);
      		$roeh=mysql_fetch_object($rh);
       		$share="SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND  FKID_C_Field='".$roeh->PKID_C_Field."'";
		    $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
			$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      		$rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>			
			</tr>			
				
			</tr>
			<tr bgcolor="#EEEEEE">
				<th align="right">Contact Name</th>
				<td colspan="3"><input type="text" name="Contact" value="'.$row2_e->Contact.'" /></td>
			</tr>
			<tr>
				<th align="right">Website</th>
				<td><input type="text" name="Website" value="'.$row2_e->Website.'" /></td>';
			$sh="SELECT * FROM C_Field WHERE Name='Website'";
      		$rh=mysql_query($sh);
      		$roeh=mysql_fetch_object($rh);
       		$share="SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND  FKID_C_Field='".$roeh->PKID_C_Field."'";
		    $rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareVip[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>';
			$share="select * from ShareWithEstablishment where FK_MasterUsers='".$_SESSION['userID']."' and  FKID_C_Field='".$roeh->PKID_C_Field."'";
      		$rs=mysql_query($share);
				$out.='
				<th><input type="checkbox" name="ShareEst[]" value="'.$roeh->PKID_C_Field.'"'; if(mysql_num_rows($rs)!=0) $out.='checked="checked"'; $out.=' /></th>			
			</tr>
			
			<tr bgcolor="#EEEEEE">
				<th align="right">Address</th>';
            $sql_q="SELECT * FROM Address  WHERE FK_MasterUsers='".$_SESSION['userID']."'";
            $rs=mysql_query($sql_q);
            $res=mysql_fetch_object($rs);
            $s_q="SELECT * FROM City WHERE PKID_City='".$res->FKID_City."'";
            $r_q=mysql_query($s_q);
            $res_q=mysql_fetch_object($r_q);
            $out.='
				<td><input type="text" name="Address_Address" value="'.$res->Address.'" /></td>';
			$sh="SELECT * FROM C_Field WHERE Name='PKID_Address'";
      		$rh=mysql_query($sh);
      		$roeh=mysql_fetch_object($rh);
       		$share="SELECT * FROM ShareWithVip WHERE FK_MasterUsers='".$_SESSION['userID']."' AND  FKID_C_Field='".$roeh->PKID_C_Field."'";
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
				<td><select name="country" onchange="this.form.action.value=\'update\';this.form.submit();">
						<option value="0">(select)</option>';
            $sql_country="select * from Country order by Name asc";
            $r_country=mysql_query($sql_country);
            while($row_country=mysql_fetch_object($r_country)){
            	$out.='<option value="'.$row_country->PKID_Country.'"'; if(isset($_POST['country'])&&$_POST['country']==$row_country->PKID_Country) { $out.='selected="selected"'; } else  { if(!isset($_POST['country'])&&$row_country->PKID_Country==$res->FKID_Country) $out.='selected="selected"';} $out.='>'.$row_country->Name.'</option>';
  			}
			$out.='</select>';
			if(isset($_POST['country'])&&$_POST['country']!="1")
  				$out.=' <input type="text" name="City_FKID_C_State" value="'.$res_q->Other_state.'">';
			elseif(!isset($_POST['country'])&&$res->FKID_Country!=1)  
  				$out.='<input type="text" name="City_FKID_C_State" value="'.$res_q->Other_state.'">';
  			else{
	  			$out.=' <select name="City_FKID_C_State"> <option value="">(select)</option>';
	            $s="select * from C_State";
	            $rq=mysql_query($s);
	            while($state=mysql_fetch_object($rq)){
		         	$out.=' <option value="'.$state->PKID_C_State.'"'; if($state->PKID_C_State==$res_q->FKID_C_State) $out.='selected="selected"'; $out.='>'.$state->Name.'</option>';
	  			}
	         	$out.='</select>';
  			}
	        $out.=' <input type="text" name="City_Name" value="'.$res_q->Name.'" maxlength="50" size="15" />, <input type="hidden" name="FKID_City" value="'.$res_q->PKID_City.'"> <input type="text" name="Address_ZipCode" value="'.$res->ZipCode.'" maxlength="9" size="9" /></td>
				<td>&nbsp;</td><td>&nbsp;</td>
			</tr>
			
			
			<tr>
				<th align="right">Add Phone Number</th>
				<td align="center">
							<select name="PhoneNumber_NEW_FKID_PhoneNumberCategory"><option value=""></option>';
            $ph="select * from PhoneNumber where FK_MasterUsers='".$_SESSION['userID']."'";
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
              $out.='  </select> ( <input type="text" name="PhoneNumber_NEW_AreaCode" value="" maxlength="3" size="3" /> ) <input type="text" name="PhoneNumber_NEW_Number" value="" maxlength="9" size="9" /> ext.<input type="text" name="PhoneNumber_NEW_Extension" value="" size="4" /></td>';
              $reqPhoneNumbers=mysql_query("SELECT PhoneNumber.*, PhoneNumberCategory.Description from PhoneNumber 
              		INNER JOIN PhoneNumberCategory ON PKID_PhoneNumberCategory = FKID_PhoneNumberCategory 
              		WHERE FK_MasterUsers='".$_SESSION['userID']."'");
               $out.='
               		<td></td>
               		<td></td>
               </tr>';
            if(mysql_num_rows($reqPhoneNumbers)!=0){
            	$i=1;
				$out.='
               	<tr>
					<td>&nbsp;</td>
					<th align="center"> Phone Numbers</th>
					<td colspan="2">&nbsp;</td>
               	</tr>';
  			}
            while($resPhoneNumbers=mysql_fetch_object($reqPhoneNumbers)){
 				$out.='
	 				<tr bgcolor="#DDDDDD">
						<td align="right"><input type="submit" value="Delete"  name="deletephone_'.$resPhoneNumbers->PKID_PhoneNumber.'" onclick="if(confirm(\'Are you sure you want to delete this phone number ?\')){this.form.action.value=\'deletephone_'.$resPhoneNumbers->PKID_PhoneNumber.'\';this.form.submit();}" /></td> 				
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

           		$out.='	<td align="center"><input type="checkbox" name="ShareVip_'.$resPhoneNumbers->PKID_PhoneNumber.'"  value="" '.$shareWithVipChecked.'></td>
						<td  align="left"><input type="checkbox" name="ShareEst_'.$resPhoneNumbers->PKID_PhoneNumber.'"  value="" '.$shareWithEstablishmentsChecked.'></td>
					</tr> ';       
            }
				$out.='

	        <tr>
       			<td colspan="2" align="center"><input type="submit" name="update" value="update" /></td>
       		</tr>
		</table>
		</td></tr></table></form>';
         if(isset($_GET['action'])&&$_GET['action']=="verify")
  {
           session_start();
           $_SESSION['userID']=$_GET['id'];
           $sql="insert into EmailConfirmationLog (FK_MasterUsers ,Email,CodeNew,Accepted) values('".$_SESSION['userID']."','".$_GET['email']."','".$_GET['code']."','1')";
           mysql_query($sql) or die("Can not insert into EmailConfirmationLog ".mysql_error());
           header("Location: index.php?section=settings_b");

  }

       if(isset($_GET['action'])&& $_GET['action']=="verification")
  {
         $emailD=$_GET['email'];
         $subject=" PlutoVip E-mail verification ";
         $id=$_SESSION['userID'];
         $code=md5($_SESSION['userID']);
         $message='For validate your email click on the link: http://www.plutovip.com/index.php?section=settings_b&id='.$id.'&action=verify&code='.$code.'&email='.$emailD;
         mail($emailD,$subject,$message,"From:webmaster@plutovip.com");
         $sql="insert into EmailConfirmation(FK_MasterUsers ,Email ,CodeNew) values('".$id."','".$emailD."','".$code."')";
         mysql_query($sql) or die("Can not insert into EmailConfirmation ".mysql_error());
         $out='';
         $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
         <tr>
       <td width="100%" rowspan="2" valign="top" align="center" class="insidetable"><br>The validate link has been sent to: '.$emailD.'</td></tr></table>';
  }
      if(isset($_POST['update']))
  {
        $sql="update Establishment set FKID_EstablishmentCategory='".$_POST['FKID_EstablishmentCategory']."',CategoryOther='".$_POST['CategoryOther']."',Name='".$_POST['Name']."',Contact='".$_POST['Contact']."',Website='".$_POST['Website']."' where FK_MasterUsers='".$_SESSION['userID']."'";
        mysql_query($sql) or die("Can not update Establishment table".mysql_error());
        $sql1="select * from Address where FK_MasterUsers='".$_SESSION['userID']."'";
        $r1=mysql_query($sql1) or die("Can not grab from database ".mysql_error());
        if(mysql_num_rows($r1)==0)
    {
          $sql_c="select * from City where Name='".$_POST['City_Name']."'";
          $r_c=mysql_query($sql_c) or die("Can not grab from City".mysql_error());
          if(mysql_num_rows($r_c)==0)
      {
          $sql_c="insert into City(Name,FKID_C_State ) values('".$_POST['City_Name']."','".$_POST['City_FKID_C_State']."')";
          mysql_query($sql_c) or die("Can not insert into City ".mysql_error());
      }
          $sql_c="select * from City where Name='".$_POST['City_Name']."'";
          $r_c=mysql_query($sql_c) or die("Can not grab from City".mysql_error());
          $row_c=mysql_fetch_object($r_c);
          $sql_a="insert into Address(FK_MasterUsers,Address,FKID_City,ZipCode) values('".$_SESSION['userID']."','".$_POST['Address']."','".$row_c->PKID_City."','".$_POST['Address_ZipCode']."')";
          mysql_query($sql_a) or die("CAn not insert into Address ".mysql_error());
    }
    else 
    {
          $sql_c="select * from City where Name='".$_POST['City_Name']."'";
          $r_c=mysql_query($sql_c) or die("Can not grab from City".mysql_error());
          if(mysql_num_rows($r_c)==0)
      {
          $sql_c="insert into City(Name,FKID_C_State ) values('".$_POST['City_Name']."','".$_POST['City_FKID_C_State']."')";
          mysql_query($sql_c) or die("Can not insert into City ".mysql_error());
      }
          $sql_c="select * from City where Name='".$_POST['City_Name']."'";
          $r_c=mysql_query($sql_c) or die("Can not grab from City".mysql_error());
          $row_c=mysql_fetch_object($r_c);
          $sql_a="update Address set Address='".@$_POST['Address']."', FKID_City='".$row_c->PKID_City."',ZipCode='".$_POST['Address_ZipCode']."' where FK_MasterUsers='".$_SESSION['userID']."'";
          mysql_query($sql_a) or die("Can not update Address".mysql_error());
    }
// "share with" management
    $j=0;
 $Vip=@$_POST['ShareVip'];
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
    for($i=0;$i<count(@$del);$i++)
    {
      
      $sql="delete from ShareWithVip where FKID_C_Field='".$del[$i]."' and FK_MasterUsers='".$_SESSION['userID']."'";
      $r=mysql_query($sql) or die("Can not delete from ShareWithVip ".mysql_error());
    }
  
  
  $j=0;
    $Est=@$_POST['ShareEst'];
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
    for($i=0;$i<count(@$del1);$i++)
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
   	}
   

    // insert new phone number
    if($_POST['PhoneNumber_NEW_FKID_PhoneNumberCategory']!=''){
    	$sql="insert into PhoneNumber (FK_MasterUsers,FKID_PhoneNumberCategory,AreaCode,Number,Extension) values('".(int)$_SESSION['userID']."','".(int)$_POST['PhoneNumber_NEW_FKID_PhoneNumberCategory']."','".(int)$_POST['PhoneNumber_NEW_AreaCode']."','".(int)$_POST['PhoneNumber_NEW_Number']."','".(int)$_POST['PhoneNumber_NEW_Extension']."')";
      	mysql_query($sql) or die("Can not insert into PhoneNumber ".mysql_error());
    }
    header("Location: index.php?section=settings_b");

  }
// delete phone numbers and their related entries from ShareWithVip and ShareWithEstablishment
if(isset($_POST['action']) && ereg("deletephone_",$_POST['action'])){
	$numberToDelete=substr($_POST['action'],12);
	
	$sql="delete from PhoneNumber where FK_MasterUsers='".$_SESSION['userID']."' AND PKID_PhoneNumber ='".$numberToDelete."'";
	mysql_query($sql) or die("Can not delete from Phone Number".mysql_error());
	
	header("Location: index.php?section=settings_b");
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