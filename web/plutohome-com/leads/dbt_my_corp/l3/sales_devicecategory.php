<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_SALES_DeviceCategory
{
	// CURRENT DEFINES:
	var $ADD_ONS = 3;
	var $CLIMATE_CONTROL = 7;
	var $CONTROLLERS = 5;
	var $LIGHTING_CONTROLS = 1;
	var $MEDIA_CONSOLES = 4;
	var $PHONE_SYSTEM = 6;
	var $SERVER = 2;
}
$DB_SALES_DeviceCategory =& new DBT_SALES_DeviceCategory();
$GLOBALS['DB_SALES_DeviceCategory'] =& $DB_SALES_DeviceCategory;

class DB3_SALES_DeviceCategory_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_DeviceCategory_Row
	 * @param $PKID_SALES_DeviceCategory str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_DeviceCategory can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_DeviceCategory_Row($PKID_SALES_DeviceCategory='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_DeviceCategory_Table',$PKID_SALES_DeviceCategory);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device by FKID_SALES_DeviceCategory
	 */
	function GETCHLD_SALES_Device__FKID_SALES_DeviceCategory()
	{
		return parent::PRT_get_field_children('FKID_SALES_DeviceCategory','SALES_Device');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_DeviceCategory
	 */
	function GET_PKID_SALES_DeviceCategory($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_DeviceCategory',$as_string);
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
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define
	 */
	function GETFF_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define as an HTML_Tag object
	 */
	function & GETFFO_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Define
	 */
	function SET_Define($value,$from_form=true)
	{
		return parent::PRT_set_field('Define',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Define
	 */
	function ORIG_Define($as_string=false)
	{
		return parent::PRT_get_orig('Define',$as_string);
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
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Name
	 */
	function GETFF_Name($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Name',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Name as an HTML_Tag object
	 */
	function & GETFFO_Name($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Name',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Name
	 */
	function SET_Name($value,$from_form=true)
	{
		return parent::PRT_set_field('Name',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Name
	 */
	function ORIG_Name($as_string=false)
	{
		return parent::PRT_get_orig('Name',$as_string);
	}
}

class DB3_SALES_DeviceCategory_Table extends DB_Table
{
	function DB3_SALES_DeviceCategory_Table(&$db)
	{
		parent::DB_Table($db,'SALES_DeviceCategory','PKID_SALES_DeviceCategory',true);

		$this->fields['PKID_SALES_DeviceCategory'] =& new DB_Field_Int('PKID_SALES_DeviceCategory',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);

		parent::PRT_add_pk_ref('SALES_Device','FKID_SALES_DeviceCategory',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_DeviceCategory' 
	 */
	function S_get_tablename()
	{
		return 'SALES_DeviceCategory';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_DeviceCategory'=>1,'Define'=>1,'Name'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_DeviceCategory column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_DeviceCategory($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_DeviceCategory',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_DeviceCategory in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_DeviceCategory($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_DeviceCategory',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_DeviceCategory Required ?
	 */
	function ISREQ_PKID_SALES_DeviceCategory()
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
}
