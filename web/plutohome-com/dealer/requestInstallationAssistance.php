<?
function requestInstallationAssistance($output,$conn,$dbADO) {
	/* @var $dbADO ADOConnection */
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	
	// get dealer ID from Users table
	$resUser=$dbADO->Execute('SELECT FK_Dealer FROM Users WHERE PK_Users=?',$_SESSION['userID']);
	if($resUser->RecordCount()==0){
		header('Location: index.php?section=dealerApplication');
		exit();
	}
	$rowUser=$resUser->FetchRow();
	$dealerID=$rowUser['FK_Dealer'];
	
	
	$out='';

if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" name="requestInstallationAssistance">
  		<input type="hidden" name="section" value="requestInstallationAssistance">
		<input type="hidden" name="action" value="add">
	<table width="100%">
		<tr>
			<td align="center" class="titletable"><B>Request Installation assistance</B></td>
		</tr>
		<tr>
			<td align="center" class="insidetable">
			<table>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext" width="50%"><B>What call this?</B></td>
					<td align="left" class="normaltext"><input type="text" name="Name" value="" size="40"></td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>When do you want to do the installation?</B></td>
					<td align="left" class="normaltext"><input type="text" name="Date" value="" size="40"></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What phone number(s)?</B></td>
					<td align="left" class="normaltext"><input type="text" name="PhoneNumber" value="" size="40"></td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of computer will you be using as the Core?</B><br>(List make and model, or if it\'s a no-name system, <br>indicate what components are inside)</td>
					<td align="left" class="normaltext"><textarea rows="3" style="width: 280;" name="ComputerType"></textarea></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>Will this computer also be a media director (ie a Hybrid)?</B></td>
					<td align="left" class="normaltext"><input type="radio" name="IsHybrid" value="1" checked> Yes <input type="radio" name="IsHybrid" value="0"> No</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>Does it have dual network cards?</B></td>
					<td align="left" class="normaltext"><input type="radio" name="DualNetworkCards" value="1" checked> Yes <input type="radio" name="DualNetworkCards" value="0"> No NOTE: You will need an internet connection, like DSL, Cable</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of computer(s) will you be using as your Media Directors?</B><br>  (List make and model, or if it\'s a no-name system, indicate what components are inside)</td>
					<td align="left" class="normaltext"><textarea rows="3" style="width: 280;" name="MediaDirectors"></textarea></td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of lighting system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext"><input type="text" name="LightingMake" value="" size="40"><br><input type="radio" name="LightingConn" value="RS232" checked> RS232 <input type="radio" name="LightingConn" value="Ethernet">Ethernet <input type="radio" name="LightingConn" value="RS485"> RS485 <input type="radio" name="LightingConn" value="Other"> Other &nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="LightingModel" size="10" value=""></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of climate system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext"><input type="text" name="ClimateMake" value="" size="40"><br><input type="radio" name="ClimateConn" value="RS232" checked> RS232 <input type="radio" name="ClimateConn" value="Ethernet">Ethernet <input type="radio" name="ClimateConn" value="RS485"> RS485 <input type="radio" name="ClimateConn" value="Other"> Other &nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="ClimateModel" size="10"></td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of security system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext"><input type="text" name="SecurityMake" value="" size="40"><br><input type="radio" name="SecurityConn" value="RS232" checked> RS232 <input type="radio" name="SecurityConn" value="Ethernet">Ethernet <input type="radio" name="SecurityConn" value="RS485"> RS485 <input type="radio" name="SecurityConn" value="Other"> Other &nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="SecurityModel" size="10"></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of device for controlling infrared?</B></td>
					<td align="left" class="normaltext"><input type="radio" name="InfraredOption" value="gc100" checked> GC100 <input type="radio" name="InfraredOption" value="other"> Other <input type="text" name="Infrared" size="10"></td>
				</tr>	
				<tr>
					<td align="center" class="normaltext"><B>Makes & Models of your A/V equipment?  We will lookup the codes.</B></td>
					<td align="left" class="normaltext"><textarea rows="6" style="width: 280;" name="AVEquipment"></textarea></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>Any other devices you will want to control?  (pool, sprinkler, etc.)</B></td>
					<td align="left" class="normaltext"><textarea rows="6" style="width: 280;" name="OtherDevices"></textarea></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="normaltext"><input type="submit" name="sendRequest" value="Send request"></td>
				</tr>							
			</table>
			</td>
		</tr>
	</table>
	</form>
';
}else{
	// process
	if(isset($_POST['sendRequest'])){
		$Name=$_POST['Name'];
		$Date=$_POST['Date'];
		$PhoneNumber=$_POST['PhoneNumber'];
		$ComputerType=$_POST['ComputerType'];
		$IsHybrid=$_POST['IsHybrid'];
		$DualNetworkCards=$_POST['DualNetworkCards'];
		$MediaDirectors=$_POST['MediaDirectors'];
		$LightingMake=$_POST['LightingMake'];
		$LightingModel=$_POST['LightingConn'].':'.$_POST['LightingModel'];
		$ClimateMake=$_POST['ClimateMake'];
		$ClimateModel=$_POST['ClimateConn'].':'.$_POST['ClimateModel'];
		$SecurityMake=$_POST['SecurityMake'];
		$SecurityModel=$_POST['SecurityConn'].':'.$_POST['SecurityModel'];
		$Infrared=$_POST['InfraredOption'].':'.$_POST['Infrared'];
		$AVEquipment=$_POST['AVEquipment'];
		$OtherDevices=$_POST['OtherDevices'];
		
		
		$emailBody='
				<style>
				.normaltext {
				   font-family: Arial, Verdana, Helvetica, sans-serif;
				   font-size: 12px;
				   color: #36456E;
				}
				</style>
				<table>
				<tr>
					<td align="center" class="normaltext" width="50%"><B>Request by dealer:</B></td>
					<td align="left" class="normaltext"><B># '.$dealerID.' user ID # '.$_SESSION['userID'].'</B></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext" width="50%"><B>What call this?</B></td>
					<td align="left" class="normaltext">'.$Name.'</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>When do you want to do the installation?</B></td>
					<td align="left" class="normaltext">'.$Date.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What phone number(s)?</B></td>
					<td align="left" class="normaltext">'.$PhoneNumber.'</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of computer will you be using as the Core?</B><br>(List make and model, or if it\'s a no-name system, <br>indicate what components are inside)</td>
					<td align="left" class="normaltext">'.nl2br($ComputerType).'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>Will this computer also be a media director (ie a Hybrid)?</B></td>
					<td align="left" class="normaltext">'.(($IsHybrid==1)?'Yes':'No').'</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>Does it have dual network cards?</B></td>
					<td align="left" class="normaltext">'.(($DualNetworkCards==1)?'Yes':'No').'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of computer(s) will you be using as your Media Directors?</B><br>  (List make and model, or if it\'s a no-name system, indicate what components are inside)</td>
					<td align="left" class="normaltext">'.nl2br($MediaDirectors).'</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of lighting system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext">'.$LightingMake.' '.$LightingModel.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of climate system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext">'.$ClimateMake.' '.$ClimateModel.'</td>
				</tr>
				<tr>
					<td align="center" class="normaltext"><B>What type of security system will you be using? <br>Make & Model?</B></td>
					<td align="left" class="normaltext">'.$SecurityMake.' '.$SecurityModel.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>What type of device for controlling infrared?</B></td>
					<td align="left" class="normaltext">'.$Infrared.'</td>
				</tr>	
				<tr>
					<td align="center" class="normaltext"><B>Makes & Models of your A/V equipment?  We will lookup the codes.</B></td>
					<td align="left" class="normaltext">'.nl2br($AVEquipment).'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td align="center" class="normaltext"><B>Any other devices you will want to control?  (pool, sprinkler, etc.)</B></td>
					<td align="left" class="normaltext">'.nl2br($OtherDevices).'</td>
				</tr>
			</table>
			';

		
		$insertRequest="
			INSERT INTO InstallationAssistance 
			(
				FK_Dealer,
				Name,
				Date,
				PhoneNumber,
				ComputerType,
				IsHybrid,
				DualNetworkCards,
				MediaDirectors,
				LightingMake,
				LightingModel,
				ClimateMake,
				ClimateModel,
				SecurityMake,
				SecurityModel,
				Infrared,
				AVEquipment,
				OtherDevices
			) VALUES
			(
				'$dealerID',
				'$Name',
				'$Date',
				'$PhoneNumber',
				'$ComputerType',
				'$IsHybrid',
				'$DualNetworkCards',
				'$MediaDirectors',
				'$LightingMake',
				'$LightingModel',
				'$ClimateMake',
				'$ClimateModel',
				'$SecurityMake',
				'$SecurityModel',
				'$Infrared',
				'$AVEquipment',
				'$OtherDevices'
			)";
		dbQuery($insertRequest,$conn);

		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'Installation assistance request';
	
		@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	}
	$out='
	<table align="center">
		<tr>
			<td class="insidetable" height="250">
				<p class="normaltext"><B>Your installation assistance request was sent.</B>
				<p class="normaltext">
	
			</td>
		<tr>
	</table>';
}


	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>