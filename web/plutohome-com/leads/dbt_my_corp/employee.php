<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/employee.php');
class DBT_Employee_Row extends DB3_Employee_Row
{
}
class DBT_Employee_Table extends DB3_Employee_Table
{
}
$DB_Employee_Table =& new DBT_Employee_Table($GLOBALS['db_my']);
$GLOBALS['DB_Employee_Table'] =& $DB_Employee_Table;
