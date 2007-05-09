<?
function message_die($message,$action)
{
	global $email_notification,$email_additional;

	if($email_notification!=''){
		send_mail('<Error notification>', $email_notification, '<Webmaster>', $email_notification, 'Error notification', $email_additional.' '.$message);
	}
	switch ($action){
		case 0:
			// do nothing, run in quiet mode
		break;
		case 1:
			// die on error and display error message, used when running in browser
			die($message);
		break;
		case 2:
			// die on critical error and log error in file
			write_log($GLOBALS['error_log'],'Critical error: '.$message);
			die();
		break;
	}
}

// database functions
function db_connect($db_server, $db_port, $db_user, $db_pass, $db_database)
{
	$conn=@mysql_connect($db_server.":".$db_port, $db_user, $db_pass,true);
	if($conn){
		$isSelected=@mysql_select_db($db_database, $conn);
		if($isSelected)
			return $conn;
		else
			message_die("Could not select database ".$db_database,$GLOBALS['action_on_error']);
	}
	else{
		message_die('Could not connect to MySQL server.',$GLOBALS['action_on_error']);
	}
}

function query($query,$conn)
{
	if($GLOBALS['db_in_debug']==1){
		print('<hr>'.$query.'<hr>');
	}
	if (empty($query)) {
		return FALSE;
	}
	$result=mysql_query($query,$conn);
	if(!$result)
		message_die ("Database request failed: <ul>"."<li>Error no: ".mysql_errno($conn)."<li>Error: ".mysql_error($conn)."<li>SQL query: ".$query."</ul>\n",$GLOBALS['action_on_error']);
	else
		return $result;
}

// format date function from format (YYYY-mm-dd) to the one specified by the $param
function format_mysql_date($data_to_format,$param){
    $date_components =explode ("-", $data_to_format);
    if(count($date_components)!=3){
    	return '';
    }
    $myday=@$date_components[2];
    $mymonth=@$date_components[1];
    $myyear=@$date_components[0];

    $new_date=mktime (0,0,0, $mymonth, $myday, $myyear);

    $my_date=@date($param,$new_date);
    return $my_date;
}
// return timestamp in miliseconds
function getmicrotime()
{
   list($usec, $sec) = explode(" ", microtime());
   return ((float)$usec + (float)$sec);
}

function send_mail($fromname, $fromaddress, $toname, $toaddress, $subject, $message)
{
	$headers = "From: $fromname<$fromaddress>\n";
	$headers .= "X-Sender: <$fromaddress>\n";
	$headers .= "X-Mailer: PHP\n"; //mailer
	$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
	$headers .= "Return-Path: <$fromaddress>\n";
	$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
	$headers .= "cc: \n";
	$headers .= "bcc: ";

	return @mail($toaddress, $subject, $message, $headers);
}

// write text to error log
function write_log($logname,$message)
{
	if(@$GLOBALS['IN_DEBUG']==1){
		print ('<hr>'.$message.'</hr>');
	}
	if ($handle = @fopen($logname, 'a')) {
		@fwrite($handle, '['.date('d-m-Y H:i:s').'] '.$message."\r\n");
	}
	@fclose($handle);
}

// display an array nicely formated - debug purposes only
function print_array($arr){
	print '<pre>';
	print_r($arr);
	print '</pre>';
}

function getAssocArray($table,$keyField,$labelField,$conn,$whereClause='',$orderClause='')
{
	$retArray=array();
	$res=query("SELECT $keyField,$labelField FROM $table $whereClause $orderClause",$conn);
	while($row=mysql_fetch_assoc($res)){
		$retArray[$row[$keyField]]=$row[cleanField($labelField)];
	}
	return $retArray;
}

// generic function to extract rows from a table
function getFields($table,$where,$conn,$order='',$fields='*'){
	$res=query("SELECT $fields FROM $table $where $order",$conn);
	$data=array();
	while($row=mysql_fetch_assoc($res)){
		$data[]=$row;
	}
	return $data;
}

function getVariable($name,$conn){
	$fields=getFields('variables','WHERE name=\''.$name.'\'',$conn);
	if(count($fields)==0){
		return null;
	}

	return $fields[0]['value'];
} 

