<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_EmailList_Row extends DB_Row
{
	/**
	 * @return DB3_EmailList_Row
	 * @param $PKID_EmailList str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_EmailList can also be an Array, an Object, or an ID-key
	 */
	function DB3_EmailList_Row($PKID_EmailList='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_EmailList_Table',$PKID_EmailList);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_EmailList
	 */
	function GET_PKID_EmailList($as_string=false)
	{
		return parent::PRT_get_field('PKID_EmailList',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Email
	 */
	function GET_Email($as_string=false)
	{
		return parent::PRT_get_field('Email',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Email
	 */
	function GETFF_Email($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Email',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Email as an HTML_Tag object
	 */
	function & GETFFO_Email($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Email',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Email
	 */
	function SET_Email($value,$from_form=true)
	{
		return parent::PRT_set_field('Email',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Email
	 */
	function ORIG_Email($as_string=false)
	{
		return parent::PRT_get_orig('Email',$as_string);
	}
}

class DB3_EmailList_Table extends DB_Table
{
	function DB3_EmailList_Table(&$db)
	{
		parent::DB_Table($db,'EmailList','PKID_EmailList',true);

		$this->fields['PKID_EmailList'] =& new DB_Field_Int('PKID_EmailList',false);
		$this->fields['Email'] =& new DB_Field_Varchar('Email',false);
	}

	/**
	 * @desc STATIC: get the tablename 'EmailList' 
	 */
	function S_get_tablename()
	{
		return 'EmailList';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_EmailList'=>1,'Email'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_EmailList column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_EmailList($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_EmailList',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_EmailList in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_EmailList($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_EmailList',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_EmailList Required ?
	 */
	function ISREQ_PKID_EmailList()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Email column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Email($id_keys=false)
	{
		return parent::PRT_fetch_col('Email',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Email in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Email($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Email',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Email Required ?
	 */
	function ISREQ_Email()
	{
		return true;
	}
}
