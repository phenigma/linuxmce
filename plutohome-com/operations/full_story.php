<?
function full_story($output){
  $out = '';
		$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td  valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/company_txt.gif" width="66" height="13"></td>
    </tr>
       </table></td>
      <tr><td  valign="top" align="center" class="insidetable">
     <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="139"><a href="index.php?section=company"><img src="images/submenus/Company/company_overview_off.gif" width="139" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=contact"><img src="images/submenus/Company/contact_off.gif" width="139" height="17" border="0"></a></td>
       
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=newsPresentation"><img src="images/submenus/Company/news_on.gif" width="139" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=privacy_policy"><img src="images/submenus/Company/privacy_policy_off.gif" width="139" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=site_map"><img src="images/submenus/Company/site_map_off.gif" width="139" height="17" border="0"></a></td>
        
      </tr>
     
    </table>
       </tr>
      </td>';
     $id=$_GET['id'];
     $sql="SELECT * FROM News where newsID='".$id."'";
     $r=mysql_query($sql) or die("Can not grab from database".mysql_error());
     $row=mysql_fetch_object($r);
     $out.='<td width="100%" class="insidetable"><img src="images/submenus/news_txt.gif" width="40" height="13">
     <br><br>
       <b>'.$row->newsTitle.'</b><br><p>'.$row->newsContent.'</p></td></table>';

       $output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME);			
  		$output->output(); 
        $output->setPageID(98); //butonul selectat
}
?>