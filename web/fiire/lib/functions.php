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

function pulldownFromArray($contentArray,$name,$selectedValue,$event='',$restricted=20)
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
function multi_page($fieldsArray,$table,$where,$order,$url,$page,$conn,$editURL='',$delURL='',$table_width='100%',$extra_links_array=array()){
	$noFields=count($fieldsArray);
	$headRow='<td align="center">#</td>';
	foreach ($fieldsArray AS $tableField=>$tableLabel){
		if($tableLabel!='id' && $tableLabel!='menu_description' && $tableLabel!='menu_notes'){
			$headRow.='<td align="center"><b>'.$tableLabel.'</b></td>';	
		}
	}
	$headRow.='<td align="center"><b>Functions</b></td>';
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
		$totalRecords=mysql_num_rows($res);
		$noPages=ceil($totalRecords/$GLOBALS['records_per_page']);
		$start=($page-1)*$GLOBALS['records_per_page']+1;
		$end=$page*$GLOBALS['records_per_page'];
		$end=($end>$totalRecords)?$totalRecords:$end;
		
		$pageLinks='Pages: ';
		for($i=1;$i<=$noPages;$i++){
			$pageLinks.=($i==$page)?' '.$i:' <a href="index.php?section='.$url.'&page='.$i.'">'.$i.'</a>';
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
					$tableRow.='<td>'.$row[cleanField($tableField)].'</td>';	
				}elseif($tableLabel=='id'){
					$primary_key=cleanField($tableField);
				}
			}
			
			$editLink=($editURL!='')?'<a href="index.php?section='.$editURL.'&id='.$row[$primary_key].'">Edit</a>':'';
			$delLink=($delURL!='')?'<a href="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'index.php?section='.$delURL.'&did='.$row[$primary_key].'\'">Del</a>':'';
			$extraLink=($url=='lista_joburi')?'<a href="index.php?section=cv&job='.$row['id'].'">CV-uri</a> ':'';

			$extra_links='';
			if(count($extra_links_array)>0){
				foreach ($extra_links_array AS $extra_url=>$extra_label){
					$extra_links.='<a href="'.$extra_url.'&id='.$row[$primary_key].'">'.$extra_label.'</a> ';
				}
			}
			
			$tableRow.='<td align="center">'.@$extraLink.$editLink.'&nbsp;'.$delLink.'&nbsp;'.$extra_links.'</td>';
			
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


// -------------- login function --------------
function processLogin(){
	$username=$_POST['username'];
	$password=md5($_POST['password']);

	$conn=db_connect($GLOBALS['db_server'], $GLOBALS['db_port'], $GLOBALS['db_user'], $GLOBALS['db_pass'], $GLOBALS['db_database']);
	$res=query("SELECT * FROM admin WHERE username='$username'",$conn);
	if(mysql_num_rows($res)>0){
		$row=mysql_fetch_assoc($res);
		if($row['password']==$password){
			$_SESSION['adminid']=$row['id'];
			$_SESSION['ausername']=$row['username'];
			
			return true;
		}else{
			return 'Wrong password.';
		}
	}else{		
		return 'Username not found.';
	}
}

function loginForm(){
	$out='
	<div align="center">
	<form name="log" method="POST" action="login.php">
		<table>
			<tr>
				<td valign="top" colspan="2"><br>Please login</td>
			</tr>
			<tr>
				<td valign="top">Username</td>
				<td valign="top"><input type="text" name="username" value=""></td>
			</tr>
			<tr>
				<td valign="top">Password</td>
				<td valign="top"><input type="password" name="password" value=""></td>
			</tr>
			<tr>
				<td valign="top" colspan="2"><input type="submit" name="login" value="Login"></td>
			</tr>
		</table>
	</form>
	</div>
	';
	
	return $out;
}
// -------------- end login function --------------


