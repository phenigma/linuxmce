<?
if($action=='form'){
	$PK_Installation=(isset($_REQUEST['instid']))?(int)$_REQUEST['instid']:$installationID;
	// display form
	$out = '<br>
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
						<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
	      				<td align="right" class="normaltext"><a href="index.php?section=wizard&step=1">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a></td>
	      			</tr>
	      			<tr>
	      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 2 of 8: Dedicated or non-dedicated</b></td>
	      			</tr>
					<tr>
						<td colspan="2" align="left" class="insidetable">';
	switch(@$_SESSION['sollutionType']){
		case 1:
		case 2:
			$out.='
				<p>Since you want a whole-house solution, the computer you will be using as the '.$_SESSION['coreName'].' must always be running--the other media directors will not be functional if the '.$_SESSION['coreName'].' is turned off.</p>

				<p>It is HIGHLY recommended for whole-house solutions that that the computer you use as your '.$_SESSION['coreName'].' be dedicated.  Dedicated means that instead of installing the Pluto software on top of your existing operating system, you use Pluto\'s kick-start CD and let it take over the computer, cleaning out the hard drive and installing its own operating system.</p>

				<p>The drawback with a dedicated '.$_SESSION['coreName'].' is that you need to have an extra computer that you will dedicate to Pluto.  But there are many advantages.  A dedicated '.$_SESSION['coreName'].' is much simpler to setup--put the CD in and let it take over.  There\'s nothing for you to do and no software to install.  A dedicated '.$_SESSION['coreName'].' is also self-configuring and maintaining and will keep itself up-to-date automatically.</p>

				<p>Plus, a dedicated '.$_SESSION['coreName'].' allows any computer in your home to do a <a href="http://plutohome.com/support/index.php?section=document&docID=144">network boot</a>.  That means these computers can boot up as a Pluto media director without using their own hard drive.  A frequent complaint with media pc\'s is that when you want to watch tv or a movie you don\'t want to mess with a pc--you want an appliance that you hit "play" on the remote control and that\'s it.  With network boot you get the best of both worlds.  Hit a button on the remote and it boots up like a normal pc, like it does now.  Hit another and it turns into a media director appliance--no start button, no software to install, no viruses to worry about.  Also even if you completely mess up the pc side--maybe install a virus or crash Windows--it won\'t affect the media director side since they are kept separate.</p>

				<p>And a dedicated '.$_SESSION['coreName'].' supports plug-and-play.  You can plug all sorts of devices into your home network--telephones, surveillance cameras, interface modules, audio players, etc.--and the dedicated '.$_SESSION['coreName'].' will install the software drivers automatically and configure them without you having to do a thing.</p>
			';
		break;
		case 3:
			$out.='
				<p>You have 2 choices for installing the Pluto software:

				<p>First, you can use the Kick-Start CD.  This will completely take over the PC--it reformats and partitions the hard drive and installs Pluto\'s software with its native operating environment.  This is the simplest solution--you have nothing to do, put the CD in and boot.  Also, a dedicated Pluto system is self-maintaining and self-configuring.  You turn it on and leave it--like an appliance.

				<p>Second, instead of the Kick-start CD, you can install Pluto\'s software on your existing Windows or Linux PC like any other software program.  You will miss out on a lot of the advanced features that a dedicated Pluto '.$_SESSION['coreName'].' offers, like streaming video throughout the house, bandwidth management and so on.  And it won\'t be self-configuring and maintaining like a dedicated '.$_SESSION['coreName'].'. However since you indicated in step 1 that you want single-pc software, rather than a whole-house solution, these advanced features might not be so important for you.
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
						<td colspan="2" align="left" class="insidetable">
							<p><input type="radio" name="dedicated" value="1" '.((@$_SESSION['dedicated']==1)?'checked':((!isset($_SESSION['dedicated']))?'checked':'')).' onClick="enableDistro();"> 
							Yes, I want a dedicated '.$_SESSION['coreName'].'.  I have an extra PC so I will use the Kick-start CD and let it clean out the hard drive.  This gives you the most functionality.
							<p><input type="radio" name="dedicated" value="2" '.((@$_SESSION['dedicated']==2)?'checked':'').' onClick="enableDistro();"> I don\'t want a dedicated '.$_SESSION['coreName'].'.  Install the software on my existing <b>Windows PC</b>.  You can still use the Orbiter and Mobile Orbiter software, but most of Pluto\'s advanced functionality will not be available, including network boot and streaming video.
							<p><input type="radio" name="dedicated" value="3" '.((@$_SESSION['dedicated']==3)?'checked':'').' onClick="enableDistro();"> I don\'t want a dedicated '.$_SESSION['coreName'].'.  Install the software on my existing <b>Linux PC</b> which is using: '.$distroPullDown.'  You can get all of Pluto\'s advanced functionality, but since Pluto was built on Debian some tweaking may be needed.
							If you\'re comfortable with Debian, you might prefer to use the Kick-Start CD anyway since that will set everything up automatically and you can still use the computer as a normal Debian box anyway.</p>
							<p><a href="http://plutohome.com/support/index.php?section=document&docID=145">More about dedicated vs non-dedicated</a> &nbsp;&nbsp; <a href="http://plutohome.com/support/index.php?section=document&docID=144">More about network boot</a></p>
						</td>
					</tr>	
					<tr>
						<td colspan="2" align="center" class="insidetable"><input type="submit" name="continue" value="Next"></td>
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
			break;
			case 3:
				$_SESSION['distro']=(int)$_POST['distro'];
				$_SESSION['OperatingSystem']=1;
			break;
		}
		header("Location: index.php?section=wizard&step=3");
		exit();
	}
}
?>