<?
// CUSTOM L4
require_once('dbt_my_corp/l3/message.php');
class DBT_Message_Row extends DB3_Message_Row
{
}
class DBT_Message_Table extends DB3_Message_Table
{
	function DBT_Message_Table(&$db)
	{
		parent::DB3_Message_Table($db);
		$this->fields['Timestamp'] =& new DB_Field_DateTime('Timestamp',true);
	}
	
	/**
	 * @return bool
	 * @desc Is Timestamp Required ? // TODO: change to autogen: pull from field
	 */
	function ISREQ_Timestamp()
	{
		return false;
	}
}
$DB_Message_Table =& new DBT_Message_Table($GLOBALS['db_my']);
$GLOBALS['DB_Message_Table'] =& $DB_Message_Table;
?>