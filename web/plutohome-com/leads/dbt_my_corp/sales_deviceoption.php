<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_deviceoption.php');
class DBT_SALES_DeviceOption_Row extends DB3_SALES_DeviceOption_Row
{
}
class DBT_SALES_DeviceOption_Table extends DB3_SALES_DeviceOption_Table
{
}
$DB_SALES_DeviceOption_Table =& new DBT_SALES_DeviceOption_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_DeviceOption_Table'] =& $DB_SALES_DeviceOption_Table;
