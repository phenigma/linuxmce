<?php
function faqManagement($output) {      

require_once("include/simpleFormBuilder.class.php"); 	

	if ((!isset($_SESSION["adminIsLogged"]))||((int)$_SESSION["adminIsLogged"]==0)) {
		header("Location: index.php");
	}	
	
	$action=@trim($_REQUEST['action']);
	if ($action=='') {
		$action='list';
	}	
	
	require_once("include/translations/faqManagement.en.inc.php");
	
	$tid = @$_REQUEST['faqID'];
	
	$formFields = array(					
					"categoryName"=>array(
							"categoryName",
							"Category",
							"select",
							"select faqFAQCategID from FAQ where faqID='$tid'",							
							"select faqCategID, faqCategName as categoryName
							 from FAQCategories where markedDeleted != '1'
							 order by faqCategName ASC",
							"",
							"req||Please select a category!^^dontselect=0||Please select a category!",
							"",
							"FAQ.faqFAQCategID",
							1,
							1,
							''					
					),
					"faqQuestion"=>array(
							"faqQuestion",
							"Question",
							"spaw",
							"select faqQuestion from FAQ where faqID='$tid'",							
							"",
							"",
							"",
							"",
							"FAQ.faqQuestion",
							1,
							1,
							'substr($row->faqQuestion,0,50)'					
					),
					"faqAnswer"=>array(
							"faqAnswer",
							"Answer",
							"spaw",
							"select faqAnswer from FAQ where faqID='$tid'",							
							"",
							"",
							"",
							"",
							"FAQ.faqAnswer",
							1,
							1,
							'substr($row->faqAnswer,0,50)'					
					)
	);

	$selectFAQ = "
		SELECT 
			faqID,
			faqCategName as categoryName,
			faqAnswer,faqQuestion			
		FROM FAQ left join FAQCategories on faqFAQCategID = faqCategID
	";
	
	
	$formBuilder = new simpleFormBuilder($output);
	$formBuilder->set_formActionToDo($action);
	$formBuilder->set_translationFile("include/translations/faqManagement.en.inc.php");
	$formBuilder->set_formIdentityFieldName("faqID");
	$formBuilder->set_formIdentityFieldValue($tid);
	$formBuilder->set_formFieldDefinition($formFields);
	$formBuilder->set_sqlString($selectFAQ);
	$formBuilder->set_scriptName(__FILE__);		/* ?? */
	$formBuilder->set_formName("faqManagement");
	$formBuilder->set_formAction("index.php?section=faqManagement");
	$formBuilder->set_listDefaultOrderField("categoryName");
	$formBuilder->set_listDefaultOrder("asc");
	$formBuilder->set_listDefaultRecordsDisplayed(20);
	$formBuilder->set_addIntoTable("FAQ");
	$formBuilder->set_deleteFromTable("FAQ");
	$formBuilder->set_doAfterSave("location::form");
	$formBuilder->set_alertMessageAfterSave("msg::FAQ modified");
	$formBuilder->set_doAfterDelete("alert::FAQ deleted");
	$formBuilder->set_doAfterAdd("FAQ added::Modify");
		
	
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
 					<td class="text" valign="top"><a href="index.php?section=faqManagement"><b>FAQ</b></a></td>
 				</tr>
			</table>		
		';

	$output->setNavigationMenu(array("Admin home"=>"index.php?section=adminHome","FAQ management"=>"index.php?section=faqManagement"));
	$output->setLeftMenu($leftMenu);	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::FAQ management");			
  	$output->output(); 
  	
}
?>