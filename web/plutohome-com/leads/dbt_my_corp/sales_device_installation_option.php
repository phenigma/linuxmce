<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_device_installation_option.php');
class DBT_SALES_Device_Installation_Option_Row extends DB3_SALES_Device_Installation_Option_Row
{
}
class DBT_SALES_Device_Installation_Option_Table extends DB3_SALES_Device_Installation_Option_Table
{
}
$DB_SALES_Device_Installation_Option_Table =& new DBT_SALES_Device_Installation_Option_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Device_Installation_Option_Table'] =& $DB_SALES_Device_Installation_Option_Table;
