<?php 
 /*
 Site page builder for template file provided.
 
 The file should have:
 	{CssFile},{MetaTags},{Title}
 	{ScriptValidateSrc},{ScriptCalendar},{ScriptTRColor},{JavaScripts},{ScriptInBody}
 	{Body},{NavigationMenu},{RightMenu},{LeftMenu},{BottomMenu},{Logout}

 
 */
 /*Changelog:
 
 V1.0 - 20040724
 	- remove any project references
 	- rename methods and variables with nameOfMethod
 */
 class Template
 {
  var $content;
  
  var $templateType="home";
  
  var $defaultMetaTags = '';
  var $defaultTitle='Plutovip.com';
  var $defaultCssFile   = "style/style.css";
  var $defaultScriptTRColor = 'javascripts/changeTRColor.js';  
  var $defaultScriptValidateSrc = 'javascripts/formValidation.js';
  var $defaultScriptCalendar = '
    <link rel="stylesheet" type="text/css" media="all" href="javascripts/jscalendar/calendar2.css" />
    <script type="text/javascript" src="javascripts/jscalendar/calendar.js"></script>
    <script type="text/javascript" src="javascripts/jscalendar/lang/calendar-en.js"></script>
    <script type="text/javascript" src="javascripts/jscalendar/calendar-setup.js"></script>';
  
  var $defaultScriptInHead = '';
  
  var $metaTags		 = '';
  var $title     = '';
  var $cssFile   = '';
  
  var $scriptTRColor = '';    
  var $scriptValidateSrc = '';
  var $scriptAnotherJS=''; //can be a list of js files,separated with commas [,]    
  var $scriptCalendar = '';
  var $scriptInBody;
  var $scriptInHead;
  
  var $navigationMenu=''; 
  
  var $loginForm   = '';
  
  var $leftMenu   = '';
  var $rightMenu  = '';
  
  var $bottomMenu = '';
  
  var $body = "";
  
  var $imgName = '';
  var $pageID = '';

  var $settingsIMG = 'images/home%20page%20img/a_my_settings.gif';
  var $placesIMG = 'images/home%20page%20img/a_places.gif';
  var $contactIMG = 'images/home%20page%20img/a_contact_vip.gif';
  var $referIMG = 'images/home%20page%20img/a_refer_business.gif';
  var $instructionsIMG = 'images/home%20page%20img/a_instructions.gif';
  var $settingIMG = 'images/home%20page%20img/a_settings.gif';
  var $visitorsIMG = 'images/home%20page%20img/a_visitors_off.gif';
function setTemplateFileType($type) {
  	
	$this->templateType = $type;
	
		switch ($type) {
			case "home":
            session_start();
            if($_SESSION['userIsLogged']!="yes")
				$this->content = implode('',file(APPROOT."include/templates/template.home.tpl.html")); 
            else $this->content = implode('',file(APPROOT."include/templates/template.user1.tpl.php"));
            break;
			case "user":
			   $this->content = implode('',file(APPROOT."include/templates/template.user.tpl.php")); 
				break;	
         case "business":
			   $this->content = implode('',file(APPROOT."include/templates/template.user_b.tpl.php")); 
				break;	
        case "business1":
			   $this->content = implode('',file(APPROOT."include/templates/template.user1_b.tpl.php")); 
				break;	
			case "admin":
				$this->content = implode('',file(APPROOT."include/templates/template.admin.tpl.html")); 
				break;
			default:
				$this->templateType = 'public';
				$this->content = implode('',file(APPROOT."include/templates/template.public.tpl.html")); 
		}
}
  
  //setting functions (if 'null' [not the NULL constant, but the 'null' string] is the parameter, the script will replace with nothing;
  //if empty string '' is passed, the default value will be used
  
  
  function setImgName($myImgName) {
  	$this->imgName = $myImgName;
  }

  function setPageID($myPageID) {
  	$this->pageID = $myPageID;
  }

  function setTitle($myTitle) {
  	$this->title = $myTitle;
  }
  
  function setMetaTags($myMetaTags) {
  	$this->metaTags = $myMetaTags;
  }
  
  function setCssFile($myCss) {
  	$this->cssFile = $myCss;
  }

  function setScriptTRColor($myScriptTRColor) {
  		$this->scriptTRColor = $myScriptTRColor;
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
  
  function setScriptAnotherJS($myScript) {
  	$this->scriptAnotherJS = $myScript;
  }
  
  function setNavigationMenu($new_menuNavigation) {  	 
  	if (count($new_menuNavigation)>0) { 	
  	$menuString = '
		<span class="style10">';
			 if (count($new_menuNavigation)>0) {		  	 	
			 	reset($new_menuNavigation);
			 	$i=1;
			 	$menuString.='<a class="text_menu3" href="index.php">Home</a> &gt; ';
		  	 	foreach($new_menuNavigation as $key=>$value){
		  	 		if (trim($value)!='') {
		  	 			$menuString.= '<a  class="text_menu3" href="'.$value.'">'.$key.'</a>';
		  	 		} else {
		  	 			$menuString.= '<span class="navMenu">'.$key.'</span>';
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
  	} else {
  		$menuString='';
  	}
  	  $menuString = $menuString."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".strftime("%A %d %B %Y,%H:%M");
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
  function displayImgName(){
	  	if ($this->imgName=='null') {
	 		$this->imgName='img.jpg';   
	  	} elseif (strlen(trim($this->imgName))==0) {
	  		$this->imgName='img.jpg';
	  	}
  		$this->content = str_replace("{ImgName}", $this->imgName, $this->content);
  }
  
  function displayPageID(){

	  	if ($this->pageID=='null') {
	 		$this->pageID=0;   
	  	} elseif (strlen(trim($this->pageID))==0) {
	  		$this->pageID=0;
	  	}
  		$this->content = str_replace("{PageID}", $this->pageID, $this->content);
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
  
  
  function displayScriptTRColor() {  	
  	
	  	if ($this->scriptTRColor=='null') {
	 		$this->scriptTRColor='';   
	  	} elseif (strlen(trim($this->scriptTRColor))==0) {
	  		$this->scriptTRColor=$this->defaultScriptTRColor;
	  	}
	  	
		if ($this->scriptTRColor!='') {
	   		$this->content = str_replace("{ScriptTRColor}",'<script src="'.$this->scriptTRColor.'" type="text/javascript" language="JavaScript"></script>'."\n",$this->content);
		} else {
			$this->content = str_replace("{ScriptTRColor}",'',$this->content);
		}
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
 
   function displaySettingsIMG(){
		$this->content = str_replace("{settingsIMG}", $this->settingsIMG, $this->content);
  }
  function displaySettingIMG(){
		$this->content = str_replace("{settingIMG}", $this->settingIMG, $this->content);
  }
  function displayVisitorsIMG(){
		$this->content = str_replace("{visitorsIMG}", $this->visitorsIMG, $this->content);
  }
   function displayPlacesIMG(){
		$this->content = str_replace("{placesIMG}", $this->placesIMG, $this->content);
  }
   function displayContactIMG(){
		$this->content = str_replace("{contactIMG}", $this->contactIMG, $this->content);
  }
  function displayReferIMG(){
		$this->content = str_replace("{referIMG}", $this->referIMG, $this->content);
  }
   function displayInstructionsIMG(){
		$this->content = str_replace("{instructionsIMG}", $this->instructionsIMG, $this->content);
  }
  function displayRightMenu(){
	if ($this->rightMenu=='null') {
		$this->rigthMenu='';   
	}
	
	$this->content = str_replace("{RightMenu}", $this->rightMenu, $this->content);
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
  		$this->content = str_replace("{Logout}", '<br /><span class="text"><b>'.$_SESSION['customerName'].'</b></span><br /><a class="text_menu_left" href="index.php?section=customerLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} elseif (@$_SESSION['adminIsLogged']==true) {
  		$this->content = str_replace("{Logout}", '<br /><span class="text"><b>'.$_SESSION['adminName'].'</b></span><br /><a class="text_menu_left" href="index.php?section=adminLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} else {
  		$this->content = str_replace("{Logout}", '', $this->content);
  	}
  }
  
  
  function displayContent(){echo $this->content;}

  function output() {
  	$this->displayTitle();
  	$this->displayImgName();
    $this->displayPageID();
   	$this->displayMetaTags();
   	$this->displayCssFile();
   
   	$this->displayScriptCalendar();
   	$this->displayScriptTRColor();
   	$this->displayScriptValidateSrc();
   	$this->displayScriptAnotherJS();
   	$this->displayScriptInHead();
   	
   	$this->displayScriptInBody();
   	
	$this->displayMenuNavigation();      	
	$this->displayLogout();
	$this->displayLoginForm();
   	$this->displayLeftMenu();
   	$this->displayRightMenu();
   	$this->displayBottomMenu();

      $this->displaySettingsIMG('images/home%20page%20img/a_my_settings_on.gif');
      $this->displaySettingIMG('images/home%20page%20img/a_settings_on.gif');
      $this->displayVisitorsIMG('images/home%20page%20img/a_visitors_on.gif');
      $this->displayPlacesIMG('images/home%20page%20img/a_places_on.gif');
      $this->displayContactIMG('images/home%20page%20img/a_contact_vip_on.gif');
      $this->displayReferIMG('images/home%20page%20img/a_contact_vip_on.gif');
      $this->displayInstructionsIMG('images/home%20page%20img/a_instructions_on.gif');
   	$this->displayBody();
   
   	$this->displayContent();
  }
  
  
 }
?>