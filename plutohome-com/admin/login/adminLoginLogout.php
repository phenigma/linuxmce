<?php
function adminLoginLogout($output) {
	$action = isset($_REQUEST['action'])?$_REQUEST['action']:'login';
	
	if ($action=='login') {
		
		$username = cleanString($_POST['usernameA'],50);
		$password = cleanString($_POST['passwordA'],50);
		
		$query = "select * from Users where UserName  = '$username' and Password = '".md5($password)."' and plutohomeAdminLevel=1";
		$res = mysql_query($query) or die_mesg_public($output,"can not select username/password","adminLoginLogout.php::".__LINE__,mysql_query()."::".$query);

		if (mysql_num_rows($res)==1) {
			$row=mysql_fetch_object($res);
			$_SESSION['adminIsLogged']=true;
			$_SESSION['adminID']=$row->PKID_Users;
			$_SESSION['adminName']=$row->FirstName." ".$row->LastName;			
			header("Location: index.php?section=adminHome");
			exit();
		} else {
			$_SESSION['adminIsLogged']=false;
			header("Location: index.php?section=adminLoginForm&error=Invalid Username Or Password!");
			exit();
		}
	} else {//logout
		$_SESSION['adminIsLogged']=false;
		session_destroy();	
		header("Location: index.php?section=adminLoggedOut");
		exit();
	}
	
}
?>