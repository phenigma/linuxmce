<?php 
 /*
 Site page builder for template file provided.
 
 The file should have:
 	{CssFile},{MetaTags},{Title}
 	{ScriptValidateSrc},{ScriptCalendar},{JavaScripts},{ScriptInBody}
 	{Body},{NavigationMenu},{RightMenu},{LeftMenu},{BottomMenu},{Logout}

 
 */
 
 /*Changelog:
 
 V1.0 - 20040724
 	- remove any project references
 	- rename methods and variables with nameOfMethod
 V1.0.1 - 20041005
 	- load template file without a case structure
 	
 */
 class Template
 {
  var $content;
  
  var $templateType="home";
  
  var $defaultMetaTags = '';
  var $defaultTitle='Pluto';
  var $defaultCssFile   = "include/styles/customer_setup_style.css";
  
  var $defaultScriptValidateSrc = 'scripts/formValidation.js';
  var $defaultScriptCalendar = '
    <link rel="stylesheet" type="text/css" media="all" href="scripts/jscalendar/calendar2.css" />
    <script type="text/javascript" src="scripts/jscalendar/calendar.js"></script>
    <script type="text/javascript" src="scripts/jscalendar/lang/calendar-en.js"></script>
    <script type="text/javascript" src="scripts/jscalendar/calendar-setup.js"></script>';
  
  var $defaultScriptInHead = '';
  
  var $metaTags		 = '';
  var $title     = '';
  var $cssFile   = '';
  
  
  var $scriptValidateSrc = '';
  var $scriptAnotherJS=''; //can be a list of js files,separated with commas [,]    
  var $scriptCalendar = '';
  var $scriptInBody;
  var $scriptInHead;
  
  var $navigationMenu=''; 
  
  var $loginForm   = '';
  
  var $leftMenu   = '';
  var $rightMenu  = '';
  
  var $topMenu = '';
  
  var $bottomMenu = '';
  var $HelpSrc= "/support/index.php?section=document&docID=35";
  var $body = "";
  
  var $leftFrameSrc = 'index.php?section=menu';
  var $rightFrameSrc = 'index.php?section=deviceTemplates';
  
  var $dbADO;
  function Template($myDbADO) {
  	$this->dbADO=$myDbADO;
  }
  
  
function setTemplateFileType($type) { 	
	$this->templateType = $type;
	if (file_exists(APPROOT."include/templates/template.$type.tpl.html")) {
		$this->content = implode('',file(APPROOT."include/templates/template.$type.tpl.html"));
	} else {
		if (file_exists(APPROOT."include/templates/template.home.tpl.html")) {
			$this->content = implode('',file(APPROOT."include/templates/template.home.tpl.html"));  		
		} else {
			die('Please provide at least a include/templates/template.home.tpl.html file to be used as a template!');
		}
	}
	
}		
  
  //setting functions (if 'null' [not the NULL constant, but the 'null' string] is the parameter, the script will replace with nothing;
  //if empty string '' is passed, the default value will be used
  
  function setTitle($myTitle) {
  	$this->title = $myTitle;
  }
  
  function setTopMenu($myTopMenu) {
  	$this->topMenu = $myTopMenu;
  }
  
  function setMetaTags($myMetaTags) {
  	$this->metaTags = $myMetaTags;
  }
  
  function setCssFile($myCss) {
  	$this->cssFile = $myCss;
  }

     
  function setScriptInBody($myScriptInBody) {
  		$this->scriptInBody = $myScriptInBody;
  }
  
  function setScriptInHead($myScriptInHead) {
  		$this->scriptInHead = $myScriptInHead;
  }
  
  function setScriptValidateSrc($myScriptValidateSrc) {
  	$this->scriptValidateSrc = $myScriptSrc;
  }
  
  function setScriptCalendar($myScript) {
  	$this->scriptCalendar = $myScript;
  }

  function setLeftFrameSrc($myLeftFrameSrc) {
  	$this->leftFrameSrc = $myLeftFrameSrc;
  }
  
  function setRightFrameSrc($myRightFrameSrc) {
  	$this->rightFrameSrc = $myRightFrameSrc;
  }
  
  function setScriptAnotherJS($myScript) {
  	$this->scriptAnotherJS = $myScript;
  }
  
  function setHelpSrc($myHelpSrc) {
  	$this->HelpSrc = $myHelpSrc;
  }
  
  function setNavigationMenu($new_menuNavigation) {  	 
  	if (count($new_menuNavigation)>0) {
  		if (is_array(@$new_menuNavigation[0])) {  			
  			$menuString = '
				<span>';
					 
				 	reset($new_menuNavigation[0]);
				 	$i=1;
				 	$menuString.='<a href="index.php?section=userHome" >Home</a> &gt; ';
				 	
			  	 	foreach($new_menuNavigation[0] as $key=>$value){
			  	 		$valueArray = explode(":!:",$value);
			  	 		if (trim($value[1])!='') {
			  	 			$menuString.= '<a href="'.$valueArray[1].'">'.$valueArray[0].'</a>';
			  	 		} else {
			  	 			$menuString.= '<span>'.$valueArray[0].'</span>';
			  	 		}
			  	 		if ($i<count($new_menuNavigation[0])) {
			  	 			$menuString.=" &gt; ";
			  	 		} else {
			  	 			break;
			  	 		}
			  	 		$i++;
			  	 	}				  	 	
			  	  		  	 
		 		
		        $menuString.= '</span>';		 
		        
  		} else {
  			$menuString = '
				<span>';
					 if (count($new_menuNavigation)>0) {		  	 	
					 	reset($new_menuNavigation);
					 	$i=1;
					 	$menuString.='<a href="index.php?section=userHome">Home</a> &gt; ';
				  	 	foreach($new_menuNavigation as $key=>$value){
				  	 		if (trim($value)!='') {
				  	 			$menuString.= '<a href="'.$value.'">'.$key.'</a>';
				  	 		} else {
				  	 			$menuString.= '<span>'.$key.'</span>';
				  	 		}
				  	 		if ($i<count($new_menuNavigation)) {
				  	 			$menuString.=" &gt; ";
				  	 		} else {
				  	 			break;
				  	 		}
				  	 		$i++;
				  	 	}				  	 	
				  	 } 		  	 
		 		
		        $menuString.= '</span>';		  	
  		}
  	} else {
  		$menuString='';
  	}
  	
  	  //$menuString = $menuString."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".strftime("%A %d %B %Y,%H:%M");
      $this->navigationMenu=$menuString;
  }
  
  
  function setLoginForm($myLoginForm) {
  	$this->loginForm = $myLoginForm;
  }
  
  function setLeftMenu($myLeftMenu) {
  	$this->leftMenu = $myLeftMenu;
  }
  
  function setRightMenu($myRightMenu) {
  	$this->rightMenu = $myRightMenu;
  }
  
  function setBottomMenu($myBottomMenu) {
  	$this->bottomMenu = $myBottomMenu;
  }
      

  
  function setBody($myBody) {
  	$this->body = $myBody;
  }
  
  //display functions
  function displayTitle(){
	  	if ($this->title=='null') {
	 		$this->title='';   
	  	} elseif (strlen(trim($this->title))==0) {
	  		$this->title=$this->defaultTitle;
	  	}
  		$this->content = str_replace("{Title}", $this->title, $this->content);
  }
  
  function displayMetaTags(){  	
	  	if ($this->metaTags=='null') {
	 		$this->metaTags='';   
	  	} elseif (strlen(trim($this->metaTags))==0) {
	  		$this->metaTags=$this->defaultMetaTags;
	  	}
    $this->content = str_replace("{MetaTags}", $this->metaTags, $this->content);
  }
  
  function displayCssFile(){  	  
	  	if ($this->cssFile=='null') {
	 		$this->cssFile='';   	 		
	  	} elseif (strlen(trim($this->cssFile))==0) {	  		
	  		$this->cssFile=$this->defaultCssFile;
	  	}
	  	
		if ($this->cssFile!='') {
   			$this->content = str_replace("{CssFile}", "<link rel='stylesheet' type='text/css' href='".$this->cssFile."'>", $this->content); 
		}else{
			$this->content = str_replace("{CssFile}", '', $this->content); 
		}
  }

  //------------------------------------------------------------------------------------------
  function displayScriptAnotherJS() {
  	$scriptsPaths = explode(",",$this->scriptAnotherJS);
  	$replacementText='';
  	if (count($scriptsPaths)>0) {
  		foreach ($scriptsPaths as $scriptPath) {
  			if (strlen(trim($scriptPath))>0) {
  				$replacementText.='<script src="'.$scriptPath.'" type="text/javascript" language="JavaScript"></script>'."\n";
  			}
  		}
  	}
   $this->content = str_replace("{JavaScripts}",$replacementText,$this->content);
  }
   
 
   
  function displayScriptValidateSrc(){
  	
	  	if ($this->scriptValidateSrc=='null') {
	 		$this->scriptValidateSrc='';   
	  	} elseif (strlen(trim($this->scriptValidateSrc))==0) {
	  		$this->scriptValidateSrc=$this->defaultScriptValidateSrc;
	  	}
	  	
	  	if ($this->scriptValidateSrc!='') {	
    		$this->content = str_replace("{ScriptValidateSrc}",'<script src="'.$this->scriptValidateSrc.'" type="text/javascript" language="JavaScript"></script>'."\n",$this->content);
	  	} else {
	  		$this->content = str_replace("{ScriptValidateSrc}",'',$this->content);
	  	}
  }

  function displayScriptCalendar() {
  	
	  	if ($this->scriptCalendar=='null') {
	 		$this->scriptCalendar='';   
	  	} elseif (strlen(trim($this->scriptCalendar))==0) {
	  		$this->scriptCalendar=$this->defaultScriptCalendar;
	  	}
	  	
   	$this->content = str_replace("{ScriptCalendar}", $this->scriptCalendar,$this->content);
  }
  
  function displayLeftFrameSrc(){  	
	  	if ($this->leftFrameSrc=='null') {
	 		$this->leftFrameSrc='';   
	  	} elseif (strlen(trim($this->leftFrameSrc))==0) {
	  		$this->leftFrameSrc=$this->leftFrameSrc;
	  	}
    $this->content = str_replace("{LeftFrame}", $this->leftFrameSrc, $this->content);
  }
  
  function displayRightFrameSrc(){  	
	  	if ($this->rightFrameSrc=='null') {
	 		$this->rightFrameSrc='';   
	  	} elseif (strlen(trim($this->rightFrameSrc))==0) {
	  		$this->rightFrameSrc=$this->rightFrameSrc;
	  	}
    $this->content = str_replace("{RightFrame}", $this->rightFrameSrc, $this->content);
  }
  
  
  function displayScriptInBody(){
  	//no default values ;)  		
    $this->content = str_replace("{ScriptInBody}",$this->scriptInBody,$this->content);
  }

  function displayScriptInHead() {
  	
	  	if ($this->scriptInHead=='null') {
	 		$this->scriptInHead='';   
	  	} elseif (strlen(trim($this->scriptInHead))==0) {
	  		$this->scriptInHead=$this->defaultScriptInHead;
	  	}
	  	
   	$this->content = str_replace("{ScriptInHead}", $this->scriptInHead,$this->content);
  }
  
   function displayHelpSrc(){
  	global $defaultHelpUrl;
  	if($this->HelpSrc=='')
  		$this->HelpSrc=$defaultHelpUrl;
	$this->content = str_replace("{HelpSrc}", $this->HelpSrc, $this->content);
  }
  
   //------------------------------------------------------------------------------------------  
	
  function displayMenuNavigation() {
  	if ($this->navigationMenu=='null') {
		$this->navigationMenu='';   
	}
    $this->content=str_replace('{NavigationMenu}', $this->navigationMenu,$this->content);
  }

  function displayLoginForm(){   
	if ($this->loginForm=='null') {
		$this->loginForm='';   
	}
	
    $this->content = str_replace("{LoginForm}", $this->loginForm, $this->content);
  }
  
  function displayLeftMenu(){   
	if ($this->leftMenu=='null') {
		$this->leftMenu='';   
	}
	
    $this->content = str_replace("{LeftMenu}", $this->leftMenu, $this->content);
  }
  
  function displayRightMenu(){
	if ($this->rightMenu=='null') {
		$this->rigthMenu='';   
	}
	
	$this->content = str_replace("{RightMenu}", $this->rightMenu, $this->content);
  }
  
  function displayTopMenu(){
	if (@$_SESSION['userLoggedIn']==true) {
	
	if($this->templateType!='index')
		$menuPages = getTopMenu(1,$this->dbADO);
			
	$topMenu='
	<SCRIPT LANGUAGE="JavaScript">
	function showSteps()
	{
		top.treeframe.location=\'index.php?section=wizard\';
	}
		    <!--
			'.
			$menuPages
			.'		    
		    Build();
		    --> 
	</script>';
			
	} else {
		$topMenu='';
	}
	$this->content = str_replace("{TopMenu}", $topMenu, $this->content);
  }
  
  function displayBottomMenu(){
  	if ($this->bottomMenu=='null') {
		$this->bottomMenu='';   
	} 
	
    $this->content = str_replace("{BottomMenu}", $this->bottomMenu, $this->content);
  }
  
  function displayBody(){
   $this->content = str_replace("{Body}", $this->body, $this->content);
  }

  function displayLogout() {
  	if (@$_SESSION['customerIsLogged']==true) {
  		$this->content = str_replace("{Logout}", '<br /><span><b>'.$_SESSION['customerName'].'</b></span><br /><a href="index.php?section=customerLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} elseif (@$_SESSION['adminIsLogged']==true) {
  		$this->content = str_replace("{Logout}", '<br /><span><b>'.$_SESSION['adminName'].'</b></span><br /><a href="index.php?section=adminLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} else {
  		$this->content = str_replace("{Logout}", '', $this->content);
  	}
  }
  
  
  function displayContent(){echo $this->content;}

  function output() {
  	$this->displayTitle();
   	$this->displayMetaTags();
   	$this->displayCssFile();
   
   	$this->displayScriptCalendar();
   	  
   	$this->displayScriptValidateSrc();
   	$this->displayScriptAnotherJS();
   	$this->displayScriptInHead();
   	
   	$this->displayLeftFrameSrc();
   	$this->displayRightFrameSrc();
   	
   	$this->displayScriptInBody();
   	
	$this->displayMenuNavigation();      	
	$this->displayLogout();
	$this->displayLoginForm();
   	$this->displayLeftMenu();
   	$this->displayRightMenu();
   	$this->displayTopMenu();
   	$this->displayHelpSrc();
   	$this->displayBottomMenu();
   	$this->displayBody();
   
   	$this->displayContent();
  }
  
  
 }
?>