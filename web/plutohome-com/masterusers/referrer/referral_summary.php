<?
function referral_summary($conn){
	if(isset($_POST['FK_MasterUsers'])){
		$FK_MasterUsers_Referrer=$_POST['FK_MasterUsers'];
		
		$queryBusinessReferrals=dbQuery("SELECT * FROM MasterUsers WHERE FK_MasterUsers_Referrer='$FK_MasterUsers_Referrer' AND FirstAccount ='1'",$conn);
		$businessReferrals=mysql_num_rows($queryBusinessReferrals);
		
		$queryIndividualReferrals=dbQuery("SELECT * FROM MasterUsers WHERE FK_MasterUsers_Referrer='$FK_MasterUsers_Referrer' AND FirstAccount ='0'",$conn);
		$individualReferrals=mysql_num_rows($queryIndividualReferrals);

		$queryPendingBusinessReferrals=dbQuery("SELECT * FROM Referral WHERE FK_MasterUsers_Referrer='$FK_MasterUsers_Referrer' AND typeReferral ='1'",$conn);
		$pendingBusinessReferrals=mysql_num_rows($queryPendingBusinessReferrals);
		
		$queryPendingIndividualReferrals=dbQuery("SELECT * FROM Referral WHERE FK_MasterUsers_Referrer='$FK_MasterUsers_Referrer' AND typeReferral ='0'",$conn);
		$pendingIndividualReferrals=mysql_num_rows($queryPendingIndividualReferrals);

		$out='
			<table align="center">
				<tr>
					<td colspan="2" align="left">Your points so far:</td>
				</tr>
				<tr>
					<td align="left">'.$businessReferrals.' Business Referrals @ 10 points each </td>
					<td>'.($businessReferrals*10).'</td>
				</tr>
				<tr>
					<td align="left">'.$individualReferrals.' Individual Referrals @ 1 points each </td>
					<td>'.$individualReferrals.'</td>
				</tr>
				<tr>
					<td align="left">&nbsp;&nbsp;&nbsp;&nbsp;Total points:</td>
					<td><b>'.($businessReferrals*10+$individualReferrals).'</b></td>
				</tr>
				<tr>
					<td colspan="2" align="left">Pending Referrals:</td>
				</tr>
				<tr>
					<td align="left">'.$pendingBusinessReferrals.' Pending Business Referrals @ 10 points each </td>
					<td>'.($pendingBusinessReferrals*10).'</td>
				</tr>
				<tr>
					<td align="left">'.$pendingIndividualReferrals.' Pending Individual Referrals @ 1 points each </td>
					<td>'.$pendingIndividualReferrals.'</td>
				</tr>
				<tr>
					<td align="left">&nbsp;&nbsp;&nbsp;&nbsp;Total points:</td>
					<td><b>'.($pendingBusinessReferrals*10+$pendingIndividualReferrals).'</b></td>
				</tr>
				<tr>
					<td align="left" colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td align="left">&nbsp;&nbsp;&nbsp;&nbsp;Total points when Pending Referrals complete:</td>
					<td><b>'.($businessReferrals*10+$individualReferrals+$pendingBusinessReferrals*10+$pendingIndividualReferrals).'</b></td>
				</tr>
			</table>
		';
	}else
		$out='';
	

	echo $out;	
 }
?>