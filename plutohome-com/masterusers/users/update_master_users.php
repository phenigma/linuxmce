<?
function update_master_users($conn,$connPlutoVip,$connPlutoHome,$connphpBB,$connMain){
	if(isset($_POST['MasterUsersID'])){
		$PK_MasterUsers=$_POST['MasterUsersID'];
		// update a MasterUsers
		$errorsCount=0;
		$updateEmail='';
		
		$updateArr=array();
		$updatephpBBArr=array();
		if(isset($_POST['Email'])){
			$Email=$_POST['Email'];
			$updateArr[]="Email='$Email'";
			$updatephpBBArr[]="user_email ='$Email'";
		}
		
		if(isset($_POST['username'])){
			$username=$_POST['username'];
			$queryValidUsername=dbQuery("SELECT * FROM MasterUsers WHERE PK_MasterUsers != '$PK_MasterUsers' AND username='$username'",$conn);
			if(mysql_num_rows($queryValidUsername)!=0){
				$errorsCount++;
				$out='Username already exists.';
			}
			else 
				$updateArr[]= "username='$username'";
				$updatephpBBArr[]="username='$username'";
		}

		$queryMasterUsers=dbQuery("SELECT * FROM MasterUsers WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
		if(mysql_num_rows($queryMasterUsers)==0){
			$out=($errorsCount==0)?'Master User not found':$out;
			$errorsCount++;
		}

		if(count($updateArr)==0){
			$out=($errorsCount==0)?'Nothing to update':$out;
			$errorsCount++;
		}
		
		if($errorsCount==0){
			$modifiedDate=date("YmdHis");
			$updateMasterFilters="Sync_PlutoVip='0', Sync_PlutoHome='0', Sync_phpBB='0', RecordVersion=RecordVersion+1, ModifiedDate='".$modifiedDate."'";
			$updateMasterUsers=dbQuery("UPDATE MasterUsers SET $updateMasterFilters,".join(",",$updateArr)." WHERE PK_MasterUsers = '$PK_MasterUsers'",$conn);
			if($updateMasterUsers){
				$updatePlutoVipMasterUsers=dbQuery("UPDATE MasterUsers SET RecordVersion=RecordVersion+1, ModifiedDate='".$modifiedDate."', ".join(",",$updateArr)." WHERE PK_MasterUsers = '$PK_MasterUsers'",$connPlutoVip);
				if($updatePlutoVipMasterUsers)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_PlutoVip='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

				$updatePlutoHomeMasterUsers=dbQuery("UPDATE MasterUsers SET RecordVersion=RecordVersion+1, ModifiedDate='".$modifiedDate."', ".join(",",$updateArr)." WHERE PK_MasterUsers = '$PK_MasterUsers'",$connPlutoHome);
				if($updatePlutoHomeMasterUsers)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_PlutoHome='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

				$updatePhpBBUsers=mysql_query("UPDATE phpbb_users SET ".join(",",$updatephpBBArr)." WHERE user_id  = '$PK_MasterUsers'",$connphpBB);
				if($updatePhpBBUsers)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_phpBB='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);
				}

				$out="The Master User data was updated.";
			}
			else
				$out=($out=='')?"Update error.":$out;
	}else
		$out='';
	
	echo $out;	
 }
?>