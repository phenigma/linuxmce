<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/curstatus.php');
class DBT_CurStatus_Row extends DB3_CurStatus_Row
{
}
class DBT_CurStatus_Table extends DB3_CurStatus_Table
{
}
$DB_CurStatus_Table =& new DBT_CurStatus_Table($GLOBALS['db_my']);
$GLOBALS['DB_CurStatus_Table'] =& $DB_CurStatus_Table;
