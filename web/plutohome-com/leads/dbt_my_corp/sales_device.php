<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_device.php');
class DBT_SALES_Device_Row extends DB3_SALES_Device_Row
{
}
class DBT_SALES_Device_Table extends DB3_SALES_Device_Table
{
}
$DB_SALES_Device_Table =& new DBT_SALES_Device_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Device_Table'] =& $DB_SALES_Device_Table;
