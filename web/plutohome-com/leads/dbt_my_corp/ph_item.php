<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/ph_item.php');
class DBT_PH_Item_Row extends DB3_PH_Item_Row
{
}
class DBT_PH_Item_Table extends DB3_PH_Item_Table
{
}
$DB_PH_Item_Table =& new DBT_PH_Item_Table($GLOBALS['db_my']);
$GLOBALS['DB_PH_Item_Table'] =& $DB_PH_Item_Table;
