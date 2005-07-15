<?php
session_name('Pluto');
session_start();
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/masterusers.inc.php');
require('include/utils.inc.php');

if($_SERVER['HTTP_HOST']=='www.plutohome.com'){
	header('Location: http://plutohome.com'.$_SERVER['PHP_SELF'].'?'.$_SERVER['QUERY_STRING']);
}
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
			$_SESSION['extPassword']=$extPassword;
			$_SESSION['userLoggedIn'] = true;
		}
	}
}
// end autologin check

$section = @$_REQUEST['section'];

switch ($section) {
	//admin area	
   	case 'search':
    	$output=new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
       	include_once('search/search.php');
	    search($output,$dbADO,$conn);
   break;
   case 'searchWeb':
       	$output=new Template();
       	$output->setConn($conn);
       	$output->setTemplateFileType('home');
       	include_once('search/searchWeb.php');
	    searchWeb($output,$conn);
	break;
   case 'searchDocs':
       	$output=new Template();
       	$output->setConn($conn);
       	$output->setTemplateFileType('home');
       	include_once('search/searchDocs.php');
	    searchDocs($output,$dbADO,$conn);
	break;
   
   case 'login':
       $output = new Template();
       $output->setConn($conn);
       $output->setTemplateFileType('home');
       include_once('client/login/login.php');
	   login($output,$dbADO,$conn);
       break;
	case 'logout':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('client/login/clientLoggedOut.php');
	    clientLoggedOut($output);
	break;
	case 'register':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('client/login/register.php');
	    register($output,$conn);
	break;
	case 'myPluto':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('nonews');
	    include_once('client/myPluto.php');
	    myPluto($output,$dbADO,$conn);
	break;
	case 'wizard':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('nonews');
	    include_once('client/wizard.php');
	    wizard($output,$dbADO);
	break;
	
	case 'userHome':
      	$output = new Template();
		$output->setTemplateFileType('customer');
	    include_once('client/userHome.php');
	    userHome($output);
	break;
	case 'newsPresentation':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/newsPresentation.php');
	    newsPresentation($output,$conn);
	break;
	case 'what_is_pluto':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/what_is_pluto.php');
	    what_is_pluto($output,$conn);
	break;
    case 'full_story':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/full_story.php');
	    full_story($output,$conn);
	break;
    case 'updateProfile':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/updateProfile.php');
	    updateProfile($output,$conn,$dbADO);
    break;
    case 'requestInstallationAssistance':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/requestInstallationAssistance.php');
	    requestInstallationAssistance($output,$conn,$dbADO);
    break;
    case 'security':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/security.php');
	    security($output);
	break;
    case 'telecom':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/telecom.php');
	    telecom($output);
	break;
	case 'home_automation':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/home_automation.php');
	    home_automation($output);
	break;
	case 'media_entertainment':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/media_entertainment.php');
	    media_entertainment($output);
	break;
	case 'personal_computing':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/personal_computing.php');
	    personal_computing($output);
	break;
	case 'why_you_need_pluto':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/why_you_need_pluto.php');
	    why_you_need_pluto($output,$conn);
	break;
	case 'how_pluto_works':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/how_pluto_works.php');
	    how_pluto_works($output,$conn);
	break;
	case 'pluto_system_overview':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/pluto_system_overview.php');
	    pluto_system_overview($output,$conn);
	break;
	case 'how_the_system_works':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/how_the_system_works.php');
	    how_the_system_works($output,$conn);
	break;	
	case 'learn_how_to_use_it':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/learn_how_to_use_it.php');
	    learn_how_to_use_it($output,$conn);
	break;
    case 'letter':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/letter.php');
	    letter($output);
	break;
	case 'developer':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/developer.php');
	    developer($output,$conn);
    break;
	case 'pluto_core':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/pluto_core.php');
	    pluto_core($output,$conn);
    break;
	case 'media_director':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/media_director.php');
	    media_director($output,$conn);
    break;
	case 'accesories':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/accesories.php');
	    accesories($output,$conn);
    break;
	case 'support':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/support.php');
	    support($output,$conn);
    break;
	case 'installation':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/installation.php');
	    installation($output,$conn);
    break;
	case 'compatibility':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/compatibility.php');
	    compatibility($output,$conn);
    break;
	case 'company':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/company.php');
	    company($output,$conn);
    break;
	case 'get_pluto':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/get_pluto.php');
	    get_pluto($output,$conn);
    break;
	case 'locator':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/locator.php');
	    locator($output,$conn);
    break;
	case 'how_to_get':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/how_to_get.php');
	    how_to_get($output,$conn);
    break;
	case 'mf_psa':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/manufacturer_price.php');
	    manufacturer($output,$conn);
    break;    
	case 'de_hjc':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/dealer_price.php');
	    dealer($output,$conn);
    break; 
	case 'mf_ori':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/manufacturer2.php');
	    manufacturer($output,$conn);
    break;    
	case 'de_akj':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/dealer2.php');
	    dealer($output,$conn);
    break; 
	case 'manufacturer':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/manufacturer.php');
	    manufacturer($output,$conn);
    break;    
	case 'dealer':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('operations/dealer.php');
	    dealer($output,$conn);
    break; 
    case 'dealerProgram':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerProgram.php');
	    dealerProgram($output);
    break;
    case 'dealerApplication':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerApplication.php');
	    dealerApplication($output,$conn);
    break;
    case 'programGuidelines':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/programGuidelines.php');
	    programGuidelines($output,$dbADO);
    break;
    case 'schedule':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/schedule.php');
	    schedule($output,$dbADO);
    break;
    case 'dealerTerms':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerTerms.php');
	    dealerTerms($output);
    break;
    case 'fundRequestApplication':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/fundRequestApplication.php');
	    fundRequestApplication($output,$conn);
    break;
     case 'reimbursementClaimForm':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('dealer/reimbursementClaimForm.php');
	    reimbursementClaimForm($output,$conn);
    break;
	case 'clientLoggedOut':
		$output = new Template();
		$output->setConn($conn);
		$output->setTemplateFileType('home');
	    include_once('client/login/clientLoggedOut.php');
	    clientLoggedOut($output);
	break;     
	case 'clientLoginForm':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('client/login/login.php');
	    login($output,$dbADO,$conn);
	break; 
	case 'contact':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('operations/contact.php');
	    contact($output,$conn);
	break; 
	case 'forgotPassword':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('client/login/forgotPassword.php');
	    forgotPassword($output,$conn);
	break; 
	case 'changePassword':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('client/login/changePassword.php');
	    changePassword($output);
	break; 
	case 'changeEmail':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('client/login/changeEmail.php');
	    changeEmail($output);
	break; 
	case 'deleteInstallation':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('home');
		include_once('client/deleteInstallation.php');
	    deleteInstallation($output,$dbADO);
	break; 
	case 'ir':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('operations/ir.php');
	    ir($output);
	break; 	
	case 'irCodes':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('operations/irCodes.php');
	    irCodes($output);
	break; 
	case 'newIRCode':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('popup');
		include_once('operations/newIRCode.php');
	    newIRCode($output);
	break;	
	case 'editAVDevice':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('operations/editAVDevice.php');
	    editAVDevice($output);
	break; 

	case 'admin':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('admin/admin.php');
	    admin($output);
	break; 
	case 'requestFeedback':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('admin/requestFeedback.php');
	    requestFeedback($output);
	break;
	case 'feedbackQuestions':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('admin/feedbackQuestions.php');
	    feedbackQuestions($output);
	break;
	case 'editQuestion':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('admin/editQuestion.php');
	    editQuestion($output);
	break;
	case 'public_license':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('operations/public_license.php');
	    public_license($output);
	break;
	case 'dealer_signin':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/dealer_signin.php');
	    dealer_signin($output,$dbADO);
	break;
	case 'manufacturer_signin':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/manufacturer_signin.php');
	    manufacturer_signin($output,$dbADO);
	break;
	case 'dealer_area':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/dealer_area.php');
	    dealer_area($output,$dbADO);
	break;
	case 'dealer_installation':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/dealer_installation.php');
	    dealer_installation($output,$dbADO);
	break;
	case 'dealer_license_device':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/dealer_license_device.php');
	    dealer_license_device($output,$dbADO);
	break;
	case 'edit_dealer_profile':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/edit_dealer_profile.php');
	    edit_dealer_profile($output,$conn,$dbADO);
	break;
	case 'manufacturer_area':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/manufacturer_area.php');
	    manufacturer_area($output,$dbADO);
	break;
	case 'edit_manufacturer_profile':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('dealer/edit_manufacturer_profile.php');
	    edit_manufacturer_profile($output,$conn,$dbADO);
	break;
	case 'irGroup':
    	$output = new Template();
    	$output->setConn($conn);
       	$output->setTemplateFileType('nonews');
		include_once('operations/irGroup.php');
	    irGroup($output,$conn,$dbADO);
	break;

	
	default:			
		$output = new Template();	
		$output->setConn($conn);
		$output->setTemplateFileType('nonav');
		include_once('operations/home.php');
	    home($output,$conn);
	break;
}
?>
