<?
// NOT CUSTOM L4
require_once('dbt_my_corp/l3/sales_equipmentcategory.php');
class DBT_SALES_EquipmentCategory_Row extends DB3_SALES_EquipmentCategory_Row
{
}
class DBT_SALES_EquipmentCategory_Table extends DB3_SALES_EquipmentCategory_Table
{
}
$DB_SALES_EquipmentCategory_Table =& new DBT_SALES_EquipmentCategory_Table($GLOBALS['db_my']);
$GLOBALS['DB_SALES_EquipmentCategory_Table'] =& $DB_SALES_EquipmentCategory_Table;
