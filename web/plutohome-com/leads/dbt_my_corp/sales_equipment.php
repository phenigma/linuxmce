<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_equipment.php');
class DBT_SALES_Equipment_Row extends DB3_SALES_Equipment_Row
{
}
class DBT_SALES_Equipment_Table extends DB3_SALES_Equipment_Table
{
}
$DB_SALES_Equipment_Table =& new DBT_SALES_Equipment_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Equipment_Table'] =& $DB_SALES_Equipment_Table;
