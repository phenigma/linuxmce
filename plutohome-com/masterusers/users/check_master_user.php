<?
function check_master_user($conn){
	// add to MasterUsers table 
	$errorsCount=0;
	if(isset($_POST['username']) && $_POST['username']!=''){
		$username=$_POST['username'];
		$password=md5($_POST['password']);
		if($_POST['password']!=''){
			// login attempt: check both user and password
			$queryMasterUser=dbQuery("SELECT * from MasterUsers WHERE Username='".$username."' AND Password='$password'",$conn);
			if(mysql_num_rows($queryMasterUser)==0){
				$out='MasterUser not found';
				$errorsCount++;
			}else {
				$loginDate=date("YmdHis");
				$updateMasterUsers=dbQuery("UPDATE MasterUsers SET LoginDate='$loginDate' WHERE Username='".$username."' AND Password='$password'",$conn);
			}
		}else{
			// query MasterUsers table for user details 
			$queryMasterUser=dbQuery("SELECT * from MasterUsers WHERE Username='".$username."'",$conn);
			if(mysql_num_rows($queryMasterUser)==0){
				$out='MasterUser not found';
				$errorsCount++;
			}
		}
		
	}elseif(isset($_POST['Email'])){
		// forgot password; only the email is provided
		$queryMasterUser=dbQuery("SELECT * from MasterUsers WHERE Email='".$_POST['Email']."'",$conn);
		if(mysql_num_rows($queryMasterUser)==0){
			$out='Email not found';
			$errorsCount++;
		}
	}
	else{
		// direct acces to index.php?section=check_master_table - display nothing
		$out='';
	}
	if($errorsCount==0){
		// add to output all the fields specified in the parameters list
		$res=mysql_fetch_object($queryMasterUser);
		$out="MasterUsersID=".$res->PK_MasterUsers;
		if(isset($_POST['PlutoId']))
			$out.="&PlutoId=".$res->PlutoId;
		if(isset($_POST['Pin']))
			$out.="&Pin=".$res->Pin;
		if(isset($_POST['FirstAccount']))
			$out.="&FirstAccount=".$res->FirstAccount;
		if(isset($_POST['Email']))
			$out.="&Password=".$res->Password;			
		if(isset($_POST['Email']))
			$out.="&Email=".$res->Email;
	}
	echo $out;	
}
?>