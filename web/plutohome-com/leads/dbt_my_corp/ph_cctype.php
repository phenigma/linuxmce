<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/ph_cctype.php');
class DBT_PH_CCType_Row extends DB3_PH_CCType_Row
{
}
class DBT_PH_CCType_Table extends DB3_PH_CCType_Table
{
}
$DB_PH_CCType_Table =& new DBT_PH_CCType_Table($GLOBALS['db_my']);
$GLOBALS['DB_PH_CCType_Table'] =& $DB_PH_CCType_Table;
