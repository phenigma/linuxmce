<?
if($action=='form'){
	$PK_Installation=(isset($_REQUEST['instid']))?(int)$_REQUEST['instid']:$installationID;
	// display form
	$out = '
		<script>
		function enableDistro()
		{
			if(document.wizard.dedicated[2].checked)
				document.wizard.distro.disabled=false;
			else
				document.wizard.distro.disabled=true;
		}
	
		function validateForm()
		{
			var tmp=document.wizard.dedicated.length;
			for(i=0;i<tmp;i++){
				if(document.wizard.dedicated[i].checked){
					selValue=document.wizard.dedicated[i].value;
					break;
				}
			}
			if(selValue==3 && document.wizard.distro.selectedIndex==0){
				alert("Please select the distro.");
				document.wizard.distro.focus();
				return false;
			}
			return true;
		}
		</script>
		   <form action="index.php" method="POST" name="wizard" onSubmit="return validateForm();">
			<input type="hidden" name="section" value="wizard">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">	
	      		<table align="center">
					<tr>
						<td align="left" class="insidetable"><img src="images/titles/installation_wizard.gif"></td>
	      				<td align="right" class="insidetable"><a href="index.php?section=wizard&step=1">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a></td>
	      			</tr>
	      			<tr>
	      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 2 of 8: Dedicated or non-dedicated</b></td>
	      			</tr>
					<tr>
						<td colspan="2" align="left" class="insidetable2">';
	switch(@$_SESSION['sollutionType']){
		case 1:
		case 2:
			$out.='
				<p>Since you want a whole-house solution, the 1 computer you will be using as the '.$_SESSION['coreName'].' must always be running--the 
				other media directors will not work when the '.$_SESSION['coreName'].' is turned off.  
				We recommended that you choose the first option below to get a <b>dedicated</b> '.$_SESSION['coreName'].', and 
				then choose <a href="http://plutohome.com/support/index.php?section=document&docID=144">network boot</a> for your media directors.
				This is the configuration Pluto\'s testers always use, and the one offered in our commercial product.</p>
				<p>Dedicated means that for the 1 computer that will be your '.$_SESSION['coreName'].' you do not install the Pluto software on top of your existing operating system, 
				but rather use our kick-start CD which erases the hard drive, and "takes over" by installing its own operating system.</p>

				<p>The drawback with a dedicated '.$_SESSION['coreName'].' is that you need to have an extra computer that you will dedicate to Pluto.  But there are many advantages:
				A dedicated '.$_SESSION['coreName'].' is much simpler to setup and more reliable.  Plus it offers a lot of advanced features that you cannot get with a "regular" 
				operating system, such as <a href="http://plutohome.com/support/index.php?section=document&docID=144">network boot</a>, which makes your media directors much
				more useable than any normal media pc, and whole-house plug and play so you can plug all sorts of devices like telephones, cameras, and audio players into any network jack 
				and they start working automatically.</p>
				
				<p>A dedicated '.$_SESSION['coreName'].' is self-maintaining and configuring--you won\'t need to touch it.  If you\'re a Linux user, you can still use a 
				dedicated '.$_SESSION['coreName'].' as a normal pc, based on Debian.  Pluto just adds a lot of extra services that run in the background.</p>
				<p><b>If you don\'t choose "yes"</b> please read <a href="http://plutohome.com/support/index.php?section=document&docID=145">this</a> to learn about the differences.</p>
			';
		break;
		case 3:
			$out.='
				<p>You have 2 choices for installing the Pluto software:

				<p>First, you can use the Kick-Start CD.  This will completely take over the PC--it reformats and partitions the hard drive and installs Pluto\'s software with 
				its native operating environment.  This is the simplest solution--you have nothing to do, put the CD in and boot.  
				Also, a dedicated Pluto system is self-maintaining and self-configuring.  You turn it on and leave it--like an appliance.  It\'s more reliable since this is what
				Pluto\'s testers use.</p>

				<p>Second, instead of the Kick-start CD, you can install Pluto\'s software on your existing Windows or Linux PC like any other software program.  
				You will miss out on a lot of the advanced features that a dedicated Pluto '.$_SESSION['coreName'].' offers, and all the other devices in your house 
				will not be plug-and-play.  In this case you may want to read <a href="http://plutohome.com/support/index.php?section=document&docID=145">differences between dedicated and non-dedicated</a>.</p>
			';		
		break;
		case'':
			header('Location: index.php');	// error case: session is lost
			exit();
		break;
	}
	$distroPullDown='<select name="distro" '.((@$_SESSION['dedicated']!=3)?'disabled':'').'>
				<option value="0">-Please select-</option>';
	$queryDistroOS='
		SELECT Distro.* 
		FROM Distro
		WHERE FK_OperatingSystem=1';
	$resDistroOS=$dbADO->Execute($queryDistroOS);
	while($row=$resDistroOS->FetchRow()){
		$distroPullDown.='<option value="'.$row['PK_Distro'].'" '.(($row['PK_Distro']==@$_SESSION['distro'])?'selected':'').'>'.$row['Description'].'</option>';
	}
	$distroPullDown.='</select>';

	$out.='
						</td>
					</tr>
					<tr>
						<td colspan="2" align="left" class="insidetable2">
							<p><input type="radio" name="dedicated" value="1" '.((@$_SESSION['dedicated']==1)?'checked':((!isset($_SESSION['dedicated']))?'checked':'')).' onClick="enableDistro();"> 
							Yes, I want a dedicated '.$_SESSION['coreName'].'.  I have an extra PC so I will use the Kick-start CD and let it clean out the hard drive.  <b>Recommended</b>
							<p><input type="radio" name="dedicated" value="2" '.((@$_SESSION['dedicated']==2)?'checked':'').' onClick="enableDistro();"> I don\'t want a dedicated '.$_SESSION['coreName'].'.  Install the software on my existing <b>Windows PC</b>.  You can still use the Orbiter and Mobile Orbiter software, but most of Pluto\'s advanced functionality will not be available, including network boot and streaming video.  
							<b>Note:</b> The Windows installer is still in an early stage.</i>
							<p><input type="radio" name="dedicated" value="3" '.((@$_SESSION['dedicated']==3)?'checked':'').' onClick="enableDistro();"> I don\'t want a dedicated '.$_SESSION['coreName'].'.  
							Install the software on my existing <b>Linux PC</b> which is using: '.$distroPullDown.'</p>
						</td>
					</tr>	
					<tr>
						<td colspan="2" align="center" class="insidetable2"><input type="submit" name="continue" value="Next"></td>
					</tr>				
	      		</table>
			</form>
	      	';

}else{
	// process form
	if(isset($_POST['continue']) || $action=='add'){
		$_SESSION['dedicated']=(int)$_POST['dedicated'];
		switch($_SESSION['dedicated']){
			case 1:
				$_SESSION['distro']=1;		// Debian kick start cd
				$_SESSION['OperatingSystem']=1;
			break;
			case 2:
				$_SESSION['distro']=7;		// 2000, XP
				$_SESSION['OperatingSystem']=2;
				unset($_SESSION['EnableDHCP']);
			break;
			case 3:
				$_SESSION['distro']=(int)$_POST['distro'];
				$_SESSION['OperatingSystem']=1;
				unset($_SESSION['EnableDHCP']);
			break;
		}
		header("Location: index.php?section=wizard&step=3");
		exit();
	}
}
?>