<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_customer.php');
class DBT_SALES_Customer_Row extends DB3_SALES_Customer_Row
{
}
class DBT_SALES_Customer_Table extends DB3_SALES_Customer_Table
{
}
$DB_SALES_Customer_Table =& new DBT_SALES_Customer_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Customer_Table'] =& $DB_SALES_Customer_Table;
