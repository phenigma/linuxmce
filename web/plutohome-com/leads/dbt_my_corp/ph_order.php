<?
// CUSTOM L4
require_once('dbt_my_corp/l3/ph_order.php');
class DBT_PH_Order_Row extends DB3_PH_Order_Row
{
}
class DBT_PH_Order_Table extends DB3_PH_Order_Table
{
	function DBT_PH_Order_Table(&$db)
	{
		parent::DB3_PH_Order_Table($db);
		$this->fields['BillState'] =& new DB_Field_Varchar('BillState',true,2,2);
		$this->fields['ShipState'] =& new DB_Field_Varchar('ShipState',true,2,2);
		$this->fields['CC'] =& new DB_Field_Varchar('CC',true,16,16);
		$this->fields['CCExp'] =& new DB_Field_Varchar('CCExp',true,5,5);
		$this->fields['CCCode'] =& new DB_Field_Int('CCCode',true,0,9999,4);
	}
}
$DB_PH_Order_Table =& new DBT_PH_Order_Table($GLOBALS['db_my']);
$GLOBALS['DB_PH_Order_Table'] =& $DB_PH_Order_Table;
