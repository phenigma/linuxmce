<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/messagelist.php');
class DBT_MessageList_Row extends DB3_MessageList_Row
{
}
class DBT_MessageList_Table extends DB3_MessageList_Table
{
}
$DB_MessageList_Table =& new DBT_MessageList_Table($GLOBALS['db_my']);
$GLOBALS['DB_MessageList_Table'] =& $DB_MessageList_Table;
?>