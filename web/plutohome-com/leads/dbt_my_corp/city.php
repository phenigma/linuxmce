<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/city.php');
class DBT_City_Row extends DB3_City_Row
{
}
class DBT_City_Table extends DB3_City_Table
{
}
$DB_City_Table =& new DBT_City_Table($GLOBALS['db_my']);
$GLOBALS['DB_City_Table'] =& $DB_City_Table;
