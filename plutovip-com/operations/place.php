<?
function place($output)
{
  $out='';
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
   $out.='
  <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
    <table cellpadding="5" cellspacing="0" border="0">
      <tr>
      <td align="left">
     <img src="images/submenu_txt/places_ive_been_txt.gif" width="113" height="14">
      </td>
      </tr>
			<tr>
				<th>Establishment</th>
				<th>Start time</th>
				<th>End Time</th>
			</tr>';
  $sql="select * from Visit where FK_MasterUsers_User='".$_SESSION['userID']."'";
  $r=mysql_query($sql);
  while($row=mysql_fetch_object($r))
  {
  $sql1="select * from Establishment where FK_MasterUsers='".$row->FK_MasterUsers_Establishment."'";
  $r1=mysql_query($sql1);
  
  $row1=mysql_fetch_object($r1);
  
  $out.='		<tr bgcolor="#CCCCCC">
					<td>'.$row1->Name.'</td>
					<td>'.$row->StartTime.'</td>
					<td>'.$row->EndTime.'</td>
					<td>
					<a href="index.php?section=place&id='.$row->FK_MasterUsers_Establishment.'">Other Pluto VIPs</a>
					</td>
                 </tr>';
  }
  if(isset($_GET['id']))
  {
    $out='';
     $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">
    <table cellpadding="5" cellspacing="0" border="0">
    <tr>
    <td colspan="3"><a href="index.php?section=place">Back to Places I\'ve been</td>
    </tr>';
  $sql="select * from Establishment where FK_MasterUsers='".$_GET['id']."'";
  $r=mysql_query($sql) or die("Can not grab from database ".mysql_error());
  $row=mysql_fetch_object($r);
  $sql1="select * from Visit where FK_MasterUsers_Establishment='".$_GET['id']."'";
  $r1=mysql_query($sql1);
  $row1=mysql_fetch_object($r1);
  $out.='<tr><td colspan="3" align="center"><b>'.$row->Name.'</b></td></tr><tr><td>'.$row1->StartTime.'</td>
  <td>to</td><td>'.$row1->EndTime.'</td></tr>';
   
  }
  $out.='</table></td></tr></table>';
   $output->setScriptCalendar('null');
		$output->setScriptTRColor('null');	
      $output->placesIMG = 'images/home%20page%20img/a_places_on.gif';
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME."::Place I've been");			
      $output->setPageID(98); //butonul selectat
  		$output->output();
}
?>