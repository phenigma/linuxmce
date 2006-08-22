<?
function editQuickStartTemplates($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
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
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editQuickStartTemplates&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$msg='Quick start templates were updated.';
		$categoriesArray=explode(',',$_POST['categories']);
		foreach ($categoriesArray AS $category){
			$description=@$_POST['category_'.$category];
			$parent=((int)@$_POST['parent_'.$category]>0)?(int)$_POST['parent_'.$category]:NULL;
			$dbADO->Execute('UPDATE QuickStartCategory SET Description=?,FK_QuickStartCategory_Parent=? WHERE PK_QuickStartCategory=?',array($description,$parent,$category));
		}
		
		$templates=explode(',',@$_POST['templates']);
		foreach ($templates AS $qst){
			if((int)$qst!=0){
				$description=cleanString($_POST['description_'.$qst]);
				$package=((int)$_POST['package_'.$qst]>0)?(int)$_POST['package_'.$qst]:NULL;
				$binary=$_POST['binary_'.$qst];
				$arguments=cleanString(str_replace(" ","\t",$_POST['arguments_'.$qst]));
				$homepage=cleanString($_POST['homepage_'.$qst]);
				$icon=cleanString($_POST['icon_'.$qst]);
				$windowclass=cleanString($_POST['windowclass_'.$qst]);
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
							FK_DesignObj_OSD=?,
							WindowClass=?
						WHERE PK_QuickStartTemplate=?',
				array($description,$package,$binary,$arguments,$homepage,$icon,$designobj,$osd,$windowclass,$qst));
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
		
		$script='
		<script>
			opener.location.reload();
			self.location="index.php?section=editQuickStartTemplates&msg='.$msg.'&error='.@$err.'";
		</script>';
		die($script);		
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
		
		$key=$row['PK_QuickStartTemplate'];
		
		$qsTemplates[$key]['category']=$row['FK_QuickStartCategory'];
		$qsTemplates[$key]['description']=$row['Description'];
		$qsTemplates[$key]['package']=$row['FK_Package'];
		$qsTemplates[$key]['binary']=$row['Binary'];
		$qsTemplates[$key]['arguments']=$row['Arguments'];
		$qsTemplates[$key]['homepage']=$row['Homepage'];
		$qsTemplates[$key]['icon']=$row['Icon'];
		$qsTemplates[$key]['designobj']=$row['FK_DesignObj'];
		$qsTemplates[$key]['osd']=$row['FK_DesignObj_OSD'];
		$qsTemplates[$key]['windowclass']=$row['WindowClass'];
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
		<tr class="tablehead">
			<td colspan="9"><B>Quick start templates</B></td>
		</tr>';
	
	// added to tree templates without category
	$nodes['root_node'][]='';
	
	$templates=array();
	foreach ($nodes['root_node'] AS $rootNode){
		$available_parents=$categories;
		unset($available_parents[$rootNode]);

		$out.='
		<tr class="alternate_back">
			<td colspan="2"><B>Category: <input type="text" name="category_'.$rootNode.'" value="'.@$nodes['description'][$rootNode].'"></B></td>
			<td colspan="4"><B>Parent category: '.pulldownFromArray($available_parents,'parent_'.$rootNode,@$nodes['parent'][$rootNode]).'</B></td>
			<td colspan="3" align="right"><B><input type="button" class="button" name="qsc_'.$rootNode.'" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start category? All templates from this category will be deleted too.\')){document.editQuickStartTemplates.deleteCategory.value=\''.$rootNode.'\';document.editQuickStartTemplates.submit();}"></B></td>
		</tr>
		<tr bgcolor="#EFFEFF">
			<td align="center"><B>Description</B></td>
			<td align="center"><B>Package</B></td>
			<td align="center"><B>Binary</B></td>
			<td align="center"><B>Arguments</B></td>
			<td align="center"><B>Homepage</B></td>
			<td align="center"><B>Icon</B></td>
			<td align="center"><B>WindowClass</B></td>
			<td align="center"><B>DesignObj</B></td>
			<td align="center"><B>OSD</B></td>
			<td align="center"><B>Action</B></td>
		</tr>		';		

		foreach ($quickStartTemplates AS $qsTemplate=>$data){
			if($data['category']==$rootNode){
				$out.='
				<tr>
					<td><input type="text" name="description_'.$qsTemplate.'" value="'.@$data['description'].'"></td>
					<td>'.pulldownFromArray($packages,'package_'.$qsTemplate,@$data['package']).'</td>
					<td><input type="text" name="binary_'.$qsTemplate.'" value="'.@$data['binary'].'"></td>
					<td><input type="text" name="arguments_'.$qsTemplate.'" value="'.@$data['arguments'].'"></td>
					<td><input type="text" name="homepage_'.$qsTemplate.'" value="'.@$data['homepage'].'"></td>
					<td><input type="text" name="icon_'.$qsTemplate.'" value="'.@$data['icon'].'"></td>
					<td><input type="text" name="windowclass_'.$qsTemplate.'" value="'.@$data['windowclass'].'"></td>
					<td>'.pulldownFromArray($designobj,'designobj_'.$qsTemplate,@$data['designobj']).'</td>
					<td>'.pulldownFromArray($designobj,'osd_'.$qsTemplate,@$data['osd']).'</td>
					<td align="right"><input type="button" class="button" name="del" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start template?\')){document.editQuickStartTemplates.deleteTemplate.value=\''.$qsTemplate.'\';document.editQuickStartTemplates.submit();}"></td>
				</tr>
				';
			}
		}
		$out.=getQuickStartChilds($nodes,$quickStartTemplates,$rootNode,$packages);

	}
	$out.='
		<input type="hidden" name="templates" value="'.@join(',',array_keys($quickStartTemplates)).'">	
		<tr>
			<td colspan="9" align="center"><input type="submit" class="button" name="update" value="Update"></td>
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
			<td><B>WindowClass</B></td>
			<td><B>DesignObj</B></td>
			<td><B>OSD</B></td>
			<td align="center"><B>Action</B></td>
		</tr>';		

		foreach ($quickStartTemplates AS $qsTemplate=>$data){
			if($data['category']==$childNode){
				$out.='
				<tr>
					<td><input type="text" name="description_'.$qsTemplate.'" value="'.@$data['description'].'"></td>
					<td>'.pulldownFromArray($packages,'package_'.$qsTemplate,@$data['package']).'</td>
					<td><input type="text" name="binary_'.$qsTemplate.'" value="'.@$data['binary'].'"></td>
					<td><input type="text" name="arguments_'.$qsTemplate.'" value="'.@$data['arguments'].'"></td>
					<td><input type="text" name="homepage_'.$qsTemplate.'" value="'.@$data['homepage'].'"></td>
					<td><input type="text" name="icon_'.$qsTemplate.'" value="'.@$data['icon'].'"></td>
					<td><input type="text" name="windowclass_'.$qsTemplate.'" value="'.@$data['windowclass'].'"></td>
					<td>'.pulldownFromArray($designobj,'designobj_'.$qsTemplate,@$data['designobj']).'</td>
					<td>'.pulldownFromArray($designobj,'osd_'.$qsTemplate,@$data['osd']).'</td>
					<td align="right"><input type="button" class="button" name="del" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start template?\')){document.editQuickStartTemplates.deleteTemplate.value=\''.$qsTemplate.'\';document.editQuickStartTemplates.submit();}"></td>
				</tr>
				';
			}
		}

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
