<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_Step_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Step_Row
	 * @param $PKID_SALES_Step str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Step can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Step_Row($PKID_SALES_Step='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Step_Table',$PKID_SALES_Step);
		if ($PKID_SALES_Step==='') parent::PRT_set_fields(array('Number'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Installation by FKID_SALES_Step
	 */
	function GETCHLD_SALES_Installation__FKID_SALES_Step()
	{
		return parent::PRT_get_field_children('FKID_SALES_Step','SALES_Installation');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Step
	 */
	function GET_PKID_SALES_Step($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Step',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Number
	 */
	function GET_Number($as_string=false)
	{
		return parent::PRT_get_field('Number',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Number
	 */
	function GETFF_Number($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Number',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Number as an HTML_Tag object
	 */
	function & GETFFO_Number($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Number',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Number
	 */
	function SET_Number($value,$from_form=true)
	{
		return parent::PRT_set_field('Number',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Number
	 */
	function ORIG_Number($as_string=false)
	{
		return parent::PRT_get_orig('Number',$as_string);
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

class DB3_SALES_Step_Table extends DB_Table
{
	function DB3_SALES_Step_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Step','PKID_SALES_Step',true);

		$this->fields['PKID_SALES_Step'] =& new DB_Field_Int('PKID_SALES_Step',false);
		$this->fields['Number'] =& new DB_Field_Int('Number',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['Description'] =& new DB_Field_Text('Description',true);

		parent::PRT_add_pk_ref('SALES_Installation','FKID_SALES_Step',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Step' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Step';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Step'=>1,'Number'=>1,'Name'=>1,'Description'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Step column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Step($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Step',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Step in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Step($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Step',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Step Required ?
	 */
	function ISREQ_PKID_SALES_Step()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Number column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Number($id_keys=false)
	{
		return parent::PRT_fetch_col('Number',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Number in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Number($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Number',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Number Required ?
	 */
	function ISREQ_Number()
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
		return false;
	}
}
