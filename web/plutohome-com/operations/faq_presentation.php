<?php
 function faq_presentation($output,$conn) {
 		$out = '';
 		$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
     <td width="30%" valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="30%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_of.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_on.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
       </tr>
      </td>
      <td width="70%" class="insidetable">';
		$selectFaqCateg = 'select faqCategID,faqCategName from FAQCategories order by faqCategName asc';
		$res = mysql_query($selectFaqCateg,$conn);
		while ($row=mysql_fetch_object($res)) {
 		$selectFaq = "select faqID,faqQuestion,faqAnswer,faqDate,faquserID,faqFAQCategID from FAQ where FAQ.markedDeleted!='1'  and faqFAQCategID='".$row->faqCategID."' order by faqDate asc";
	 	$rsFaq = mysql_query($selectFaq,$conn) or die("Can not grab faq from database!!");
      $out.='<table align="center" border="0" width=100% class="insidetable">
      <td align="left"><br><b>'.$row->faqCategName.'</b><hr size="1" color="#9BAEDB"></td></table><table  border="0" width=100% class="insidetable" >
      ';
			while ($rowFaq=mysql_fetch_object($rsFaq)) {
            
            if($rowFaq->faqAnswer != "" ) $answer="Yes";
            else $answer="No";
				$out.='<tr><td><img src="images/sagetzica.gif" width="19" height="17" align="middle"> '.$rowFaq->faqDate[4].$rowFaq->faqDate[5].'.'.$rowFaq->faqDate[6].$rowFaq->faqDate[7].'.'.$rowFaq->faqDate[0].$rowFaq->faqDate[1].$rowFaq->faqDate[2].$rowFaq->faqDate[3].'</td><td align="left" width="80%"><a href="index.php?section=faq_presentation&action='.$rowFaq->faqID.'"><b>'.$rowFaq->faqQuestion.'</b></a></td></tr>';
			}
      }
         /*$out.='</table><table align="center" border="0" width="100%" class="insidetable"><form name="form2" method="post" action="index.php?section=faq_presentation">
	            <input type="hidden" name="section" value="faq_presentation">
                <tr><td  align="left"><br><input type="submit" name="addfaq" value="Add New Faq" /></td></tr>
                 </form>';*/
         $out.='</table>';

         if(isset($_GET['action']))
         {
           $id=$_GET['action'];
           $sql="select faqID,faqQuestion,faqAnswer,faqDetails,faqDate from FAQ where faqID='".$id."'";
           $r=mysql_query($sql,$conn) or die("Can not grab from database".mysql_error());
           $row=mysql_fetch_object($r);
           $out='';
           $out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"> <td width="30%" valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="30%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_of.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_on.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
       </tr>
      </td><td class="insidetable" valign="top">';
           $out.='<table align="center"><tr><td align="left"><b>'.$row->faqQuestion.'</b></td></tr>
                  <tr><td align="left">'.$row->faqDetails.'</td></tr>';
           if($row->faqAnswer!='') $out.='<tr><td align="left"><hr>'.$row->faqAnswer.'</td></tr></table>';
           else $out.='<tr><td align="center"><hr> No answer!</td></tr></table>';
         }
         $out.='</td></table>';

		//}
		
 		
 		$output->setNavigationMenu(array("FAQ"=>"index.php?section=faqPresentation"));
 		
		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME."::FAQ");			
  		$output->output(); 
 }
?>