<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_device_installation.php');
class DBT_SALES_Device_Installation_Row extends DB3_SALES_Device_Installation_Row
{
}
class DBT_SALES_Device_Installation_Table extends DB3_SALES_Device_Installation_Table
{
}
$DB_SALES_Device_Installation_Table =& new DBT_SALES_Device_Installation_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Device_Installation_Table'] =& $DB_SALES_Device_Installation_Table;
