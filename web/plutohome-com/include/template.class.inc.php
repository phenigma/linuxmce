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
  var $defaultTitle='Plutohome.com';
  var $defaultCssFile   = "style/style.css";
  var $defaultScriptTRColor = 'javascripts/changeTRColor.js';  
  var $defaultScriptValidateSrc = 'javascripts/formValidation.js';
  var $defaultScriptCalendar = '';
  
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

  var $conn;
  // default buttons
  var $b_what_is_pluto='b_what_is_pluto_off.gif';
  var $b_why_you_need_pluto='b_why_you_need_pluto_off.gif';
  var $b_how_pluto_works='b_how_pluto_works_off.gif';
  var $b_support='b_support_off.gif';
  var $b_get_pluto='b_get_pluto_off.gif';
  
function setConn($myConn){
	$this->conn=$myConn;
}  
  
function setTemplateFileType($type) {
  	
	$this->templateType = $type;
	
		switch ($type) {
			case "home":
				$this->content = implode('',file("include/templates/template.home.tpl.html"));  		
			break;
			case "nonav":
			    $this->content = implode('',file("include/templates/template.nonav.tpl.html")); 
			break;	
			default:
				$this->templateType = 'public';
				$this->content = implode('',file("include/templates/template.home.tpl.html")); 
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
		<span>';
			 if (count($new_menuNavigation)>0) {		  	 	
			 	reset($new_menuNavigation);
			 	$i=1;
			 	$menuString.='<a href="index.php">Home</a> &gt; ';
		  	 	foreach($new_menuNavigation as $key=>$value){
		  	 		if (trim($value)!='' && $i!=count($new_menuNavigation)) {
		  	 			$menuString.= '<a  href="'.$value.'">'.$key.'</a>';
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
  function setPressedButton($pressedButton) {
  	switch($pressedButton){
  		case 1:
  			$this->b_what_is_pluto='b_what_is_pluto_on.gif';
  		break;
  		case 2:
  			$this->b_why_you_need_pluto='b_why_you_need_pluto_on.gif';
  		break;
  		case 3:
  			$this->b_how_pluto_works='b_how_pluto_works_on.gif';
  		break;
  		case 4:
  			$this->b_support='b_support_on.gif';
  		break;
  		case 5:
  			$this->b_get_pluto='b_get_pluto_on.gif';
  		break;
  	}
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
  	$newsText='
<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">';
   $sql="select * from News  where markedDeleted!=1 order by newsDate desc limit 0,2";
   $r=mysql_query($sql,$this->conn) or die("Can not grab from database" .mysql_error());
   while($row=mysql_fetch_object($r))
  {
     $text=$row->newsContent;
     $str=substr($text,0,699);
     $take=substr($text,699,700);
     $t=explode(" ",$take);
     $str=$str.$t[0];
     $date=substr($row->newsDate,0,10);
     $title=strip_tags($row->newsTitle);
     $newsText.='
     	<tr>
     		<td class="title2"><b>'.$date[5].$date[6].'.'.$date[8].$date[9].'.'.$date[0].$date[1].$date[2].$date[3].'<br>'.$title.'<b></td>
     	</tr>
     	<tr>
     		<td class="news">'.$str.'...<br><a href="index.php?section=full_story&id='.$row->newsID.'" class="newstitle">[ ... ]</a></td>
     	</tr>
		<tr>
     		<td class="news">&nbsp;</td>
     	</tr>     ';
  }

    $newsText.='
 </table>';
  	$this->content = str_replace("{News}", $newsText, $this->content);
  	
  	$this->content = str_replace("{b_what_is_pluto}", $this->b_what_is_pluto, $this->content);
  	$this->content = str_replace("{b_why_you_need_pluto}", $this->b_why_you_need_pluto, $this->content);
  	$this->content = str_replace("{b_how_pluto_works}", $this->b_how_pluto_works, $this->content);
  	$this->content = str_replace("{b_support}", $this->b_support, $this->content);
  	$this->content = str_replace("{b_get_pluto}", $this->b_get_pluto, $this->content);
  	
   	$this->content = str_replace("{Body}", $this->body, $this->content);
  }

  function displayLogout() {
  	 $login='<a href="index.php?section=login" class="login">Login</a> | <a href="index.php?section=register" class="login">Register</a>';
  	$logout='<a href="index.php?section=logout" class="login">Logout</a> | <a href="index.php?section=myPluto" class="login">MyPluto</a>';
  	if (@$_SESSION['userIsLogged']!='yes') {
  		$this->content = str_replace("{LoginLogoutButton}", $login, $this->content);
  	} else {
  		$this->content = str_replace("{LoginLogoutButton}", $logout, $this->content);
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
   	$this->displayBody();
   
   	$this->displayContent();
  }
  
  
 }
?>
