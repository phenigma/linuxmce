<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_SALES_Device
{
	// CURRENT DEFINES:
	var $FLARE = 11;
	var $MC_101 = 6;
	var $MC_201 = 7;
	var $PDA = 9;
	var $WEB_DT = 10;
	var $X10 = 12;
}
$DB_SALES_Device =& new DBT_SALES_Device();
$GLOBALS['DB_SALES_Device'] =& $DB_SALES_Device;

class DB3_SALES_Device_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Device_Row
	 * @param $PKID_SALES_Device str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Device can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Device_Row($PKID_SALES_Device='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Device_Table',$PKID_SALES_Device);
		if ($PKID_SALES_Device==='') parent::PRT_set_fields(array('FKID_SALES_DeviceCategory'=>0,'FirstUnitSurcharge'=>0.00,'PricePerUnit'=>0.00));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_DeviceOption by FKID_SALES_Device
	 */
	function GETCHLD_SALES_DeviceOption__FKID_SALES_Device()
	{
		return parent::PRT_get_field_children('FKID_SALES_Device','SALES_DeviceOption');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device_Installation by FKID_SALES_Device
	 */
	function GETCHLD_SALES_Device_Installation__FKID_SALES_Device()
	{
		return parent::PRT_get_field_children('FKID_SALES_Device','SALES_Device_Installation');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Device
	 */
	function GET_PKID_SALES_Device($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Device',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_DeviceCategory
	 */
	function GET_FKID_SALES_DeviceCategory($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_DeviceCategory',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_DeviceCategory
	 */
	function GETFF_FKID_SALES_DeviceCategory($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_DeviceCategory',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_DeviceCategory as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_DeviceCategory($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_DeviceCategory',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_DeviceCategory
	 */
	function SET_FKID_SALES_DeviceCategory($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_DeviceCategory',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_DeviceCategory
	 */
	function ORIG_FKID_SALES_DeviceCategory($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_DeviceCategory',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_DeviceCategory to $match_arr
	 */
	function SETFLTR_FKID_SALES_DeviceCategory($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_DeviceCategory',$match_arr,$field_wide);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of FirstUnitSurcharge
	 */
	function GET_FirstUnitSurcharge($as_string=false)
	{
		return parent::PRT_get_field('FirstUnitSurcharge',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FirstUnitSurcharge
	 */
	function GETFF_FirstUnitSurcharge($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FirstUnitSurcharge',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FirstUnitSurcharge as an HTML_Tag object
	 */
	function & GETFFO_FirstUnitSurcharge($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FirstUnitSurcharge',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of FirstUnitSurcharge
	 */
	function SET_FirstUnitSurcharge($value,$from_form=true)
	{
		return parent::PRT_set_field('FirstUnitSurcharge',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of FirstUnitSurcharge
	 */
	function ORIG_FirstUnitSurcharge($as_string=false)
	{
		return parent::PRT_get_orig('FirstUnitSurcharge',$as_string);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of PricePerUnit
	 */
	function GET_PricePerUnit($as_string=false)
	{
		return parent::PRT_get_field('PricePerUnit',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PricePerUnit
	 */
	function GETFF_PricePerUnit($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('PricePerUnit',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PricePerUnit as an HTML_Tag object
	 */
	function & GETFFO_PricePerUnit($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('PricePerUnit',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of PricePerUnit
	 */
	function SET_PricePerUnit($value,$from_form=true)
	{
		return parent::PRT_set_field('PricePerUnit',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of PricePerUnit
	 */
	function ORIG_PricePerUnit($as_string=false)
	{
		return parent::PRT_get_orig('PricePerUnit',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Define
	 */
	function GET_Define($as_string=false)
	{
		return parent::PRT_get_field('Define',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define
	 */
	function GETFF_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define as an HTML_Tag object
	 */
	function & GETFFO_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Define
	 */
	function SET_Define($value,$from_form=true)
	{
		return parent::PRT_set_field('Define',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Define
	 */
	function ORIG_Define($as_string=false)
	{
		return parent::PRT_get_orig('Define',$as_string);
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

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_MS_MasterDeviceList
	 */
	function GET_FKID_MS_MasterDeviceList($as_string=false)
	{
		return parent::PRT_get_field('FKID_MS_MasterDeviceList',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_MasterDeviceList
	 */
	function GETFF_FKID_MS_MasterDeviceList($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_MS_MasterDeviceList',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_MasterDeviceList as an HTML_Tag object
	 */
	function & GETFFO_FKID_MS_MasterDeviceList($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_MS_MasterDeviceList',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_MS_MasterDeviceList
	 */
	function SET_FKID_MS_MasterDeviceList($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_MS_MasterDeviceList',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_MS_MasterDeviceList
	 */
	function ORIG_FKID_MS_MasterDeviceList($as_string=false)
	{
		return parent::PRT_get_orig('FKID_MS_MasterDeviceList',$as_string);
	}
}

class DB3_SALES_Device_Table extends DB_Table
{
	function DB3_SALES_Device_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Device','PKID_SALES_Device',true);

		$this->fields['PKID_SALES_Device'] =& new DB_Field_Int('PKID_SALES_Device',false);
		$this->fields['FKID_SALES_DeviceCategory'] =& new DB_Field_FK('FKID_SALES_DeviceCategory',false,'SALES_DeviceCategory','PKID_SALES_DeviceCategory','Name',$db,'DB_Field_Int');
		$this->fields['FirstUnitSurcharge'] =& new DB_Field_Float('FirstUnitSurcharge',false);
		$this->fields['PricePerUnit'] =& new DB_Field_Float('PricePerUnit',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',true);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['Description'] =& new DB_Field_Text('Description',true);
		$this->fields['FKID_MS_MasterDeviceList'] =& new DB_Field_Int('FKID_MS_MasterDeviceList',true);

		parent::PRT_add_pk_ref('SALES_DeviceOption','FKID_SALES_Device',false);
		parent::PRT_add_pk_ref('SALES_Device_Installation','FKID_SALES_Device',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Device' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Device';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Device'=>1,'FKID_SALES_DeviceCategory'=>1,'FirstUnitSurcharge'=>1,'PricePerUnit'=>1,'Define'=>1,'Name'=>1,'Description'=>1,'FKID_MS_MasterDeviceList'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Device column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Device($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Device',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Device in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Device($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Device',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Device Required ?
	 */
	function ISREQ_PKID_SALES_Device()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_SALES_DeviceCategory column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_DeviceCategory($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_DeviceCategory',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_DeviceCategory in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_DeviceCategory($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_DeviceCategory',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_DeviceCategory to $match_arr
	 */
	function SETFLTR_FKID_SALES_DeviceCategory($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_DeviceCategory',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_DeviceCategory Required ?
	 */
	function ISREQ_FKID_SALES_DeviceCategory()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FirstUnitSurcharge column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FirstUnitSurcharge($id_keys=false)
	{
		return parent::PRT_fetch_col('FirstUnitSurcharge',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FirstUnitSurcharge in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FirstUnitSurcharge($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FirstUnitSurcharge',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FirstUnitSurcharge Required ?
	 */
	function ISREQ_FirstUnitSurcharge()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PricePerUnit column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PricePerUnit($id_keys=false)
	{
		return parent::PRT_fetch_col('PricePerUnit',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PricePerUnit in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PricePerUnit($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PricePerUnit',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PricePerUnit Required ?
	 */
	function ISREQ_PricePerUnit()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Define column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Define($id_keys=false)
	{
		return parent::PRT_fetch_col('Define',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Define in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
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

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_MS_MasterDeviceList column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_MS_MasterDeviceList($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_MS_MasterDeviceList',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_MS_MasterDeviceList in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_MS_MasterDeviceList($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_MS_MasterDeviceList',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_MS_MasterDeviceList Required ?
	 */
	function ISREQ_FKID_MS_MasterDeviceList()
	{
		return false;
	}
}
