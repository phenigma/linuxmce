<?
// CUSTOM L4
require_once('dbt_my_corp/l3/lead_actionpending.php');
class DBT_ActionPending_Row extends DB3_ActionPending_Row
{
}
class DBT_ActionPending_Table extends DB3_ActionPending_Table
{
	function DBT_ActionPending_Table(&$db)
	{
		parent::DB3_ActionPending_Table($db);
		$this->fields['FKID_Employee'] =& new DB_Field_FK('FKID_Employee',true,'Employee','PKID_Employee','FirstName',$db,'DB_Field_Int');
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true,7,30);
	}
}
$DB_ActionPending_Table =& new DBT_ActionPending_Table($GLOBALS['db_my']);
$GLOBALS['DB_ActionPending_Table'] =& $DB_ActionPending_Table;
?>