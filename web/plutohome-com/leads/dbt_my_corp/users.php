<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/users.php');
class DBT_Users_Row extends DB3_Users_Row
{
}
class DBT_Users_Table extends DB3_Users_Table
{
}
$DB_Users_Table =& new DBT_Users_Table($GLOBALS['db_my']);
$GLOBALS['DB_Users_Table'] =& $DB_Users_Table;