function pulldownFromArray($contentArray,$name,$selectedValue,$event='',$restricted=60)
{
	if(!is_array($contentArray))
		return '';	// error
	$out='<select name="'.$name.'" '.$event.'>';
	foreach ($contentArray AS $key=>$label){
		$title='';
		if($restricted!=0 && strlen($label)>$restricted){
			$title=$label;
			$label=substr($label,0,$restricted).' ...';
		}
		$out.='<option value="'.$key.'" '.(($key==$selectedValue)?'selected':'').' title="'.$title.'">'.$label.'</option>';
	}
	$out.='</select>';
	
	return $out;
}

function date_to_mysql($data,$separator){
	$parts=explode($separator,$data);
	if(count($parts)==1){
		return false;
	}
	
	return $parts[2].'-'.$parts[1].'-'.$parts[0];
}

// remove slashes if the option is enabled
function cleanString($str){
	
	if (!get_magic_quotes_gpc()) {
		$cleanStr = addslashes($str);
	} else {
   		$cleanStr= $str;
	}
	
	return $cleanStr;
}

function str_makerand ($minlength, $maxlength, $useupper, $usespecial, $usenumbers) 
{ 
/* 
Author: Peter Mugane Kionga-Kamau 
http://www.pmkmedia.com 

Description: string str_makerand(int $minlength, int $maxlength, bool $useupper, bool $usespecial, bool $usenumbers) 
returns a randomly generated string of length between $minlength and $maxlength inclusively. 

Notes: 
- If $useupper is true uppercase characters will be used; if false they will be excluded. 
- If $usespecial is true special characters will be used; if false they will be excluded. 
- If $usenumbers is true numerical characters will be used; if false they will be excluded. 
- If $minlength is equal to $maxlength a string of length $maxlength will be returned. 
- Not all special characters are included since they could cause parse errors with queries. 

Modify at will. 
*/ 
	$key='';
    $charset = "abcdefghijklmnopqrstuvwxyz"; 
    if ($useupper)   $charset .= "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
    if ($usenumbers) $charset .= "0123456789"; 
    if ($usespecial) $charset .= "~@#$%^*()_+-={}|][";   // Note: using all special characters this reads: "~!@#$%^&*()_+`-={}|\\]?[\":;'><,./"; 
    if ($minlength > $maxlength) $length = mt_rand ($maxlength, $minlength); 
    else                         $length = mt_rand ($minlength, $maxlength); 
    for ($i=0; $i<$length; $i++) $key .= $charset[(mt_rand(0,(strlen($charset)-1)))]; 
    return $key; 
}  

function set_variable($variables,$name,$value){
	$variables[$name]=$value;
	
	return $variables;
}

function outputHTML($variables,$page_template,$asVariable=0){
	// replace all variables from page with their values and output HTML page 
	foreach ($variables as $toReplace=>$value) {
		$page_template=str_replace('{'.$toReplace.'}',$value,$page_template);
	}
	
	// replace any other occurence of {something} with empty string to preserve compatibilty with other template systems
	$page_template=preg_replace('/{(.*?)}/','',$page_template);
	
	
	if($asVariable==0){
		print $page_template;
	}else{
		return $page_template;
	}
}

function create_thumb($image_filename,$output_filename,$width){
	require_once('phpThumb/phpthumb.class.php');	
	
	// create new phpThumb() object
	$phpThumb = new phpThumb();

	// set data
	$phpThumb->setSourceFilename($image_filename);

	// set parameters (see "URL Parameters" in phpthumb.readme.txt)
	$phpThumb->w = $width;

	// here you must preface each option with "config_"
	$phpThumb->config_output_format    = 'jpeg';

	// generate & output thumbnail
	if ($phpThumb->GenerateThumbnail()) { // this line is VERY important, do not remove it!
	if ($output_filename) {
		if ($phpThumb->RenderToFile($output_filename)) {
			// do something on success
			return true;
		} else {
			// do something with debug/error messages
			message_die('Resize for '.$image_filename.' failed.',1);
		}
	} else {
		$phpThumb->OutputThumbnail();
	}
	} else {
		// do something with debug/error messages
		die('Failed:<pre>'.implode("\n\n", $phpThumb->debugmessages).'</pre>');
		
		message_die('Resize for '.$image_filename.' failed.',1);
	}
}

function check_valid_email($mail){
	@list($userName, $mailDomain) =split("@",$mail); 
	if (!checkdnsrr($mailDomain, "MX")){
		return false;
	}
	
	return true;
}


