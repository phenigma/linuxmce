<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_devicecategory.php');
class DBT_SALES_DeviceCategory_Row extends DB3_SALES_DeviceCategory_Row
{
}
class DBT_SALES_DeviceCategory_Table extends DB3_SALES_DeviceCategory_Table
{
}
$DB_SALES_DeviceCategory_Table =& new DBT_SALES_DeviceCategory_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_DeviceCategory_Table'] =& $DB_SALES_DeviceCategory_Table;