// -------------- admin menu function --------------
function display_content($section,$conn){
	switch ($section){
		case 'edit_section':
			if(isset($_POST['update'])){
				$out=process_edit_section($conn);
			}else{
				$out=edit_section((int)@$_REQUEST['id'],$conn);
			}
		break;
		case 'news':
			$out=news($conn);
		break;
		case 'add_news':
			if(isset($_POST['add'])){
				$out=process_add_news($conn);
			}else{
				$out=add_news($conn);
			}
		break;		
		case 'edit_news':
			if(isset($_POST['update'])){
				$out=process_edit_news($conn);
			}else{
				$out=edit_news((int)@$_REQUEST['id'],$conn);
			}
		break;	
		case 'del_news':
			$out=del_news((int)@$_REQUEST['did'],$conn);
		break;
		case 'edit_variable':
			if(isset($_POST['update'])){
				$out=process_edit_variable($conn);
			}else{
				$out=edit_variable(@$_REQUEST['name'],$conn);
			}
		break;
		case 'add_screenshot':
			if(isset($_POST['add'])){
				$out=process_add_screenshot($conn);
			}else{
				$out=add_screenshot($conn);
			}
		break;		
		case 'screenshots':
			$out=screenshots($conn);
		break;
		case 'del_screenshot':
			$out=del_screenshot((int)@$_REQUEST['id'],$conn);
		break;
		case 'edit_screenshot':
			if(isset($_POST['update'])){
				$out=process_edit_screenshot($conn);
			}else{
				$out=edit_screenshot((int)@$_REQUEST['id'],$conn);
			}
		break;
		case 'pending_comments':
			$out=pending_comments($conn);
		break;
		case 'del_comment':
			$out=del_comment((int)@$_REQUEST['did'],$conn);
		break;
		case 'edit_comment':
			if(isset($_POST['update'])){
				$out=process_edit_comment($conn);
			}else{
				$out=edit_comment((int)@$_REQUEST['id'],$conn);
			}
		break;
		case 'users':
			$out=users($conn);
		break;
		case 'del_user':
			$out=del_user((int)@$_REQUEST['did'],$conn);
		break;
		case 'validate_user':
			$out=validate_user((int)@$_REQUEST['id'],$conn);
		break;
		case 'logout':
			$out=admin_logout($conn);
		break;
				
		default:
			$out='';
		break;
	}
	
	return $out;
}

function admin_logout($conn){
	unset($_SESSION['adminid']);
	unset($_SESSION['ausername']);
	
	header('Location: login.php');
	exit();
}

function admin_menu(){
	$out='
	Welcome '.@$_SESSION['ausername'].' <a href="index.php?section=logout">[ Logout ]</a>
	<fieldset><legend><b>Features</b></legend>
	- <a href="index.php?section=edit_section&id=1">Edit features</a><br>
	</fieldset>

	<fieldset><legend><b>Screenshots</b><br></legend>
	- <a href="index.php?section=screenshots">View screenshots</a><br>
	- <a href="index.php?section=add_screenshot">Add screenshot</a><br>
	</fieldset>
	
	<fieldset><legend><b>News</b><br><legend>
	- <a href="index.php?section=news">View news</a><br>
	- <a href="index.php?section=add_news">Add news</a><br>
	- <a href="index.php?section=pending_comments">Unvalidated comments</a><br>
	</fieldset>
	
	<fieldset><legend><b>MCE vs MS Media Center</b><br></legend>
	- <a href="index.php?section=edit_section&id=2&full=1">Edit content</a><br>
	</fieldset>
	
	<fieldset><legend><b>Product stats</b><br></legend>
	- <a href="index.php?section=edit_variable&name=version">Version</a><br>
	- <a href="index.php?section=edit_variable&name=developers">Developers</a><br>
	- <a href="index.php?section=edit_variable&name=downloads">Downloads</a><br>
	</fieldset>
	

	';
	
	return $out;
}

// initialise javascript for wysiwyg editor
function get_tiny_mce(){
	$out='
<!-- tinyMCE -->
<script language="javascript" type="text/javascript" src="tiny_mce/tiny_mce.js"></script>
<script language="javascript" type="text/javascript">
	// Notice: The simple theme does not use all options some of them are limited to the advanced theme
	tinyMCE.init({
		mode : "textareas",
		theme : "simple"
	});
</script>
<!-- /tinyMCE -->	
	';
	
	return $out;
}

