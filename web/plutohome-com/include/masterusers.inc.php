<?
// all functions required to connect with MasterUsers Database

function queryExternalServer($params,$url)
{
	$defined_vars = get_defined_vars();
	$user_agent = @$defined_vars['HTTP_USER_AGENT'];

	$ch = curl_init();
	curl_setopt($ch, CURLOPT_POST,1);
	curl_setopt($ch, CURLOPT_POSTFIELDS,$params);
	curl_setopt($ch, CURLOPT_URL,$url);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST,  false);
	curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
	$result=curl_exec ($ch);
	curl_close ($ch);
	return $result;
}


function addtoMasterUsers($typeUser,$emailUser,$username,$referrer,$password,$url)
{
	$params = "typeUser=".$typeUser."&email=".$emailUser."&username=".$username ."&password=".$password."&referrer=".$referrer;
	$result=queryExternalServer($params,$url);
	if(ereg("MasterUsersID",$result)){
		return array(true,$result);	// return true and PK_MasterUsers
	}
	return array(false,$result);	// return false and the error message
}

function updateMasterUsersPassword($PK_MasterUsers,$NewPassword,$url,$oldPassword='')
{
	$params = "PK_MasterUsers=".$PK_MasterUsers."&password=".$NewPassword."&oldPassword=".$oldPassword;
	$result=queryExternalServer($params,$url);
	if($result=="Password changed"){
		return array(true,"");
	}
	return array(false,$result);
}

// function will connect to MasterUsers database and will return PK_MasterUsers and the value of the fields specified in
// $FieldsNames like: &email=&FK_MasterUsers_Referrer= to be processed with parse_str function
function checkMasterUsers($username, $password,$url,$FieldsNames='')
{
	$params = "username=".$username."&password=".$password.$FieldsNames;
	$MasterUsersResponse=queryExternalServer($params,$url);
	if(ereg("MasterUsersID",$MasterUsersResponse)){
		return array(true,$MasterUsersResponse);	// return true and PK_MasterUsers
	}
	return array(false,$MasterUsersResponse);	// return false and the error message
}

// $changedFields MUST be send URL-like: Email=emai@site.com&username=newname
// accepted fields: Email, username so far, but others could be added later
function updateMasterUsers($PK_MasterUsers,$changedFields,$url)
{
	$params = "MasterUsersID=".$PK_MasterUsers.$changedFields;
	$MasterUsersResponse=queryExternalServer($params,$url);

	if($MasterUsersResponse=='The Master User data was updated.'){
		return true;	// return true
	}
	return false;	// return false and the error message

}

function addNewReferrer($FK_MasterUsers,$typeReferral,$ContactName,$BusinessName,$Email,$ReferrerNotes,$url)
{
	$params="FK_MasterUsers=$FK_MasterUsers&typeReferral=$typeReferral&ContactName=$ContactName&BusinessName=$BusinessName&Email=$Email&ReferrerNotes=".urlencode($ReferrerNotes);
	$result=queryExternalServer($params,$url);

	if($result=="Succes")
		return '';
	else
		return $result;
}

function getReferrerSummary($FK_MasterUsers,$url)
{
	$params="FK_MasterUsers=$FK_MasterUsers";
	$result=queryExternalServer($params,$url);

	return $result;
}

function phpBBLogin($username, $password, $url)
{
	$params = "username=".$username."&password=".$password."&login=&autologin=1&external=1";
	$result=queryExternalServer($params,$url);

	return $result;
	//return false;	
}
?>