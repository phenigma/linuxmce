<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_LeadType
{
	// CURRENT DEFINES:
	var $CUSTOMER = 2;
	var $DEALER = 1;
	var $INVESTOR = 3;
	var $OEM = 6;
	var $OTHER = 5;
	var $REPRESENTATIVE = 4;
}
$DB_LeadType =& new DBT_LeadType();
$GLOBALS['DB_LeadType'] =& $DB_LeadType;

class DB3_LeadType_Row extends DB_Row
{
	/**
	 * @return DB3_LeadType_Row
	 * @param $PKID_LeadType str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_LeadType can also be an Array, an Object, or an ID-key
	 */
	function DB3_LeadType_Row($PKID_LeadType='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_LeadType_Table',$PKID_LeadType);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from Lead by FKID_LeadType
	 */
	function GETCHLD_Lead__FKID_LeadType()
	{
		return parent::PRT_get_field_children('FKID_LeadType','Lead');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_LeadType
	 */
	function GET_PKID_LeadType($as_string=false)
	{
		return parent::PRT_get_field('PKID_LeadType',$as_string);
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
}

class DB3_LeadType_Table extends DB_Table
{
	function DB3_LeadType_Table(&$db)
	{
		parent::DB_Table($db,'LeadType','PKID_LeadType',true);

		$this->fields['PKID_LeadType'] =& new DB_Field_Int('PKID_LeadType',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);
		$this->fields['Description'] =& new DB_Field_Varchar('Description',false);

		parent::PRT_add_pk_ref('Lead','FKID_LeadType',false);
	}

	/**
	 * @desc STATIC: get the tablename 'LeadType' 
	 */
	function S_get_tablename()
	{
		return 'LeadType';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_LeadType'=>1,'Define'=>1,'Description'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_LeadType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_LeadType($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_LeadType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_LeadType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_LeadType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_LeadType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_LeadType Required ?
	 */
	function ISREQ_PKID_LeadType()
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
}
