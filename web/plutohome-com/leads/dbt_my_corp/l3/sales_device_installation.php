<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_Device_Installation_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Device_Installation_Row
	 * @param $PKID_SALES_Device_Installation str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Device_Installation can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Device_Installation_Row($PKID_SALES_Device_Installation='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Device_Installation_Table',$PKID_SALES_Device_Installation);
		if ($PKID_SALES_Device_Installation==='') parent::PRT_set_fields(array('FKID_SALES_Device'=>0,'FKID_SALES_Installation'=>0,'Quantity'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device_Installation_Option by FKID_SALES_Device_Installation
	 */
	function GETCHLD_SALES_Device_Installation_Option__FKID_SALES_Device_Installation()
	{
		return parent::PRT_get_field_children('FKID_SALES_Device_Installation','SALES_Device_Installation_Option');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Device_Installation
	 */
	function GET_PKID_SALES_Device_Installation($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Device_Installation',$as_string);
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
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_Installation
	 */
	function GET_FKID_SALES_Installation($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Installation',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Installation
	 */
	function GETFF_FKID_SALES_Installation($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Installation',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Installation as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Installation($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Installation',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Installation
	 */
	function SET_FKID_SALES_Installation($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Installation',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Installation
	 */
	function ORIG_FKID_SALES_Installation($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Installation',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Installation to $match_arr
	 */
	function SETFLTR_FKID_SALES_Installation($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Installation',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_Room
	 */
	function GET_FKID_SALES_Room($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Room',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Room
	 */
	function GETFF_FKID_SALES_Room($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Room',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Room as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Room($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Room',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Room
	 */
	function SET_FKID_SALES_Room($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Room',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Room
	 */
	function ORIG_FKID_SALES_Room($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Room',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Room to $match_arr
	 */
	function SETFLTR_FKID_SALES_Room($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Room',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Quantity
	 */
	function GET_Quantity($as_string=false)
	{
		return parent::PRT_get_field('Quantity',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Quantity
	 */
	function GETFF_Quantity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Quantity',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Quantity as an HTML_Tag object
	 */
	function & GETFFO_Quantity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Quantity',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Quantity
	 */
	function SET_Quantity($value,$from_form=true)
	{
		return parent::PRT_set_field('Quantity',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Quantity
	 */
	function ORIG_Quantity($as_string=false)
	{
		return parent::PRT_get_orig('Quantity',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of FKID_MS_Device
	 */
	function GET_FKID_MS_Device($as_string=false)
	{
		return parent::PRT_get_field('FKID_MS_Device',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Device
	 */
	function GETFF_FKID_MS_Device($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_MS_Device',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Device as an HTML_Tag object
	 */
	function & GETFFO_FKID_MS_Device($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_MS_Device',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of FKID_MS_Device
	 */
	function SET_FKID_MS_Device($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_MS_Device',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_MS_Device
	 */
	function ORIG_FKID_MS_Device($as_string=false)
	{
		return parent::PRT_get_orig('FKID_MS_Device',$as_string);
	}
}

class DB3_SALES_Device_Installation_Table extends DB_Table
{
	function DB3_SALES_Device_Installation_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Device_Installation','PKID_SALES_Device_Installation',true);

		$this->fields['PKID_SALES_Device_Installation'] =& new DB_Field_Int('PKID_SALES_Device_Installation',false);
		$this->fields['FKID_SALES_Device'] =& new DB_Field_FK('FKID_SALES_Device',false,'SALES_Device','PKID_SALES_Device','Name',$db,'DB_Field_Int');
		$this->fields['FKID_SALES_Installation'] =& new DB_Field_FK('FKID_SALES_Installation',false,'SALES_Installation','PKID_SALES_Installation','Name',$db,'DB_Field_Int');
		$this->fields['FKID_SALES_Room'] =& new DB_Field_FK('FKID_SALES_Room',true,'SALES_Room','PKID_SALES_Room','Name',$db,'DB_Field_Int');
		$this->fields['Quantity'] =& new DB_Field_Int('Quantity',false);
		$this->fields['FKID_MS_Device'] =& new DB_Field_Varchar('FKID_MS_Device',true);

		parent::PRT_add_pk_ref('SALES_Device_Installation_Option','FKID_SALES_Device_Installation',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Device_Installation' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Device_Installation';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Device_Installation'=>1,'FKID_SALES_Device'=>1,'FKID_SALES_Installation'=>1,'FKID_SALES_Room'=>1,'Quantity'=>1,'FKID_MS_Device'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Device_Installation column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Device_Installation($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Device_Installation',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Device_Installation in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Device_Installation($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Device_Installation',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Device_Installation Required ?
	 */
	function ISREQ_PKID_SALES_Device_Installation()
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
	 * @desc Fetch FKID_SALES_Installation column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Installation($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Installation',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Installation in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Installation($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Installation',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Installation to $match_arr
	 */
	function SETFLTR_FKID_SALES_Installation($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Installation',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Installation Required ?
	 */
	function ISREQ_FKID_SALES_Installation()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_SALES_Room column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Room($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Room',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Room in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Room($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Room',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Room to $match_arr
	 */
	function SETFLTR_FKID_SALES_Room($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Room',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Room Required ?
	 */
	function ISREQ_FKID_SALES_Room()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Quantity column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Quantity($id_keys=false)
	{
		return parent::PRT_fetch_col('Quantity',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Quantity in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Quantity($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Quantity',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Quantity Required ?
	 */
	function ISREQ_Quantity()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_MS_Device column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_MS_Device($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_MS_Device',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_MS_Device in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_MS_Device($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_MS_Device',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_MS_Device Required ?
	 */
	function ISREQ_FKID_MS_Device()
	{
		return false;
	}
}
