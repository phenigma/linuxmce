<?
if($action=='form'){
	$PK_Installation=(isset($_REQUEST['instid']))?(int)$_REQUEST['instid']:$installationID;
	if($PK_Installation!=0){
		$queryInstallations='
						SELECT * FROM Installation
							INNER JOIN Installation_Users ON FK_Installation=PK_Installation
						WHERE PK_Installation=? AND FK_Users=?';
		$resInstallations=$dbADO->Execute($queryInstallations,array($PK_Installation,$FK_Users));
		if($resInstallations){
			$rowInstallations=$resInstallations->FetchRow();
			$_SESSION['installationID']=$PK_Installation;
			$_SESSION['installationDescription']=$rowInstallations['Description'];
			$_SESSION['ActivationCode']=$rowInstallations['ActivationCode'];
		}
	}else{
		unset($_SESSION['installationID']);
		unset($_SESSION['installationDescription']);
		unset($_SESSION['ActivationCode']);
	}
	// display form
	$out = '<br>
		   <form action="index.php" method="POST" name="wizard">
			<input type="hidden" name="section" value="wizard">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">	
	      		<table align="center">
					<tr>
						<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
	      				<td align="right" class="normaltext"><a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a></td>
	      			</tr>
	      			<tr>
	      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 1 of 8: Whole house solution or stand-alone pc</b></td>
	      			</tr>
					<tr>
						<td colspan="2" align="left" class="insidetable"><p>Before proceeding you may want to read about <a href="http://plutohome.com/support/index.php?section=document&docID=177">recommended hardware</a>.</p>
						<p>A Pluto whole-house solution consists of 1 computer that becomes the server--we call it the Core.  The Core is the "brains" which coordinates all the activities in the house.  It also stores all your media.</p>

						<p>At each TV where you want to watch media stored on the Core you need another computer, which we call a media director, to hook up to the TV.  The media director is also used as a general purpose PC for computing.  All the media directors in the house connect to the Core over the network.  There are also low-cost audio directors (under $200) that you can add in any room where you only want music.</p>

						<p>In Pluto\'s high-end installations, the Core is a commercial server with massive storage that sits in a rack somewhere out of the way--the homeowner never touches it.  It maintains itself and is more like an appliance than a PC.</p>
						<p>But you can also run both the Core and Media Director software on the same computer.  We call this a "hybrid" since it runs both the Media Director software to give you media in that room, and also the Core software to orchestrate other Media Directors.
						You do not lose any functionality by having a hybrid--it can still be the brains behind a whole-house solution and store all your media for the whole house.</p>
						</td>
					</tr>
	      			<tr>
	      				<td align="left" colspan="2" bgcolor="lightblue" class="normaltext" style="padding-left: 20px;padding-right: 20px;"><B>Whole-house solution with multiple media directors</B></td>
	      			</tr>	
	      			<tr>
	      				<td align="left" colspan="2" class="normaltext" style="padding-left: 20px;padding-right: 20px;"><input type="radio" name="sollutionType" value="1" '.((@$_SESSION['sollutionType']==1)?'checked':((!isset($_SESSION['sollutionType']))?'checked':'')).'> I have a pc that I will use as the Core, or server, for the whole house.  I do not need to use it as a media director.  It will be a stand-alone Core that I put somewhere out of the way.  I have separate pc(s) that I will hook up to my tv\'s and use as media directors.</td>
	      			</tr>	
	      			<tr>
	      				<td align="left" colspan="2" class="normaltext" style="padding-left: 20px;padding-right: 20px;"><input type="radio" name="sollutionType" value="2" '.((@$_SESSION['sollutionType']==2)?'checked':'').'> I want a hybrid--a pc that will be my Core, but will also be a media director that I hook up to the tv.  I have other pc(s) that will connect to it to be additional media directors.</td>
	      			</tr>	
	      			<tr>
	      				<td align="left" colspan="2" bgcolor="lightblue" class="normaltext" style="padding-left: 20px;padding-right: 20px;"><B>Single-pc solution -- only 1 media director</B></td>
	      			</tr>	
	      			<tr>
	      				<td align="left" colspan="2" class="normaltext" style="padding-left: 20px;padding-right: 20px;"><input type="radio" name="sollutionType" value="3" '.((@$_SESSION['sollutionType']==3)?'checked':'').'>  
	      				I just want to install Pluto software on a stand-alone PC. I don\'t have other pc\'s that I want to be media directors.  You can still use this pc to control your home automation sub-systems 
	      				(lighting, climate, etc.) and a/v equipment, and it works with both tablet and mobile phone Orbiters.  Also it can stream music to other rooms using audio directors, 
	      				but you will not be distributing movies and tv to other rooms, since that would require media directors in those rooms.</td>
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
		$_SESSION['sollutionType']=(int)$_POST['sollutionType'];
		$_SESSION['coreName']=($_SESSION['sollutionType']==1)?'Core':'Hybrid';
		header("Location: index.php?section=wizard&step=2");
		exit();
	}
}
?>