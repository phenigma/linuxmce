<?php
/*SELECT 
	u.PKID_Users, u.isPowerUser, i.FKID_household, i.PKID_Installation, h.Username 
	FROM 
		Users AS u 
		INNER JOIN Installation_Users AS iu ON iu.FKID_Users=u.PKID_Users 
		INNER JOIN Installation AS i ON iu.FKID_Installation=i.PKID_Installation 
		INNER JOIN Household AS h ON FKID_Household=h.PKID_Household 
	WHERE (u.UserName='marco') AND (u.Password='f5888d0bb58d611107e11f7cbc41c97a') 
	AND (i.PKID_Installation='64')*/

session_start();

require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');
require('include/masterusers.inc.php');

$section = @$_REQUEST['section'];

switch ($section) {
	//admin area	
   case 'search':
       $output=new Template();
       $output->setTemplateFileType('home');
       include_once('search/search.php');
	    search($output);
       break;
   case 'login':
       $output = new Template();
       $output->setTemplateFileType('home');
       include_once('client/login.php');
	    login($output);
       break;
   case 'userHome':
      $output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/userHome.php');
	    userHome($output);
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
   case 'userLogout':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/userLogout.php');
	    userLogout($output);
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
   case 'register':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/register.php');
	    register($output);
       break;
   case 'forgot':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/forgot.php');
	    forgot($output);
       break;
   case 'register_b':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('client/register_b.php');
	    register_b($output);
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
   case 'settings':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/settings.php');
	    settings($output);
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
	    faq_presentation($output);
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
	    full_story($output);
       break;
    case 'letter':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/letter.php');
	    letter($output);
       break;
   case 'individuals':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/individuals.php');
	    individuals($output);
       break;
   case 'businesses':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/businesses.php');
	    businesses($output);
       break;
   case 'how_pluto_vip_works':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/how_pluto_vip_works.php');
	    how_pluto_vip_works($output);
       break;
   case 'individual':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/individual.php');
	    individual($output);
       break;
   case 'business':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/business.php');
	    business($output);
       break;
    case 'im_manufacturer':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/im_manufacturer.php');
	    im_manufacturer($output);
       break;
    case 'contact_vip':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/contact_vip.php');
	    contact_vip($output);
       break;
    case 'refer_a_business':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/refer_a_business.php');
	    refer_a_business($output,1);
       break;
    case 'refer_a_individual':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/refer_a_business.php');
	    refer_a_business($output,0);
       break;
    case 'referral_summary':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/referral_summary.php');
	    referral_summary($output);
       break;
   case 'instructions':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/instructions.php');
	    instructions($output);
       break;
   case 'binstructions':
		$output = new Template();
		$output->setTemplateFileType('business1');
	    include_once('operations/binstructions.php');
	    binstructions($output);
       break;
    case 'credit':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/credit_cards.php');
	    credit_cards($output);
       break;
    case 'info_business':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/info_business.php');
	    info_business($output);
       break;
   case 'secure_payment':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/secure_payment.php');
	    secure_payment($output);
       break;
   case 'change_pass':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/change_pass.php');
	    change_pass($output);
       break;
    case 'change_email':
		$output = new Template();
		$output->setTemplateFileType('user');
	    include_once('operations/change_email.php');
	    change_email($output);
       break;
    case 'place':
		$output = new Template();
		$output->setTemplateFileType('home');
	    include_once('operations/place.php');
	    place($output);
       break;
    case 'visits':
		$output = new Template();
		$output->setTemplateFileType('business1');
	    include_once('operations/visits.php');
	    visits($output);
       break;
   case 'settings_b':
		$output = new Template();
		$output->setTemplateFileType('business');
	    include_once('operations/settings_b.php');
	    settings_b($output);
    break;
	case 'secure_payment_establishment':
		$output = new Template();
		$output->setTemplateFileType('business');
	    include_once('operations/secure_payment_establishment.php');
	    secure_payment_establishment($output);
	break;
   case 'bchange_pass':
		$output = new Template();
		$output->setTemplateFileType('business');
	    include_once('operations/bchange_pass.php');
	    bchange_pass($output);
       break;
    case 'bchange_email':
		$output = new Template();
		$output->setTemplateFileType('business');
	    include_once('operations/bchange_email.php');
	    bchange_email($output);
       break;
   case 'customer_changes':
	    include_once('client/customer_changes.php');
	    customer_changes();
       break;
      default:		
      	$output = new Template();
		$output->setTemplateFileType('home');	
		include_once('operations/news.php');
	    news($output);

	break;
}
?>