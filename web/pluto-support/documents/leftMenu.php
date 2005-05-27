<?
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$currentSection = @cleanString($_REQUEST['rightSection']);

	$docID=(isset($_GET['docID']))?$_GET['docID']:0;
	$edit=(isset($_SESSION['editVar']))?$_SESSION['editVar']:0;
	$package=$_SESSION['package'];	
	
	$treeType = isset($_REQUEST['treeType'])?cleanString(strip_tags($_REQUEST['treeType'])):'devices';
	$jsTree='';
	$rootTitle='Document tree';
	$rootLink='right.php?section=documents/userHome&reloadTree=true';

	if($docID!=0){
		getDocumentParentsArray($docID,$dbADO);
		$docParentsArray=cleanArray($GLOBALS['parentsArray']);
		$parentID=@$docParentsArray[0];
		$docParentsArray=array_reverse($docParentsArray);
		unset($GLOBALS['parentsArray']);

		$out.= '	
				<table border=0 align="left">';
		if(count($docParentsArray)!=1)
			$out.='
					<tr>
						<td><a href="right.php?section=documents/userHome&reloadTree=true"><b>Document tree &gt;&gt;</a></b></td>
					</tr>';
		foreach($docParentsArray as $elem){
				if($elem && $elem!=0){
					$query = 'SELECT * FROM Document WHERE PK_Document=?';
					$res = $dbADO->Execute($query,$elem);
					if ($elem!=$parentID) {
						while ($row = $res->FetchRow()) {
						$linePrefix=($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?$row['PK_Document'].' - '.$row['Order'].' ':'';
						$out.='
						<tr>
							<td><a href="right.php?section=documents/userHome&docID='.$row['PK_Document'].'&reloadTree=true"><b>'.$linePrefix.$row['Title'].' &gt;&gt;</b></a></td>
						</tr>';
					}
				}else{
					while ($row = $res->FetchRow()) {
						$linePrefix=($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?$row['PK_Document'].' - '.$row['Order'].' ':'';
						$rootTitle=$linePrefix.$row['Title'];
						$rootLink='right.php?section=documents/userHome&docID='.$row['PK_Document'].'&reloadTree=true';
					}
				}
			}
		}
		$parentID=@$elem;

		// display the siblings 
		if($parentID!=0)
			$query = 'SELECT * FROM Document WHERE FK_Document_Parent=? ORDER BY `Order`';
		else
			$query = "SELECT * from Document WHERE (FK_Document_Parent IS NULL OR FK_Document_Parent=0) ORDER BY `Order`";
		$res = $dbADO->Execute($query,$parentID);
		if ($res) {
			while ($row = $res->FetchRow()) {
				$linePrefix=($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?$row['PK_Document'].' - '.$row['Order'].' ':'';
				
				$jsTree.='
					auxS'.$row['PK_Document'].' = insFld(foldersTree,gFld("'.$linePrefix.addslashes($row['Title']).'", "right.php?section=documents/documentDisplay&docID='.$row['PK_Document'].'"))
					auxS'.$row['PK_Document'].'.xID = '.$row['PK_Document'].';
					';
				$jsTree.=getChilds($row['PK_Document'],$dbADO);
			}
		}
	}else{

		$query = "SELECT * from Document WHERE (FK_Document_Parent IS NULL OR FK_Document_Parent=0) ORDER BY `Order`";
		$res = $dbADO->_Execute($query);
		if ($res) {
			while ($row = $res->FetchRow()) {
				$linePrefix=($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?$row['PK_Document'].' - '.$row['Order'].' ':'';
				$jsTree.='
						auxS'.$row['PK_Document'].' = insFld(foldersTree, gFld("'.$linePrefix.addslashes($row['Title']).'", "right.php?section=documents/documentDisplay&docID='.$row['PK_Document'].'"));
						auxS'.$row['PK_Document'].'.xID = -'.$row['PK_Document'].';
						';
				$jsTree.=getChilds($row['PK_Document'],$dbADO);
			}
		}
	}



	$scriptInHead = "
			
			<!-- As in a client-side built tree, all the tree infrastructure is put in place
			     within the HEAD block, but the actual tree rendering is trigered within the
			     BODY -->
			
			<!-- Code for browser detection -->
			<script src=\"scripts/treeview/ua.js\"></script>
			
			<!-- Infrastructure code for the tree -->
			<script src=\"scripts/treeview/ftiens4.js\"></script>
			
			<!-- Execution of the code that actually builds the specific tree.
			     The variable foldersTree creates its structure with calls to
				 gFld, insFld, and insDoc -->
			<script>
	
			USETEXTLINKS = 1
			STARTALLOPEN = 1
			USEFRAMES = 1
			USEICONS = 1
			WRAPTEXT = 1
			PRESERVESTATE = 1
			ICONPATH = 'scripts/treeview/' 
			HIGHLIGHT = 1
			GLOBALTARGET = 'R'
			foldersTree = gFld('<b>".addslashes($rootTitle)."</b>', '".$rootLink."');
			foldersTree.xID = 1001635872

			$jsTree
			".(($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?"
								auxS = insFld(foldersTree, gFld(\"Add document\", \"right.php?section=documents/addDocument&parentID=0\"));
								auxS.xID = 0;
								auxS.iconSrc = ICONPATH + \"diffFolder.gif\"
			  					auxS.iconSrcClosed = ICONPATH + \"diffFolder.gif\" 
			":'')."
	
			foldersTree.treeID = 't2';";
			if($docID!=0)
				$scriptInHead.="highlightObjLink(auxS".$docID.");";
			$scriptInHead.="
			function getObj(obj) {
					if (document.layers) {
						if (typeof obj == 'string') {
							return document.layers[obj];
						} else 	{
							return obj;
						}
					}
					if (document.all) {
						if (typeof obj == 'string') {
							return document.all(obj);
						} else {
							return obj;
						}
					}
					if (getObj) {
						if (typeof obj == 'string') {
							return getObj(obj);
						} else {
							return obj;
						}
					}
					return null;	
				} 
			
				function showHideObject(obj) {
					obj = getObj(obj);					
					if(document.all || getObj) {  			
							if (obj.style.visibility == \"visible\") {
								obj.style.visibility = \"hidden\";
								obj.style.display = \"none\";
							} else {
							    obj.style.visibility = \"visible\";
								obj.style.display = \"block\";
							}
					} else if (document.layers) {
							if (obj.style.visibility == \"visible\") {
								obj.visibility = \"hide\";	
							} else {
							obj.visibility = \"show\";	
							}
					}
				} 
			
			</script>
			";

	$out.= '	<tr valign="top">
					<td valign="top" nowrap>
						<span style="font-color:#FFFFFF">
								<table border=0 valign="top">
									<tr style="display:none">
										<td>
											<font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font>
										</td>
									</tr>
								</table>
						</span>
								<span>
								<script>
									initializeDocument();						
								</script>
								<noscript>
								A tree for site navigation will open here if you enable JavaScript in your browser.
								</noscript>
						'.(($GLOBALS['disableDocumentsEdit']!=1)?'<input type="button" class="button" name="setEdit" value="'.(($_SESSION['editVar']!=1)?'Activate':'Deactivate').' edit mode" onClick="top.location=\'index.php?edit='.(($_SESSION['editVar']!=1)?'1':'0').'\'">':'').'
					</td>
				</tr>
				<tr>
					<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
				</tr>
			';



	$output->setScriptInHead($scriptInHead);


					
		

	$out.='</table>';
	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  



function getChilds($parentID,$dbADO) {
	$queryGP = "SELECT * FROM Document WHERE FK_Document_Parent = $parentID ORDER BY `Order`";
	$resGP = $dbADO->Execute($queryGP);
	$jsTree='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$linePrefix=($_SESSION['editVar']!='0' && $GLOBALS['disableDocumentsEdit']!=1 )?$row['PK_Document'].' - '.$row['Order'].' ':'';
				$jsTree.= '
					auxS'.$row['PK_Document'].' = insFld(auxS'.$parentID.', gFld("'.$linePrefix.addslashes($row['Title']).'", "right.php?section=documents/documentDisplay&docID='.$row['PK_Document'].'"))
					auxS'.$row['PK_Document'].'.xID = '.$row['PK_Document'].';
				';
				$jsTree.=getChilds($row['PK_Document'],$dbADO);
		}
	}
	return $jsTree;
}
?>