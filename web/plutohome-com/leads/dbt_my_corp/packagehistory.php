<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/packagehistory.php');
class DBT_PackageHistory_Row extends DB3_PackageHistory_Row
{
}
class DBT_PackageHistory_Table extends DB3_PackageHistory_Table
{
}
$DB_PackageHistory_Table =& new DBT_PackageHistory_Table($GLOBALS['db_my']);
$GLOBALS['DB_PackageHistory_Table'] =& $DB_PackageHistory_Table;
