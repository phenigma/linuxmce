<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_otherexistingequipment.php');
class DBT_SALES_OtherExistingEquipment_Row extends DB3_SALES_OtherExistingEquipment_Row
{
}
class DBT_SALES_OtherExistingEquipment_Table extends DB3_SALES_OtherExistingEquipment_Table
{
}
$DB_SALES_OtherExistingEquipment_Table =& new DBT_SALES_OtherExistingEquipment_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_OtherExistingEquipment_Table'] =& $DB_SALES_OtherExistingEquipment_Table;