function edit_section($id,$conn){
	if($id==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
	
	$fields=get_fields('sections','WHERE id='.$id,$conn);
	if(count($fields)==0){
		return '<span class="error">ERROR: Section not found.</span>';
	}
	
	$out=get_tiny_mce();
	$out.='
	<form name="admin" action="index.php" method="POST">
		<input type="hidden" name="section" value="edit_section">
		<input type="hidden" name="id" value="'.$id.'">
	
		<table>
		<tr>
			<td valign="top"><B>Edit section</B></td>
		</tr>	
		<tr>
			<td valign="top">Summary<br><textarea name="summary" style="width:400px;height:150px;">'.$fields[0]['summary'].'</textarea></td>
		</tr>';
		if(isset($_REQUEST['full'])){
			$out.='
		<tr>
			<td valign="top">Content<br><textarea name="content" style="width:400px;height:250px;">'.$fields[0]['content'].'</textarea></td>
		</tr>';
		}
		$out.='
		<tr>
			<td valign="top"><input type="submit" name="update" value="Update"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function process_edit_section($conn){
	$id=(int)@$_POST['id'];
	$summary=cleanString($_POST['summary']);
	
	if(isset($_POST['content'])){
		$content=cleanString($_POST['content']);
		$update_content=", content='$content'";
	}else{
		$update_content='';
	}

	if($summary==''){
		return '<span class="error">ERROR: All fields are required.</span>';
	}
	
	query("UPDATE sections SET summary='$summary' $update_content WHERE id=$id",$conn);
	
	return '<span class="confirm">The content was updated.</span>';
}

function news($conn){
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$fieldsArray=array(
		'id'=>'id',
		'ndate'=>'Date',
		'title'=>'Title'
	);
	$out='<p class="normal_row"><b>News</b><br>
	'.multi_page($fieldsArray,'news','','ORDER BY ndate DESC,title ASC','news',$page,$conn,'edit_news','del_news');

	return $out;
}


function add_news($conn){
	
	$out=get_tiny_mce();
	$out.='
	<form name="admin" action="index.php" method="POST">
		<input type="hidden" name="section" value="add_news">
		<table>
		<tr>
			<td valign="top"><B>Add news</B></td>
		</tr>	
		<tr>
			<td valign="top">Title<br>
				<input type="text" name="title" value="">
			</td>
		</tr>
		<tr>
			<td valign="top">Date<br>
				<input type="text" name="ndate" value="'.date('Y-m-d').'">
			</td>
		</tr>	
		<tr>
			<td valign="top">Summary<br>
				<textarea name="summary" style="width:400px;height:100px;"></textarea>
			</td>
		</tr>
		<tr>
			<td valign="top">Full news<br>
				<textarea name="body" style="width:400px;height:250px;"></textarea>
			</td>
		</tr>	
		<tr>
			<td valign="top"><input type="submit" name="add" value="Add"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function process_add_news($conn){

	$title=cleanString($_POST['title']);
	$summary=cleanString($_POST['summary']);
	$body=cleanString($_POST['body']);
	$ndate=cleanString($_POST['ndate']);
	
	if($title=='' || $summary=='' || $body==''){
		return '<span class="error">ERROR: All fields are required.</span>';
	}
	
	query("INSERT INTO news (title, summary, body, ndate) VALUES ('$title','$summary','$body','$ndate')",$conn);
	
	return '<span class="confirm">The news was added.</span>';
}

function edit_news($id,$conn){
	if($id==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
	
	$fields=get_fields('news','WHERE id='.$id,$conn);
	if(count($fields)==0){
		return '<span class="error">ERROR: Section not found.</span>';
	}
		
	$out=get_tiny_mce();
	$out.='
	<form name="admin" action="index.php" method="POST">
		<input type="hidden" name="section" value="edit_news">
		<input type="hidden" name="id" value="'.$id.'">
	
		<table>
		<tr>
			<td valign="top"><B>Edit news #'.$id.'</B></td>
		</tr>	
		<tr>
			<td valign="top">Title<br>
				<input type="text" name="title" value="'.$fields[0]['title'].'">
			</td>
		</tr>
		<tr>
			<td valign="top">Date<br>
				<input type="text" name="ndate" value="'.$fields[0]['ndate'].'">
			</td>
		</tr>	
		<tr>
			<td valign="top">Summary<br>
				<textarea name="summary" style="width:400px;height:100px;">'.$fields[0]['summary'].'</textarea>
			</td>
		</tr>
		<tr>
			<td valign="top">Full news<br>
				<textarea name="body" style="width:400px;height:250px;">'.$fields[0]['body'].'</textarea>
			</td>
		</tr>	
		<tr>
			<td valign="top"><input type="submit" name="update" value="Update"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function process_edit_news($conn){
	$id=(int)@$_POST['id'];
	$title=cleanString($_POST['title']);
	$summary=cleanString($_POST['summary']);
	$body=cleanString($_POST['body']);
	$ndate=cleanString($_POST['ndate']);

	if($title=='' || $summary=='' || $body==''){
		return '<span class="error">ERROR: All fields are required.</span>';
	}
	
	query("UPDATE news SET title='$title', summary='$summary', body='$body', ndate='$ndate' WHERE id=$id",$conn);
	
	return '<span class="confirm">The news was updated.</span>';
}

function del_news($did,$conn)
{
	if($did==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
			
	$res=query("DELETE FROM news WHERE id='$did'",$conn);
	$out='The news was deleted.<br>
	<a href="index.php?section=news">Back to news</a>';
	
	return $out;
}

function pending_comments($conn){
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$fieldsArray=array(
		'comments.id AS id'=>'id',
		'cdate'=>'Date',
		'news.title AS ntitle'=>'News',
		'users.name AS user'=>'Posted by'
	);

	$out='<p class="normal_row"><b>Unvalidated comments</b><br>
	'.multi_page($fieldsArray,'comments','INNER JOIN news ON news_id=news.id INNER JOIN users on user_id=users.id WHERE valid=0','ORDER BY cdate DESC,title ASC','pending_comments',$page,$conn,'edit_comment','del_comment');

	return $out;
}

function edit_comment($id,$conn){
	if($id==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
	
	$fields=get_fields('comments','INNER JOIN users ON user_id=users.id INNER JOIN news on news_id=news.id WHERE comments.id='.$id,$conn,'','comments.id AS id, users.name AS name, news.title AS news_title,news_id,comment,cdate,valid');
	if(count($fields)==0){
		return '<span class="error">ERROR: Section not found.</span>';
	}
		
	$out='
	<form name="admin" action="index.php" method="POST">
		<input type="hidden" name="section" value="edit_comment">
		<input type="hidden" name="id" value="'.$id.'">
	
		<table>
		<tr>
			<td valign="top"><B>Edit comment #'.$id.'</B></td>
		</tr>	
		<tr>
			<td valign="top">News: <a href="index.php?section=edit_news&id='.$fields[0]['news_id'].'">'.$fields[0]['news_title'].'</a></td>
		</tr>
		<tr>
			<td valign="top">Comment date: '.$fields[0]['cdate'].'</td>
		</tr>	
		<tr>
			<td valign="top">Comment<br>
				<textarea name="comment" style="width:400px;height:100px;">'.$fields[0]['comment'].'</textarea>
			</td>
		</tr>
		<tr>
			<td valign="top"><input type="checkbox" name="valid" value="1" '.(($fields[0]['valid']==1)?'checked':'').'> Validate</td>
		</tr>	
		<tr>
			<td valign="top"><input type="submit" name="update" value="Update"> <input type="button" name="del" value="Delete" onClick="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'index.php?section=del_comment&did='.$id.'\'"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function process_edit_comment($conn){
	$id=(int)@$_POST['id'];
	$valid=(int)@$_POST['valid'];
	$comment=cleanString($_POST['comment']);
	
	query("UPDATE comments SET comment='$comment',valid=$valid WHERE id=$id",$conn);
	
	return 'The comment was updated.<br>Back to <a href="index.php?section=pending_comments">unvalidated comments</a>';
}

function del_comment($did,$conn)
{
	if($did==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
			
	$res=query("DELETE FROM comments WHERE id='$did'",$conn);
	$out='The comment was deleted.<br>
	<a href="index.php?section=pending_comments">Back to unvalidated comments</a>';
	
	return $out;
}

function edit_variable($name,$conn){
	if($name==''){
		return '<span class="error">ERROR: variable name not specified.</span>';
	}
	
	$fields=get_fields('variables','WHERE name=\''.$name.'\'',$conn);
	if(count($fields)==0){
		return '<span class="error">ERROR: Variable not found.</span>';
	}
		
	$out='
	<form name="admin" action="index.php" method="POST">
		<input type="hidden" name="section" value="edit_variable">
		<input type="hidden" name="name" value="'.$name.'">
	
		<table width="400">
		<tr>
			<td valign="top" colspan="2"><B>Edit variable</B></td>
		</tr>	
		<tr>
			<td><B>'.ucfirst($name).'</B></td>
			<td><input type="text" name="value" value="'.$fields[0]['value'].'"></td>
		</tr>	
		<tr>
			<td valign="top" colspan="2">'.nl2br($fields[0]['comment']).'</td>
		</tr>	
	
		<tr>
			<td valign="top"><input type="submit" name="update" value="Update"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function process_edit_variable($conn){
	$name=cleanString($_POST['name']);
	$value=cleanString($_POST['value']);
	
	query("UPDATE variables SET value='$value' WHERE name='$name'",$conn);
	
	return 'The variable was updated.';
}





// -------------- end admin menu function --------------

// -------------- front end functions --------------

function save_cookie($reset=0){
	// set cookie
	$cookie='cuserid='.@$_SESSION['uID'].'&cstr='.@$_SESSION['cookie_str'];
	$time=($reset==0)?time()+3600*24*365:time()-3600*24*365;
	$cookieIsSet=setcookie("FiireCookie",$cookie,$time,"/", false); 
}

function do_cookie_autologin($conn){

	if(!isset($_SESSION['uID'])){
		if(isset($_COOKIE['FiireCookie'])){
			parse_str($_COOKIE['FiireCookie']);
			$res=query("SELECT * FROM users WHERE id=".(int)$cuserid." AND cookie_str='$cstr'",$conn);
			if(mysql_num_rows($res)>0){
				$row=mysql_fetch_assoc($res);
	
				$_SESSION['uID']=$row['id'];
				$_SESSION['username']=$row['username'];
				$_SESSION['cookie_str']=$cstr;
			}
		}
	}
}

function register($conn){
	if(isset($_POST['register'])){
		return process_register($conn);
	}
	return register_form();
}

function register_form($regusername='',$referrer='',$error=''){

	$variables=array();
	
	
	$title='FIIRE registration page';
	
	// main content
	$page_template=implode('',file('templates/register_form.html'));
	
	// set dynamic fields
	$variables=set_variable($variables,'regusername',$regusername);
	$variables=set_variable($variables,'referrer',$referrer);
	$variables=set_variable($variables,'error',$error);
	
	return outputHTML($variables,$page_template,1);	
}

function process_register($conn){
	$regusername=trim(cleanString($_POST['regusername']));
	$regpassword1=trim(cleanString($_POST['regpassword1']));
	$regpassword2=trim(cleanString($_POST['regpassword2']));
	$referrerName=trim(cleanString($_POST['referrer']));
	$comments=cleanString($_POST['comments']);
	$publicUser=(int)@$_POST['publicUser'];
	$publicComments=(int)@$_POST['publicComments'];
	
	// check if any field is empty
	if($regusername=='' || $regpassword1==''){
		$error='All fields are required.';
		return register_form($regusername,$referrerName,$error);
	}
	
	
	// check if the username is available
	$nameData=getAssocArray('users','id','name',$conn,'WHERE username LIKE \''.$regusername.'\'');
	if(count($nameData)>0){
		$error='This username is already used.';
		return register_form($regusername,$referrerName,$error);
	}

	// check if the password is the same
	if($regpassword1!=$regpassword2){
		$error='Passwords doesn\'t match.';
		return register_form($regusername,$referrerName,$error);
	}
	
	$regpassword=md5($regpassword1);
	$unique=md5(microtime());
	$referrer=get_user_id($referrerName,$conn);
	
	$res=query("INSERT INTO users (username, password, uniqueid,regdate,referrer_id,activated,comments,publicUser,publicComments) VALUES ('$regusername','$regpassword','$unique',NOW(),'$referrer',1,'$comments',$publicUser,$publicComments)",$conn);
	if(mysql_affected_rows($conn)==0){
		$error='Registration failed due a database error.An email was dispached to a site admin, please try again later.  We are sorry for the inconvenience.';
		// todo: mail to admin
		
		return register_form($regusername,$referrerName,$error);
	}else{
		// login user
		do_login($regusername,$regpassword1,0,$conn);
		
		return 'Thank you for registration. <br><a href="myaccount.php">Proceed to your account</a>';
	}
}

function users($conn){
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$fieldsArray=array(
		'id AS id'=>'id',
		'username'=>'Username',
		'name'=>'Name',
		'email'=>'Email',
		'if(activated=1,\'Yes\',\'No\') AS enabled'=>'Enabled'
	);
	$out='<p class="normal_row"><b>Users</b><br>
	'.multi_page($fieldsArray,'users','','ORDER BY activated ASC, regdate DESC','users',$page,$conn,'','del_user','100%',array('index.php?section=validate_user'=>'Validate'));

	return $out;
}

function del_user($did,$conn)
{
	if($did==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
			
	$res=query("DELETE FROM users WHERE id='$did'",$conn);
	$out='The user was deleted.<br>
	<a href="index.php?section=users">Back to users</a>';
	
	return $out;
}

function validate_user($id,$conn){
	query("UPDATE users SET activated=1 WHERE id=$id",$conn);
	
	return 'The user was validated.<br>Back to <a href="index.php?section=users">users</a>';
}







// -------------- end front end functions --------------

function getSection($sID,$conn,$summary=0){
	$fields=getFields('sections','WHERE id=\''.$sID.'\'',$conn);
	if(count($fields)==0){
		return null;
	}

	return ($summary==1)?$fields[0]['summary']:$fields[0]['content'];
} 

function login_header($conn){
	if(isset($_SESSION['uID'])){
		return logout_form($conn);
	}else{
		return get_login_form();
	}
}


function get_login_form(){
	$variables=array();
	$page_template=implode('',file('templates/login_form.html'));
	
	if(isset($_SESSION['login_error'])){
		$variables=set_variable($variables,'login_error',$_SESSION['login_error']);
		unset($_SESSION['login_error']);
	}

	return outputHTML($variables,$page_template,1);	
}



function setVariable($name,$value,$conn){
	
	query("UPDATE variables SET value='$value' WHERE name='$name'",$conn);
	
	return 'The variable was updated.';
}

function do_login($user,$pass,$remember,$conn){
	$res=query("SELECT * FROM users WHERE username='$user' AND password='".md5($pass)."'",$conn);
	if(mysql_num_rows($res)==0){
		$_SESSION['login_error']='Pasword doesn\'t match or username does not exist.';
	}else{
		$row=mysql_fetch_assoc($res);

		$_SESSION['uID']=$row['id'];
		$_SESSION['username']=$row['username'];
		$_SESSION['cookie_str']=md5(str_makerand(10,10,1,0,1));
		query("UPDATE users SET cookie_str='".$_SESSION['cookie_str']."' WHERE id=".$row['id'],$conn);
		
		if($remember==1){
			save_cookie(0);
		}
	}
}

function logout_form($conn){
	$variables=array();
	$page_template=implode('',file('templates/logout_form.html'));
	$variables=set_variable($variables,'uname',$_SESSION['username']);
	
	return outputHTML($variables,$page_template,1);	
	
}

function do_logout(){
	unset($_SESSION['uID']);
	unset($_SESSION['username']);
	save_cookie(1);
}

function get_main(){
	$variables=array();
	$page_template=implode('',file('templates/main.html'));
	$variables=set_variable($variables,'shop_fiire_engine',polywell_redirect($GLOBALS['fiire_engine_url']));
	$variables=set_variable($variables,'shop_fiire_station',polywell_redirect($GLOBALS['fiire_station_url']));
	$variables=set_variable($variables,'shop_fiire_chief',polywell_redirect($GLOBALS['fiire_chief_url']));
	
	return outputHTML($variables,$page_template,1);		
}

function get_myaccount($conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to access your account.';
	}	
	
	if(isset($_POST['update'])){
		$message=process_update_account($conn);
	}
	
	$referrals=get_referrals_summary_array($_SESSION['uID'],$conn);
	$udata=getFields('users','LEFT JOIN users ref ON users.referrer_id=ref.id WHERE users.id='.$_SESSION['uID'],$conn,'','users.*,ref.username AS referrer');

	$variables=array();
	$page_template=implode('',file('templates/myaccount.html'));

	$variables=set_variable($variables,'referrals_level_1',$referrals[1]);
	$variables=set_variable($variables,'referrals_level_2',$referrals[2]);
	$variables=set_variable($variables,'referrals_level_3',$referrals[3]);
	$variables=set_variable($variables,'uname',$_SESSION['username']);
	$variables=set_variable($variables,'commisionsTotal',$udata[0]['commisionsTotal']);
	$variables=set_variable($variables,'commisionsBlocked',$udata[0]['commisionsBlocked']);
	$variables=set_variable($variables,'commisionsAvailable',$udata[0]['commisionsAvailable']);
	$variables=set_variable($variables,'referred_by',((!is_null($udata[0]['referrer']))?$udata[0]['referrer']:'none'));
	$variables=set_variable($variables,'publicUserChecked',(($udata[0]['publicUser']==1)?'checked':''));
	$variables=set_variable($variables,'publicCommentsChecked',(($udata[0]['publicComments']==1)?'checked':''));
	$variables=set_variable($variables,'polywell_email',$udata[0]['polywell_email']);
	$variables=set_variable($variables,'comments',$udata[0]['comments']);
	$variables=set_variable($variables,'support_requests',get_support_request($conn));	
	$variables=set_variable($variables,'orders_history',get_orders_history($conn));	
	$variables=set_variable($variables,'message',@$message);	
	
	return outputHTML($variables,$page_template,1);		
}

function get_company(){
	$variables=array();
	$page_template=implode('',file('templates/company.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_sure_fiire(){
	$variables=array();
	$page_template=implode('',file('templates/sure_fiire.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_fiire_is_hiring(){
	$variables=array();
	$page_template=implode('',file('templates/fiire-is-hiring.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_fiire_up_the_press(){
	$variables=array();
	$page_template=implode('',file('templates/fiire-up-the-press.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_open_fiire(){
	$variables=array();
	$page_template=implode('',file('templates/open_fiire.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_get_fiired(){
	$variables=array();
	$page_template=implode('',file('templates/get-fiired.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_products($conn){
	$variables=array();
	$page_template=implode('',file('templates/products.html'));
	$variables=set_variable($variables,'shop_fiire_engine',polywell_redirect($GLOBALS['fiire_engine_url']));
	$variables=set_variable($variables,'shop_fiire_station',polywell_redirect($GLOBALS['fiire_station_url']));
	$variables=set_variable($variables,'shop_fiire_chief',polywell_redirect($GLOBALS['fiire_chief_url']));	

	return outputHTML($variables,$page_template,1);		
}

function get_fiire_engine($conn){
	$variables=array();
	$page_template=implode('',file('templates/fiire-engine.html'));
	$variables=set_variable($variables,'shop_fiire_engine',polywell_redirect($GLOBALS['fiire_engine_url']));

	return outputHTML($variables,$page_template,1);		
}

function get_fiire_station($conn){
	$variables=array();
	$page_template=implode('',file('templates/fiire-station.html'));
	$variables=set_variable($variables,'shop_fiire_station',polywell_redirect($GLOBALS['fiire_station_url']));

	return outputHTML($variables,$page_template,1);		
}

function get_fiire_chief($conn){
	$variables=array();
	$page_template=implode('',file('templates/fiire-chief.html'));
	$variables=set_variable($variables,'shop_fiire_chief',polywell_redirect($GLOBALS['fiire_chief_url']));

	return outputHTML($variables,$page_template,1);		
}

function get_features(){
	$variables=array();
	$page_template=implode('',file('templates/features.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_support(){
	$variables=array();
	$page_template=implode('',file('templates/support.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_downloads(){
	$variables=array();
	$page_template=implode('',file('templates/downloads.html'));
	

	return outputHTML($variables,$page_template,1);		
}

function get_user_id($username,$conn){
	$res=query("SELECt id from users WHERE username='$username'",$conn);
	
	// return null as string to be used in sql queries
	if(mysql_num_rows($res)==0){
		return 'null';
	}else{
		$row=mysql_fetch_assoc($res);
		return $row['id'];
	}
}

function get_referrals_summary_array($uID,$conn){
	$rArray=array(
		1=>0,
		2=>0,
		3=>0
	);
	$parents=array($uID);
	foreach ($rArray AS $pos=>$count){
		$referrals=array();
		if(count($parents)!=0){
			$referrals=getAssocArray('users','id','username',$conn,'WHERE referrer_id IN ('.join(',',$parents).')');
		}
		$parents=array_keys($referrals);
		$rArray[$pos]=count($referrals);
	}
	
	return $rArray;
}

function get_support_request($conn){
	// TODO
	return 'No support requests.';
}

function get_orders_history($conn){
	// TODO
	return 'No orders yet.';
}


function process_update_account($conn){
	$id=$_SESSION['uID'];
	$old_password=trim(cleanString($_POST['old_password']));
	$password1=trim(cleanString($_POST['password1']));
	$password2=trim(cleanString($_POST['password2']));
	$comments=cleanString($_POST['comments']);
	$publicUser=(int)@$_POST['publicUser'];
	$publicComments=(int)@$_POST['publicComments'];
	$pemail=cleanString($_POST['pemail']);
//$GLOBALS['db_in_debug']=1;
	$res=query("UPDATE users SET comments='$comments',publicUser=$publicUser,publicComments=$publicComments,polywell_email='$pemail' WHERE id=$id",$conn);
	$message=msg_notice('Account info updated.');
	
	if($old_password!=''){
		$res=query("SELECT id FROM users WHERE id=$id AND password='".md5($old_password)."'",$conn);
		if(mysql_num_rows($res)==0){
			return msg_error('Old password doesn\'t match.');
		}
		
		if($password1==''){
			return msg_error('Password cannot be blank.');
		}
		
		if($password1!=$password2){
			return msg_error('Confirmed password doesn\'t match.');
		}else{
			$password=md5($password1);	
			$res=query("UPDATE users SET password='$password' WHERE id=$id",$conn);
			$message=msg_notice('Password updated.');
		}
	}
	
	return $message;
}

function msg_error($str){
	return '<span class="err">'.$str.'</span>';
}

function msg_notice($str){
	return '<span class="notice">'.$str.'</span>';
}

function polywell_redirect($url){
	return 'polywell-redirect.php?go='.$url;
}

function get_polywell_redirect($url){
	if($url==''){
		return msg_error('Redirect parameter not specified. Aborting ...');
	}
	
	$variables=array();
	$page_template=implode('',file('templates/polywell-redirect.html'));
	$variables=set_variable($variables,'url',$url);

	return outputHTML($variables,$page_template,1);		
}

function nav_menu($navArray){
	$out='<div class="nav_menu">';
	$pos=0;
	$max=count($navArray);
	foreach ($navArray AS $url=>$label){
		$pos++;
		if($pos!=$max){
			$out.='<a href="'.$url.'">'.$label.'</a> &gt; ';
		}else{
			$out.=$label;
		}
	}
	$out.='</div>';
	
	return $out;
}

function get_my_commisions($conn){
	$variables=array();
	$page_template=implode('',file('templates/mycommisions.html'));

	return outputHTML($variables,$page_template,1);		
}

function get_accesories($conn){
	$variables=array();
	$page_template=implode('',file('templates/accesories.html'));

	return outputHTML($variables,$page_template,1);		
}
?>