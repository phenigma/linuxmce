<?
if($action=='form'){
	$version=isset($_SESSION['version'])?$_SESSION['version']:'Development';
	// display form
	$out = '
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="center" border="0">
							<tr>
								<td colspan="2"><table width="100%">
				      			<tr>
									<td align="left" class="insidetable"><img src="images/titles/installation_wizard.gif"></td>
									<td align="right" class="insidetable"><a href="index.php?section=wizard&step=3">&lt;&lt; Previous</a> <a href="index.php?section=wizard&step=5">Next &gt;&gt;</a></td>
								</tr>
								</table>
								</td>
							</tr>
							<tr>
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 4 of 8: Version</b></td>
			      			</tr>
							<tr>
								<td colspan="2" align="left" class="insidetable2"><br>There are 3 versions of the Pluto software:</td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Development" '.(($version=='Development')?'checked':'').'></td>
								<td class="normaltext"><B>Development</B> – This is the active development version.  It may not be completely stable, but if you’re a developer and want to make your own DCE devices, or work with the source code, this is the version you should install.</td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Testing" disabled '.(($version=='Testing')?'checked':'').'></td>
								<td class="normaltext"><span style="color:#CCCCCC"><B>Testing</B> – This version is frozen, and there is no active development.  The modules have passed our in-house tests and our ready for field testing.  This is the best version if you want the latest features and devices, but still want a stable version.</span></td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Release" disabled '.(($version=='Release')?'checked':'').'></td>
								<td class="normaltext"><span style="color:#CCCCCC"><B>Release</B> – This is the retail, stable version that has gone through exhaustive in-house and field testing.  It is completely stable.</span><br><br></td>
							</tr>
							<tr>
								<td colspan="2" align="center"><input type="submit" name="continue" value="Next"></td>
							</tr>				
			      		</table>
					</form><br>
			      	';

}else{
	// process form
	if(isset($_POST['continue'])){
		$_SESSION['version']=$_POST['version'];
		header("Location: index.php?section=wizard&step=5");
	}
}

?>
