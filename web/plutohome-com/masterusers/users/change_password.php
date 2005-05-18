<?
function change_password($conn,$connMain){
	if(isset($_POST['PK_MasterUsers'])){
		// change password for a specific Master User
		// the password received is not MD5 
		$PK_MasterUsers=$_POST['PK_MasterUsers'];
		$password=MD5($_POST['password']);
		
		// check if the user exists
		$queryMasterUsers=dbQuery("SELECT * FROM MasterUsers WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
		if(mysql_num_rows($queryMasterUsers)==0)
			$out='Master User not found';
		else{
			if(isset($_POST['oldPassword']) && $_POST['oldPassword']!=''){
				$res=mysql_fetch_object($queryMasterUsers);
				if($res->Password!=md5($_POST['oldPassword'])){
					$out="Old password do not match.";
					echo $out;
					exit();
				}
			}
            $SambaPass = @exec("/usr/pluto/bin/smbpass.pl $_POST[password]", $output, $retcode);
            if ($retcode != 0)
                $SambaPass=''; // we can't issue an error here, can we?
            $LinuxSalt = '$1$Pluto$'; // should we generate this? :)
            $LinuxPass = crypt($_POST['password'], $LinuxSalt);
			$updateMasterUsers=dbQuery("UPDATE MasterUsers SET Password='$password' WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
			
			$updateUsers=dbQuery("UPDATE Users SET Password='$password', Password_Unix='$LinuxPass', Password_Samba='$SambaPass' WHERE PK_Users = '$PK_MasterUsers'",$connMain);
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