<?
function add_referrer($conn){
	// add to MasterUsers table 
	if(isset($_POST['FK_MasterUsers'])){
		$FK_MasterUsers=$_POST['FK_MasterUsers'];
		$typeReferral=$_POST['typeReferral'];
		$ContactName=$_POST['ContactName'];
		$BusinessName=$_POST['BusinessName'];
		$Email=$_POST['Email'];
		$ReferrerNotes=$_POST['ReferrerNotes'];
		
		// check if the user doesn't already referr the user
		$queryReferedEmail=dbQuery("SELECT * from Referral WHERE FK_MasterUsers_Referrer='$FK_MasterUsers' AND Email='".$Email."'",$conn);
		$queryEmail=dbQuery("SELECT * from MasterUsers WHERE Email='".$Email."'",$conn);
		if(mysql_num_rows($queryReferedEmail)>0)
			$out='You already referred this email.';
		elseif(mysql_num_rows($queryEmail)!=0){
			// check if the email isn't already sign in
			$out='The user with this Email already sign in.';
		}else{
			$insertReferrer=dbQuery("
			INSERT INTO Referral 
				(FK_MasterUsers_Referrer, typeReferral, ContactName, BusinessName, Email, Notes, ReferrerNotes, ReferralTime)
			VALUES
				('$FK_MasterUsers', '$typeReferral', '$ContactName', '$BusinessName', '$Email', '$Notes', '$ReferrerNotes', '$ReferralTime')
			",$conn);
			if($insertReferrer)
				$out="Succes";
		}
	}else
		$out='';

	echo $out;	
 }
?>