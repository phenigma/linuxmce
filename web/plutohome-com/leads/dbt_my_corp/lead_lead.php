<?
// CUSTOM L4
require_once('dbt_my_corp/l3/lead_lead.php');
class DBT_Lead_Row extends DB3_Lead_Row
{
	function GET_Full_Address()
	{
		$address = '';
		if (!is_null($this->GET_Address1())) $address .= $this->GET_Address1();
		if (!is_null($this->GET_Address2())) $address .= '<br />'.$this->GET_Address2();
		if (!is_null($this->GET_City())) $address .= '<br />'.$this->GET_City();
		if (!is_null($this->GET_State())) $address .= ', '.$this->GET_State();
		if (!is_null($this->GET_Postal_Code())) $address .= ' '.$this->GET_Postal_Code();
		if (!is_null($this->GET_Country())) $address .= '<br />'.$this->GET_Country();
		return $address;
	}
}
class DBT_Lead_Table extends DB3_Lead_Table
{
	function DBT_Lead_Table(&$db)
	{
		parent::DB3_Lead_Table($db);
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true,7,30);
	}
}
$DB_Lead_Table =& new DBT_Lead_Table($GLOBALS['db_my']);
$GLOBALS['DB_Lead_Table'] =& $DB_Lead_Table;
?>