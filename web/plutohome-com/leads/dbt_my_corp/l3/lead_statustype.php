<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_StatusType
{
	// CURRENT DEFINES:
	var $ACTIVE = 1;
	var $CLOSED = 3;
	var $SEMI_ACTIVE = 2;
}
$DB_StatusType =& new DBT_StatusType();
$GLOBALS['DB_StatusType'] =& $DB_StatusType;

class DB3_StatusType_Row extends DB_Row
{
	/**
	 * @return DB3_StatusType_Row
	 * @param $PKID_StatusType str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_StatusType can also be an Array, an Object, or an ID-key
	 */
	function DB3_StatusType_Row($PKID_StatusType='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_StatusType_Table',$PKID_StatusType);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from Status by FKID_StatusType
	 */
	function GETCHLD_Status__FKID_StatusType()
	{
		return parent::PRT_get_field_children('FKID_StatusType','Status');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_StatusType
	 */
	function GET_PKID_StatusType($as_string=false)
	{
		return parent::PRT_get_field('PKID_StatusType',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Description
	 */
	function GET_Description($as_string=false)
	{
		return parent::PRT_get_field('Description',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Description
	 */
	function GETFF_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Description as an HTML_Tag object
	 */
	function & GETFFO_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Description
	 */
	function SET_Description($value,$from_form=true)
	{
		return parent::PRT_set_field('Description',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Description
	 */
	function ORIG_Description($as_string=false)
	{
		return parent::PRT_get_orig('Description',$as_string);
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
}

class DB3_StatusType_Table extends DB_Table
{
	function DB3_StatusType_Table(&$db)
	{
		parent::DB_Table($db,'StatusType','PKID_StatusType',true);

		$this->fields['PKID_StatusType'] =& new DB_Field_Int('PKID_StatusType',false);
		$this->fields['Description'] =& new DB_Field_Varchar('Description',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);

		parent::PRT_add_pk_ref('Status','FKID_StatusType',false);
	}

	/**
	 * @desc STATIC: get the tablename 'StatusType' 
	 */
	function S_get_tablename()
	{
		return 'StatusType';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_StatusType'=>1,'Description'=>1,'Define'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_StatusType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_StatusType($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_StatusType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_StatusType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_StatusType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_StatusType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_StatusType Required ?
	 */
	function ISREQ_PKID_StatusType()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Description column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Description($id_keys=false)
	{
		return parent::PRT_fetch_col('Description',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Description in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Description($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Description',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Description Required ?
	 */
	function ISREQ_Description()
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
