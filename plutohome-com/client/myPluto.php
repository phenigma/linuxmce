<?
function myPluto($output,$dbADO,$conn) {
	global $forumHost,$MantisHost;
	/* @var $dbADO ADOConnection */
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}

	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;

	$queryInstallations='
			SELECT * FROM Installation_Users
				INNER JOIN Installation ON FK_Installation=PK_Installation
			WHERE Installation_Users.FK_Users=?';
	$res=$dbADO->Execute($queryInstallations,$FK_Users);
	if($res->RecordCount()==0)
		$installationsText='No Pluto installations.<br><br>
		To get Pluto, choose <a href="index.php?section=wizard&step=1&instid=0">New Installation</a>.';
	else {
		$installationsText='You have the following installations:<br>';
		while($rowInstallations=$res->FetchRow()){
			$installationsText.='
				Installation no. <b>'.$rowInstallations['PK_Installation'].'</b><br>
				Description: <a href="index.php?section=wizard&step=1&instid='.$rowInstallations['PK_Installation'].'"><b>'.$rowInstallations['Description'].'</b></a><br>';
		}
		$installationsText.='<br><a href="index.php?section=wizard&step=1&instid=0"><b>New installation</b></a><br>';
	}

	if(isset($_GET['redirect'])){
		if($_GET['redirect']=='forum'){
			header("Location: ".$forumHost."login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1");
		}
		else
			header("Location: ".$MantisHost."login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1");
	}
		
	$out = '<br>
      		<table width="100%">
	      		<tr>
      				<td align="left" width="33%" class="normaltext"><img src="images/my_pluto.gif" border="0"><br>You are logged in as <b>'.$_SESSION['username'].'</b></td>
      				<td align="center" width="33%" class="normaltext"></td>
      				<td align="right" width="34%" class="normaltext"><a href="index.php?section=myPluto&redirect=forum"><img src="images/forum.gif" border="0"></a> <a href="index.php?section=myPluto&redirect=mantis"><img src="images/mantis.gif" border="0"></a> <a href="support/index.php"><img src="images/support.gif" border="0"></a></td>
      			</tr>
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Installations</b></td>
      				<td align="center" width="33%" class="normaltext"><b>Referrals</b></td>
      				<td align="center" width="34%" class="normaltext"><b>Developer’s corner</b></td>
      			</tr>
      	      	<tr>
      				<td align="left" valign="top" class="normaltext">'.$installationsText.'<br>					
					</td>
      				<td align="center">
						<table width="100%">
							<tr>
								<td colspan="2" align="left" class="normaltext">Pluto rewards you for spreading the word.  <a href="index.php?section=special_offer">more</a></td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext">&nbsp;</td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext">Instant cash for referring Pluto Home customers:</td>
							</tr>
							<tr>
								<td align="right" class="normaltext"><B>Your earnings so far:</B></td>
								<td align="left" class="normaltext"> $0</td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext"><br>Commission on licensing deals:</td>
							</tr>
							<tr>
								<td align="right" class="normaltext"><B>Your earnings so far:</B></td>
								<td align="left" class="normaltext"> $0</td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext"><br>Refer PlutoVIP users for shares in a future IPO:</td>
							</tr>
							<tr>
								<td align="right" class="normaltext"><B>Your points so far:</B></td>
								<td align="left" class="normaltext"> 0</td>
							</tr>	
							<tr>
								<td colspan="2" align="left" class="normaltext"></td>
							</tr>	
						</table>
					</td>
      				<td align="left" valign="top" class="normaltext">An introduction to Pluto’s software architecture.  Start <a href="index.php?section=developer">here</a> for a quick overview, and links to all the documentation.<br><br>
Write your own Pluto Plug-in or DCE Device in 5 minutes.<br><br>
The Pluto software map lists all the various projects and devices that make up a Pluto system, with links to the source code.
					</td>
      			</tr>
	      		<tr>
      				<td align="center" width="33%" class="normaltext" colspan="3">&nbsp;</td>
      			</tr>	
	      		<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Title</b></td>
      				<td align="center" width="33%" class="normaltext"><b>Title</b></td>
      				<td align="center" width="34%" class="normaltext"><b>Title</b></td>
      			</tr>
	      		<tr>
      				<td align="left" width="33%" class="normaltext" valign="top">Text text text text text text text text text text text text text</td>
      				<td align="left" width="33%" class="normaltext"  valign="top">Text  text text text text text text text text text text text text text text</td>
      				<td align="left" width="34%" class="normaltext"  valign="top">Text text text text text text text text text text text text text</td>
      			</tr>
      		</table>
      	';


	$output->setNavigationMenu(array("Client home"=>"index.php?section=userHome"));

	$output->setLeftMenu($leftMenu);

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>