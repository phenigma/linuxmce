<?
// CUSTOM L4
require_once('dbt_my_corp/l3/lead_actionhistory.php');
class DBT_ActionHistory_Row extends DB3_ActionHistory_Row
{
}
class DBT_ActionHistory_Table extends DB3_ActionHistory_Table
{
	function DBT_ActionHistory_Table(&$db)
	{
		parent::DB3_ActionHistory_Table($db);
		$this->fields['FKID_Employee'] =& new DB_Field_FK('FKID_Employee',true,'Employee','PKID_Employee','FirstName',$db,'DB_Field_Int');
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true,7,30);
	}
}
$DB_ActionHistory_Table =& new DBT_ActionHistory_Table($GLOBALS['db_my']);
$GLOBALS['DB_ActionHistory_Table'] =& $DB_ActionHistory_Table;
?>