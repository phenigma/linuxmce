<?php
session_start('Pluto');
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
		}
	}
}
// end autologin check

$section = @$_REQUEST['section'];

switch ($section) {
	//admin area	
   	case 'search':
    	$output=new Template();
       	$output->setTemplateFileType('home');
       	include_once('search/search.php');
	    search($output,$dbADO,$conn);
   break;
   case 'searchWeb':
       	$output=new Template();
       	$output->setTemplateFileType('home');
       	include_once('search/searchWeb.php');
	    searchWeb($output,$conn);
	break;
   case 'searchDocs':
       	$output=new Template();
       	$output->setTemplateFileType('home');
       	include_once('search/searchDocs.php');
	    searchDocs($output,$conn);
	break;
   
   case 'clientLoginForm':
       $output = new Template();
       $output->setTemplateFileType('home');
       include_once('client/login/login.php');
	    login($output,$dbADO,$conn);
       break;
	case 'clientLoggedOut':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/login/clientLoggedOut.php');
	    clientLoggedOut($output);
	break;
	case 'register':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/login/register.php');
	    register($output,$conn);
	break;
	case 'myPluto':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/myPluto.php');
	    myPluto($output,$dbADO,$conn);
	break;
	case 'wizard':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/wizard.php');
	    wizard($output,$dbADO);
	break;
	
	case 'userHome':
      	$output = new Template();
		$output->setTemplateFileType('customer');
	    include_once('client/userHome.php');
	    userHome($output);
	break;
	case 'adminLoginForm':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('admin/login/adminLoginForm.php');
	    adminLoginForm($output);
	break;
	case 'adminLoginLogout':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('admin/login/adminLoginLogout.php');
	    adminLoginLogout($output);
	break;
	case 'adminLoggedOut':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('admin/login/adminLoggedOut.php');
	    adminLoggedOut($output);
	break;
	case 'adminHome':
		$output = new Template();
		$output->setTemplateFileType('admin');
	    include_once('admin/adminHome.php');
	    adminHome($output);
	break;
	case 'newsCategoriesManagement':
		$output = new Template();
		$output->setTemplateFileType('admin');
	    include_once('admin/news/newsCategoriesManagement.php');
	    newsCategoriesManagement($output);
	break;
	case 'newsManagement':
		$output = new Template();
		$output->setTemplateFileType('admin');
	    include_once('admin/news/newsManagement.php');
	    newsManagement($output);
	break;
	case 'faqCategoriesManagement':
		$output = new Template();
		$output->setTemplateFileType('admin');
	    include_once('admin/faq/faqCategoriesManagement.php');
	    faqCategoriesManagement($output);
	break;
	case 'faqManagement':
		$output = new Template();
		$output->setTemplateFileType('admin');
	    include_once('admin/faq/faqManagement.php');
	    faqManagement($output);
	break;
	case 'newsPresentation':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/newsPresentation.php');
	    newsPresentation($output);
	break;
	case 'samplePage':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/samplePage.php');
	    samplePage($output);
	break;
    case 'support':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/support.php');
	    support($output);
	break;
     case 'security':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/security.php');
	    security($output);
	break;
    case 'company':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/company.php');
	    company($output);
	break;
    case 'telecom':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/telecom.php');
	    telecom($output);
	break;
        case 'home_automation':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/home_automation.php');
	    home_automation($output);
	break;
        case 'media_entertainment':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/media_entertainment.php');
	    media_entertainment($output);
	break;
        case 'personal_computing':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/personal_computing.php');
	    personal_computing($output);
	break;
        case 'benefits_comparison':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/benefits_comparison.php');
	    benefits_comparison($output);
	break;
        case 'equipment_overview':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/equipment_overview.php');
	    equipment_overview($output);
	break;
        case 'core':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/core.php');
	    core($output);
	break;
        case 'media_director':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/media_director.php');
	    media_director($output);
    break;
        case 'orbiter':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/orbiter.php');
	    orbiter($output);
	break;
        case 'mobile_orbiter':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/mobile_orbiter.php');
	    mobile_orbiter($output);
	break;
        case 'accesories':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/accesories.php');
	    accesories($output);
	break;
        case 'special_offer':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/special_offer.php');
	    special_offer($output);
	break;
       case 'simplicity':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/simplicity.php');
	    simplicity($output);
	break;
        case 'benefits':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/benefits.php');
	    benefits($output);
	break;
        case 'reliability':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/reliability.php');
	    reliability($output);
	break;
        case 'cost':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/cost.php');
	    cost($output);
	break;
        case 'future':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/future.php');
	    future($output);
	break;
        case 'installation':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/installation.php');
	    installation($output);
	break;
        case 'project':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/project.php');
	    project($output);
	break;
        case 'warranty':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/warranty.php');
	    warranty($output);
	break;
         case 'download':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/download.php');
	    download($output);
    break;
        case 'compatibility':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/compatibility.php');
	    compatibility($output);
    break;
        case 'follow_me_works':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/follow_me_works.php');
	    follow_me_works($output);
    break;
        case 'use_mobile_phone':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/use_mobile_phone.php');
	    use_mobile_phone($output);
    break;
        case 'search_media':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/search_media.php');
	    search_media($output);
    break;
        case 'dvd_remote':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/dvd_remote.php');
	    dvd_remote($output);
    break;
        case 'tv_remote':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/tv_remote.php');
	    tv_remote($output);
    break;
        case 'manage_house':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/manage_house.php');
	    manage_house($output);
    break;
        case 'handle_security':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/handle_security.php');
	    handle_security($output);
    break;
        case 'manage_call':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/manage_call.php');
	    manage_call($output);
    break;
        case 'maps':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/maps.php');
	    maps($output);
    break;
        case 'software':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/software.php');
	    software($output);
    break;
        case 'get_pluto':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/get_pluto.php');
	    get_pluto($output);
    break;
        case 'locator':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/locator.php');
	    get_pluto($output);
    break;
        case 'dealer':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/dealer.php');
	    dealer($output);
    break;
        case 'manufacturer':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/manufacturer.php');
	    manufacturer($output);
    break;
        case 'developer':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/developer.php');
	    developer($output);
    break;
        case 'privacy_policy':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/privacy_policy.php');
	    privacy_policy($output);
    break;
        case 'site_map':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/site_map.php');
	    site_map($output);
    break;
        case 'faq_presentation':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/faq_presentation.php');
	    faq_presentation($output,$conn);
    break;
    case 'contact':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/contact.php');
	    contact($output);
    break;
    case 'technical_support':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/technical_support.php');
	    technical_support($output);
       break;
    case 'full_story':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/full_story.php');
	    full_story($output,$conn);
       break;
    case 'letter':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/letter.php');
	    letter($output);
       break;
    case 'ref_comm':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/ref_comm.php');
	    ref_comm($output);
    break;
    case 'public_license':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/public_license.php');
	    public_license($output);
    break;
    case 'dealerProgram':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerProgram.php');
	    dealerProgram($output);
    break;
    case 'dealerApplication':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerApplication.php');
	    dealerApplication($output,$conn);
    break;
    case 'updateProfile':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/updateProfile.php');
	    updateProfile($output,$conn,$dbADO);
    break;
    
    case 'programGuidelines':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/programGuidelines.php');
	    programGuidelines($output,$dbADO);
    break;
    case 'schedule':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/schedule.php');
	    schedule($output,$dbADO);
    break;
    case 'dealerTerms':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/dealerTerms.php');
	    dealerTerms($output);
    break;
    case 'fundRequestApplication':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/fundRequestApplication.php');
	    fundRequestApplication($output,$conn);
    break;
     case 'reimbursementClaimForm':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('dealer/reimbursementClaimForm.php');
	    reimbursementClaimForm($output,$conn);
    break;
       
	default:			
		$output = new Template();	
		$output->setTemplateFileType('home');
		include_once('operations/news.php');
	   news($output,$conn);
	break;
}
?>
