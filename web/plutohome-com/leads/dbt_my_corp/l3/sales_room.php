<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_Room_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Room_Row
	 * @param $PKID_SALES_Room str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Room can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Room_Row($PKID_SALES_Room='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Room_Table',$PKID_SALES_Room);
		if ($PKID_SALES_Room==='') parent::PRT_set_fields(array('FKID_SALES_Installation'=>0,'FKID_SALES_RoomType'=>0,'HasAV'=>0,'HasThermostat'=>0,'LightingControls'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device_Installation by FKID_SALES_Room
	 */
	function GETCHLD_SALES_Device_Installation__FKID_SALES_Room()
	{
		return parent::PRT_get_field_children('FKID_SALES_Room','SALES_Device_Installation');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Room
	 */
	function GET_PKID_SALES_Room($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Room',$as_string);
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
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_RoomType
	 */
	function GET_FKID_SALES_RoomType($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_RoomType',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_RoomType
	 */
	function GETFF_FKID_SALES_RoomType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_RoomType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_RoomType as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_RoomType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_RoomType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_RoomType
	 */
	function SET_FKID_SALES_RoomType($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_RoomType',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_RoomType
	 */
	function ORIG_FKID_SALES_RoomType($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_RoomType',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_RoomType to $match_arr
	 */
	function SETFLTR_FKID_SALES_RoomType($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_RoomType',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of HasAV
	 */
	function GET_HasAV($as_string=false)
	{
		return parent::PRT_get_field('HasAV',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HasAV
	 */
	function GETFF_HasAV($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('HasAV',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HasAV as an HTML_Tag object
	 */
	function & GETFFO_HasAV($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('HasAV',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of HasAV
	 */
	function SET_HasAV($value,$from_form=true)
	{
		return parent::PRT_set_field('HasAV',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of HasAV
	 */
	function ORIG_HasAV($as_string=false)
	{
		return parent::PRT_get_orig('HasAV',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of HasThermostat
	 */
	function GET_HasThermostat($as_string=false)
	{
		return parent::PRT_get_field('HasThermostat',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HasThermostat
	 */
	function GETFF_HasThermostat($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('HasThermostat',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HasThermostat as an HTML_Tag object
	 */
	function & GETFFO_HasThermostat($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('HasThermostat',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of HasThermostat
	 */
	function SET_HasThermostat($value,$from_form=true)
	{
		return parent::PRT_set_field('HasThermostat',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of HasThermostat
	 */
	function ORIG_HasThermostat($as_string=false)
	{
		return parent::PRT_get_orig('HasThermostat',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of LightingControls
	 */
	function GET_LightingControls($as_string=false)
	{
		return parent::PRT_get_field('LightingControls',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LightingControls
	 */
	function GETFF_LightingControls($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('LightingControls',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LightingControls as an HTML_Tag object
	 */
	function & GETFFO_LightingControls($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('LightingControls',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of LightingControls
	 */
	function SET_LightingControls($value,$from_form=true)
	{
		return parent::PRT_set_field('LightingControls',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of LightingControls
	 */
	function ORIG_LightingControls($as_string=false)
	{
		return parent::PRT_get_orig('LightingControls',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_MS_Room
	 */
	function GET_FKID_MS_Room($as_string=false)
	{
		return parent::PRT_get_field('FKID_MS_Room',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Room
	 */
	function GETFF_FKID_MS_Room($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_MS_Room',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Room as an HTML_Tag object
	 */
	function & GETFFO_FKID_MS_Room($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_MS_Room',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_MS_Room
	 */
	function SET_FKID_MS_Room($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_MS_Room',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_MS_Room
	 */
	function ORIG_FKID_MS_Room($as_string=false)
	{
		return parent::PRT_get_orig('FKID_MS_Room',$as_string);
	}
}

class DB3_SALES_Room_Table extends DB_Table
{
	function DB3_SALES_Room_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Room','PKID_SALES_Room',true);

		$this->fields['PKID_SALES_Room'] =& new DB_Field_Int('PKID_SALES_Room',false);
		$this->fields['FKID_SALES_Installation'] =& new DB_Field_FK('FKID_SALES_Installation',false,'SALES_Installation','PKID_SALES_Installation','Name',$db,'DB_Field_Int');
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['FKID_SALES_RoomType'] =& new DB_Field_FK('FKID_SALES_RoomType',false,'SALES_RoomType','PKID_SALES_RoomType','Name',$db,'DB_Field_Int');
		$this->fields['HasAV'] =& new DB_Field_Int('HasAV',false);
		$this->fields['HasThermostat'] =& new DB_Field_Int('HasThermostat',false);
		$this->fields['LightingControls'] =& new DB_Field_Int('LightingControls',false);
		$this->fields['FKID_MS_Room'] =& new DB_Field_Int('FKID_MS_Room',true);

		parent::PRT_add_pk_ref('SALES_Device_Installation','FKID_SALES_Room',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Room' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Room';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Room'=>1,'FKID_SALES_Installation'=>1,'Name'=>1,'FKID_SALES_RoomType'=>1,'HasAV'=>1,'HasThermostat'=>1,'LightingControls'=>1,'FKID_MS_Room'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Room column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Room($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Room',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Room in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Room($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Room',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Room Required ?
	 */
	function ISREQ_PKID_SALES_Room()
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
	 * @desc Fetch FKID_SALES_RoomType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_RoomType($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_RoomType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_RoomType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_RoomType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_RoomType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_RoomType to $match_arr
	 */
	function SETFLTR_FKID_SALES_RoomType($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_RoomType',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_RoomType Required ?
	 */
	function ISREQ_FKID_SALES_RoomType()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch HasAV column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_HasAV($id_keys=false)
	{
		return parent::PRT_fetch_col('HasAV',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where HasAV in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_HasAV($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('HasAV',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is HasAV Required ?
	 */
	function ISREQ_HasAV()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch HasThermostat column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_HasThermostat($id_keys=false)
	{
		return parent::PRT_fetch_col('HasThermostat',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where HasThermostat in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_HasThermostat($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('HasThermostat',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is HasThermostat Required ?
	 */
	function ISREQ_HasThermostat()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch LightingControls column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_LightingControls($id_keys=false)
	{
		return parent::PRT_fetch_col('LightingControls',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where LightingControls in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_LightingControls($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('LightingControls',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is LightingControls Required ?
	 */
	function ISREQ_LightingControls()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_MS_Room column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_MS_Room($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_MS_Room',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_MS_Room in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_MS_Room($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_MS_Room',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_MS_Room Required ?
	 */
	function ISREQ_FKID_MS_Room()
	{
		return false;
	}
}
