<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/emaillist.php');
class DBT_EmailList_Row extends DB3_EmailList_Row
{
}
class DBT_EmailList_Table extends DB3_EmailList_Table
{
}
$DB_EmailList_Table =& new DBT_EmailList_Table($GLOBALS['db_my']);
$GLOBALS['DB_EmailList_Table'] =& $DB_EmailList_Table;
