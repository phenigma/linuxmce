<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_MessageList_Row extends DB_Row
{
	/**
	 * @return DB3_MessageList_Row
	 * @param PKID_MessageList str
	 * @desc Will return FALSE if a matching row is not found
	 * <br> PKID_MessageList can also be an Array, an Object, or an ID-key
	 */
	function DB3_MessageList_Row($PKID_MessageList='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_MessageList_Table',$PKID_MessageList);
		if ($PKID_MessageList==='') parent::PRT_set_fields(array('Severity'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from Message by FKID_MessageList
	 */
	function GETCHLD_Message__FKID_MessageList()
	{
		return parent::PRT_get_field_children('FKID_MessageList','Message');
	}

	/**
	 * @return int
	 * @param as_string bool
	 * @desc Get Value of PKID_MessageList
	 */
	function GET_PKID_MessageList($as_string=false)
	{
		return parent::PRT_get_field('PKID_MessageList',$as_string);
	}

	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Value of Define
	 */
	function GET_Define($as_string=false)
	{
		return parent::PRT_get_field('Define',$as_string);
	}
	/**
	 * @return str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Define
	 */
	function GETFF_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Define as an HTML_Tag object
	 */
	function GETFFO_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param value str
	 * @param from_form bool
	 * @desc Set Value of Define
	 */
	function SET_Define($value,$from_form=true)
	{
		return parent::PRT_set_field('Define',$value,$from_form);
	}
	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Original Value of Define
	 */
	function ORIG_Define($as_string=false)
	{
		return parent::PRT_get_orig('Define',$as_string);
	}

	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Value of Description
	 */
	function GET_Description($as_string=false)
	{
		return parent::PRT_get_field('Description',$as_string);
	}
	/**
	 * @return str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Description
	 */
	function GETFF_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Description as an HTML_Tag object
	 */
	function GETFFO_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param value str
	 * @param from_form bool
	 * @desc Set Value of Description
	 */
	function SET_Description($value,$from_form=true)
	{
		return parent::PRT_set_field('Description',$value,$from_form);
	}
	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Original Value of Description
	 */
	function ORIG_Description($as_string=false)
	{
		return parent::PRT_get_orig('Description',$as_string);
	}

	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Value of LongDescription
	 */
	function GET_LongDescription($as_string=false)
	{
		return parent::PRT_get_field('LongDescription',$as_string);
	}
	/**
	 * @return str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for LongDescription
	 */
	function GETFF_LongDescription($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('LongDescription',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for LongDescription as an HTML_Tag object
	 */
	function GETFFO_LongDescription($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('LongDescription',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param value str
	 * @param from_form bool
	 * @desc Set Value of LongDescription
	 */
	function SET_LongDescription($value,$from_form=true)
	{
		return parent::PRT_set_field('LongDescription',$value,$from_form);
	}
	/**
	 * @return str
	 * @param as_string bool
	 * @desc Get Original Value of LongDescription
	 */
	function ORIG_LongDescription($as_string=false)
	{
		return parent::PRT_get_orig('LongDescription',$as_string);
	}

	/**
	 * @return int
	 * @param as_string bool
	 * @desc Get Value of Severity
	 */
	function GET_Severity($as_string=false)
	{
		return parent::PRT_get_field('Severity',$as_string);
	}
	/**
	 * @return str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Severity
	 */
	function GETFF_Severity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Severity',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @desc Get Form Field for Severity as an HTML_Tag object
	 */
	function GETFFO_Severity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Severity',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param value int
	 * @param from_form bool
	 * @desc Set Value of Severity
	 */
	function SET_Severity($value,$from_form=true)
	{
		return parent::PRT_set_field('Severity',$value,$from_form);
	}
	/**
	 * @return int
	 * @param as_string bool
	 * @desc Get Original Value of Severity
	 */
	function ORIG_Severity($as_string=false)
	{
		return parent::PRT_get_orig('Severity',$as_string);
	}
}

class DB3_MessageList_Table extends DB_Table
{
	function DB3_MessageList_Table(&$db)
	{
		parent::DB_Table($db,'MessageList','PKID_MessageList',true);

		$this->fields['PKID_MessageList'] =& new DB_Field_Int('PKID_MessageList',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',true);
		$this->fields['Description'] =& new DB_Field_Varchar('Description',false);
		$this->fields['LongDescription'] =& new DB_Field_Text('LongDescription',true);
		$this->fields['Severity'] =& new DB_Field_Int('Severity',false);

		parent::PRT_add_pk_ref('Message','FKID_MessageList',false);
	}

	/**
	 * @desc STATIC: get the tablename 'MessageList' 
	 */
	function S_get_tablename()
	{
		return 'MessageList';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_MessageList'=>1,'Define'=>1,'Description'=>1,'LongDescription'=>1,'Severity'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param id_keys arr
	 * @desc Fetch PKID_MessageList column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_MessageList($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_MessageList',$id_keys);
	}
	/**
	 * @return arr
	 * @param values str|arr
	 * @param orderby str
	 * @param limit int
	 * @param start int
	 * @desc Load Table where PKID_MessageList in $values <br> returns $id_keys for use in FETCH_*() <br> NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_MessageList($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_MessageList',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_MessageList Required ?
	 */
	function ISREQ_PKID_MessageList()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param id_keys arr
	 * @desc Fetch Define column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Define($id_keys=false)
	{
		return parent::PRT_fetch_col('Define',$id_keys);
	}
	/**
	 * @return arr
	 * @param values str|arr
	 * @param orderby str
	 * @param limit int
	 * @param start int
	 * @desc Load Table where Define in $values <br> returns $id_keys for use in FETCH_*() <br> NOTE: $start NOT supported by MS-SQL
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
		return false;
	}

	/**
	 * @return arr
	 * @param id_keys arr
	 * @desc Fetch Description column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Description($id_keys=false)
	{
		return parent::PRT_fetch_col('Description',$id_keys);
	}
	/**
	 * @return arr
	 * @param values str|arr
	 * @param orderby str
	 * @param limit int
	 * @param start int
	 * @desc Load Table where Description in $values <br> returns $id_keys for use in FETCH_*() <br> NOTE: $start NOT supported by MS-SQL
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
	 * @param id_keys arr
	 * @desc Fetch LongDescription column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_LongDescription($id_keys=false)
	{
		return parent::PRT_fetch_col('LongDescription',$id_keys);
	}
	/**
	 * @return arr
	 * @param values str|arr
	 * @param orderby str
	 * @param limit int
	 * @param start int
	 * @desc Load Table where LongDescription in $values <br> returns $id_keys for use in FETCH_*() <br> NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_LongDescription($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('LongDescription',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is LongDescription Required ?
	 */
	function ISREQ_LongDescription()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param id_keys arr
	 * @desc Fetch Severity column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Severity($id_keys=false)
	{
		return parent::PRT_fetch_col('Severity',$id_keys);
	}
	/**
	 * @return arr
	 * @param values str|arr
	 * @param orderby str
	 * @param limit int
	 * @param start int
	 * @desc Load Table where Severity in $values <br> returns $id_keys for use in FETCH_*() <br> NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Severity($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Severity',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Severity Required ?
	 */
	function ISREQ_Severity()
	{
		return true;
	}
}
?>