<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_State_Row extends DB_Row
{
	/**
	 * @return DB3_State_Row
	 * @param $PKID_State str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_State can also be an Array, an Object, or an ID-key
	 */
	function DB3_State_Row($PKID_State='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_State_Table',$PKID_State);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from City by FKID_State
	 */
	function GETCHLD_City__FKID_State()
	{
		return parent::PRT_get_field_children('FKID_State','City');
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of PKID_State
	 */
	function GET_PKID_State($as_string=false)
	{
		return parent::PRT_get_field('PKID_State',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PKID_State
	 */
	function GETFF_PKID_State($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('PKID_State',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PKID_State as an HTML_Tag object
	 */
	function & GETFFO_PKID_State($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('PKID_State',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of PKID_State
	 */
	function SET_PKID_State($value,$from_form=true)
	{
		return parent::PRT_set_field('PKID_State',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of PKID_State
	 */
	function ORIG_PKID_State($as_string=false)
	{
		return parent::PRT_get_orig('PKID_State',$as_string);
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

class DB3_State_Table extends DB_Table
{
	function DB3_State_Table(&$db)
	{
		parent::DB_Table($db,'State','PKID_State',false);

		$this->fields['PKID_State'] =& new DB_Field_Varchar('PKID_State',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);

		parent::PRT_add_pk_ref('City','FKID_State',false);
	}

	/**
	 * @desc STATIC: get the tablename 'State' 
	 */
	function S_get_tablename()
	{
		return 'State';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_State'=>1,'Name'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_State column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_State($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_State',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_State in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_State($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_State',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_State Required ?
	 */
	function ISREQ_PKID_State()
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