// generic function to extract rows from a table
function get_fields($table,$where,$conn,$order='',$fields='*'){
	$res=query("SELECT $fields FROM $table $where $order",$conn);
	$data=array();
	while($row=mysql_fetch_assoc($res)){
		$data[]=$row;
	}
	return $data;
}

// convert xxx.yyy or aaa.bbb as ccc to yyy or ccc
function cleanField($field){
	$cleanField=(strpos($field,' AS ')!==false)?substr($field,strpos($field,' AS ')+4):$field;
	
	return $cleanField;
}

// generic multipage function
function multi_page($fieldsArray,$table,$where,$order,$url,$conn,$editURL='',$delURL='',$table_width='100%',$records_per_page=10,$extra_links_array=array()){
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$noFields=count($fieldsArray);
	$headRow='<td align="center">#</td>';
	foreach ($fieldsArray AS $tableField=>$tableLabel){
		if($tableLabel!='id' && $tableLabel!='menu_description' && $tableLabel!='menu_notes'){
			$headRow.='<td align="center"><b>'.$tableLabel.'</b></td>';	
		}
	}
	if($editURL!='' || $delURL!='' || count($extra_links_array)!=0){
		$headRow.='<td align="center"><b>Functions</b></td>';
	}
	$res=query("SELECT ".join(',',array_keys($fieldsArray))." FROM $table $where $order",$conn);
	
	$out='
	<table border="1" cellpadding="3" style="border-collapse: collapse" bordercolor="#111111" width="'.$table_width.'">';
	$pos=0;
	if(mysql_num_rows($res)==0){
		$NoRecordsRow='
		<tr>
			<td colspan="'.($noFields+2).'" class="normal_row">No records.</td>	
		</tr>
		';
	}else{
		if(isset($_GET['go'])){
			$page=1;
		}
		
		$totalRecords=mysql_num_rows($res);
		$noPages=ceil($totalRecords/$records_per_page);
		$start=($page-1)*$records_per_page+1;
		$end=$page*$records_per_page;
		$end=($end>$totalRecords)?$totalRecords:$end;
		
		$pageLinks='Pages: ';
		for($i=1;$i<=$noPages;$i++){
			$pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace(array('&page='.$page,'&go=Display'),'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
		}
		$out.='
		<span class="normal_row">Records '.$start.'-'.$end.' of '.$totalRecords.'</span>';
	}

	$out.='
		<tr class="head_row">
			'.$headRow.'
		</tr>'.@$NoRecordsRow;
	while($row=mysql_fetch_assoc($res)){
		$pos++;

		if($pos>=$start && $pos<=$end){
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';		
			
	
			$tableRow='<td '.((in_array('menu_description',$fieldsArray))?'rowspan="2"':'').'>'.$pos.'</td>';
			foreach ($fieldsArray AS $tableField=>$tableLabel){
				if($tableLabel!='id'){
					$value=nl2br($row[cleanField($tableField)]);
					
					// if field start with ff_ call dynamic function instead
					if(substr(cleanField($tableField),0,3)=='ff_'){
						$func=substr(cleanField($tableField),3);
						$value=$func($value,$conn,$row['id']);
					}					
					
					$tableRow.='<td>'.$value.'</td>';	
				}elseif($tableLabel=='id'){
					$primary_key=cleanField($tableField);
				}
			}
			
			$editLink=($editURL!='')?'<a href="index.php?section='.$editURL.'&id='.$row[$primary_key].'">Edit</a> ':'';
			$delLink=($delURL!='')?'<a href="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'index.php?section='.$delURL.'&did='.$row[$primary_key].'\'">Del</a> ':'';

			$extra_links='';
			if(count($extra_links_array)>0){
				foreach ($extra_links_array AS $extra_url=>$extra_label){
					$extra_links.='<a href="'.$extra_url.'&id='.$row[$primary_key].'">'.$extra_label.'</a> ';
				}
			}
			
			if($editURL!='' || $delURL!='' || count($extra_links_array)!=0){
				$tableRow.='<td align="center">'.@$extraLink.$editLink.$delLink.$extra_links.'</td>';
			}
			
			$out.='
			<tr class="normal_row" bgcolor="'.$color.'">
				'.$tableRow.'			
			</tr>';
		}
	}
	$out.='</table>
	<span class="normal_row">'.@$pageLinks.'</span>';
	
	return $out;

}
// ***********************************************************************************


?>