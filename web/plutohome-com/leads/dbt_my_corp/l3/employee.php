<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Employee_Row extends DB_Row
{
	/**
	 * @return DB3_Employee_Row
	 * @param $PKID_Employee str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Employee can also be an Array, an Object, or an ID-key
	 */
	function DB3_Employee_Row($PKID_Employee='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Employee_Table',$PKID_Employee);
		if ($PKID_Employee==='') parent::PRT_set_fields(array('ManagesLeads'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from ActionHistory by FKID_Employee
	 */
	function GETCHLD_ActionHistory__FKID_Employee()
	{
		return parent::PRT_get_field_children('FKID_Employee','ActionHistory');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from ActionPending by FKID_Employee
	 */
	function GETCHLD_ActionPending__FKID_Employee()
	{
		return parent::PRT_get_field_children('FKID_Employee','ActionPending');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Employee
	 */
	function GET_PKID_Employee($as_string=false)
	{
		return parent::PRT_get_field('PKID_Employee',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of FirstName
	 */
	function GET_FirstName($as_string=false)
	{
		return parent::PRT_get_field('FirstName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FirstName
	 */
	function GETFF_FirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FirstName as an HTML_Tag object
	 */
	function & GETFFO_FirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of FirstName
	 */
	function SET_FirstName($value,$from_form=true)
	{
		return parent::PRT_set_field('FirstName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of FirstName
	 */
	function ORIG_FirstName($as_string=false)
	{
		return parent::PRT_get_orig('FirstName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of LastName
	 */
	function GET_LastName($as_string=false)
	{
		return parent::PRT_get_field('LastName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastName
	 */
	function GETFF_LastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('LastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastName as an HTML_Tag object
	 */
	function & GETFFO_LastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('LastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of LastName
	 */
	function SET_LastName($value,$from_form=true)
	{
		return parent::PRT_set_field('LastName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of LastName
	 */
	function ORIG_LastName($as_string=false)
	{
		return parent::PRT_get_orig('LastName',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of ManagesLeads
	 */
	function GET_ManagesLeads($as_string=false)
	{
		return parent::PRT_get_field('ManagesLeads',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ManagesLeads
	 */
	function GETFF_ManagesLeads($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ManagesLeads',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ManagesLeads as an HTML_Tag object
	 */
	function & GETFFO_ManagesLeads($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ManagesLeads',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of ManagesLeads
	 */
	function SET_ManagesLeads($value,$from_form=true)
	{
		return parent::PRT_set_field('ManagesLeads',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of ManagesLeads
	 */
	function ORIG_ManagesLeads($as_string=false)
	{
		return parent::PRT_get_orig('ManagesLeads',$as_string);
	}
}

class DB3_Employee_Table extends DB_Table
{
	function DB3_Employee_Table(&$db)
	{
		parent::DB_Table($db,'Employee','PKID_Employee',true);

		$this->fields['PKID_Employee'] =& new DB_Field_Int('PKID_Employee',false);
		$this->fields['FirstName'] =& new DB_Field_Varchar('FirstName',false);
		$this->fields['LastName'] =& new DB_Field_Varchar('LastName',false);
		$this->fields['ManagesLeads'] =& new DB_Field_Int('ManagesLeads',false);

		parent::PRT_add_pk_ref('ActionHistory','FKID_Employee',false);
		parent::PRT_add_pk_ref('ActionPending','FKID_Employee',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Employee' 
	 */
	function S_get_tablename()
	{
		return 'Employee';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Employee'=>1,'FirstName'=>1,'LastName'=>1,'ManagesLeads'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Employee column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Employee($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Employee',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Employee in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Employee($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Employee',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Employee Required ?
	 */
	function ISREQ_PKID_Employee()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FirstName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FirstName($id_keys=false)
	{
		return parent::PRT_fetch_col('FirstName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FirstName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FirstName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FirstName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FirstName Required ?
	 */
	function ISREQ_FirstName()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch LastName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_LastName($id_keys=false)
	{
		return parent::PRT_fetch_col('LastName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where LastName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_LastName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('LastName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is LastName Required ?
	 */
	function ISREQ_LastName()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ManagesLeads column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ManagesLeads($id_keys=false)
	{
		return parent::PRT_fetch_col('ManagesLeads',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ManagesLeads in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ManagesLeads($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ManagesLeads',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ManagesLeads Required ?
	 */
	function ISREQ_ManagesLeads()
	{
		return true;
	}
}
