<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_step.php');
class DBT_SALES_Step_Row extends DB3_SALES_Step_Row
{
}
class DBT_SALES_Step_Table extends DB3_SALES_Step_Table
{
}
$DB_SALES_Step_Table =& new DBT_SALES_Step_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_Step_Table'] =& $DB_SALES_Step_Table;
