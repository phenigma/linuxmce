<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_Status
{
	// CURRENT DEFINES:
	var $ACTIVE_ACTIONREQUIRED = 1;
	var $ACTIVE_DEMOSETUP = 3;
	var $ACTIVE_WAITING = 2;
	var $CLOSED_DEALDONE = 9;
	var $CLOSED_DIDNT_LIKE = 7;
	var $CLOSED_NOT_SERIOUS = 6;
	var $CLOSED_OTHER = 8;
	var $CLOSED_TOO_EXPENSIVE = 5;
	var $SEMI_ACTIVE = 4;
}
$DB_Status =& new DBT_Status();
$GLOBALS['DB_Status'] =& $DB_Status;

class DB3_Status_Row extends DB_Row
{
	/**
	 * @return DB3_Status_Row
	 * @param $PKID_Status str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Status can also be an Array, an Object, or an ID-key
	 */
	function DB3_Status_Row($PKID_Status='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Status_Table',$PKID_Status);
		if ($PKID_Status==='') parent::PRT_set_fields(array('FKID_StatusType'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from Lead by FKID_Status
	 */
	function GETCHLD_Lead__FKID_Status()
	{
		return parent::PRT_get_field_children('FKID_Status','Lead');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Status
	 */
	function GET_PKID_Status($as_string=false)
	{
		return parent::PRT_get_field('PKID_Status',$as_string);
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

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_StatusType
	 */
	function GET_FKID_StatusType($as_string=false)
	{
		return parent::PRT_get_field('FKID_StatusType',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_StatusType
	 */
	function GETFF_FKID_StatusType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_StatusType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_StatusType as an HTML_Tag object
	 */
	function & GETFFO_FKID_StatusType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_StatusType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_StatusType
	 */
	function SET_FKID_StatusType($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_StatusType',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_StatusType
	 */
	function ORIG_FKID_StatusType($as_string=false)
	{
		return parent::PRT_get_orig('FKID_StatusType',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_StatusType to $match_arr
	 */
	function SETFLTR_FKID_StatusType($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_StatusType',$match_arr,$field_wide);
	}
}

class DB3_Status_Table extends DB_Table
{
	function DB3_Status_Table(&$db)
	{
		parent::DB_Table($db,'Status','PKID_Status',true);

		$this->fields['PKID_Status'] =& new DB_Field_Int('PKID_Status',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);
		$this->fields['Description'] =& new DB_Field_Varchar('Description',false);
		$this->fields['FKID_StatusType'] =& new DB_Field_FK('FKID_StatusType',false,'StatusType','PKID_StatusType','Description',$db,'DB_Field_Int');

		parent::PRT_add_pk_ref('Lead','FKID_Status',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Status' 
	 */
	function S_get_tablename()
	{
		return 'Status';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Status'=>1,'Define'=>1,'Description'=>1,'FKID_StatusType'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Status column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Status($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Status',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Status in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Status($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Status',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Status Required ?
	 */
	function ISREQ_PKID_Status()
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

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_StatusType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_StatusType($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_StatusType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_StatusType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_StatusType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_StatusType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_StatusType to $match_arr
	 */
	function SETFLTR_FKID_StatusType($match_arr)
	{
		parent::PRT_set_filter('FKID_StatusType',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_StatusType Required ?
	 */
	function ISREQ_FKID_StatusType()
	{
		return true;
	}
}
