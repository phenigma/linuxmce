<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/log.php');
class DBT_Log_Row extends DB3_Log_Row
{
}
class DBT_Log_Table extends DB3_Log_Table
{
}
$DB_Log_Table =& new DBT_Log_Table($GLOBALS['db_my']);
$GLOBALS['DB_Log_Table'] =& $DB_Log_Table;
