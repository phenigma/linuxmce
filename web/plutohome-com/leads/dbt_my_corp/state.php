<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/state.php');
class DBT_State_Row extends DB3_State_Row
{
}
class DBT_State_Table extends DB3_State_Table
{
}
$DB_State_Table =& new DBT_State_Table($GLOBALS['db_my']);
$GLOBALS['DB_State_Table'] =& $DB_State_Table;
