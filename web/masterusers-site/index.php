<?php
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$section = @$_REQUEST['section'];

switch ($section) {
	// master users area
	case 'add_master_user':
	    include_once('users/add_master_user.php');
	    add_master_user($conn,$connPlutoVip,$connPlutoHome,$connphpBB,$connMantis);
	break;
	case 'change_password':
	    include_once('users/change_password.php');
	    change_password($conn);
	break;
	case 'check_master_user':
	    include_once('users/check_master_user.php');
	    check_master_user($conn);
	break;
	case 'update_master_users':
	    include_once('users/update_master_users.php');
	    update_master_users($conn,$connPlutoVip,$connPlutoHome,$connphpBB,$connMantis);
	break;
	
	// referrer area
	case 'add_referrer':
	    include_once('referrer/add_referrer.php');
	    add_referrer($conn);
	break;
	case 'referral_summary':
	    include_once('referrer/referral_summary.php');
	    referral_summary($conn);
	break;
	
	default:		
      	echo "";
	break;
}
?>