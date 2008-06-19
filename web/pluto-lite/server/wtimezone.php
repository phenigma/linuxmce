<?
// database settings
$GLOBALS['db_server']="localhost";
$GLOBALS['db_port']="3306";
$GLOBALS['db_user']="root";
$GLOBALS['db_pass']="";
$GLOBALS['db_database']="pluto_main";

// database functions
function db_connect($db_server, $db_port, $db_user, $db_pass, $db_database)
{
	$conn=@mysql_connect($db_server.":".$db_port, $db_user, $db_pass);
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

function db_query($query,$conn)
{
	if($GLOBALS['db_in_debug']==1){
		print('<hr>'.$query.'<hr>');
	}
	if (empty($query)) {
		return FALSE;
	}
	$result=mysql_query($query,$conn);
	if(!$result)
		message_die ("Database request failed: <ul>"."<li>Error no: ".mysql_errno()."<li>Error: ".mysql_error()."<li>SQL query: ".$query."</ul>\n",$GLOBALS['action_on_error']);
	else{
		return $result;
	}
}

function getAssocArray($table,$keyField,$labelField,$conn,$whereClause='',$orderClause='')
{
	$retArray=array();
	$res=db_query("SELECT $keyField,$labelField FROM $table $whereClause $orderClause",$conn);
	while($row=mysql_fetch_assoc($res)){
		$cleanField=cleanField(trim(str_replace('DISTINCT ','',$keyField)));
		$retArray[$row[$cleanField]]=$row[cleanField($labelField)];
	}
	return $retArray;
} 

function pulldownFromArray($contentArray,$name,$selectedValue,$extra='',$restricted=20)
{
	if(!is_array($contentArray))
		return '';	// error

	$selectedValuesArray=array();
	// for multi-select lists
	if(!is_array($selectedValue)){
		$selectedValuesArray[]=$selectedValue;
	}else{
		$selectedValuesArray=$selectedValue;
	}
	$out='<select name="'.$name.'" '.$extra.'>';
	foreach ($contentArray AS $key=>$label){
		$title='';
		if($restricted!=0 && strlen($label)>$restricted){
			$title=$label;
			$label=substr($label,0,$restricted).' ...';
		}		
		$out.='<option value="'.$key.'" '.((in_array($key,$selectedValuesArray))?'selected':'').' title="'.$title.'">'.$label.'</option>';
	}
	$out.='</select>';
	
	return $out;
}

// convert xxx.yyy or aaa.bbb as ccc to yyy or ccc
function cleanField($field){
	$cleanField=(strpos($field,' AS ')!==false)?substr($field,strpos($field,' AS ')+4):$field;
	
	return $cleanField;
}

// generic function to extract rows from a table
function getFields($table,$where,$conn,$order='',$fields='*'){
	$res=db_query("SELECT $fields FROM $table $where $order",$conn);
	$data=array();
	while($row=mysql_fetch_assoc($res)){
		$data[]=$row;
	}
	return $data;
}



// start db connection
$conn=db_connect($GLOBALS['db_server'], $GLOBALS['db_port'], $GLOBALS['db_user'], $GLOBALS['db_pass'], $GLOBALS['db_database']); 


// countryID param - return regions list JSON format
if(isset($_REQUEST['countryID']) && (int)$_REQUEST['countryID']!=0){
	$countryID=(int)$_REQUEST['countryID'];
	$regions=getFields('Region','WHERE FK_Country='.$countryID,$conn,'ORDER BY Region ASC','PK_Region,Region,FK_Country');
	$json='';
	for($i=0;$i<count($regions);$i++){
		$json.="\"{$regions[$i]['PK_Region']}\":\"".htmlentities($regions[$i]['Region'])."\",\n";
	}
	
	die("{\n$json\n}");
}

// regionID param - return cities list
if(isset($_REQUEST['regionID']) && (int)$_REQUEST['regionID']!=0){
	$regionID=(int)$_REQUEST['regionID'];
	$json='';
	$cities=getFields('City','WHERE FK_Region='.$regionID,$conn,'ORDER BY City ASC','PK_City,City,FK_Country,FK_Region,FK_Timezone');
	for($i=0;$i<count($cities);$i++){
		$json.="\"{$cities[$i]['FK_Timezone']}\":\"".htmlentities($cities[$i]['City'])."\",\n";
	}
	
	die("{\n$json\n}");
}

// timezones - return timezones list
if(isset($_REQUEST['timezones'])){
	$json='';
	$timezones=getFields('TimeZone','',$conn,'ORDER BY ZoneName ASC');
	for($i=0;$i<count($timezones);$i++){
		$json.="\"{$timezones[$i]['PK_TimeZone']}\":\"".htmlentities($timezones[$i]['ZoneName'])."\",\n";
	}
	
	die("{\n$json\n}");
}


// no params, return countries list json format
$json='';
$countries=getAssocArray('Country','PK_Country','Description',$conn,'','ORDER BY Description ASC');
foreach ($countries AS $countryID=>$countryName){
	$json.="\"$countryID\":\"".htmlentities(ucwords(strtolower($countryName)))."\",\n";

}
die("{\n$json\n}");

?>