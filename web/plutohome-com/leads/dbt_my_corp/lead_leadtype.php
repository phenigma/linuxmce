<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/lead_leadtype.php');
class DBT_LeadType_Row extends DB3_LeadType_Row
{
}
class DBT_LeadType_Table extends DB3_LeadType_Table
{
}
$DB_LeadType_Table =& new DBT_LeadType_Table($GLOBALS['db_my']);
$GLOBALS['DB_LeadType_Table'] =& $DB_LeadType_Table;
