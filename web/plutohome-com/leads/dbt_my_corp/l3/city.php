<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_City_Row extends DB_Row
{
	/**
	 * @return DB3_City_Row
	 * @param $PKID_City str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_City can also be an Array, an Object, or an ID-key
	 */
	function DB3_City_Row($PKID_City='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_City_Table',$PKID_City);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Installation by FKID_City
	 */
	function GETCHLD_SALES_Installation__FKID_City()
	{
		return parent::PRT_get_field_children('FKID_City','SALES_Installation');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_City
	 */
	function GET_PKID_City($as_string=false)
	{
		return parent::PRT_get_field('PKID_City',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of FKID_State
	 */
	function GET_FKID_State($as_string=false)
	{
		return parent::PRT_get_field('FKID_State',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_State
	 */
	function GETFF_FKID_State($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_State',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_State as an HTML_Tag object
	 */
	function & GETFFO_FKID_State($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_State',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of FKID_State
	 */
	function SET_FKID_State($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_State',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_State
	 */
	function ORIG_FKID_State($as_string=false)
	{
		return parent::PRT_get_orig('FKID_State',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_State to $match_arr
	 */
	function SETFLTR_FKID_State($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_State',$match_arr,$field_wide);
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

class DB3_City_Table extends DB_Table
{
	function DB3_City_Table(&$db)
	{
		parent::DB_Table($db,'City','PKID_City',true);

		$this->fields['PKID_City'] =& new DB_Field_Int('PKID_City',false);
		$this->fields['FKID_State'] =& new DB_Field_FK('FKID_State',false,'State','PKID_State','Name',$db,'DB_Field_Varchar');
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);

		parent::PRT_add_pk_ref('SALES_Installation','FKID_City',false);
	}

	/**
	 * @desc STATIC: get the tablename 'City' 
	 */
	function S_get_tablename()
	{
		return 'City';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_City'=>1,'FKID_State'=>1,'Name'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_City column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_City($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_City',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_City in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_City($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_City',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_City Required ?
	 */
	function ISREQ_PKID_City()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_State column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_State($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_State',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_State in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_State($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_State',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_State to $match_arr
	 */
	function SETFLTR_FKID_State($match_arr)
	{
		parent::PRT_set_filter('FKID_State',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_State Required ?
	 */
	function ISREQ_FKID_State()
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
