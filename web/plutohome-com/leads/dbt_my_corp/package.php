<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/package.php');
class DBT_Package_Row extends DB3_Package_Row
{
}
class DBT_Package_Table extends DB3_Package_Table
{
}
$DB_Package_Table =& new DBT_Package_Table($GLOBALS['db_my']);
$GLOBALS['DB_Package_Table'] =& $DB_Package_Table;
