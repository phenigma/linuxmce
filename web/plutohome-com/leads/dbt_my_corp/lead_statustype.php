<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/lead_statustype.php');
class DBT_Status_Row extends DB3_Status_Row
{
}
class DBT_Status_Table extends DB3_Status_Table
{
}
$DB_Status_Table =& new DBT_Status_Table($GLOBALS['db_my']);
$GLOBALS['DB_Status_Table'] =& $DB_Status_Table;
