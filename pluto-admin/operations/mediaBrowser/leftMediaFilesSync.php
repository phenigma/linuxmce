<?
function leftMediaFilesSync($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$GLOBALS['physicalPathsArray']=array();
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';	
	
	if($action=='form'){
		$out='
		<form action="index.php" method="POST" name="leftMediaFilesSync">
		<input type="hidden" name="section" value="leftMediaFilesSync">
		<input type="hidden" name="action" value="search">
		
		<table border=0 align="left">				
				<tr>
					<td colspan="2">
						<img src="include/images/logo_pluto.jpg">
					</td>
				</tr>';
		
		if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login&action=logout" target="basefrm" >Logout</a></td>					
					</tr>
			';
		} else {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login"  target="basefrm">Login</a></td>
					</tr>
			';
		}
		
		$physicalNodesNames=array();
		$nodesNames=array();
		$nodesIDs=array();
		$nodesParentIDs=array();
		$nodesPictures=array();
		$nodeCount=0;

		// get physical paths in $GLOBALS['physicalPathsArray']		
		$startPath='/home/public';
		grabDirectory($startPath, 0);
		foreach($GLOBALS['physicalPathsArray'] AS $physicalPath){
			$directoriesArray=explode('/',substr($physicalPath,1));
			for($level=0;$level<count($directoriesArray);$level++){
				if(!isset($nodesNames[$level]) || !in_array($directoriesArray[$level],$nodesNames[$level])){
					$nodeCount++;
					$nodesNames[$level][]=$directoriesArray[$level];
					$physicalNodesNames[$level][]=$directoriesArray[$level];
					$nodesIDs[$level][]=$nodeCount;
					$parentID=getParentID($nodesNames,$nodesIDs,($level-1),@$directoriesArray[$level-1]);
					$nodesParentIDs[$level][]=($parentID!='')?$parentID:0;
				}
			}
		}

		$queryDistinctPath="SELECT DISTINCT Path FROM File ORDER BY Path ASC";
		$rs=$mediadbADO->Execute($queryDistinctPath);
		$dbPathsArray=array();
		while($rowDistinctPath=$rs->FetchRow()){
			$dbPathsArray[]=$rowDistinctPath['Path'];
			$directoriesArray=explode('/',substr($rowDistinctPath['Path'],1));
			for($level=0;$level<count($directoriesArray);$level++){
				if(!isset($nodesNames[$level]) || !in_array($directoriesArray[$level],$nodesNames[$level])){
					$nodeCount++;
					$nodesNames[$level][]=$directoriesArray[$level];
					$nodesIDs[$level][]=$nodeCount;
					$nodesPictures[$level][]='<img src=include/images/db.gif align=middle border=0>';
					$parentID=getParentID($nodesNames,$nodesIDs,($level-1),@$directoriesArray[$level-1]);
					$nodesParentIDs[$level][]=($parentID!='')?$parentID:0;
				}else{
					$key=array_search($directoriesArray[$level],$physicalNodesNames[$level]);
					if($key!==FALSE)
						$nodesPictures[$level][$key]='<img src=include/images/sync.gif align=middle border=0>';
				}
			}
		}

		// build the db treeview 
		$jsTree='';
		
		for($level=0;$level<count($nodesNames);$level++){
			foreach($nodesNames[$level] as $key=> $directory){
				$directoryPath=getFullPath($directory,$level,$nodesNames,$nodesIDs,$nodesParentIDs);
				$prefixImage=(isset($nodesPictures[$level][$key]))?$nodesPictures[$level][$key]:"<img src=include/images/disk.gif align=middle border=0>";
				$jsTree.='				    
					auxS'.$nodesIDs[$level][$key].' = insFld('.(($nodesParentIDs[$level][$key]!=0)?'auxS'.$nodesParentIDs[$level][$key]:'foldersTree').', gFld("'.$directory.$prefixImage.'", "index.php?section=mainMediaFilesSync&path='.urlencode($directoryPath).'"));
					auxS'.$nodesIDs[$level][$key].'.xID = '.$nodesIDs[$level][$key].';
					';
			}
		}

		$out.='
			<tr>
				<td colspan="2">
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
				</td>
			</tr>
			<tr>
				<td colspan="2"><B>LEGEND FOR DIRECTORIES</B><br>
				<img src=include/images/disk.gif align=middle border=0>	Exist only on disk<br>
				<img src=include/images/db.gif align=middle border=0> Exist only in database<br>
				<img src=include/images/sync.gif align=middle border=0>	Exist both on disk and in database
			</tr>
			</table>
		</form>';
	}else{
		// processing area
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
			STARTALLOPEN = 0
			USEFRAMES = 1
			USEICONS = 1
			WRAPTEXT = 1
			PRESERVESTATE = 0
			ICONPATH = 'scripts/treeview/' 
			HIGHLIGHT = 1
			GLOBALTARGET = 'R'
			
			foldersTree = gFld('<b>Root</b>', \"\");
			foldersTree.xID = 1001635872
			$jsTree
			
			
			foldersTree.treeID = 't2'
			
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

	$output->setScriptInHead($scriptInHead);
	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getParentID($nodesNames,$nodesIDs,$level,$parentName)
{
	if($parentName=='' || $level==-1)
		return 0;
	$key=array_search($parentName,$nodesNames[$level]);
	return $nodesIDs[$level][$key];
}


function getNodeName($nodesID,$level,$nodesNames,$nodesIDs,$nodesParentIDs)
{
	$key=array_search($nodesID,$nodesIDs[$level]);
	return $nodesNames[$level][$key];
}

function getFullPath($currentDir,$currentLevel,$nodesNames,$nodesIDs,$nodesParentIDs)
{
	if($currentLevel>0){
		$key=array_search($currentDir,$nodesNames[$currentLevel]);
		$parentID=$nodesParentIDs[$currentLevel][$key];
		$parentName=getNodeName($parentID,$currentLevel-1,$nodesNames,$nodesIDs,$nodesParentIDs);
	
		if($parentID!=0){
			$fullPath=getFullPath($parentName,$currentLevel-1,$nodesNames,$nodesIDs,$nodesParentIDs).'/'.$currentDir;
		}
	}else
		$fullPath='/'.getNodeName(0,0,$nodesNames,$nodesIDs,$nodesParentIDs).@$fullPath;
	return $fullPath;
}
?>