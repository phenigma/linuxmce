<?
/* @var $dbADO ADOConnection */

$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
$packageName=(isset($_SESSION['packageName']))?$_SESSION['packageName']:'';

if($package!=0){
	$out='
	<p>The following developers have worked on the module <b>**package**</b>.</p>
	<p>Developers with a * icon are on Pluto\'s permanent staff.  You can send them email directly to their [username]@plutohome.com.
	The developer(s) in <i>italics</i> are the administrators for **package**.</p>
	To see a list of all the developers on Pluto, and what modules they are responsible for, <a href="/support/index.php?section=authors&package=0">click here</a>.
	<br><br>';
	$queryDevelopers='
		SELECT Username,Administrator,Staff,Package_Users.Comments 
			FROM Package_Users
		JOIN Package ON FK_Package=PK_Package
		JOIN Users ON FK_Users=PK_Users
			WHERE FK_Package=?
		ORDER BY Administrator DESC,Username';

	$resDevelopers=$dbADO->Execute($queryDevelopers,$package);
	while($rowDevelopers=$resDevelopers->FetchRow()){
		$stars=(($rowDevelopers['Staff'])==1)?'*':$stars;
		$out.=$stars.' '.($rowDevelopers['Administrator']==1?'<i>':'').$rowDevelopers['Username'].($rowDevelopers['Administrator']==1?'</i>':'').' '.$rowDevelopers['Comments'];
	}
}else{
	$out='<p>The following developers have worked on the modules indicated.  If the module is in <i>italics</i>, the developer is an administrator for that module.</p>
	<p>Developers with a * icon are on Pluto\'s permanent staff.  You can send them email directly to their [username]@plutohome.com.</p>
	To see what each developer\'s reponsibility is for each software module, visit that module\'s homepage.
	<br>';
	$queryDevelopers='
		SELECT Username,Administrator,Staff,Package_Users.Comments,Package.Description as Package,PK_Package 
			FROM Package_Users
		JOIN Package ON FK_Package=PK_Package
		JOIN Users ON FK_Users=PK_Users
			ORDER BY Username';
	$resDevelopers=$dbADO->Execute($queryDevelopers);
	$out.='<center><table>
			<tr bgcolor="lightblue">
				<td><B><center>Developer<center></B></td>
				<td><B><center>Module</center></B></td>
				<td><B><center>Comments</center></B></td>
			</tr>
	';
	$ind = 0 ;
	while($rowDevelopers=$resDevelopers->FetchRow()){
		if ($ind%2 == 1 ) {
			$col = ' bgcolor="#C0C0C0"';
		}
		else {
			$col = '';
		}
		$ind++;
		$stars=(($rowDevelopers['Staff'])==1)?'*':'';
		$startItalic=(($rowDevelopers['Administrator'])==1)?'<em>':'';
		$endItalic=(($rowDevelopers['Administrator'])==1)?'</em>':'';
		$out.='
			<tr'.$col.'>
				<td><center>'.$stars.' '.$rowDevelopers['Username'].'</center></center></td>
				<td><center>'.($rowDevelopers['Administrator']==1?'<i>':'').$rowDevelopers['Package'].($rowDevelopers['Administrator']==1?'</i>':'').'</center></td>
				<td><center>'.$rowDevelopers['Comments'].'</center></td>
			</tr>
		';
	}
	$out.='</table></center>';
	
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>