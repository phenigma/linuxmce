<?php
	session_name('Pluto');
	session_start();

	if($_SERVER['HTTP_HOST']=='www.plutohome.com'){
		header('Location: http://plutohome.com'.$_SERVER['PHP_SELF'].'?'.$_SERVER['QUERY_STRING']);
	}
	
	require('include/config/config.inc.php');
	require('include/utils.inc.php');
	require('include/masterusers.inc.php');
	require('include/template.class.inc.php');
	require('include/package.inc.php');

	// autologin check: if cookie is set grab the user's data from database
	if (@$_SESSION['userIsLogged']!="yes"){
		//print_r($_COOKIE);
		if(isset($_COOKIE['PlutoHomeAutoLogin'])){
			parse_str(base64_decode($_COOKIE['PlutoHomeAutoLogin']));
			$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
			if($isMasterUsers[0]){
				parse_str($isMasterUsers[1]);
				$_SESSION['userID'] = $MasterUsersID;
				$_SESSION['PlutoId'] = $PlutoId;
				$_SESSION['Pin'] = $Pin;
				$_SESSION['username'] = $username;
				$_SESSION['userIsLogged']="yes";
				$_SESSION['categ']=$FirstAccount;
				$_SESSION['Email']=$Email;
				$_SESSION['extPassword']=trim($extPassword);
				$_SESSION['userLoggedIn'] = true;
				$_SESSION['sqlCvsAdmin']=$sqlCvsAdmin;
			}
		}
	}
	// end autologin check	
	
	$section = @$_REQUEST['section'];
	$out='';
	$package=(isset($_SESSION['package']))?$_SESSION['package']:0;

	
	switch ($section) {	
		case '':
			$contentSection='operations/mainPage';
		break;
		case 'home':
			// todo: fix
			$contentSection='operations/mainPage';
			// $params='&docID=main';
		break;
		case 'logout':
			$contentSection='operations/logout';
		break;	
		case 'forum':
			header("Location: operations/forum.php?".$_SERVER['QUERY_STRING']);
		break;	
		case 'mantis':
			header("Location: operations/mantis.php?".$_SERVER['QUERY_STRING']);
		break;	
		case 'platforms':
			$contentSection='operations/platforms';
		break;
		case 'versions':
			$contentSection='operations/versions';
		break;
		case 'authors':
			$contentSection='operations/authors';
		break;
		case 'license':
			$contentSection='operations/license';
		break;
		case 'help':
			$contentSection='operations/help';
		break;
		case 'faq':
			$contentSection='operations/faq';
		break;
		case 'mail':
			$contentSection='operations/mail';
		break;
		case 'news':
			$contentSection='operations/news';
		break;
		case 'down_package':
			$contentSection='operations/download';
		break;
		case 'source_package':
			$contentSection='operations/source';
		break;
		case 'mainDownload':
			$contentSection='operations/mainDownload';
		break;
		case 'packageDownload':
			$contentSection='operations/packageDownload';
		break;
		case 'modules':
			$contentSection='operations/modules';
		break;
		case 'pluto':
			$contentSection='home';
		break;
		default:
			die('{Invalid section}'.$section);
		break;
	}
	

	$output = new Template($dbADO);
	$output->setTemplateFileType('large');
	include_once($contentSection.'.php');

	
?>