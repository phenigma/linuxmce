<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/events.php');
class DBT_Events_Row extends DB3_Events_Row
{
}
class DBT_Events_Table extends DB3_Events_Table
{
}
$DB_Events_Table =& new DBT_Events_Table($GLOBALS['db_my']);
$GLOBALS['DB_Events_Table'] =& $DB_Events_Table;
