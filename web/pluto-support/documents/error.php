<?
$out='';
$dbADO->debug=false;


$out.='<b>'.@stripslashes($_REQUEST['msg']).'</b>';


$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		

?>