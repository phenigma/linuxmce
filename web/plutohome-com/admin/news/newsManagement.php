<?php
function newsManagement($output) {      

require_once("include/simpleFormBuilder.class.php"); 	

	if ((!isset($_SESSION["adminIsLogged"]))||((int)$_SESSION["adminIsLogged"]==0)) {
		header("Location: index.php");
	}	
	
	$action=@trim($_REQUEST['action']);
	if ($action=='') {
		$action='list';
	}	
	
	require_once("include/translations/newsManagement.en.inc.php");
	
	$tid = @$_REQUEST['newsID'];
	
	$formFields = array(
					"newsTitle"=>array(
							"newsTitle",
							"Title",
							"input,20",
							"select newsTitle from News where newsID='$tid'",
							"",
							"",
							"req||$trName^^maxlen=150||$trName",
							"",
							"News.newsTitle",
							1,
							1,
							''					
					),
					"categoryName"=>array(
							"categoryName",
							"Category",
							"select",
							"select newsNewsCategID from News where newsID='$tid'",							
							"select ncID, ncName as categoryName
							 from NewsCategories where markedDeleted != '1'
							 order by ncName ASC",
							"",
							"req||Please select a category!^^dontselect=0||Please select a category!",
							"",
							"News.newsNewsCategID",
							1,
							1,
							''					
					),
					"newsContent"=>array(
							"newsContent",
							"Content",
							"spaw",
							"select newsContent from News where newsID='$tid'",							
							"",
							"",
							"",
							"",
							"News.newsContent",
							0,
							0,
							''					
					)
	);

	$selectNews = "
		SELECT 
			newsID,
			ncName as categoryName,
			newsTitle,
			newsNewsCategID,
			newsContent
		FROM News left join NewsCategories on newsNewsCategID = ncID
	";
	
	
	$formBuilder = new simpleFormBuilder($output);
	$formBuilder->set_formActionToDo($action);
	$formBuilder->set_translationFile("include/translations/newsManagement.en.inc.php");
	$formBuilder->set_formIdentityFieldName("newsID");
	$formBuilder->set_formIdentityFieldValue($tid);
	$formBuilder->set_formFieldDefinition($formFields);
	$formBuilder->set_sqlString($selectNews);
	$formBuilder->set_scriptName(__FILE__);		/* ?? */
	$formBuilder->set_formName("newsManagement");
	$formBuilder->set_formAction("index.php?section=newsManagement");
	$formBuilder->set_listDefaultOrderField("newsTitle");
	$formBuilder->set_listDefaultOrder("asc");
	$formBuilder->set_listDefaultRecordsDisplayed(20);
	$formBuilder->set_addIntoTable("News");
	$formBuilder->set_deleteFromTable("News");
	$formBuilder->set_doAfterSave("location::form");
	$formBuilder->set_alertMessageAfterSave("msg::News modified");
	$formBuilder->set_doAfterDelete("alert::News deleted");
	$formBuilder->set_doAfterAdd("News added::Modify");
		
	
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

	$output->setNavigationMenu(array("Admin home"=>"index.php?section=adminHome","News management"=>"index.php?section=newsManagement"));
	$output->setLeftMenu($leftMenu);	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::News management");			
  	$output->output(); 
  	
}
?>