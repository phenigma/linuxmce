<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_roomtype.php');
class DBT_SALES_RoomType_Row extends DB3_SALES_RoomType_Row
{
}
class DBT_SALES_RoomType_Table extends DB3_SALES_RoomType_Table
{
}
$DB_SALES_RoomType_Table =& new DBT_SALES_RoomType_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_RoomType_Table'] =& $DB_SALES_RoomType_Table;
