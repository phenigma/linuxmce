<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_existingequipment.php');
class DBT_SALES_ExistingEquipment_Row extends DB3_SALES_ExistingEquipment_Row
{
}
class DBT_SALES_ExistingEquipment_Table extends DB3_SALES_ExistingEquipment_Table
{
}
$DB_SALES_ExistingEquipment_Table =& new DBT_SALES_ExistingEquipment_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_ExistingEquipment_Table'] =& $DB_SALES_ExistingEquipment_Table;
