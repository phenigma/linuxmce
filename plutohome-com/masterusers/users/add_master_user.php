<?
function add_master_user($conn,$connPlutoVip,$connPlutoHome,$connphpBB,$connMantis,$connMain){
	// add to MasterUsers table 
	if(isset($_POST['typeUser'])){
		$typeUser=$_POST['typeUser'];
		$email=$_POST['email'];
		$username=$_POST['username'];
		$password=$_POST['password'];
		$referrer=$_POST['referrer'];
		
		// check if the email isn't already used
		$queryEmail=mysql_query("SELECT * from MasterUsers WHERE Email='".$email."'",$conn);
		$queryUsername=mysql_query("SELECT * from MasterUsers WHERE Username='".$username."'",$conn);
		if(mysql_num_rows($queryEmail)!=0){
			$out='MasterUsers Email already exists';
		}elseif(mysql_num_rows($queryUsername)!=0){
			$out='MasterUsers Username already exists';
		}else{
		// add user to MasterUsers table
			if($typeUser=='Users')
				$prefix="PV";
			else
				$prefix="PE";
			$pin=calculatePIN();
			$signupDate=date("YmdHis");	
		// insert in MaterUsers table
			$insertMasterUsers=mysql_query("INSERT INTO MasterUsers 
													(PlutoId,Pin,FirstAccount,Username,Password,PwdIsTemp,Email,SignupDate, FK_MasterUsers_Referrer,RecordVersion) 
											VALUES	('$prefix','$pin','$typeUser','$username','$password',0,'$email','$signupDate','$referrer',1)",$conn) OR die ('Could not insert into MasterUsers Database'.mysql_error($conn));
			$PK_MasterUsers=mysql_insert_id($conn);
			$plutoId=(calculatePlutoID($PK_MasterUsers));
			$updateMasterUsers=mysql_query("UPDATE MasterUsers SET PlutoId='$plutoId' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

			if($updateMasterUsers){
				// sync with PlutoVip
				$insertPlutoVipMasterUsers=mysql_query("INSERT INTO MasterUsers 
													(PK_MasterUsers,PlutoId,Pin,FirstAccount,Username,Email,SignupDate, FK_MasterUsers_Referrer,RecordVersion) 
											VALUES	('$PK_MasterUsers','$plutoId','$pin','$typeUser','$username','$email','$signupDate','$referrer',1)",$connPlutoVip);
				if($insertPlutoVipMasterUsers)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_PlutoVip='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);
				// sync with PlutoHome
				$insertPlutoHomeMasterUsers=mysql_query("INSERT INTO MasterUsers 
													(PK_MasterUsers,PlutoId,Pin,FirstAccount,Username,Email,SignupDate, FK_MasterUsers_Referrer,RecordVersion) 
											VALUES	('$PK_MasterUsers', '$plutoId','$pin','$typeUser','$username','$email','$signupDate','$referrer',1)",$connPlutoHome);
				if($insertPlutoHomeMasterUsers)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_PlutoHome='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

				$random_password=generatePassword();
				$random_passwordMD5=md5($random_password);
				// sync with phpBB
				$insertphpBB=mysql_query("INSERT INTO phpbb_users (user_id,username,user_password,user_email,user_lang)
													VALUES	('$PK_MasterUsers','$username','$random_passwordMD5','$email','english')",$connphpBB);
			
				$insertExtPassword=mysql_query("INSERT INTO Users(FK_MasterUsers,ExtPassword)
													VALUES	('$PK_MasterUsers','$random_password')",$connPlutoHome);
				if($insertphpBB && $insertExtPassword)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_phpBB='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);
				
				// sync with Mantis
				$insertMantis=mysql_query("INSERT INTO mantis_user_table (id, username, password, email,date_created,cookie_string)
													VALUES	('$PK_MasterUsers','$username','$random_passwordMD5','$email','NOW()','$PK_MasterUsers')",$connMantis);
				if($insertMantis && $insertExtPassword)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_Mantis='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

				
				// sync with Main
				$insertMain=mysql_query("INSERT INTO Users (PK_Users, Username, Password, samePasswordMasterUsers, ForwardEmail, FirstName, LastName, Nickname)
													VALUES	('$PK_MasterUsers','$username','$password','1', '$email','$username','$username','$username')",$connMain) OR die('error: '.mysql_error($connMain));
				if($insertMain)
					$updateMasterUsers=mysql_query("UPDATE MasterUsers SET Sync_Main='1' WHERE PK_MasterUsers='$PK_MasterUsers'",$conn);

			}
			if($insertMasterUsers){
				$out="MasterUsersID=$PK_MasterUsers";
			}
			else
				$out="MasterUsers database error."; 
		}
	}else{
		$out='';
	}
	echo $out;	
 }
?>