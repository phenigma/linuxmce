<?php
function faqCategoriesManagement($output) {      

require_once("include/simpleFormBuilder.class.php"); 	

	if ((!isset($_SESSION["adminIsLogged"]))||((int)$_SESSION["adminIsLogged"]==0)) {
		header("Location: index.php");
	}	
	
	$action=@trim($_REQUEST['action']);
	if ($action=='') {
		$action='list';
	}	
	
	require_once("include/translations/faqCategoriesManagement.en.inc.php");
	
	$tid = @$_REQUEST['faqCategID'];
	
	$formFields = array(
					"faqCategName"=>array(
							"faqCategName",
							"Faq categ. name",
							"input",
							"select faqCategName from FAQCategories where faqCategID='$tid'",
							"",
							"",
							"req||$trName^^maxlen=150||$trName",
							"",
							"FAQCategories.faqCategName",
							1,
							1,
							''					
					)
	);

	$selectFAQCategories = "
		SELECT 
			faqCategID,
	        faqCategName			
		FROM FAQCategories
	";
	
	
	$formBuilder = new simpleFormBuilder($output);
	$formBuilder->set_formActionToDo($action);
	$formBuilder->set_translationFile("include/translations/faqCategoriesManagement.en.inc.php");
	$formBuilder->set_formIdentityFieldName("faqCategID");
	$formBuilder->set_formIdentityFieldValue($tid);
	$formBuilder->set_formFieldDefinition($formFields);
	$formBuilder->set_sqlString($selectFAQCategories);
	$formBuilder->set_scriptName(__FILE__);		/* ?? */
	$formBuilder->set_formName("faqCategoriesManagement");
	$formBuilder->set_formAction("index.php?section=faqCategoriesManagement");
	$formBuilder->set_listDefaultOrderField("faqCategName");
	$formBuilder->set_listDefaultOrder("asc");
	$formBuilder->set_listDefaultRecordsDisplayed(20);
	$formBuilder->set_addIntoTable("FAQCategories");
	$formBuilder->set_deleteFromTable("FAQCategories");
	$formBuilder->set_doAfterSave("location::form");
	$formBuilder->set_alertMessageAfterSave("msg::Category modified");
	$formBuilder->set_doAfterDelete("alert::Category deleted");
	$formBuilder->set_doAfterAdd("Category added::Modify");
		
	
	$filter = array();
	$formBuilder->set_filterForList($filter);
	$formBuilder->set_filterPatternForList("1 AND 2 AND 3");
	/*
	$formBuilder->buildFilter(); 
	
	
	$formBuilder->set_auditParameters(array('am_productsaudit','paCategID','pa','am_admins.adminID.adminName','productName'));
	$formBuilder->set_auditQuery("insert into am_productsaudit(paCategID,paAdminID,paOperationType,paOldValue,paNewValue,paFieldName,paFieldType,paDate) values('::ID::','".$_SESSION['adminID']."','::TYPE::','::NEWV::','::OLDV::','::FIELD_NAME::','::FIELD_TYPE::',now())");
	$formBuilder->set_auditEnabled(1);
	*/

	
	
	$formBuilder->set_allowedOperations(array("list","add","modify", "delete", "show"));
	
	$out=$formBuilder->getHTMLResult();
	
	
 	
		$leftMenu = '	
			<table border=0>				
				<tr>
					<td class="text" valign="top" nowrap><a href="index.php?section=newsCategoriesManagement"><b>News Categs</b></a></td>
 				</tr>
 				<tr>
 					<td class="text" valign="top"><a href="index.php?section=newsManagement"><b>News</b></a></td>
 				</tr>
				<tr>
					<td class="text" valign="top" nowrap><a href="index.php?section=faqCategoriesManagement"><b>FAQ Categs</b></a></td>
 				</tr>
 				<tr>
 					<td class="text" valign="top"><a href="index.php?section=faqManagement"><b>FAQ</a></a></td>
 				</tr>
			</table>		
		';

	$output->setNavigationMenu(array("Admin home"=>"index.php?section=adminHome","FAQ categories management"=>"index.php?section=faqCategoriesManagement"));
	$output->setLeftMenu($leftMenu);	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::FAQ categories management");			
  	$output->output(); 
  	
}
?>