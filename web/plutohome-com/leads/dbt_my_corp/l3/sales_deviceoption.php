<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_DeviceOption_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_DeviceOption_Row
	 * @param $PKID_SALES_DeviceOption str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_DeviceOption can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_DeviceOption_Row($PKID_SALES_DeviceOption='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_DeviceOption_Table',$PKID_SALES_DeviceOption);
		if ($PKID_SALES_DeviceOption==='') parent::PRT_set_fields(array('FKID_SALES_Device'=>0,'PriceChange'=>0.00));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device_Installation_Option by FKID_SALES_DeviceOption
	 */
	function GETCHLD_SALES_Device_Installation_Option__FKID_SALES_DeviceOption()
	{
		return parent::PRT_get_field_children('FKID_SALES_DeviceOption','SALES_Device_Installation_Option');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_DeviceOption
	 */
	function GET_PKID_SALES_DeviceOption($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_DeviceOption',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_Device
	 */
	function GET_FKID_SALES_Device($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Device',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Device
	 */
	function GETFF_FKID_SALES_Device($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Device',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Device as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Device($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Device',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Device
	 */
	function SET_FKID_SALES_Device($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Device',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Device
	 */
	function ORIG_FKID_SALES_Device($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Device',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Device to $match_arr
	 */
	function SETFLTR_FKID_SALES_Device($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Device',$match_arr,$field_wide);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of PriceChange
	 */
	function GET_PriceChange($as_string=false)
	{
		return parent::PRT_get_field('PriceChange',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PriceChange
	 */
	function GETFF_PriceChange($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('PriceChange',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PriceChange as an HTML_Tag object
	 */
	function & GETFFO_PriceChange($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('PriceChange',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of PriceChange
	 */
	function SET_PriceChange($value,$from_form=true)
	{
		return parent::PRT_set_field('PriceChange',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of PriceChange
	 */
	function ORIG_PriceChange($as_string=false)
	{
		return parent::PRT_get_orig('PriceChange',$as_string);
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

class DB3_SALES_DeviceOption_Table extends DB_Table
{
	function DB3_SALES_DeviceOption_Table(&$db)
	{
		parent::DB_Table($db,'SALES_DeviceOption','PKID_SALES_DeviceOption',true);

		$this->fields['PKID_SALES_DeviceOption'] =& new DB_Field_Int('PKID_SALES_DeviceOption',false);
		$this->fields['FKID_SALES_Device'] =& new DB_Field_FK('FKID_SALES_Device',false,'SALES_Device','PKID_SALES_Device','Name',$db,'DB_Field_Int');
		$this->fields['PriceChange'] =& new DB_Field_Float('PriceChange',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['Description'] =& new DB_Field_Text('Description',true);

		parent::PRT_add_pk_ref('SALES_Device_Installation_Option','FKID_SALES_DeviceOption',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_DeviceOption' 
	 */
	function S_get_tablename()
	{
		return 'SALES_DeviceOption';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_DeviceOption'=>1,'FKID_SALES_Device'=>1,'PriceChange'=>1,'Name'=>1,'Description'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_DeviceOption column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_DeviceOption($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_DeviceOption',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_DeviceOption in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_DeviceOption($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_DeviceOption',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_DeviceOption Required ?
	 */
	function ISREQ_PKID_SALES_DeviceOption()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_SALES_Device column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Device($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Device',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Device in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Device($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Device',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Device to $match_arr
	 */
	function SETFLTR_FKID_SALES_Device($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Device',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Device Required ?
	 */
	function ISREQ_FKID_SALES_Device()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PriceChange column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PriceChange($id_keys=false)
	{
		return parent::PRT_fetch_col('PriceChange',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PriceChange in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PriceChange($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PriceChange',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PriceChange Required ?
	 */
	function ISREQ_PriceChange()
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
