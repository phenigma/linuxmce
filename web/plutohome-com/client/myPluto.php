<?
function myPluto($output,$dbADO,$conn) {
	global $forumHost,$MantisHost;
	/* @var $dbADO ADOConnection */
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}

	if(isset($_SESSION['setAutoCookie']) && $_SESSION['setAutoCookie']==1){
		$cookieStr='username='.$_SESSION['username'].'&password='.$_SESSION['password'].'&extPassword='.$_SESSION['extPassword'];
		$cookieIsSet=setcookie("PlutoHomeAutoLogin",base64_encode($cookieStr),time()+3600*24*365,"/", false);
		unset($_SESSION['setAutoCookie']);
		unset($_SESSION['password']);
	}
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;

	$queryInstallations='
			SELECT * FROM Installation_Users
				INNER JOIN Installation ON FK_Installation=PK_Installation
			WHERE Installation_Users.FK_Users=?';
	$res=$dbADO->Execute($queryInstallations,$FK_Users);
	if($res->RecordCount()==0)
		$installationsText='No Pluto installations.<br><br>
		<b>To get Pluto</b>, choose <a href="index.php?section=wizard&step=1&instid=0">Start wizard</a>.';
	else {
		$installationsText='You have the following installations:<br>';
		while($rowInstallations=$res->FetchRow()){
			$installationsText.='
				Installation no. <b>'.$rowInstallations['PK_Installation'].'</b><br>
				Description: <a href="index.php?section=wizard&step=8&instid='.$rowInstallations['PK_Installation'].'"><b>'.stripslashes($rowInstallations['Description']).'</b></a><br>';
		}
		$installationsText.='<br><a href="index.php?section=wizard&step=1&instid=0"><b>Start wizard</b></a><br>';
	}

	if(isset($_GET['redirect'])){
		if($_GET['redirect']=='forum'){
			header("Location: support/phpbb2/login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1");
		}
		else{
			//header("Location: ".$MantisHost."login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1");
			echo "
			<script>
				window.open('support/mantis/login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1','_blank','');
				self.location='index.php?section=myPluto'
			</script>";
		}
	}
		
	$out = '<br>
      		<table width="100%">
	      		<tr>
      				<td align="left" width="33%" class="normaltext"><img src="images/my_pluto.gif" border="0"><br>You are logged in as <b>'.$_SESSION['username'].'</b></td>
      				<td align="center" width="33%" class="normaltext"></td>
      				<td align="right" width="34%" class="normaltext"><a href="index.php?section=myPluto&redirect=forum"><img src="images/forum.gif" border="0"></a>'. /*<a href="index.php?section=myPluto&redirect=mantis"><img src="images/mantis.gif" border="0"></a>*/'&nbsp;&nbsp;&nbsp;&nbsp;<a href="support/index.php"><img src="images/support.gif" border="0"></a></td>
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
								<td colspan="2" align="left" class="normaltext">Pluto has unique and generous rewards for referring both open source and retail customers as well as
								 dealers and manufacturers who license the Pluto platform for their smarthome appliances.  Send us an email or fill out the contact form for details.
								 </td>
							</tr>

						</table>
					</td>
      				<td align="left" valign="top" class="normaltext">Pluto 2 has been written from the ground up to be a very comfortable development platform for open source programmers.<br><br>
      				We have developed class generators that will build a fully complete, ready-to-compile <a href="http://plutohome.com/support/index.php?section=document&docID=51">DCE Devices</a> in minutes.
      				They\'re standard C++, run on both Linux & Windows, ready talk to any other DCE Device on any platform.  See our <a href="http://plutohome.com/support/index.php?section=document&docID=15">Programmer\'s guide</a> for a quick intro.<br><br>
      				</td>
      			</tr>
	      		<tr>
      				<td align="center" width="33%" class="normaltext" colspan="3">&nbsp;</td>
      			</tr>	
	      		<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Status</b></td>
      				<td align="center" width="33%" class="normaltext"><b>Support Site</b></td>
      				<td align="center" width="34%" class="normaltext"><b>Tools for developers</b></td>
      			</tr>
	      		<tr>
      				<td align="left" width="33%" class="normaltext" valign="top">
      				Pluto has made available a preview release of the new Pluto.  This is targeted mainly for developers, since it is not yet ready for end users.
      				Click "New Installation" above to get a Kick-Start CD, or download the installer.
      				</td>
      				<td align="left" width="33%" class="normaltext"  valign="top">
      				With the launch of this preview version, Pluto has put up it\'s new support web site at <a href="http://plutohome.com/support/index.php?section=home">www.plutohome.com/support</a>.<br><br>
      				There is online documentation, support forums, bug tracking and mailing lists.  Plus we have a live chat system so you can talk directly to one of our tech support team, or a developer.
      				</td>
      				<td align="left" width="34%" class="normaltext"  valign="top">
      				Pluto has released several general purpose classes that you may find interesting, such as <a href="http://plutohome.com/support/index.php?section=document&docID=42">sqlCVS</a>, which is basically a CVS for databases.  
      				It\'s what allows different programmers to all work on local copies of Pluto\'s master database and handles check-ins, updates, and remembers who owns each record.<br><br>
      				
      				There are many other libraries too, such as our <a href="http://plutohome.com/support/index.php?section=document&docID=108">Serialize Class</a>, 
      				<a href="http://plutohome.com/support/index.php?section=document&docID=109">sql2cpp</a> class generator,
      				<a href="http://plutohome.com/support/index.php?section=document&docID=41">Request/Action</a> client-server library</a>, 
      				<a href="http://plutohome.com/support/index.php?section=document&docID=39">Bi-Directional communications library</a>.  They\'re all GPL.<br><br>
      				
      				</td>
      			</tr>
      		</table>
      	';


	$output->setNavigationMenu(array("Client home"=>"index.php?section=userHome"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>