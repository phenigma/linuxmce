<?php

  function formValidation($simpleForm, $key, $validationString) {
  	$validationString = explode("||" , $validationString , 2);
  	$validationString[0] = explode("=", $validationString[0], 2);
  	
  	$message = $validationString[1];
  	$function = 'valid'.$validationString[0][0];
  	$parameter = $validationString[0][1];
  	
  	if (strlen(trim($parameter)) == 0)
  		$error = $function($simpleForm, $message, $key);
  	else
  		$error = $function($simpleForm, $message, $key, $parameter);
  	return $error;
  }
  
  
  /* integerNotZero || $message */
  function validIntegerNotZero($simpleForm, $message, $key) {
  	$defaultMessage = "Invalide input !";
  	$hasPhpErrors = 0;	
	if (is_numeric($_POST[$key])) {								
		if ((int)$_POST[$key]==0) {		    							    						    					
			if (strlen(trim($message))>1) {
	   			$validationPhpMessage=trim($message);
	    	}else{
	    		$validationPhpMessage=$simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;;
	    	}
	    	$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    	$_SESSION[$simpleForm->formName]['hasErrors']=1;
	    	$hasPhpErrors=1;
	    }
	}
	return $hasPhpErrors;
  }
 
  
  /* integer || $message */
  function validInteger($simpleForm, $message, $key) {
  	$defaultMessage = "Value must be an integer !";
  	$hasPhpErrors = 0;
  	if ( trim($_POST[$key]) != trim(intval($_POST[$key])) ) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;
  }
  
  
  /* req || $message */
  function validReq($simpleForm, $message, $key) {
  	$defaultMessage = "Must not be empty !".$_POST[$key];
   	$hasPhpErrors = 0;
  	if ( strlen(trim($_POST[$key])) == 0) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;
  }
  
  
  /* float || $message */
  function validFloat($simpleForm, $message, $key) {
  	$defaultMessage = "Must be a float value !";
  	$hasPhpErrors = 0;
  	if (trim($_POST[$key]) != trim(floatval($_POST[$key]))) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;
  }
  
  
  /* dateFormatDD || $message */
  function validDateFormatDD($simpleForm, $message, $key) {
  	$defaultMessage = "Invalide date format !";
  	$hasPhpErrors = 0;
  	// '%d/%m/%Y'
  	$tempDate = $_POST[$key];
  	$tempDate = explode("/",$tempDate);
  	
 	if (count($tempDate) != 3)
 		$hasPhpErrors = 1;
 		
  	if ($tempDate[0] < 1 || $tempDate[0] > 31)
  		$hasPhpErrors = 1;
  	
  	if ($tempDate[1] <1 || $tempDate[1] > 12)
  		$hasPhpErrors = 1;
  		
  	if (strlen(trim($tempDate[2])) == 0)
  		$hasPhpErrors = 1;
  	
  	if ($hasPhpErrors == 1) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
  	}
  	return $hasPhpErrors;
  }
  
  
  /* maxLength=$parameter || $message */
  function validMaxlength($simpleForm, $message, $key, $parameter) {
  	$defaultMessage = "String must have less than ".($parameter)." chars !";
  	$hasPhpErrors = 0;
  	if ( strlen($_POST[$key]) > $parameter) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;  	
  }
  
  
  /* minLength=$parameter || $message */
  function validMinlength($simpleForm, $message, $key, $parameter) {
  	$defaultMessage = "String must be at least ".($parameter)." chars long !";
  	$hasPhpErrors = 0;
  	if ( strlen($_POST[$key]) < $parameter) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;  	
  }

  
  /* minVal=$parameter || $message */
  function validMinVal($simpleForm, $message, $key, $parameter) {
  	$defaultMessage = "Value must be > ".($parameter - 1)." !";
  	$hasPhpErrors = 0;
  	if ( $_POST[$key] < $parameter ) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;  	
  }
  
  
  /* maxVal=$parameter || $message */
  function validMaxVal($simpleForm, $message, $key, $parameter) {
  	$defaultMessage = "Value must be < ".($parameter + 1)." !";
  	$hasPhpErrors = 0;
  	if ( $_POST[$key] > $parameter ) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
	    $hasPhpErrors=1;
  	}
  	return $hasPhpErrors;  	
  }

  
  /* email || $message */
  function validEmail($simpleForm, $message, $key) {
  	$defaultMessage = "Invalid email !";
  	$hasPhpErrors = 0;
  	// user@host
  	$tempEmail = $_POST[$key];
  	$tempEmail = explode("@",$tempEmail);
  	
 	if (count($tempEmail) != 2)
 		$hasPhpErrors = 1;
 				
  	if (strlen(trim($tempEmail[0])) == 0)
  		$hasPhpErrors = 1;
  	
  	if (strlen(trim($tempEmail[1])) == 0)
  		$hasPhpErrors = 1;

  	if ($hasPhpErrors == 1) {
  		if(strlen(trim($message)) > 1) {
  			$validationPhpMessage = trim($message);
  		} else {
  			$validationPhpMessage = $simpleForm->formFieldDefinition[$key][1]." : ".$defaultMessage;
  		}
  		$_SESSION[$simpleForm->formName][$key]['message']=$validationPhpMessage;
	    $_SESSION[$simpleForm->formName]['hasErrors']=1;
  	}
  	return $hasPhpErrors;
  }
?>