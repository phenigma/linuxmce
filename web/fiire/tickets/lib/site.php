<?
function get_index_menu($conn){
	if(!isset($_SESSION['staffID'])){
		header("Location: login.php");
		exit();
	}
	
	$page_template=implode('',file('templates/index_menu.html'));

	$variables=array();

	return outputHTML($variables,$page_template,1);
}

function get_login($conn){
	if(isset($_POST['login'])){
		return process_login($conn);
	}
	
	return get_login_form();
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

function process_login($conn){
	$isAut=authetificate($conn);

	if($isAut!==true){
		$_SESSION['login_error']=$isAut;
		return get_login_form();
	}
	
	header("Location: index.php");
	exit();
}

function authetificate($conn){
	$user=$_POST['user'];
	$pass=md5($_POST['pass']);

	$res=query("SELECT * FROM Staff WHERE username='$user'",$conn);
	if(mysql_num_rows($res)>0){
		$row=mysql_fetch_assoc($res);
		if($row['password']==$pass){
			$_SESSION['staffID']=$row['id'];
			$_SESSION['staffName']=$row['username'];
			
			return true;
		}else{
			return 'Wrong password.';
		}
	}else{		
		return 'Username not found.';
	}
}

function get_incidents($conn){
	
	$userfield='\'xxxx\' AS username';
	if(isset($_SESSION['staffID'])){
		$extra_links=array(
			'edit_incident.php?from='.base64_encode($_SERVER['QUERY_STRING'])=>'Update'
		);
		$userfield='Users.username AS user';	
	}else{
		$extra_links=array(
			'details.php?from='.base64_encode($_SERVER['QUERY_STRING'])=>'Details'
		);
	}

	$f_description=(int)@$_REQUEST['f_description'];
	$f_workaround=(int)@$_REQUEST['f_workaround'];
	$f_resolution=(int)@$_REQUEST['f_resolution'];
	$f_bug_report=(int)@$_REQUEST['f_bug_report'];
	$f_bug_result=(int)@$_REQUEST['f_bug_result'];
	
	$timeFilterArray=array(
		0=>'ANYTIME',
		1=>'Last week',
		2=>'Last 2 weeks',
		3=>'Last month',
		4=>'Older than a month'
	);
	
	// setting defaults
	if(!isset($_REQUEST['sstring'])){
		$f_description=1;
		$f_workaround=1;
		$f_bug_report=1;
		$f_resolution=1;
		$f_bug_result=1;
	}
	
	$fieldsArray=array(
		'Incidents.id AS id'=>'id',
		'date_reported'=>'Date',
		$userfield=>'Reported by'
	);
	
	if($f_description==1){
		$fieldsArray['description']='Description';
	}
	if($f_workaround==1){
		$fieldsArray['workaround']='Workaround';
	}
	if($f_resolution==1){
		$fieldsArray['resolution']='Resolution';
	}
	if($f_bug_report==1){
		$fieldsArray['CONCAT(bug_report,\'|\',IF(bug_option IS NULL,\'Not specified\',bug_option)) AS ff_bug_report_url']='Bug Report';
	}
	if($f_bug_result==1){
		$fieldsArray['bug_result']='Bug Result';
	}
	if(isset($_SESSION['staffID'])){
		$fieldsArray['IF(staff_id IS NULL,\'user\',Staff.username) AS staff']='Recorded by';		
	}
	$fieldsArray['status_id AS ff_get_status']='Status';		
	
	// filters
	$srange=(int)@$_REQUEST['srange'];
	$sstring=cleanString(@$_REQUEST['sstring']);
	$sstatus=(int)@$_REQUEST['sstatus'];
	$suser=(int)@$_REQUEST['suser'];
	$stime=(int)@$_REQUEST['stime'];
	
	$whereArray=array();
	if($sstring!=''){
		$words=explode(' ',strtolower($sstring));

		
		switch($srange){
			case 0:
				foreach ($words AS $word){
					if($word!='AND' && $word!='OR'){
						$whereArray[]="(LOWER(description) LIKE '%$word%' OR LOWER(workaround) LIKE '%$word%' OR LOWER(resolution) LIKE '%$word%')";
					}
				}
			break;
			case 1:
				foreach ($words AS $word){
					if($word!='AND' && $word!='OR'){
						$whereArray[]="LOWER(description) LIKE '%$word%'";
					}
				}
			break;
			case 2:
				foreach ($words AS $word){
					if($word!='AND' && $word!='OR'){
						$whereArray[]="LOWER(workaround) LIKE '%$word%'";
					}
				}
			break;
			case 3:
				foreach ($words AS $word){
					if($word!='AND' && $word!='OR'){
						$whereArray[]="LOWER(resolution) LIKE '%$word%'";
					}
				}
			break;
		}
		$operator='OR';
		if(in_array('and',$words)){
			$operator='AND';
		}
		
		$where=join(' '.$operator.' ',$whereArray);
	}
	
	if($sstatus!=0){
		$where=((@$where!='')?"($where) AND":'')." status_id=".$sstatus;
	}

	if($suser!=0){
		$where=((@$where!='')?"($where) AND":'')." user_id=".$suser;
	}

	if($stime!=0){
		switch ($stime){
			case 1:
				$timeFilter='date_reported >= DATE_SUB(NOW(),INTERVAL 7 day)';
			break;
			case 2:
				$timeFilter='date_reported > DATE_SUB(NOW(),INTERVAL 14 day)';
			break;
			case 3:
				$timeFilter='date_reported > DATE_SUB(NOW(),INTERVAL 30 day)';
			break;
			case 4:
				$timeFilter='date_reported < DATE_SUB(NOW(),INTERVAL 30 day)';
			break;
			
			default:
				$timeFilter='';
			break;
		}
		$where=((@$where!='')?"($where) AND":'')." ".$timeFilter;
	}
	
	$where=(@$where!='')?'WHERE '.$where:'';

	$out='
	<a href="index.php"> &lt;&lt; Back</a>
	<p class="normal_row"><b>Incidents</b><br>
	<span class="notification">'.@$_SESSION['notification'].'</span>
	<form name="incidents" method="GET" action="'.$_SERVER['PHP_SELF'].'">
	<input type="hidden" name="page" value="'.(((int)@$_REQUEST['page']==0)?1:(int)@$_REQUEST['page']).'">
	<table cellpadding="2" cellspacing="0">
		<tr>
			<td colspan="12" class="alternate" align="center"><B>Select fields to be displayed</B></td>
		</tr>
		<tr>
			<td><input type="checkbox" value="1" name="f_description" onclick="document.incidents.submit();" '.(($f_description==1)?'checked':'').'></td>
			<td>Description</td>
			<td><input type="checkbox" value="1" name="f_workaround" onclick="document.incidents.submit();" '.(($f_workaround==1)?'checked':'').'></td>
			<td>Workaround</td>
			<td><input type="checkbox" value="1" name="f_resolution" onclick="document.incidents.submit();" '.(($f_resolution==1)?'checked':'').'></td>
			<td>Resolution</td>
			<td><input type="checkbox" value="1" name="f_bug_report" onclick="document.incidents.submit();" '.(($f_bug_report==1)?'checked':'').'></td>
			<td>Bug report</td>
			<td><input type="checkbox" value="1" name="f_bug_result" onclick="document.incidents.submit();" '.(($f_bug_result==1)?'checked':'').'></td>
			<td>Bug result</td>
		</tr>
		<tr>
			<td colspan="12" class="alternate" align="center"><B>Filters	</B></td>
		</tr>
		<tr>
			<td colspan="12"><table>
				<tr>
					<td>Search *</td>
					<td><input type="text" name="sstring" value="'.$sstring.'"> 
						<input type="radio" name="srange" value="0" '.(($srange==0)?'checked':'').'> Everywhere 
						<input type="radio" name="srange" value="1" '.(($srange==1)?'checked':'').'> Description 
						<input type="radio" name="srange" value="2" '.(($srange==2)?'checked':'').'> Workaround 
						<input type="radio" name="srange" value="3" '.(($srange==3)?'checked':'').'> Resolution
					</td>
				</tr>
				<tr>
					<td>Status</td>
					<td>'.pulldownFromArray(array(0=>'ANY')+$GLOBALS['incident_status_array'],'sstatus',$sstatus).'</td>
				</tr>';
	if(isset($_SESSION['staffID'])){
		$usersArray=getAssocArray('Users','id',"CONCAT(username,' - ',email)",$conn,'','ORDER BY username ASC,email ASC');
		$out.='
				<tr>
					<td>Reported by </td>
					<td>'.pulldownFromArray(array(0=>'ANYONE')+$usersArray,'suser',$suser).'</td>
				</tr>		
				<tr>
					<td>Interval reported</td>
					<td>'.pulldownFromArray($timeFilterArray,'stime',$stime).'</td>
				</tr>				
		';
	}
	$out.='				
			</table>
			</td>
		</tr>
		<tr>
			<td colspan="12" class="alternate" align="center"><input type="submit" name="go" value="Display"></td>
		</tr>
	</table>
	</form>
	'.multi_page($fieldsArray,'Incidents','INNER JOIN Users ON Users.id=user_id LEFT JOIN BugOptions ON BugOptions.id=bug_option_id LEFT JOIN BugResults ON BugResults.id=bug_result_id LEFT JOIN Staff on staff_id=Staff.id '.@$where,'ORDER BY date_reported DESC','incidents',$conn,'','','100%',10,@$extra_links);
	$out.='
	<br>
	<em>* Logical operators AND and OR are allowed</em>
	<br>';
	unset($_SESSION['notification']);
	
	return $out;
}

function bug_report_url($value,$conn,$id){

	$parts=explode('|',$value);
	if(count($parts)!=2){
		return $value;
	}
	$url=(strpos($parts[0],'http://')===false)?$parts[1].': '.$parts[0]:'<a href="'.$parts[0].'">'.$parts[1].'</a>';
	
	return $url;
}

function get_status($id){
	return $GLOBALS['incident_status_array'][$id];
}

function add_incident($conn){
	if(!isset($_SESSION['staffID'])){
		header("Location: login.php");
		exit();
	}	
	
	if(isset($_POST['save'])){
		return process_add_incident($conn);
	}
	
	return add_incident_form($conn);
}

function add_incident_form($conn){
	$page_template=implode('',file('templates/incident_form.html'));
	
	$bug_options_array=array(0=>'Not specified')+getAssocArray('BugOptions','id','bug_option',$conn);
	$results_array=getAssocArray('BugResults','id','bug_result',$conn);
	
	$status_pulldown=pulldownFromArray($GLOBALS['incident_status_array'],'status',1);
	$bug_options_pulldown=pulldownFromArray($bug_options_array,'bug_option',(int)@$_REQUEST['bug_option']);
	$result_pulldown=pulldownFromArray($results_array,'bug_result',(int)@$_REQUEST['bug_result']);
	$users=getAssocArray('Users','id',"CONCAT(username,' - ',email)",$conn,'','ORDER BY username ASC,email ASC')+array(-1=>'Not in the list, add new user');
	$users_pulldown=pulldownFromArray($users,'user_id',0,'onChange="display_new_user(document.incident_form.user_id.value);"');
	
	$variables=array();
	$variables=set_variable($variables,'action','add_incident.php');
	$variables=set_variable($variables,'form_title','Add incident');
	$variables=set_variable($variables,'date_reported',date('Y-m-d h:i:s'));
	$variables=set_variable($variables,'status_pulldown',$status_pulldown);
	$variables=set_variable($variables,'bug_option',$bug_options_pulldown);
	$variables=set_variable($variables,'bug_result',$result_pulldown);
	$variables=set_variable($variables,'contact_rows',get_contact_rows(0,$conn));
	$variables=set_variable($variables,'users_pulldown',$users_pulldown.'<br>');
	$variables=set_variable($variables,'description',@$_REQUEST['description']);
	$variables=set_variable($variables,'resolution',@$_REQUEST['resolution']);
	$variables=set_variable($variables,'workaround',@$_REQUEST['workaround']);
	$variables=set_variable($variables,'bug_report',@$_REQUEST['bug_report']);

	
	if(isset($_SESSION['notification'])){
		$variables=set_variable($variables,'notification',$_SESSION['notification']);
		unset($_SESSION['notification']);
	}
	
	return outputHTML($variables,$page_template,1);
}

function process_add_incident($conn){
	$username=cleanString($_POST['username']);
	$email=cleanString($_POST['email']);
	$date_reported=cleanString($_POST['date_reported']);
	$description=cleanString($_POST['description']);
	$resolution=cleanString($_POST['resolution']);
	$workaround=cleanString($_POST['workaround']);
	$bug_report=cleanString(@$_POST['bug_report']);
	$bug_option_id=(int)@$_POST['bug_option'];
	$bug_option_id=($bug_option_id==0)?NULL:$bug_option_id;
	$bug_result_id=(int)@$_POST['bug_result'];
	$status_id=(int)@$_POST['status'];
	$user_id=(int)@$_POST['user_id'];
	$staff_id=(int)@$_SESSION['staffID'];
	

	if($user_id==-1 && ($username=='' && $email=='')){
		$_SESSION['notification']='Pick an username from drop down or type the YM/Skype/Forum ID or email address.';
		return add_incident_form($conn);
	}
	
	if($user_id==-1){
		query("INSERT IGNORE INTO Users (username,email) VALUES ('$username','$email')",$conn);
		$user_id=mysql_insert_id($conn);
	}

	query("INSERT INTO Incidents (user_id,date_reported,description,resolution,workaround,bug_report,bug_option_id,bug_result_id, `status_id`,staff_id) VALUES ($user_id,'$date_reported','$description','$resolution','$workaround','$bug_report','$bug_option_id',$bug_result_id,$status_id,'$staff_id')",$conn);
	$incident_id=mysql_insert_id($conn);
	
	$duration=(int)@$_POST['duration'];
	$comments=cleanString($_POST['comments']);
	
	if($duration!=0 && $comments!=''){
		query("INSERT INTO Contacts (incident_id,contact_date,duration,comments,staff_id) VALUES ($incident_id,NOW(),$duration,'$comments','$staff_id')",$conn);
	}
	
	$_SESSION['notification']='The incident was recorded.';
	header("Location: incidents.php");
	exit();
}

function edit_incident($conn){
	if(!isset($_SESSION['staffID'])){
		header("Location: login.php");
		exit();
	}
		
	if(isset($_POST['del'])){
		return process_del_incident($conn);
	}
	
	if(isset($_POST['id'])){
		return process_edit_incident($conn);
	}
	return edit_incident_form($conn);
}

function edit_incident_form($conn){
	$id=(int)@$_REQUEST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('Incidents','INNER JOIN Users ON Users.id=user_id LEFT JOIN BugOptions ON BugOptions.id=bug_option_id LEFT JOIN BugResults ON BugResults.id=bug_result_id LEFT JOIN Staff on Staff.id=staff_id WHERE Incidents.id='.$id,$conn,'','Incidents.*,Users.username AS username,Users.email AS email,Staff.Username AS Staff');
	if(count($data)==0){
		return 'Invalid ID';
	}
	
	$page_template=implode('',file('templates/incident_form.html'));
	
	$bug_options_array=array(0=>'Not specified')+getAssocArray('BugOptions','id','bug_option',$conn);
	$results_array=getAssocArray('BugResults','id','bug_result',$conn);
	
	$status_pulldown=pulldownFromArray($GLOBALS['incident_status_array'],'status',$data[0]['status_id']);
	$bug_options_pulldown=pulldownFromArray($bug_options_array,'bug_option',$data[0]['bug_option_id']);
	$result_pulldown=pulldownFromArray($results_array,'bug_result',$data[0]['bug_result_id']);
	$users=getAssocArray('Users','id',"CONCAT(username,' - ',email)",$conn,'','ORDER BY username ASC,email ASC')+array(-1=>'Not in the list, add new user');
	$users_pulldown=pulldownFromArray($users,'user_id',$data[0]['user_id'],'onChange="display_new_user(document.incident_form.user_id.value);"');
	
	$variables=array();
	$variables=set_variable($variables,'action','edit_incident.php');
	$variables=set_variable($variables,'form_title','Edit incident #'.$id.' (reported by '.$data[0]['Staff'].')');
	$variables=set_variable($variables,'id',$id);
	$variables=set_variable($variables,'date_reported',$data[0]['date_reported']);
	$variables=set_variable($variables,'username',$data[0]['username']);
	$variables=set_variable($variables,'email',$data[0]['email']);
	$variables=set_variable($variables,'description',$data[0]['description']);
	$variables=set_variable($variables,'resolution',$data[0]['resolution']);
	$variables=set_variable($variables,'workaround',$data[0]['workaround']);
	$variables=set_variable($variables,'bug_report',$data[0]['bug_report']);
	$variables=set_variable($variables,'status_pulldown',$status_pulldown);
	$variables=set_variable($variables,'bug_option',$bug_options_pulldown);
	$variables=set_variable($variables,'bug_result',$result_pulldown);
	$variables=set_variable($variables,'contact_rows',get_contact_rows($id,$conn));
	$variables=set_variable($variables,'delete_button','<input type="submit" name="del" value="Delete" onclick="if(!confirm(\'Are you sure you want to delete this incident?\'))return false;">');
	$variables=set_variable($variables,'users_pulldown',$users_pulldown.'<br>');
	$variables=set_variable($variables,'from',@$_REQUEST['from']);

	if(isset($_SESSION['notification'])){
		$variables=set_variable($variables,'notification',$_SESSION['notification']);
		unset($_SESSION['notification']);
	}
	
	return outputHTML($variables,$page_template,1);	
}

function get_contact_table($id,$conn){
	$contacts=getFields('Contacts','INNER JOIN Staff on Staff.id=staff_id WHERE incident_id='.$id,$conn,'ORDER BY contact_date DESC','Contacts.*,Staff.username AS staff');
	if(count($contacts)==0){
		return '';
	}
	$out=implode('',file('templates/previous_contacts.html'));
	foreach ($contacts AS $contact){
		$out.=contact_row($contact);
	}
	
	return $out;
}

function contact_row($contact){
	$page_template=implode('',file('templates/repeat_contacts.html'));
	
	$variables=array();
	$variables=set_variable($variables,'id',$contact['id']);
	$variables=set_variable($variables,'duration',$contact['duration']);
	$variables=set_variable($variables,'comments',$contact['comments']);
	$variables=set_variable($variables,'staff',$contact['staff']);
	
	return outputHTML($variables,$page_template,1);	
}

function get_contact_rows($id,$conn){
	$page_template=implode('',file('templates/add_contact_form.html'));
	
	$variables=array();
	$variables=set_variable($variables,'contacts',get_contact_table($id,$conn));
	
	return outputHTML($variables,$page_template,1);		
}

function process_edit_incident($conn){
	$id=(int)@$_REQUEST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('Incidents','INNER JOIN Users ON Users.id=user_id LEFT JOIN BugOptions ON BugOptions.id=bug_option_id LEFT JOIN BugResults ON BugResults.id=bug_result_id WHERE Incidents.id='.$id,$conn);
	if(count($data)==0){
		return 'Invalid ID';
	}
		
	$username=cleanString($_POST['username']);
	$email=cleanString($_POST['email']);
	$date_reported=cleanString($_POST['date_reported']);
	$description=cleanString($_POST['description']);
	$resolution=cleanString($_POST['resolution']);
	$workaround=cleanString($_POST['workaround']);
	$bug_report=cleanString(@$_POST['bug_report']);
	$bug_option_id=(int)@$_POST['bug_option'];
	$bug_option_id=($bug_option_id==0)?NULL:$bug_option_id;
	$bug_result_id=(int)@$_POST['bug_result'];
	$status_id=(int)@$_POST['status'];
	$user_id=(int)@$_POST['user_id'];
	$staff_id=(int)@$_SESSION['staffID'];
	$from=$_POST['from'];
	
	$duration=(int)@$_POST['duration'];
	$comments=cleanString($_POST['comments']);
	
	if($user_id==-1 && ($username=='' && $email=='')){
		$_SESSION['notification']='Pick an username from drop down or type the YM/Skype/Forum ID or email address.';
		return edit_incident_form($conn);
	}
	
	query("UPDATE Users SET username='$username', email='$email' WHERE id=".$data[0]['user_id'],$conn);
	
	query("UPDATE Incidents SET date_reported='$date_reported',description='$description',resolution='$resolution',workaround='$workaround',bug_report='$bug_report',bug_option_id='$bug_option_id',bug_result_id=$bug_result_id,`status_id`=$status_id WHERE id=$id",$conn);

	// update contacts
	$contacts=array_keys(getAssocArray('Contacts','id','id',$conn,'WHERE incident_id='.$id));

	foreach ($contacts AS $cid){
		if(isset($_POST['del_'.$cid])){
			query("DELETE FROM Contacts WHERE id=$cid",$conn);
			
			header("Location: edit_incident.php?id=".$id);
			exit();
			
		}else{
			if(isset($_POST['duration_'.$cid])){
				$cduration=(int)@$_POST['duration_'.$cid];
				$ccomments=cleanString($_POST['comments_'.$cid]);
				query("UPDATE Contacts SET duration='$cduration',comments='$ccomments' WHERE id=$cid",$conn);
			}
		}
	}

	if($duration!=0 && $comments!=''){
		query("INSERT INTO Contacts (incident_id,contact_date,duration,comments,staff_id) VALUES ($id,NOW(),$duration,'$comments','$staff_id')",$conn);
	}	
	
	$_SESSION['notification']='The incident '.$id.' was updated.';
	header("Location: incidents.php?".base64_decode($from));
	exit();
}

function process_del_incident($conn){
	$id=(int)@$_POST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('Incidents','LEFT JOIN Users ON Users.id=user_id LEFT JOIN BugOptions ON BugOptions.id=bug_option_id LEFT JOIN BugResults ON BugResults.id=bug_result_id WHERE Incidents.id='.$id,$conn);
	if(count($data)==0){
		return 'Invalid ID';
	}

	query("DELETE FROM Incidents WHERE id=$id",$conn);
	query("DELETE FROM Contacts WHERE incident_id=$id",$conn);
	
	return 'The incident #'.$id.' was deleted.<br>
	<a href="incidents.php">Back to incidents</a>';	
}

function logout($conn){
	unset($_SESSION['staffID']);
	unset($_SESSION['staffName']);
	
	header("Location: login.php");
	exit();
}

function bug_options($conn){
	$action=@$_REQUEST['action'];
	switch ($action) {
		case 'add':
			if(isset($_POST['save'])){
				return process_add_bug_option($conn);
			}else{
				return add_bug_option_form();
			}
		break;
		case 'edit':
			if(isset($_POST['save'])){
				return process_edit_bug_option($conn);
			}else{
				return edit_bug_option_form($conn);
			}
		break;
	
		default:
			return bug_options_list($conn);
		break;
	}
}

function bug_options_list($conn){
	$fieldsArray=array(
		'id'=>'id',
		'bug_option'=>'Bug Option'
	);
	
	$extra_links=array(
		'bug_options.php?action=edit'=>'Edit'
	);
	
	$out='
	<a href="index.php"> &lt;&lt; Back to index</a>
	<p class="normal_row"><b>Bug options</b><br>
	'.multi_page($fieldsArray,'BugOptions','','ORDER BY bug_option ASC','bug_options',$conn,'','','400',10,$extra_links);
	$out.='<br><br>
	<a href="bug_options.php?action=add"> Add bug option</a><br>';

	return $out;
}

function add_bug_option_form(){
	
	// main content
	$page_template=implode('',file('templates/bug_option_form.html'));
	
	$variables=array();
	$variables=set_variable($variables,'action','add');
	$variables=set_variable($variables,'form_title','Add bug option');
	
	if(isset($_SESSION['form_error'])){
		$variables=set_variable($variables,'form_error',$_SESSION['form_error']);
		unset($_SESSION['form_error']);
	}
	

	return outputHTML($variables,$page_template,1);	
}

function process_add_bug_option($conn){
	$bug_option=cleanString($_POST['bug_option']);
	if($bug_option==''){
		$_SESSION['form_error']='Please type bug option.';
		return add_bug_option_form();
	}
	
	query("INSERT INTO BugOptions (bug_option) VALUES ('$bug_option')",$conn);
	
	return 'The bug option was added.<br><a href="bug_options.php">Back to bug options</a>';
}

function edit_bug_option_form($conn){
	$id=(int)@$_REQUEST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('BugOptions','WHERE id='.$id,$conn);
	if(count($data)==0){
		return 'Invalid ID';
	}
	
	// main content
	$page_template=implode('',file('templates/bug_option_form.html'));
	
	$variables=array();
	$variables=set_variable($variables,'action','edit');
	$variables=set_variable($variables,'form_title','Edit bug option #'.$id);
	$variables=set_variable($variables,'bug_option',$data[0]['bug_option']);
	$variables=set_variable($variables,'id',$id);
	
	if(isset($_SESSION['form_error'])){
		$variables=set_variable($variables,'form_error',$_SESSION['form_error']);
		unset($_SESSION['form_error']);
	}
	

	return outputHTML($variables,$page_template,1);	
}

function process_edit_bug_option($conn){
	$bug_option=cleanString($_POST['bug_option']);
	$id=(int)$_POST['id'];
	
	if($bug_option==''){
		$_SESSION['form_error']='Please type bug option.';
		return edit_bug_option_form($conn);
	}
	
	query("UPDATE BugOptions SET bug_option='$bug_option' WHERE id=$id",$conn);
	
	return 'The bug option was updated.<br><a href="bug_options.php">Back to bug options</a>';
}


function bug_results($conn){
	$action=@$_REQUEST['action'];
	switch ($action) {
		case 'add':
			if(isset($_POST['save'])){
				return process_add_bug_result($conn);
			}else{
				return add_bug_result_form();
			}
		break;
		case 'edit':
			if(isset($_POST['save'])){
				return process_edit_bug_result($conn);
			}else{
				return edit_bug_result_form($conn);
			}
		break;
	
		default:
			return bug_results_list($conn);
		break;
	}
}

function bug_results_list($conn){
	$fieldsArray=array(
		'id'=>'id',
		'bug_result'=>'Bug Result'
	);
	
	$extra_links=array(
		'bug_results.php?action=edit'=>'Edit'
	);
	
	$out='
	<a href="index.php"> &lt;&lt; Back to index</a>
	<p class="normal_row"><b>Bug options</b><br>
	'.multi_page($fieldsArray,'BugResults','','ORDER BY bug_result ASC','bug_results',$conn,'','','400',10,$extra_links);
	$out.='<br><br>
	<a href="bug_results.php?action=add"> Add bug result</a><br>';

	return $out;
}

function add_bug_result_form(){
	
	// main content
	$page_template=implode('',file('templates/bug_result_form.html'));
	
	$variables=array();
	$variables=set_variable($variables,'action','add');
	$variables=set_variable($variables,'form_title','Add bug result');
	
	if(isset($_SESSION['form_error'])){
		$variables=set_variable($variables,'form_error',$_SESSION['form_error']);
		unset($_SESSION['form_error']);
	}
	

	return outputHTML($variables,$page_template,1);	
}

function process_add_bug_result($conn){
	$bug_result=cleanString($_POST['bug_result']);
	if($bug_result==''){
		$_SESSION['form_error']='Please type bug option.';
		return add_bug_result_form();
	}
	
	query("INSERT INTO BugResults (bug_result) VALUES ('$bug_result')",$conn);
	
	return 'The bug result was added.<br><a href="bug_results.php">Back to bug results</a>';
}

function edit_bug_result_form($conn){
	$id=(int)@$_REQUEST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('BugResults','WHERE id='.$id,$conn);
	if(count($data)==0){
		return 'Invalid ID';
	}
	
	// main content
	$page_template=implode('',file('templates/bug_result_form.html'));
	
	$variables=array();
	$variables=set_variable($variables,'action','edit');
	$variables=set_variable($variables,'form_title','Edit bug result #'.$id);
	$variables=set_variable($variables,'bug_result',$data[0]['bug_result']);
	$variables=set_variable($variables,'id',$id);
	
	if(isset($_SESSION['form_error'])){
		$variables=set_variable($variables,'form_error',$_SESSION['form_error']);
		unset($_SESSION['form_error']);
	}
	

	return outputHTML($variables,$page_template,1);	
}

function process_edit_bug_result($conn){
	$bug_result=cleanString($_POST['bug_result']);
	$id=(int)$_POST['id'];
	
	if($bug_result==''){
		$_SESSION['form_error']='Please type bug option.';
		return edit_bug_result_form($conn);
	}
	
	query("UPDATE BugResults SET bug_result='$bug_result' WHERE id=$id",$conn);
	
	return 'The bug result was updated.<br><a href="bug_results.php">Back to bug options</a>';
}

function incident_details($conn){
	$id=(int)@$_REQUEST['id'];
	if($id==0){
		return 'Invalid ID';
	}
	$data=getFields('Incidents','INNER JOIN Users ON Users.id=user_id LEFT JOIN BugOptions ON BugOptions.id=bug_option_id INNER JOIN BugResults ON BugResults.id=bug_result_id WHERE Incidents.id='.$id,$conn);
	if(count($data)==0){
		return 'Invalid ID';
	}
	
	$page_template=implode('',file('templates/incident_details.html'));
	
	$bug_label=($data[0]['bug_option']!=0)?$data[0]['bug_option']:'Not specified';
	$bug_report=(strpos($data[0]['bug_report'],'http://')===false)?$bug_label.': '.$data[0]['bug_report']:'<a href="'.$data[0]['bug_report'].'">'.$bug_label.'</a>';
	
	$variables=array();
	$variables=set_variable($variables,'action','edit_incident.php');
	$variables=set_variable($variables,'title','Incident #'.$id);
	$variables=set_variable($variables,'date_reported',$data[0]['date_reported']);
	$variables=set_variable($variables,'username',$data[0]['username']);
	$variables=set_variable($variables,'email',$data[0]['email']);
	$variables=set_variable($variables,'description',nl2br($data[0]['description']));
	$variables=set_variable($variables,'resolution',nl2br($data[0]['resolution']));
	$variables=set_variable($variables,'workaround',nl2br($data[0]['workaround']));
	$variables=set_variable($variables,'bug_report',$bug_report);
	$variables=set_variable($variables,'status',$GLOBALS['incident_status_array'][$data[0]['status_id']]);
	$variables=set_variable($variables,'bug_result',$data[0]['bug_result']);
	
	$variables=set_variable($variables,'contacts_label','Contacts');
	$variables=set_variable($variables,'contacts_table',get_contacts_table($id,$conn));

	return outputHTML($variables,$page_template,1);	
}


function get_contacts_table($id,$conn){
	$res=query("SELECT * FROM Contacts WHERE incident_id=$id ORDER BY contact_date DESC",$conn);
	if(mysql_num_rows($res)==0){
		return 'No contacts';
	}
	
	$out='<B>Contacts</B>
	<table>';
	while($row=mysql_fetch_assoc($res)){
		$out.='
		<tr>
			<td><em>'.$row['contact_date'].' ('.$row['duration'].')</em></td>
		</tr>
		<tr>
			<td>'.nl2br($row['comments']).'</td>
		</tr>
		
		';	
	}
	$out.='</table>';
	
	return $out;
}
?>