<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_C_Status_Row extends DB_Row_Const
{
	/**
	 * @return DB3_C_Status_Row
	 * @param $PKID_Status str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Status can also be an Array, an Object, or an ID-key
	 */
	function DB3_C_Status_Row($PKID_Status)
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row_Const('DB_C_Status_Table',$PKID_Status);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from Events by FKID_C_Status
	 */
	function GETCHLD_Events__FKID_C_Status()
	{
		return parent::PRT_get_field_children('FKID_C_Status','Events');
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
	 * @desc Get Value of Description
	 */
	function GET_Description($as_string=false)
	{
		return parent::PRT_get_field('Description',$as_string);
	}
}

class DB3_C_Status_Table extends DB_Table
{
	function DB3_C_Status_Table(&$db)
	{
		parent::DB_Table($db,'C_Status','PKID_Status',true);

		$this->fields['PKID_Status'] =& new DB_Field_Int('PKID_Status',false);
		$this->fields['Description'] =& new DB_Field_Varchar('Description',false);

		parent::PRT_add_pk_ref('Events','FKID_C_Status',false);
	}

	/**
	 * @desc STATIC: get the tablename 'C_Status' 
	 */
	function S_get_tablename()
	{
		return 'C_Status';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Status'=>1,'Description'=>1);
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
