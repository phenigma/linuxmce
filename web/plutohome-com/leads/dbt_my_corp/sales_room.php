<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_room.php');
class DBT_SALES_Room_Row extends DB3_SALES_Room_Row
{
}
class DBT_SALES_Room_Table extends DB3_SALES_Room_Table
{
}
$DB_SALES_Room_Table =& new DBT_SALES_Room_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Room_Table'] =& $DB_SALES_Room_Table;
