<?php
function adminLoginLogout($output) {
	global $checkMasterUserUrl;
	
	$action = isset($_REQUEST['action'])?$_REQUEST['action']:'login';
	
	if ($action=='login') {
		
		$username = cleanString($_POST['usernameA'],50);
		$password = cleanString($_POST['passwordA'],50);
		
		$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
		if(!$isMasterUsers[0]){
			$_SESSION['adminIsLogged']=false;
			header("Location: index.php?section=adminLoginForm&error=Invalid Username Or Password!");
			exit();
		}
		else{
			parse_str($isMasterUsers[1]);
			$res=mysql_query("SELECT * FROM Users WHERE FK_MasterUsers='$MasterUsersID'");
			if(mysql_num_rows($res)!=0){
				$res=mysql_fetch_object($res);
				if($res->Level==1){
					$_SESSION['userID'] = $MasterUsersID;
					$_SESSION['PlutoId'] = $PlutoId;
					$_SESSION['Pin'] = $Pin;
					$_SESSION['username'] = $username;
					$_SESSION['userIsLogged']="yes";
					$_SESSION['categ']=$FirstAccount;
					$_SESSION['Email']=$Email;
					$_SESSION['adminIsLogged']=true;
					$_SESSION['adminID']=$row->PKID_Users;
					$_SESSION['adminName']=$row->FirstName." ".$row->LastName;			
					header("Location: index.php?section=adminHome");
					exit();
				}else {
					$_SESSION['adminIsLogged']=false;
					header("Location: index.php?section=adminLoginForm&error=Unauthorised user!");
					exit();
				}
			}
		} 
	} else {//logout
		$_SESSION['adminIsLogged']=false;
		session_destroy();	
		header("Location: index.php?section=adminLoggedOut");
		exit();
	}
	
}
?>