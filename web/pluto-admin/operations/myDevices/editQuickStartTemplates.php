<?
function editQuickStartTemplates($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	if ($action == 'form') {
		
		
	
		
	$tree=getQuickStartTree($dbADO);
	
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="editQuickStartTemplates" enctype="multipart/form-data">
		<input type="hidden" name="section" value="editQuickStartTemplates">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="currentPos" value="">
		
	<div align="center"><h3>Edit Quick Start templates</h3></div><br>
	'.$tree.'
	<input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editQuickStartTemplates&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$msg='Quick start templates was updated.';
		$categoriesArray=explode(',',$_POST['categories']);
		foreach ($categoriesArray AS $category){
			$description=@$_POST['category_'.$category];
			$parent=((int)@$_POST['parent_'.$category]>0)?(int)$_POST['parent_'.$category]:NULL;
			$dbADO->Execute('UPDATE QuickStartCategory SET Description=?,FK_QuickStartCategory_Parent=? WHERE PK_QuickStartCategory=?',array($description,$parent,$category));
			
			$templates=explode(',',@$_POST['templates_'.$category]);
			foreach ($templates AS $qst){
				if((int)$qst!=0){
					$description=$_POST['description_'.$qst];
					$package=((int)$_POST['package_'.$qst]>0)?(int)$_POST['package_'.$qst]:NULL;
					$binary=$_POST['binary_'.$qst];
					$arguments=str_replace(" ","\t",$_POST['arguments_'.$qst]);
					$homepage=$_POST['homepage_'.$qst];
					$icon=$_POST['icon_'.$qst];
					$designobj=((int)$_POST['designobj_'.$qst]>0)?(int)$_POST['designobj_'.$qst]:NULL;
					$osd=((int)$_POST['osd_'.$qst]>0)?(int)$_POST['osd_'.$qst]:NULL;
					
					$dbADO->Execute('
						UPDATE QuickStartTemplate SET 
							Description=?,  
							FK_Package=?,
							`Binary`=?,
							Arguments=?,
							Homepage=?,
							Icon=?,
							FK_DesignObj=?,
							FK_DesignObj_OSD=?
						WHERE PK_QuickStartTemplate=?',
					array($description,$package,$binary,$arguments,$homepage,$icon,$designobj,$osd,$qst));
				}
			}
		}
		
		$newCategory=stripslashes($_POST['newCategory']);
		$newParent=((int)$_POST['newParent']>0)?(int)$_POST['newParent']:NULL;
		if($newCategory!=''){
			$dbADO->Execute('INSERT INTO QuickStartCategory (Description,FK_QuickStartCategory_Parent) VALUES (?,?)',array($newCategory,$newParent));
			$msg.='<br>New quick start category was added.';
		}
		
		
		$newTemplate=stripslashes($_POST['newTemplate']);
		$newTemplateCategory=((int)$_POST['newTemplateCategory']>0)?(int)$_POST['newTemplateCategory']:NULL;
		if($newTemplate!=''){
			if((int)$newTemplateCategory!=0){
				$dbADO->Execute('INSERT INTO QuickStartTemplate (Description,FK_QuickStartCategory) VALUES (?,?)',array($newTemplate,$newTemplateCategory));
				$msg.='<br>New quick start template was added.';
			}else{
				$err='Please select category type for quick start template.';
			}
		}
		
		$deleteCategory=(int)@$_POST['deleteCategory'];
		if($deleteCategory>0){
			$dbADO->Execute('DELETE FROM QuickStartTemplate WHERE FK_QuickStartCategory=?',$deleteCategory);
			$dbADO->Execute('UPDATE QuickStartCategory SET FK_QuickStartCategory_Parent=NULL WHERE FK_QuickStartCategory_Parent=?',$deleteCategory);
			$dbADO->Execute('DELETE FROM QuickStartCategory WHERE PK_QuickStartCategory=?',$deleteCategory);
			$msg.='<br>The category and the templates from it had been deleted.';
		}

		$deleteTemplate=(int)@$_POST['deleteTemplate'];
		if($deleteTemplate>0){
			$dbADO->Execute('DELETE FROM QuickStartTemplate WHERE PK_QuickStartTemplate=?',$deleteTemplate);
			$msg.='<br>The quick start template had been deleted.';
		}
		
		header("Location: index.php?section=editQuickStartTemplates&msg=$msg&error=".@$err);		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Quick start templates');
	$output->output();
}


function getQuickStartTemplates($dbADO){
	$res=$dbADO->Execute('SELECT * FROM QuickStartTemplate');
	$qsTemplates=array();
	while($row=$res->FetchRow()){
		$qsTemplates[$row['FK_QuickStartCategory']][]=$row['PK_QuickStartTemplate'];
		
		$qsTemplates['description'][$row['PK_QuickStartTemplate']]=$row['Description'];
		$qsTemplates['package'][$row['PK_QuickStartTemplate']]=$row['FK_Package'];
		$qsTemplates['binary'][$row['PK_QuickStartTemplate']]=$row['Binary'];
		$qsTemplates['arguments'][$row['PK_QuickStartTemplate']]=$row['Arguments'];
		$qsTemplates['homepage'][$row['PK_QuickStartTemplate']]=$row['Homepage'];
		$qsTemplates['icon'][$row['PK_QuickStartTemplate']]=$row['Icon'];
		$qsTemplates['designobj'][$row['PK_QuickStartTemplate']]=$row['FK_DesignObj'];
		$qsTemplates['osd'][$row['PK_QuickStartTemplate']]=$row['FK_DesignObj_OSD'];
	}
	
	return $qsTemplates;
}

function getQuickStartTree($dbADO){
	global $designobj,$categories;
	$nodes=getNodesArray('QuickStartCategory','PK_QuickStartCategory','FK_QuickStartCategory_Parent',$dbADO);
	$quickStartTemplates=getQuickStartTemplates($dbADO);
	$packages=getAssocArray('Package','PK_Package','Description',$dbADO,'','ORDER BY Description ASC');
	$designobj=getAssocArray('DesignObj','PK_DesignObj','Description',$dbADO,'','ORDER BY Description ASC');
	$categories=getQSCategories($nodes);

	$out='
		<input type="hidden" name="deleteCategory" value="">
		<input type="hidden" name="deleteTemplate" value="">
	<table cellpadding="3" cellspacing="0" align="center">
		<tr bgcolor="lightblue">
			<td colspan="9"><B>Quick start templates</B></td>
		</tr>';
	foreach ($nodes['root_node'] AS $rootNode){
		$available_parents=$categories;
		unset($available_parents[$rootNode]);

		$out.='
		<tr>
			<td bgcolor="#EEEEEE" colspan="2"><B>Category: <input type="text" name="category_'.$rootNode.'" value="'.$nodes['description'][$rootNode].'"></B></td>
			<td bgcolor="#EEEEEE" colspan="4"><B>Parent category: '.pulldownFromArray($available_parents,'parent_'.$rootNode,$nodes['parent'][$rootNode]).'</B></td>
			<td bgcolor="#EEEEEE" colspan="3" align="right"><B><input type="button" class="button" name="qsc_'.$rootNode.'" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start category? All templates from this category will be deleted too.\')){document.editQuickStartTemplates.deleteCategory.value=\''.$rootNode.'\';document.editQuickStartTemplates.submit();}"></B></td>
		</tr>
		<tr bgcolor="#EFFEFF">
			<td align="center"><B>Description</B></td>
			<td align="center"><B>Package</B></td>
			<td align="center"><B>Binary</B></td>
			<td align="center"><B>Arguments</B></td>
			<td align="center"><B>Homepage</B></td>
			<td align="center"><B>Icon</B></td>
			<td align="center"><B>DesignObj</B></td>
			<td align="center"><B>OSD</B></td>
			<td align="center"><B>Action</B></td>
		</tr>		';		
		if(count(@$quickStartTemplates[$rootNode])==0){
			$out.='
				<tr>
					<td style="padding-left:20px;" colspan="9">No quick start templates in this category.</td>
				</tr>
			';
		}
		$templates=array();
		foreach ($quickStartTemplates[$rootNode] AS $qsTemplate){
			$templates[$rootNode][]=$qsTemplate;
			$out.='
				<tr>
					<td><input type="text" name="description_'.$qsTemplate.'" value="'.$quickStartTemplates['description'][$qsTemplate].'"></td>
					<td>'.pulldownFromArray($packages,'package_'.$qsTemplate,$quickStartTemplates['package'][$qsTemplate]).'</td>
					<td><input type="text" name="binary_'.$qsTemplate.'" value="'.$quickStartTemplates['binary'][$qsTemplate].'"></td>
					<td><input type="text" name="arguments_'.$qsTemplate.'" value="'.$quickStartTemplates['arguments'][$qsTemplate].'"></td>
					<td><input type="text" name="homepage_'.$qsTemplate.'" value="'.$quickStartTemplates['homepage'][$qsTemplate].'"></td>
					<td><input type="text" name="icon_'.$qsTemplate.'" value="'.$quickStartTemplates['icon'][$qsTemplate].'"></td>
					<td>'.pulldownFromArray($designobj,'designobj_'.$qsTemplate,$quickStartTemplates['designobj'][$qsTemplate]).'</td>
					<td>'.pulldownFromArray($designobj,'osd_'.$qsTemplate,$quickStartTemplates['osd'][$qsTemplate]).'</td>
					<td align="right"><input type="button" class="button" name="del" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start template?\')){document.editQuickStartTemplates.deleteTemplate.value=\''.$qsTemplate.'\';document.editQuickStartTemplates.submit();}"></td>
				</tr>
			';
		}
		$out.='<input type="hidden" name="templates_'.$rootNode.'" value="'.@join(',',$templates[$rootNode]).'">';
		$out.=getQuickStartChilds($nodes,$quickStartTemplates,$rootNode,$packages);

	}
	$out.='
		<tr>
			<td colspan="9" align="center"><input type="submit" class="button" name="add" value="Update"></td>
		</tr>
		<tr>
			<td colspan="4" align="right"><B>Add category:</B> <input type="text" name="newCategory" value=""></td>
			<td colspan="5"> in '.pulldownFromArray($categories,'newParent',0).'</td>
		</tr>	
		<tr>
			<td colspan="4" align="right"><B>Add template:</B> <input type="text" name="newTemplate" value=""></td>
			<td colspan="5"> in '.pulldownFromArray($categories,'newTemplateCategory',0).'</td>
		</tr>	
		<tr>
			<td colspan="9" align="center"><input type="submit" class="button" name="add" value="Add"> <input type="button" class="button" name="close" value="Close" onclick="self.close();"></td>
		</tr>	
		<tr>
			<td colspan="9" align="left">* To make this changes permanent, please use sqlCVS module from <B>Advanced &gt; sqlCVS &gt; CheckIn</B></td>
		</tr>	
	</table>
	<input type="hidden" name="categories" value="'.join(',',array_keys($categories)).'">';
	
	return $out;
}

function getQuickStartChilds($nodes,$quickStartTemplates,$selectedCategory,$packages){
	global $designobj,$categories;
	$out='';
	foreach ($nodes[$selectedCategory] AS $childNode){
		$available_parents=$categories;
		unset($available_parents[$childNode]);

		$out.='
		<tr>
			<td bgcolor="#EEEEEE" colspan="2"><B>Category: <input type="text" name="category_'.$childNode.'" value="'.$nodes['description'][$childNode].'"></B></td>
			<td bgcolor="#EEEEEE" colspan="4"><B>Parent category: '.pulldownFromArray($available_parents,'parent_'.$childNode,$nodes['parent'][$childNode]).'</B></td>
			<td bgcolor="#EEEEEE" colspan="3" align="right"><B><input type="button" class="button" name="qsc_'.$childNode.'" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start category? All templates from this category will be deleted too.\')){document.editQuickStartTemplates.deleteCategory.value=\''.$childNode.'\';document.editQuickStartTemplates.submit();}"></B></td>
		</tr>
		<tr bgcolor="#EFFEFF">
			<td><B>Description</B></td>
			<td><B>Package</B></td>
			<td><B>Binary</B></td>
			<td><B>Arguments</B></td>
			<td><B>Homepage</B></td>
			<td><B>Icon</B></td>
			<td><B>DesignObj</B></td>
			<td><B>OSD</B></td>
			<td align="center"><B>Action</B></td>
		</tr>';		
		if(count(@$quickStartTemplates[$childNode])==0){
			$out.='
				<tr>
					<td style="padding-left:20px;" colspan="9">No quick start templates in this category.</td>
				</tr>
			';
		}
		$templates=array();
		foreach ($quickStartTemplates[$childNode] AS $qsTemplate){
			$templates[$childNode][]=$qsTemplate;
			$out.='
				<tr>
					<td><input type="text" name="description_'.$qsTemplate.'" value="'.$quickStartTemplates['description'][$qsTemplate].'"></td>
					<td>'.pulldownFromArray($packages,'package_'.$qsTemplate,$quickStartTemplates['package'][$qsTemplate]).'</td>
					<td><input type="text" name="binary_'.$qsTemplate.'" value="'.$quickStartTemplates['binary'][$qsTemplate].'"></td>
					<td><input type="text" name="arguments_'.$qsTemplate.'" value="'.$quickStartTemplates['arguments'][$qsTemplate].'"></td>
					<td><input type="text" name="homepage_'.$qsTemplate.'" value="'.$quickStartTemplates['homepage'][$qsTemplate].'"></td>
					<td><input type="text" name="icon_'.$qsTemplate.'" value="'.$quickStartTemplates['icon'][$qsTemplate].'"></td>
					<td>'.pulldownFromArray($designobj,'designobj_'.$qsTemplate,$quickStartTemplates['osd'][$qsTemplate]).'</td>
					<td>'.pulldownFromArray($designobj,'osd_'.$qsTemplate,$quickStartTemplates['designobj'][$qsTemplate]).'</td>
					<td align="right"><input type="button" class="button" name="del" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start template?\')){document.editQuickStartTemplates.deleteTemplate.value=\''.$qsTemplate.'\';document.editQuickStartTemplates.submit();}"></td>
				</tr>
			';
		}

		$out.='<input type="hidden" name="templates_'.$childNode.'" value="'.@join(',',$templates[$childNode]).'">';
		$out.=getQuickStartChilds($nodes,$quickStartTemplates,$childNode,$packages);		
	}
	
	return $out;
}

function getQSCategories($nodes){
	$reorderedArray=array();
	foreach ($nodes['root_node'] AS $rootNode){
		$reorderedArray[$rootNode]=$nodes['description'][$rootNode];
		
		$reorderedArray=$reorderedArray+getReorderedChilds($nodes,$rootNode,$nodes['description'][$rootNode]);
	}
	
	return $reorderedArray;
}

function getReorderedChilds($nodes,$rootNode,$description){
	$reorderedArray=array();
	foreach ($nodes[$rootNode] as $childNode) {
		$reorderedArray[$childNode]=$description.' - '.$nodes['description'][$childNode];
		
		$reorderedArray=$reorderedArray+getReorderedChilds($nodes,$childNode,$nodes['description'][$childNode]);
	}
	
	return $reorderedArray;
}
?>
