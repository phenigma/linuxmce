<?php
function userLogout($output) {
	
	
		$_SESSION['adminIsLogged']=false;
		session_destroy();	
		header("Location: index.php");
		exit();

	
}
?>