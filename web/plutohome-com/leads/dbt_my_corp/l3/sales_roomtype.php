<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_RoomType_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_RoomType_Row
	 * @param $PKID_SALES_RoomType str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_RoomType can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_RoomType_Row($PKID_SALES_RoomType='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_RoomType_Table',$PKID_SALES_RoomType);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Room by FKID_SALES_RoomType
	 */
	function GETCHLD_SALES_Room__FKID_SALES_RoomType()
	{
		return parent::PRT_get_field_children('FKID_SALES_RoomType','SALES_Room');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_RoomType
	 */
	function GET_PKID_SALES_RoomType($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_RoomType',$as_string);
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

class DB3_SALES_RoomType_Table extends DB_Table
{
	function DB3_SALES_RoomType_Table(&$db)
	{
		parent::DB_Table($db,'SALES_RoomType','PKID_SALES_RoomType',true);

		$this->fields['PKID_SALES_RoomType'] =& new DB_Field_Int('PKID_SALES_RoomType',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);

		parent::PRT_add_pk_ref('SALES_Room','FKID_SALES_RoomType',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_RoomType' 
	 */
	function S_get_tablename()
	{
		return 'SALES_RoomType';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_RoomType'=>1,'Name'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_RoomType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_RoomType($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_RoomType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_RoomType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_RoomType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_RoomType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_RoomType Required ?
	 */
	function ISREQ_PKID_SALES_RoomType()
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
