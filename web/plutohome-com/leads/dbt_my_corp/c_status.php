<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/c_status.php');
class DBT_C_Status_Row extends DB3_C_Status_Row
{
}
class DBT_C_Status_Table extends DB3_C_Status_Table
{
}
$DB_C_Status_Table =& new DBT_C_Status_Table($GLOBALS['db_my']);
$GLOBALS['DB_C_Status_Table'] =& $DB_C_Status_Table;
