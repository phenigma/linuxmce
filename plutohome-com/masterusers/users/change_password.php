<?
function change_password($conn){
	if(isset($_POST['PK_MasterUsers'])){
		// change password for a specific Master User
		// the password received is MD5 already
		$PK_MasterUsers=$_POST['PK_MasterUsers'];
		$password=$_POST['password'];
		
		// check if the user exists
		$queryMasterUsers=dbQuery("SELECT * FROM MasterUsers WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
		if(mysql_num_rows($queryMasterUsers)==0)
			$out='Master User not found';
		else{
			if(isset($_POST['oldPassword']) && $_POST['oldPassword']!=''){
				$res=mysql_fetch_object($queryMasterUsers);
				if($res->Password!=$_POST['oldPassword']){
					$out="Old password do not match.";
					echo $out;
					exit();
				}
			}

			$updateMasterUsers=dbQuery("UPDATE MasterUsers SET Password='$password' WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
			if($updateMasterUsers)
				$out="Password changed";
			else
				$out="Password update error.";
		}
	}else
		$out='';
	// printing messages is required because without output the curl connection will return 1
	echo $out;	
 }
?>