<?php
 function visits($output) {
		$out = '';

$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
   <td width="100%" class="insidetable" align="center" height="200" valign="top">
    <h3>Visitors to my establishment</h3>
      	<table>
			<tr>
				<td><b>User<b></td>
				<td><b>Start Time<b></td>
				<td><b>End Time</b></td>
			</tr>';

		$queryVisits=dbQuery("SELECT * FROM Visit 
					INNER JOIN MasterUsers ON  PK_MasterUsers = FK_MasterUsers_User
					WHERE FK_MasterUsers_Establishment='".$_SESSION['userID']."'");
		while($resVisits=mysql_fetch_object($queryVisits)){
			$out.='
			<tr>
				<td>'.$resVisits->Username.'</td>
				<td>'.$resVisits->StartTime.'</td>
				<td>'.$resVisits->EndTime.'</td>
			</tr>';
		}

$out.='		</table>
 	</td>
	</tr>
</table>'; 
 
	
	$output->setTitle(APPLICATION_NAME."::Visitors to my establishment");			//page title
	
	
	$output->visitorsIMG = 'images/home%20page%20img/a_visitors_on.gif';
	//don't touch them
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
 }