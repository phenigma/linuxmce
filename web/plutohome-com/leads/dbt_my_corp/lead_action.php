<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/lead_action.php');
class DBT_Action_Row extends DB3_Action_Row
{
}
class DBT_Action_Table extends DB3_Action_Table
{
}
$DB_Action_Table =& new DBT_Action_Table($GLOBALS['db_my']);
$GLOBALS['DB_Action_Table'] =& $DB_Action_Table;
