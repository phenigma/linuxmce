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
  var $defaultCssFile   = "include/styles/plutoSupport.css";
  
  var $defaultScriptValidateSrc = 'scripts/formValidation.js';
  var $defaultScriptCalendar = '';
  
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
  
  var $body = "";

  var $ProjectName= "";
  var $HelpSrc= "";
  
  var $leftFrameSrc = 'index.php?section=leftMenu';
  var $rightFrameSrc = 'index.php?section=userHome';
  
  var $dbADO;
  function Template($myDbADO) {
  	$this->dbADO=$myDbADO;
  }
  
  
function setTemplateFileType($type) { 	
	$this->templateType = $type;
	if (file_exists("include/templates/template.$type.tpl.html")) {
		$this->content = implode('',file("include/templates/template.$type.tpl.html"));
	} else {
		if (file_exists("include/templates/template.home.tpl.html")) {
			$this->content = implode('',file("include/templates/template.home.tpl.html"));  		
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

  function setProjectName($myProjectName) {
  	$this->ProjectName = $myProjectName;
  }

  function setHelpSrc($myHelpSrc) {
  	$this->HelpSrc = $myHelpSrc;
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
	$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	if($package!=0){
		$menuPages = getTopMenu(2,$this->dbADO);
	}
	else {
		$menuPages = getTopMenu(3,$this->dbADO);
	}			
	$topMenu='
	<SCRIPT LANGUAGE="JavaScript">
		    <!--
			'.
			$menuPages
			.'		    
		    Build();
		    --> 
	</script>';
			
	$this->content = str_replace("{TopMenu}", $topMenu, $this->content);
  }
  
  function displayBottomMenu(){
  	if ($this->bottomMenu=='null') {
		$this->bottomMenu='';   
	} 
	
    $this->content = str_replace("{BottomMenu}", $this->bottomMenu, $this->content);
  }
  
  function displayBody(){
  	global $PlutoHomeHost;
   $this->body = str_replace("**package**", @$_SESSION['packageName'], $this->body);
   $this->content = str_replace("{PlutoHomeHost}", $PlutoHomeHost, $this->content);

   $this->content = str_replace("{Body}", $this->body, $this->content);
 }

  function displayProjectName(){
   $this->content = str_replace("{ProjectName}", $this->ProjectName, $this->content);
  }

  function displayHelpSrc(){
  	global $defaultHelpUrl;
  	if($this->HelpSrc=='')
  		$this->HelpSrc=$defaultHelpUrl;
	$this->content = str_replace("{HelpSrc}", $this->HelpSrc, $this->content);
  }

  
  function displayLogout() {
  	global $PlutoHomeHost;
  	$login='<table width="239"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td width="73"><a href="'.$PlutoHomeHost.'index.php"><img src="include/images/buton_home.jpg" width="73" height="36" border="0"/></a></td>
                <td width="31"><img src="include/images/header_colt_1.jpg" width="31" height="36" /></td>
                <td width="135"><a href="'.$PlutoHomeHost.'index.php?section=clientLoginForm" target="_top"><img src="include/images/buton_login_register.jpg" width="135" height="36" border="0"/></a></td>
              </tr>
          </table>';
  	$logout='<table width="214" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="115"><a href="'.$PlutoHomeHost.'index.php?section=myPluto" target="_top"><img src="include/images/buton_my_pluto.jpg" width="115" height="36" border="0"/></a></td>
    <td width="99"><a href="'.$PlutoHomeHost.'index.php?section=clientLoggedOut"><img src="include/images/buton_logout.jpg" width="99" height="36" border="0"/></a></td>
  </tr>
</table>';
  	if (@$_SESSION['userIsLogged']!='yes') {
  		$this->content = str_replace("{LoginLogout}", $login, $this->content);
  	} else {
  		$this->content = str_replace("{LoginLogout}", $logout, $this->content);
  	}
  }
  
  
  function displayContent(){echo $this->content;}

  function output() {
  	$this->setProjectName(@$_SESSION['packageName']);
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
   	$this->displayBottomMenu();
   	$this->displayProjectName();
   	$this->displayHelpSrc();
   	$this->displayBody();
   	$this->displayContent();
  }
  
  
 }
?>