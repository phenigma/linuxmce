<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_PH_CCType
{
	// CURRENT DEFINES:
	var $AMEX = 3;
	var $DISCO = 4;
	var $MC = 2;
	var $VISA = 1;
}
$DB_PH_CCType =& new DBT_PH_CCType();
$GLOBALS['DB_PH_CCType'] =& $DB_PH_CCType;

class DB3_PH_CCType_Row extends DB_Row_Const
{
	/**
	 * @return DB3_PH_CCType_Row
	 * @param $PKID_PH_CCType str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_PH_CCType can also be an Array, an Object, or an ID-key
	 */
	function DB3_PH_CCType_Row($PKID_PH_CCType)
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row_Const('DB_PH_CCType_Table',$PKID_PH_CCType);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from PH_Order by FKID_PH_CCType
	 */
	function GETCHLD_PH_Order__FKID_PH_CCType()
	{
		return parent::PRT_get_field_children('FKID_PH_CCType','PH_Order');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_PH_CCType
	 */
	function GET_PKID_PH_CCType($as_string=false)
	{
		return parent::PRT_get_field('PKID_PH_CCType',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Name
	 */
	function GET_Name($as_string=false)
	{
		return parent::PRT_get_field('Name',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Define
	 */
	function GET_Define($as_string=false)
	{
		return parent::PRT_get_field('Define',$as_string);
	}
}

class DB3_PH_CCType_Table extends DB_Table
{
	function DB3_PH_CCType_Table(&$db)
	{
		parent::DB_Table($db,'PH_CCType','PKID_PH_CCType',true);

		$this->fields['PKID_PH_CCType'] =& new DB_Field_Int('PKID_PH_CCType',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);

		parent::PRT_add_pk_ref('PH_Order','FKID_PH_CCType',false);
	}

	/**
	 * @desc STATIC: get the tablename 'PH_CCType' 
	 */
	function S_get_tablename()
	{
		return 'PH_CCType';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_PH_CCType'=>1,'Name'=>1,'Define'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_PH_CCType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_PH_CCType($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_PH_CCType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_PH_CCType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_PH_CCType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_PH_CCType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_PH_CCType Required ?
	 */
	function ISREQ_PKID_PH_CCType()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Name column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Name($id_keys=false)
	{
		return parent::PRT_fetch_col('Name',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Name in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Name($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Name',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Name Required ?
	 */
	function ISREQ_Name()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Define column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Define($id_keys=false)
	{
		return parent::PRT_fetch_col('Define',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Define in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Define($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Define',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Define Required ?
	 */
	function ISREQ_Define()
	{
		return true;
	}
}
