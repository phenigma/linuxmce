<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/startuppackages.php');
class DBT_StartupPackages_Row extends DB3_StartupPackages_Row
{
}
class DBT_StartupPackages_Table extends DB3_StartupPackages_Table
{
}
$DB_StartupPackages_Table =& new DBT_StartupPackages_Table($GLOBALS['db_my']);
$GLOBALS['DB_StartupPackages_Table'] =& $DB_StartupPackages_Table;
