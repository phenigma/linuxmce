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

function pulldownFromArray($contentArray,$name,$selectedValue,$event='',$restricted=50)
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
		'Users.name AS user'=>'Posted by'
	);

	$out='<p class="normal_row"><b>Unvalidated comments</b><br>
	'.multi_page($fieldsArray,'comments','INNER JOIN news ON news_id=news.id INNER JOIN Users on user_id=Users.id WHERE valid=0','ORDER BY cdate DESC,title ASC','pending_comments',$page,$conn,'edit_comment','del_comment');

	return $out;
}

function edit_comment($id,$conn){
	if($id==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
	
	$fields=get_fields('comments','INNER JOIN Users ON user_id=Users.id INNER JOIN news on news_id=news.id WHERE comments.id='.$id,$conn,'','comments.id AS id, Users.name AS name, news.title AS news_title,news_id,comment,cdate,valid');
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
			$res=query("SELECT * FROM Users WHERE id=".(int)$cuserid." AND cookie_str='$cstr'",$conn);
			if(mysql_num_rows($res)>0){
				$row=mysql_fetch_assoc($res);
	
				$_SESSION['uID']=$row['id'];
				$_SESSION['username']=$row['username'];
				$_SESSION['uemail']=$row['email'];
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

function register_form($regusername='',$regemail='',$referrer='',$error=''){

	$variables=array();
	
	
	$title='FIIRE registration page';
	
	// main content
	$page_template=implode('',file('templates/register_form.html'));
	
	// set dynamic fields
	$variables=set_variable($variables,'regusername',$regusername);
	$variables=set_variable($variables,'regemail',$regemail);
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
	$regemail=trim(cleanString($_POST['regemail']));
	
	// check if any field is empty
	if($regusername=='' || $regpassword1=='' || $regemail==''){
		$error='All fields are required.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}
	
	// check if the email is not already registered
	$emailData=getAssocArray('Users','id','name',$conn,'WHERE email LIKE \''.$regemail.'\'');
	if(count($emailData)>0){
		$error='This email address is already registered.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}
	
	// check if the email address is valid
	if(!check_valid_email($regemail)){
		$error='This email address is invalid.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}
		
	// check if the username is available
	$nameData=getAssocArray('Users','id','name',$conn,'WHERE username LIKE \''.$regusername.'\'');
	if(count($nameData)>0){
		$error='This username is already used.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}

	// check if the password is the same
	if($regpassword1!=$regpassword2){
		$error='Passwords doesn\'t match.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}
	
	$regpassword=md5($regpassword1);
	$unique=md5(microtime());
	$referrer=get_user_id($referrerName,$conn);
	
	// check if referrer name is valid
	if($referrerName!='' && $referrer=='null'){
		$error='Invalid referrer.';
		return register_form($regusername,$regemail,$referrerName,$error);
	}

	
	$res=query("INSERT INTO Users (username, password, uniqueid,regdate,referrer_id,activated,comments,publicUser,publicComments,email) VALUES ('$regusername','$regpassword','$unique',NOW(),'$referrer',1,'$comments',$publicUser,$publicComments,'$regemail')",$conn);
	if(mysql_affected_rows($conn)==0){
		$error='Registration failed due a database error.An email was dispached to a site admin, please try again later.  We are sorry for the inconvenience.';
		// todo: mail to admin
		
		return register_form($regusername,$regemail,$referrerName,$error);
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
	'.multi_page($fieldsArray,'Users','','ORDER BY activated ASC, regdate DESC','Users',$page,$conn,'','del_user','100%',array('index.php?section=validate_user'=>'Validate'));

	return $out;
}

function del_user($did,$conn)
{
	if($did==0){
		return '<span class="error">ERROR: Invalid ID.</span>';
	}
			
	$res=query("DELETE FROM Users WHERE id='$did'",$conn);
	$out='The user was deleted.<br>
	<a href="index.php?section=users">Back to users</a>';
	
	return $out;
}

function validate_user($id,$conn){
	query("UPDATE Users SET activated=1 WHERE id=$id",$conn);
	
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
	$res=query("SELECT * FROM Users WHERE username='$user' AND password='".md5($pass)."'",$conn);
	if(mysql_num_rows($res)==0){
		$_SESSION['login_error']='Pasword doesn\'t match or username does not exist.';
	}else{
		$row=mysql_fetch_assoc($res);

		$_SESSION['uID']=$row['id'];
		$_SESSION['username']=$row['username'];
		$_SESSION['uemail']=$row['email'];
		$_SESSION['cookie_str']=md5(str_makerand(10,10,1,0,1));
		query("UPDATE Users SET cookie_str='".$_SESSION['cookie_str']."' WHERE id=".$row['id'],$conn);
		
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
	unset($_SESSION['uemail']);
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
	$udata=getFields('Users','LEFT JOIN Users ref ON Users.referrer_id=ref.id WHERE Users.id='.$_SESSION['uID'],$conn,'','Users.*,ref.username AS referrer');

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
	$variables=set_variable($variables,'email',$udata[0]['email']);
	$variables=set_variable($variables,'support_requests',get_support_request($conn));	
	$variables=set_variable($variables,'orders_history',get_orders_history($conn));	
	$variables=set_variable($variables,'mydealers',get_my_dealers($_SESSION['uID'],$conn));	
	$variables=set_variable($variables,'my_bug_reports',get_my_mantis($_SESSION['uID'],$conn));	
	$variables=set_variable($variables,'my_code_submissions',get_my_svn($_SESSION['uID'],$conn));	
	$variables=set_variable($variables,'message',@$message);	
	
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
	return outputHTML($variables,$page_template,1);		
}

function get_my_dealers($user_id,$conn){
	$res=query("
		SELECT dealers.*, Country.Description AS Country,Region.Region AS Region,City.City AS City
		FROM dealers 
		INNER JOIN Country ON dealers.FK_Country=PK_Country
		INNER JOIN Region ON dealers.FK_Region=PK_Region
		INNER JOIN City ON dealers.FK_City=PK_City
		WHERE user_id=$user_id ORDER BY category ASC",$conn);
	if(mysql_num_rows($res)==0){
		return '';
	}
	$categ_titles=array(
		1=>'Dealer info',
		2=>'Custom installer info',
		3=>'Open house event'
	);	
	
	$out='<table>';
	while($row=mysql_fetch_assoc($res)){
		$out.='
		<tr class="alternate">
			<td><b>'.$categ_titles[$row['category']].': '.$row['title'].'</b></td>
			<td align="right"><a href="edit_dealer.php?id='.$row['id'].'&categ='.$row['category'].'">Edit</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'dremove.php?id='.$row['id'].'\'">Remove</a></td>
		</tr>';
		if($row['category']==3){
			$out.='
			<tr>
				<td colspan="2">From <b>'.format_mysql_date($row['date_from'],'d M Y h:i').'</b> to <b>'.format_mysql_date($row['date_to'],'d M Y h:i').'</b></td>
			</tr>
			
			';
		}
		$out.='
		<tr>
			<td colspan="2">Country: '.$row['Country'].', region '.$row['Region'].', city '.$row['City'].' </td>
		</tr>
		<tr>
			<td colspan="2"><a href="'.$row['url'].'">'.$row['url'].'</a><br>'.nl2br($row['description']).'</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;</td>
		</tr>';
	}
	$out.='</table>';
	
	return $out;
}


function delete_dealer($id,$conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		Header("Location: myaccount.php");
		exit();
	}	
	
	if($id==0){
		return false;
	}	
	$user_id=(int)@$_SESSION['uID'];
	query("DELETE FROM dealers WHERE user_id=$user_id AND id=$id",$conn);
	// return message ??
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

function get_dealer($conn){
	process_add_rating($conn);
	
	$selCountry=(int)@$_REQUEST['country'];
	$selRegion=(int)@$_REQUEST['region'];
	$selCity=(int)@$_REQUEST['city'];
	

	$variables=array();
	$page_template=implode('',file('templates/dealer.html'));
	$variables=set_variable($variables,'country_pulldown',countriesPulldown('country',$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'region_pulldown',regionsPulldown('region',$selRegion,$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'city_pulldown',citiesPulldown('city',$selCity,$selRegion,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'dealers_table',get_dealers_list($conn,$selCountry,$selRegion,$selCity));
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
	
	return outputHTML($variables,$page_template,1);		
}

function get_showcase(){
	$variables=array();
	$page_template=implode('',file('templates/showcase.html'));
	

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
	$res=query("SELECT id from Users WHERE username='$username'",$conn);
	
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
			$referrals=getAssocArray('Users','id','username',$conn,'WHERE referrer_id IN ('.join(',',$parents).')');
		}
		$parents=array_keys($referrals);
		$rArray[$pos]=count($referrals);
	}
	
	return $rArray;
}

function get_support_request($conn){

	$userID=(int)@$_SESSION['uID'];
	$res=query("SELECT * FROM Incidents WHERE user_id=$userID ORDER BY date_reported DESC",$conn);
	if(mysql_num_rows($res)==0){
		return 'No support requests.';
	}

	$out='<table>
		<tr class="alternate">
			<td><b>Ticket ID</b></td>
			<td><b>Date reported</b></td>
			<td><b>Status</b></td>
		</tr>';
	while($row=mysql_fetch_assoc($res)){
		$out.='
		<tr class="alternate">
			<td><a href="ticket.php?id='.$row['id'].'">'.$row['id'].'</a></td>
			<td>'.format_mysql_date($row['date_reported'],'d M Y h:i').'</td>
			<td>'.$GLOBALS['incident_status_array'][$row['status_id']].'</td>
		</tr>
		<tr>
			<td colspan="3">'.nl2br($row['description']).'</td>
		</tr>';
	}
	$out.='</table>';
	
	return $out;
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
	$email=cleanString($_POST['email']);
	
	$emailData=getAssocArray('Users','id','name',$conn,'WHERE email LIKE \''.$email.'\' AND id!='.$id);
	if(count($emailData)>0){
		$_SESSION['message']=msg_error('This email address is already registered.');
		header("Location: myaccount.php");
		exit();
	}
	
	$res=query("UPDATE Users SET comments='$comments',publicUser=$publicUser,publicComments=$publicComments,polywell_email='$pemail',email='$email' WHERE id=$id",$conn);
	$message=msg_notice('Account info updated.');
	
	if($old_password!=''){
		$res=query("SELECT id FROM Users WHERE id=$id AND password='".md5($old_password)."'",$conn);
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
			$res=query("UPDATE Users SET password='$password' WHERE id=$id",$conn);
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
	
	$polywell_info=get_polywell_info();
	
	$variables=array();
	$page_template=implode('',file('templates/polywell-redirect.html'));
	$variables=set_variable($variables,'url',$url);
	$variables=set_variable($variables,'poywell_info',$polywell_info);

	return outputHTML($variables,$page_template,1);		
}

function get_polywell_info(){
	if(isset($_SESSION['uID'])){
		$template='templates/polywell_logged.html';
	}else{
		$template='templates/polywell_not_logged.html';
	}
	
	$variables=array();
	$page_template=implode('',file($template));
	$variables=set_variable($variables,'email',@$_SESSION['uemail']);

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

function forgot_password($conn){
	if(isset($_POST['send'])){
		return process_forgot_password($conn);
	}
	
	return forgot_password_form();
}

function forgot_password_form(){
	$variables=array();
	$page_template=implode('',file('templates/forgot_password.html'));

	return outputHTML($variables,$page_template,1);		
}

function process_forgot_password($conn){
	
	$email=cleanString($_POST['email']);
	$subject="Your Fiire password";
	$pass=str_makerand(8,8,1,0,0);
	
	$emailData=getAssocArray('Users','email','id',$conn,'WHERE email LIKE \''.$email.'\'');
	if(count($emailData)==0){
		return msg_error('This email address is not registered with Fiire.').'<br><a href="forgot.php">Try again</a>';
	}
	
	
	$message='Hello,<br>
	 Your new password for Fiire Website is <b>'.$pass. '</b>  <br><br>
	 <a href="'.$GLOBALS['website_url'].'">Click here</a> to login and after that you can change the password from your account.<br><br>
	 Best regards, <br>Fiire staff.';
	$mdpass=md5($pass);
	$id=$emailData[$email];
	query("UPDATE Users SET password='$mdpass' WHERE id=$id",$conn);
	
	send_mail('Fiire','noreply@fiire.com',$email,$email,$subject,$message);
	return 'A new password was sent to the email address you provided. <br>
	Please check your email.';
}

function contact($page,$conn){
	if(isset($_POST['send']) && (int)@$_SESSION['msg_sent']!=1){
		unset($_SESSION['msg_sent']);
		return process_contact_form($page,$conn);
	}else{
		unset($_SESSION['msg_sent']);
		return get_contact_form($page,$conn);
	}
}


	function get_contact_form($page,$conn){
	$variables=array();
	$page_template=implode('',file('templates/contact_form.html'));
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}

	return outputHTML($variables,$page_template,1);		
}

function process_contact_form($page,$conn){
	$cname=cleanString($_POST['cname']);
	$cemail=cleanString($_POST['cemail']);
	$csubject=cleanString($_POST['csubject']);
	$cdescription=cleanString($_POST['cdescription']);
	if($cname=='' || $cemail=='' || $csubject=='' || $cdescription==''){
		$_SESSION['message']=msg_error('Please fill all fields.');
		return get_contact_form($page,$conn);
	}
	
	query("INSERT INTO SiteContacts (page,name,email,subject,description) VALUES ('$page','$cname','$cemail','$csubject','$cdescription')",$conn);
	
	$mailSubject='Fiire contact form';
	$message='Contact name: '.$cname.'<br>';
	$message.='Contact email: '.$cemail.'<br>';
	$message.='Subject: '.$csubject.'<br>';
	$message.='Description: '.nl2br($cdescription).'<br>';
	
	send_mail('Fiire','noreply@fiire.com',$GLOBALS['contact_email'],$GLOBALS['contact_email'],$mailSubject,$message);
	$_SESSION['msg_sent']=1;
	return msg_notice('Thank you, your message was recorded.');
}

function record_dealer($categs,$conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to update your account.';
	}	

	
	if(isset($_POST['save'])){
		return process_record_dealer($categs,$conn);
	}
	return record_dealer_form($categs,$conn);
}

function record_dealer_form($categs,$conn){
	$id=(int)@$_SESSION['uID'];
	$data=getFields('dealers','WHERE user_id='.$id,$conn);
	
	if(isset($_REQUEST['country'])){
		$selCountry=(int)@$_REQUEST['country'];
		$selRegion=(int)@$_REQUEST['region'];
		$selCity=(int)@$_REQUEST['city'];
		$date_from=@$_REQUEST['date_from'];
		$date_to=@$_REQUEST['date_to'];
		$title=cleanString($_POST['title']);
		$description=cleanString($_POST['description']);
		$url=cleanString($_POST['url']);
	}else{
		$selCountry=0;
		$selRegion=0;
		$selCity=0;
		$date_from='0000-00-00 00:00';
		$date_to='0000-00-00 00:00';
		$title='';
		$description='';
		$url='';
	}

	$categ=(int)@$_REQUEST['categ'];
	
	$variables=array();
	$page_template=implode('',file('templates/record_dealer.html'));
	$variables=set_variable($variables,'form_action','record_dealer.php');
	$variables=set_variable($variables,'country_pulldown',countriesPulldown('country',$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'region_pulldown',regionsPulldown('region',$selRegion,$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'city_pulldown',citiesPulldown('city',$selCity,$selRegion,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'categ_pulldown',pulldownFromArray($categs,'categ',$categ,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'from_to_dates',($categ==3)?' from <input type="text" name="date_from" value="'.$date_from.'"> to <input type="text" name="date_to" value="'.$date_to.'">':'');
	$variables=set_variable($variables,'title',$title);
	$variables=set_variable($variables,'description',$description);
	$variables=set_variable($variables,'url',$url);

	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function countriesPulldown($name,$selected,$conn,$extra){
	$res=query("
		SELECT Country.*,user_id FROM Country 
		LEFT JOIN dealers on FK_Country=PK_Country
		GROUP BY PK_Country
		ORDER BY Country.Description ASC",$conn);
	$select='<select name="'.$name.'" '.$extra.'>
		<option value="0">Pick the country</option>';
	while($row=mysql_fetch_assoc($res)){
		$select.='<option value="'.$row['PK_Country'].'" '.(($row['PK_Country']==$selected)?'selected':'').' '.((!is_null($row['user_id']))?'style="background:#DFFFEF;"':'').'>'.$row['Description'].'</option>';
	}
	$select.='
	</select>';
	
	return $select;
}

function regionsPulldown($name,$selected,$country,$conn,$extra){
	$res=query("
		SELECT Region.*,user_id FROM Region 
		LEFT JOIN dealers on FK_Region=PK_Region
		WHERE Region.FK_Country=$country
		GROUP BY PK_Region
		ORDER BY Region.Region ASC",$conn);
	$select='<select name="'.$name.'" '.$extra.'>
		<option value="0">Pick the region</option>';
	while($row=mysql_fetch_assoc($res)){
		$select.='<option value="'.$row['PK_Region'].'" '.(($row['PK_Region']==$selected)?'selected':'').' '.((!is_null($row['user_id']))?'style="background:#DFFFEF;"':'').'>'.$row['Region'].'</option>';
	}
	$select.='
	</select>';
	
	return $select;
}

function citiesPulldown($name,$selected,$region,$conn,$extra){
	$res=query("
		SELECT City.*,user_id FROM City
		LEFT JOIN dealers on FK_City=PK_City
		WHERE City.FK_Region=$region
		GROUP BY PK_City
		ORDER BY City.City ASC",$conn);
	$select='<select name="'.$name.'" '.$extra.'>
		<option value="0">Pick the city</option>';
	while($row=mysql_fetch_assoc($res)){
		$select.='<option value="'.$row['PK_City'].'" '.(($row['PK_City']==$selected)?'selected':'').' '.((!is_null($row['user_id']))?'style="background:#DFFFEF;"':'').'>'.$row['City'].'</option>';
	}
	$select.='
	</select>';
	
	return $select;
}

function process_record_dealer($categs,$conn){
	$id=(int)@$_SESSION['uID'];
	
	$selCountry=(int)@$_REQUEST['country'];
	$selRegion=(int)@$_REQUEST['region'];
	$selCity=(int)@$_REQUEST['city'];
	$cityname=cleanString($_POST['cityname']);
	$categ=(int)@$_REQUEST['categ'];
	$title=cleanString($_POST['title']);
	$url=cleanString($_POST['url']);
	$description=cleanString($_POST['description']);
	$date_from=cleanString(@$_POST['date_from']);
	$date_to=cleanString(@$_POST['date_to']);
	
	if($selCountry==0 || $selRegion==0 || ($selCity==0 && $cityname=='')){
		$_SESSION['message']=msg_error('Please provide geographical info.');
		return record_dealer_form($categs,$conn);
	}
	
	if($url=='' || $description==''){
		$_SESSION['message']=msg_error('Please fill in your offering info.');
		return record_dealer_form($categs,$conn);
	}
	
	if($selCity==0 && $cityname!=''){
		query("INSERT INTO City (FK_Country,FK_Region,City) VALUES ($selCountry,$selRegion,'$cityname')",$conn);
		$selCity=mysql_insert_id($conn);
	}
	query("
	INSERT INTO dealers (FK_Country,FK_Region,FK_City,cityname,category,url,description,user_id,date_from,date_to,title) 
	VALUES 
	('$selCountry','$selRegion','$selCity','$cityname','$categ','$url','$description',$id,'$date_from','$date_to','$title')",$conn);
	$_SESSION['message']=msg_notice('Account updated.');
	unset($_POST);
	
	return record_dealer_form($categs,$conn);
}

function get_dealers_list($conn,$selCountry,$selRegion,$selCity){
	if($selCountry==0){
		return '';
	}
	$filters='';
	if($selRegion!=0){
		$filters.=' AND dealers.FK_Region='.$selRegion;
	}
	if($selCity!=0){
		$filters.=' AND dealers.FK_City='.$selCity;
	}
	$ratingArray=array(
		0=>'Rate dealer',
		5=>'Excelent',
		4=>'Very good',
		3=>'Good',
		2=>'Average',
		1=>'Bad'
	);
	$ipaddress=$_SERVER['REMOTE_ADDR'];
	
	$res=query("SELECT dealers.*,avg(rating.rating) AS rating,myrating.rating as myrating,City.City, Region.Region
		FROM dealers
		INNER JOIN City ON dealers.FK_City=PK_City
		INNER JOIN Region ON dealers.FK_Region=PK_Region
		LEFT JOIN rating ON rating.dealer_id=dealers.id
		LEFT JOIN rating myrating on myrating.dealer_id=dealers.id AND myrating.ipaddress='$ipaddress'
		WHERE dealers.FK_Country=$selCountry $filters
		GROUP BY dealers.id ",$conn);
	$no=mysql_num_rows($res);
	if($no==0){
		return 'We are sorry, there are no dealers for the area you selected.';
	}
	$out='
	<table width="100%">
		<tr>
			<td>Found '.$no.' records.<hr></td>
		</tr>';
	$dealers=array();
	while($row=mysql_fetch_assoc($res)){
		$rating=(is_null($row['myrating']))?pulldownFromArray($ratingArray,'rating_'.$row['id'],0,'onChange="document.form1.submit();"'):'';
		$out.='
			<tr>
				<td><b>'.$row['title'].' ('.$row['Region'].', '.$row['City'].')</b><br><a href="'.$row['url'].'">'.$row['url'].'</a><br>'.nl2br($row['description']).'</td>
			</tr>
			<tr>
				<td align="right">'.star_rating(ceil($row['rating'])).' '.$rating.'<hr></td>
			</tr>			
		';
		$dealers[]=$row['id'];
	}
	$out.='</table>
	<input type="hidden" name="dealers" value="'.join(',',$dealers).'">';
	
	return $out;
}

function process_add_rating($conn){
	$ipaddress=$_SERVER['REMOTE_ADDR'];
	$dealers=explode(',',@$_POST['dealers']);
	if(count($dealers)>0){
		foreach ($dealers AS $id){
			$rating=(int)@$_POST['rating_'.$id];
			if($rating!=0){
				query("INSERT IGNORE INTO rating (dealer_id,rating,ipaddress) values ($id,$rating,'$ipaddress')",$conn);
			}
		}
	}
}

function star_rating($no){
	if($no==0){
		return '';
	}
	
	$out='';
	for($i=0;$i<$no;$i++){
		$out.='<img src="images/star.gif" border="0">';
	}
	
	return $out;
}

function edit_dealer($id,$categs,$conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to update your account.';
	}	

	
	if(isset($_POST['save'])){
		return process_edit_dealer($categs,$conn);
	}
	return edit_dealer_form($id,$categs,$conn);
}

function edit_dealer_form($id,$categs,$conn){
	$user_id=(int)@$_SESSION['uID'];
	$data=getFields('dealers','WHERE user_id='.$user_id.' AND id='.$id,$conn);
	
	if(isset($_REQUEST['country'])){
		$selCountry=(int)@$_REQUEST['country'];
		$selRegion=(int)@$_REQUEST['region'];
		$selCity=(int)@$_REQUEST['city'];
		$date_from=@$_REQUEST['date_from'];
		$date_to=@$_REQUEST['date_to'];
		$title=cleanString($_POST['title']);
		$description=cleanString($_POST['description']);
		$url=cleanString($_POST['url']);
	}else{
		$selCountry=$data[0]['FK_Country'];
		$selRegion=$data[0]['FK_Region'];
		$selCity=$data[0]['FK_City'];
		$date_from=$data[0]['date_from'];
		$date_to=$data[0]['date_to'];
		$title=$data[0]['title'];
		$description=$data[0]['description'];
		$url=$data[0]['url'];
	}
	
	
	$categ=(int)@$_REQUEST['categ'];
	
	$variables=array();
	$page_template=implode('',file('templates/record_dealer.html'));
	$variables=set_variable($variables,'form_action','edit_dealer.php');
	$variables=set_variable($variables,'country_pulldown',countriesPulldown('country',$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'region_pulldown',regionsPulldown('region',$selRegion,$selCountry,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'city_pulldown',citiesPulldown('city',$selCity,$selRegion,$conn,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'categ_pulldown',pulldownFromArray($categs,'categ',$categ,'onChange="document.form1.submit();"'));
	$variables=set_variable($variables,'from_to_dates',($categ==3)?' from <input type="text" name="date_from" value="'.$date_from.'"> to <input type="text" name="date_to" value="'.$date_to.'">':'');
	$variables=set_variable($variables,'id',$id);
	$variables=set_variable($variables,'title',$title);
	$variables=set_variable($variables,'description',$description);
	$variables=set_variable($variables,'url',$url);
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function process_edit_dealer($categs,$conn){
	$user_id=(int)@$_SESSION['uID'];
	$id=(int)@$_POST['id'];
	
	$selCountry=(int)@$_REQUEST['country'];
	$selRegion=(int)@$_REQUEST['region'];
	$selCity=(int)@$_REQUEST['city'];
	$cityname=cleanString($_POST['cityname']);
	$categ=(int)@$_REQUEST['categ'];
	$title=cleanString($_POST['title']);
	$url=cleanString($_POST['url']);
	$description=cleanString($_POST['description']);
	$date_from=cleanString(@$_POST['date_from']);
	$date_to=cleanString(@$_POST['date_to']);

	if($selCountry==0 || $selRegion==0 || ($selCity==0 && $cityname=='')){
		$_SESSION['message']=msg_error('Please provide geographical info.');
		return edit_dealer_form($id,$categs,$conn);
	}
	
	if($url=='' || $description==''){
		$_SESSION['message']=msg_error('Please fill in your offering info.');
		return edit_dealer_form($id,$categs,$conn);
	}
	
	if($selCity==0 && $cityname!=''){
		query("INSERT INTO City (FK_Country,FK_Region,City) VALUES ($selCountry,$selRegion,'$cityname')",$conn);
		$selCity=mysql_insert_id($conn);
	}
	
	query("UPDATE dealers SET FK_Country='$selCountry',FK_Region='$selRegion',FK_City='$selCity',cityname='$cityname',category='$categ',url='$url',description='$url',date_from='$date_from',date_to='$date_to',title='$title' WHERE id=$id",$conn);
	$_SESSION['message']=msg_notice('Account updated.');
	unset($_POST);
	
	return edit_dealer_form($id,$categs,$conn);
}

function report_issue($conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to report an issue.';
	}	
		
	if(isset($_POST['save'])){
		return process_report_issue($conn);
	}
	
	return report_issue_form($conn);
}

function report_issue_form($conn){
	$variables=array();
	$page_template=implode('',file('templates/report_issue.html'));
	
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}

	return outputHTML($variables,$page_template,1);	
}


function process_report_issue($conn){
	$description=cleanString($_POST['description']);
	$user_id=(int)@$_SESSION['uID'];
	
	query("INSERT INTO Incidents (user_id,date_reported,description,status_id) VALUES ($user_id,NOW(),'$description',10)",$conn);
	$ticketID=mysql_insert_id($conn);
	
	$msg='
	New support ticket opened: #'.$ticketID.'<br>
	Opener: '.$_SESSION['username'].'<br>
	Email address: '.$_SESSION['uemail'].'<br>
	Details: <hr>'.nl2br(stripslashes($description)).'<br><br>
	
	Go to <a href="'.$GLOBALS['website_url'].'/tickets/edit_incident.php?id='.$ticketID.'">ticketing application</a>';
	
	send_mail('Fiire website','noreply@fiire.com','Fiire Support',$GLOBALS['support_email'],'Support request #'.$ticketID,$msg);
	
	return 'Thank you, your issue was recorded.<br>This is your ticket number: <span class="price">'.$ticketID.'</span><br>
	You can check it\'s status at any time from "My Account / Support request" section.';
}

function ticket_history($id,$conn){
	if($id==0){
		return msg_error('Invalid ticket ID');
	}
	$user_id=(int)@$_SESSION['uID'];
	$ticket=getFields('Incidents','WHERE id='.$id.' AND user_id='.$user_id,$conn);
	
	
	$variables=array();
	$page_template=implode('',file('templates/ticket_history.html'));
	$variables=set_variable($variables,'date_reported',format_mysql_date($ticket[0]['date_reported'],'d-M Y h:i'));	
	$variables=set_variable($variables,'description',nl2br($ticket[0]['description']));	
	$variables=set_variable($variables,'resolution',($ticket[0]['resolution']!='')?nl2br($ticket[0]['resolution']):'Not available.');	
	$variables=set_variable($variables,'status',$GLOBALS['incident_status_array'][$ticket[0]['status_id']]);	
	$variables=set_variable($variables,'notes',get_contacts_table($id,$conn));	
	
	return outputHTML($variables,$page_template,1);	
}

function get_contacts_table($incident_id,$conn){
	$res=query("SELECT * FROM Contacts WHERE incident_id=$incident_id ORDER BY contact_date ASC",$conn);
	if(mysql_num_rows($res)==0){
		return 'No notes.';
	}
	
	$out='';
	$pos=0;
	while($row=mysql_fetch_assoc($res)){
		$pos++;
		$out.='
		<table width="570" border="0" cellpadding="0" cellspacing="0" class="'.(($pos%2!=0)?'gri':'').'">
        <tr>

          <td style="padding:10px 10px 0 10px;"><strong>'.format_mysql_date($row['contact_date'],'d M Y h:i').'</strong></td>
        </tr>
        <tr>
          <td style="word-wrap:break-word;"><div class="comment">'.nl2br($row['comments']).'</div></td>
        </tr>
        <tr>
          <td style="word-wrap:break-word;">&nbsp;</td>
        </tr>
      </table>
		
		';
	}
	
	return $out;
}

function get_my_mantis($userID,$conn){
	$res=query("SELECT * FROM linuxmce_mantis WHERE user_id=$userID",$conn);
	if(mysql_num_rows($res)==0){
		return '<tr><td>No records.</td></tr>';
	}	
	$out='
		<tr class="alternate">
			<td><b>Mantis ID</b></td>
			<td><b>Description</b></td>
			<td><b>Functions</b></td>
		</tr>
	
	';
	$pos=0;
	while($row=mysql_fetch_assoc($res)){
		$pos++;
		$class=($pos%2==0)?'alternate':'';
		$out.='
		<tr class="'.$class.'">
			<td align="center"><a href="'.$GLOBALS['linuxmce_mantis_url'].'view.php?id='.$row['mantis_id'].'" target="_blank">'.$row['mantis_id'].'</a></td>
			<td>'.nl2br($row['description']).'</td>
			<td align="center"><a href="bug_report.php?action=edit&id='.$row['id'].'">Edit</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'bug_report.php?action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>
		';
	}
	
	return $out;
}

function get_my_svn($userID,$conn){
	$res=query("SELECT * FROM linuxmce_svn WHERE user_id=$userID",$conn);
	if(mysql_num_rows($res)==0){
		return '<tr><td>No records.</td></tr>';
	}

	$out='
		<tr class="alternate">
			<td><b>SVN revision</b></td>
			<td><b>Description</b></td>
			<td><b>Functions</b></td>
		</tr>
		';
	$pos=0;
	while($row=mysql_fetch_assoc($res)){
		$pos++;
		$class=($pos%2==0)?'alternate':'';
		$out.='
		<tr class="'.$class.'">
			<td align="center">'.$row['svnrev'].'</td>
			<td>'.nl2br($row['description']).'</td>
			<td align="center"><a href="svn.php?action=edit&id='.$row['id'].'">Edit</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this record?\'))self.location=\'svn.php?action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>
		';
	}
	
	return $out;	
}


function bug_report($action,$conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to add/edit bug reports.';
	}

	switch($action){
		case 'edit':
			if(isset($_POST['save'])){
				return process_edit_bug_report($conn);
			}
			return edit_bug_report_form((int)@$_REQUEST['id'],$conn);		
			
		break;
		case 'del':
			return del_bug_report((int)@$_REQUEST['id'],$conn);		
		break;
		default:
			if(isset($_POST['save'])){
				return process_add_bug_report($conn);
			}
			return add_bug_report_form($conn);		
			
		break;
	}
}

function add_bug_report_form($conn){
	$variables=array();
	$page_template=implode('',file('templates/my_bug.html'));
	$variables=set_variable($variables,'form_action','add');

	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function process_add_bug_report($conn){
	$userID=$_SESSION['uID'];
	$mantisID=(int)@$_POST['mantis_id'];
	$description=cleanString($_POST['description']);
	
	if($mantisID==0 || $description==''){
		$_SESSION['message']=msg_error('Please fill all fields.');	
		return add_bug_report_form($conn);
	}
	
	query("INSERT INTO linuxmce_mantis (user_id,mantis_id,description) VALUES ($userID,$mantisID,'$description')",$conn);
	return msg_notice('The record was saved.').'<br><br><a href="myaccount.php">Back to my account</a>';
}

function edit_bug_report_form($id,$conn){
	$user_id=(int)@$_SESSION['uID'];
	
	if($id==0){
		return msg_error('Invalid bug report ID');
	}
	
	$data=getFields('linuxmce_mantis','WHERE id='.$id.' AND user_id='.$user_id,$conn);
	if(count($data)==0){
		return msg_error('Invalid bug report ID');
	}	
	
	$variables=array();
	$page_template=implode('',file('templates/my_bug.html'));
	$variables=set_variable($variables,'form_action','edit');
	$variables=set_variable($variables,'id',$id);
	$variables=set_variable($variables,'mantis_id',$data[0]['mantis_id']);
	$variables=set_variable($variables,'description',$data[0]['description']);
	
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function process_edit_bug_report($conn){
	$userID=$_SESSION['uID'];
	$id=(int)@$_POST['id'];
	
	if($id==0){
		return msg_error('Invalid bug report ID');
	}
	
	$data=getFields('linuxmce_mantis','WHERE id='.$id.' AND user_id='.$userID,$conn);
	if(count($data)==0){
		return msg_error('Invalid bug report ID');
	}	
	
	$mantisID=(int)@$_POST['mantis_id'];
	$description=cleanString($_POST['description']);

	if($mantisID==0 || $description==''){
		$_SESSION['message']=msg_error('Please fill all fields.');	
		return edit_bug_report_form($id,$conn);
	}

	
	query("UPDATE linuxmce_mantis SET mantis_id=$mantisID,description='$description' WHERE id=$id",$conn);
	return msg_notice('The record was saved.').'<br><br><a href="myaccount.php">Back to my account</a>';
}

function del_bug_report($id,$conn){
	$userID=$_SESSION['uID'];
	
	if($id==0){
		return msg_error('Invalid bug report ID');
	}
	
	$data=getFields('linuxmce_mantis','WHERE id='.$id.' AND user_id='.$userID,$conn);
	if(count($data)==0){
		return msg_error('Invalid bug report ID');
	}	
	
	query("DELETE FROM linuxmce_mantis WHERE id=$id",$conn);
	return msg_notice('The record was deleted.').'<br><br><a href="myaccount.php">Back to my account</a>';
}

function svn($action,$conn){
	if(!isset($_SESSION['uID']) || (int)$_SESSION['uID']==0){
		return 'Please login in order to add/edit bug reports.';
	}

	switch($action){
		case 'edit':
			if(isset($_POST['save'])){
				return process_edit_svn($conn);
			}
			return edit_svn_form((int)@$_REQUEST['id'],$conn);		
			
		break;
		case 'del':
			return del_svn((int)@$_REQUEST['id'],$conn);		
		break;
		default:
			if(isset($_POST['save'])){
				return process_add_svn($conn);
			}
			return add_svn_form($conn);		
			
		break;
	}
}

function add_svn_form($conn){
	$variables=array();
	$page_template=implode('',file('templates/my_svn.html'));
	$variables=set_variable($variables,'form_action','add');

	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function process_add_svn($conn){
	$userID=$_SESSION['uID'];
	$svnrev=(int)@$_POST['svnrev'];
	$description=cleanString($_POST['description']);
	
	if($svnrev==0 || $description==''){
		$_SESSION['message']=msg_error('Please fill all fields.');	
		return add_svn_form($conn);
	}

	
	query("INSERT INTO linuxmce_svn (user_id,svnrev,description) VALUES ($userID,$svnrev,'$description')",$conn);
	return msg_notice('The record was saved.').'<br><br><a href="myaccount.php">Back to my account</a>';
}

function edit_svn_form($id,$conn){
	$user_id=(int)@$_SESSION['uID'];
	
	if($id==0){
		return msg_error('Invalid bug report ID');
	}
	
	$data=getFields('linuxmce_svn','WHERE id='.$id.' AND user_id='.$user_id,$conn);
	if(count($data)==0){
		return msg_error('Invalid bug report ID');
	}	
	
	$variables=array();
	$page_template=implode('',file('templates/my_svn.html'));
	$variables=set_variable($variables,'form_action','edit');
	$variables=set_variable($variables,'id',$id);
	$variables=set_variable($variables,'svnrev',$data[0]['svnrev']);
	$variables=set_variable($variables,'description',$data[0]['description']);
	
	if(isset($_SESSION['message'])){
		$variables=set_variable($variables,'message',$_SESSION['message']);
		unset($_SESSION['message']);
	}
		
	return outputHTML($variables,$page_template,1);		
}

function process_edit_svn($conn){
	$userID=$_SESSION['uID'];
	$id=(int)@$_POST['id'];
	
	if($id==0){
		return msg_error('Invalid record.');
	}
	
	$data=getFields('linuxmce_svn','WHERE id='.$id.' AND user_id='.$userID,$conn);
	if(count($data)==0){
		return msg_error('Invalid record.');
	}	
	
	$svnrev=(int)@$_POST['svnrev'];
	$description=cleanString($_POST['description']);
	
	if($svnrev==0 || $description==''){
		$_SESSION['message']=msg_error('Please fill all fields.');	
		return edit_svn_form($id,$conn);
	}
	
	query("UPDATE linuxmce_svn SET svnrev=$svnrev,description='$description' WHERE id=$id",$conn);
	return msg_notice('The record was saved.').'<br><br><a href="myaccount.php">Back to my account</a>';
}

function del_svn($id,$conn){
	$userID=$_SESSION['uID'];
	
	if($id==0){
		return msg_error('Invalid record.');
	}
	
	$data=getFields('linuxmce_svn','WHERE id='.$id.' AND user_id='.$userID,$conn);
	if(count($data)==0){
		return msg_error('Invalid record.');
	}	
	
	query("DELETE FROM linuxmce_svn WHERE id=$id",$conn);
	return msg_notice('The record was deleted.').'<br><br><a href="myaccount.php">Back to my account</a>';
}


?>
