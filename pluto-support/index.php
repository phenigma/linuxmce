<?php
	session_start('Pluto');

	require('include/config/config.inc.php');
	require('include/utils.inc.php');
	require('include/masterusers.inc.php');
	require('include/template.class.inc.php');
	require('include/package.inc.php');
	
	$section = @$_REQUEST['section'];
	$out='';
	$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	//echo 'pack : '.$package;
	
	switch ($section) {	
		case '':
			$leftFile='documents/leftMenu';
			$leftParams='';

			$rightFile='documents/documentDisplay';
			$rightParams='&docID=1';
		break;
		case 'home':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/home';
			$rightParams='&docID=main';
		break;
		case 'logout':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/logout';
			$rightParams='';
		break;	
		case 'forum':
			header("Location: operations/forum.php?".$_SERVER['QUERY_STRING']);
		break;	
		case 'platforms':
			$leftFile='operations/leftHome';
			$leftParams='&docID='.@$_REQUEST['docID'];

			$rightFile='operations/platforms';
			$rightParams='&pkid='.@$_REQUEST['pkid'];
		break;
		case 'versions':
			$leftFile='operations/leftVersions';
			$leftParams='';

			$rightFile='operations/versions';
			$rightParams='';
		break;
		case 'authors':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/authors';
			$rightParams='';
		break;
		case 'license':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/license';
			$rightParams='';
			break;
		case 'help':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/help';
			$rightParams='';
			break;
		case 'faq':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/faq';
			$rightParams='';
			break;
		case 'mail':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/mail';
			$rightParams='';
			break;
		case 'news':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/news';
			$rightParams='';
		break;
		case 'usersmanual':
			$queryUM = 'SELECT FK_Document_UsersManual
							FROM Package
							WHERE PK_Package = ?';
			$resUM = $dbADO->Execute($queryUM,$package);
			$rowUM = $resUM->FetchRow();
			$idUM = $rowUM['FK_Document_UsersManual'];
			
			$leftFile='documents/leftMenu';
			$leftParams='&docID='.$idUM;
			
			$rightFile='documents/documentDisplay';
			$rightParams='&docID='.$idUM;
		break;
		case 'progguide':
			$queryUM = 'SELECT FK_Document_ProgrammersGuide
							FROM Package
							WHERE PK_Package = ?';
			$resUM = $dbADO->Execute($queryUM,$package);
			$rowUM = $resUM->FetchRow();
			$idUM = $rowUM['FK_Document_ProgrammersGuide'];

			$leftFile='documents/leftMenu';
			$leftParams='&docID='.$idUM;
			
			$rightFile='documents/documentDisplay';
			$rightParams='&docID='.$idUM;
		break;
		case 'down_package':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/download';
			$rightParams='';
		break;
		case 'source_package':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/source';
			$rightParams='';
		break;
		case 'document':
			$leftFile='documents/leftMenu';
			$leftParams='&docID='.@$_REQUEST['docID'];
			
			$rightFile='documents/documentDisplay';
			$rightParams='&docID='.@$_REQUEST['docID'];
		break;
		case 'mainDownload':
			$leftFile='operations/leftHome';
			$leftParams='&docID='.@$_REQUEST['docID'];
			
			$rightFile='operations/mainDownload';
			$rightParams='';
		break;
		case 'packageDownload':
			$leftFile='operations/leftHome';
			$leftParams='&docID='.@$_REQUEST['docID'];
			
			$rightFile='operations/packageDownload';
			$rightParams='&pkid='.@$_REQUEST['pkid'];
		break;
		case 'modules':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='operations/modules';
			$rightParams='';
		break;
		case 'pluto':
			$leftFile='operations/leftHome';
			$leftParams='';

			$rightFile='home';
			$rightParams='';
		break;
				
		default:			
			$out='{Invalid section}'.$section;	
		break;
	}
	
	
	
	// display frameset if no errors
	if($out=='')
	$out='
	<HTML>
		<HEAD>
		
		<meta http-equiv="Content-Language" content="en">        
		        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
		        <meta http-equiv="Pragma" content="no-cache" />
		        <meta http-equiv="Cache-Control" content="no-cache" />
		        <meta http-equiv="Expires" content="Mon, 03 Jan 2000 00:00:01 GMT" />
		        <meta name="keywords" content="">
				<meta name="description" content="">
		        <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
		        
		<title>'.APPLICATION_NAME.'</title>		
			<FRAMESET cols="250,*" onResize="if (navigator.family == \'nn4\') window.location.reload()" border="1"> 
		  		<FRAME src="left.php?section='.$leftFile.$leftParams.'" name="treeframe" > 
		  		<FRAME SRC="right.php?section='.$rightFile.$rightParams.'" name="basefrm"> 
			</FRAMESET>	
		</HTML>
		
	';
	
	print $out;
?>