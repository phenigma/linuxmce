<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_Installation_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Installation_Row
	 * @param $PKID_SALES_Installation str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Installation can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Installation_Row($PKID_SALES_Installation='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Installation_Table',$PKID_SALES_Installation);
		if ($PKID_SALES_Installation==='') parent::PRT_set_fields(array('FKID_SALES_Customer'=>0,'CreatedOn'=>'0000-00-00 00:00:00','ModifiedOn'=>'0000-00-00 00:00:00','LastAccessedOn'=>'0000-00-00 00:00:00','FKID_SALES_Step'=>1));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Device_Installation by FKID_SALES_Installation
	 */
	function GETCHLD_SALES_Device_Installation__FKID_SALES_Installation()
	{
		return parent::PRT_get_field_children('FKID_SALES_Installation','SALES_Device_Installation');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_ExistingEquipment by FKID_SALES_Installation
	 */
	function GETCHLD_SALES_ExistingEquipment__FKID_SALES_Installation()
	{
		return parent::PRT_get_field_children('FKID_SALES_Installation','SALES_ExistingEquipment');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_OtherExistingEquipment by FKID_SALES_Installation
	 */
	function GETCHLD_SALES_OtherExistingEquipment__FKID_SALES_Installation()
	{
		return parent::PRT_get_field_children('FKID_SALES_Installation','SALES_OtherExistingEquipment');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Room by FKID_SALES_Installation
	 */
	function GETCHLD_SALES_Room__FKID_SALES_Installation()
	{
		return parent::PRT_get_field_children('FKID_SALES_Installation','SALES_Room');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Installation
	 */
	function GET_PKID_SALES_Installation($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Installation',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_Customer
	 */
	function GET_FKID_SALES_Customer($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Customer',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Customer
	 */
	function GETFF_FKID_SALES_Customer($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Customer',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Customer as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Customer($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Customer',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Customer
	 */
	function SET_FKID_SALES_Customer($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Customer',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Customer
	 */
	function ORIG_FKID_SALES_Customer($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Customer',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Customer to $match_arr
	 */
	function SETFLTR_FKID_SALES_Customer($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Customer',$match_arr,$field_wide);
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
	 * @desc Get Value of Address
	 */
	function GET_Address($as_string=false)
	{
		return parent::PRT_get_field('Address',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address
	 */
	function GETFF_Address($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Address',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address as an HTML_Tag object
	 */
	function & GETFFO_Address($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Address',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Address
	 */
	function SET_Address($value,$from_form=true)
	{
		return parent::PRT_set_field('Address',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Address
	 */
	function ORIG_Address($as_string=false)
	{
		return parent::PRT_get_orig('Address',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_City
	 */
	function GET_FKID_City($as_string=false)
	{
		return parent::PRT_get_field('FKID_City',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_City
	 */
	function GETFF_FKID_City($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_City',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_City as an HTML_Tag object
	 */
	function & GETFFO_FKID_City($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_City',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_City
	 */
	function SET_FKID_City($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_City',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_City
	 */
	function ORIG_FKID_City($as_string=false)
	{
		return parent::PRT_get_orig('FKID_City',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_City to $match_arr
	 */
	function SETFLTR_FKID_City($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_City',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Zip
	 */
	function GET_Zip($as_string=false)
	{
		return parent::PRT_get_field('Zip',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Zip
	 */
	function GETFF_Zip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Zip',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Zip as an HTML_Tag object
	 */
	function & GETFFO_Zip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Zip',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Zip
	 */
	function SET_Zip($value,$from_form=true)
	{
		return parent::PRT_set_field('Zip',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Zip
	 */
	function ORIG_Zip($as_string=false)
	{
		return parent::PRT_get_orig('Zip',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of CreatedOn
	 */
	function GET_CreatedOn($as_string=false)
	{
		return parent::PRT_get_field('CreatedOn',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CreatedOn
	 */
	function GETFF_CreatedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CreatedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CreatedOn as an HTML_Tag object
	 */
	function & GETFFO_CreatedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CreatedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CreatedOn
	 */
	function SET_CreatedOn($value,$from_form=true)
	{
		return parent::PRT_set_field('CreatedOn',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CreatedOn
	 */
	function ORIG_CreatedOn($as_string=false)
	{
		return parent::PRT_get_orig('CreatedOn',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ModifiedOn
	 */
	function GET_ModifiedOn($as_string=false)
	{
		return parent::PRT_get_field('ModifiedOn',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ModifiedOn
	 */
	function GETFF_ModifiedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ModifiedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ModifiedOn as an HTML_Tag object
	 */
	function & GETFFO_ModifiedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ModifiedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ModifiedOn
	 */
	function SET_ModifiedOn($value,$from_form=true)
	{
		return parent::PRT_set_field('ModifiedOn',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ModifiedOn
	 */
	function ORIG_ModifiedOn($as_string=false)
	{
		return parent::PRT_get_orig('ModifiedOn',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of LastAccessedOn
	 */
	function GET_LastAccessedOn($as_string=false)
	{
		return parent::PRT_get_field('LastAccessedOn',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastAccessedOn
	 */
	function GETFF_LastAccessedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('LastAccessedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastAccessedOn as an HTML_Tag object
	 */
	function & GETFFO_LastAccessedOn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('LastAccessedOn',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of LastAccessedOn
	 */
	function SET_LastAccessedOn($value,$from_form=true)
	{
		return parent::PRT_set_field('LastAccessedOn',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of LastAccessedOn
	 */
	function ORIG_LastAccessedOn($as_string=false)
	{
		return parent::PRT_get_orig('LastAccessedOn',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_SALES_Step
	 */
	function GET_FKID_SALES_Step($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Step',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Step
	 */
	function GETFF_FKID_SALES_Step($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Step',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Step as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Step($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Step',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Step
	 */
	function SET_FKID_SALES_Step($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Step',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Step
	 */
	function ORIG_FKID_SALES_Step($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Step',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Step to $match_arr
	 */
	function SETFLTR_FKID_SALES_Step($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Step',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_MS_Installation
	 */
	function GET_FKID_MS_Installation($as_string=false)
	{
		return parent::PRT_get_field('FKID_MS_Installation',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Installation
	 */
	function GETFF_FKID_MS_Installation($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_MS_Installation',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Installation as an HTML_Tag object
	 */
	function & GETFFO_FKID_MS_Installation($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_MS_Installation',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_MS_Installation
	 */
	function SET_FKID_MS_Installation($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_MS_Installation',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_MS_Installation
	 */
	function ORIG_FKID_MS_Installation($as_string=false)
	{
		return parent::PRT_get_orig('FKID_MS_Installation',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_MS_Users
	 */
	function GET_FKID_MS_Users($as_string=false)
	{
		return parent::PRT_get_field('FKID_MS_Users',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Users
	 */
	function GETFF_FKID_MS_Users($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_MS_Users',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_MS_Users as an HTML_Tag object
	 */
	function & GETFFO_FKID_MS_Users($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_MS_Users',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_MS_Users
	 */
	function SET_FKID_MS_Users($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_MS_Users',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_MS_Users
	 */
	function ORIG_FKID_MS_Users($as_string=false)
	{
		return parent::PRT_get_orig('FKID_MS_Users',$as_string);
	}
}

class DB3_SALES_Installation_Table extends DB_Table
{
	function DB3_SALES_Installation_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Installation','PKID_SALES_Installation',true);

		$this->fields['PKID_SALES_Installation'] =& new DB_Field_Int('PKID_SALES_Installation',false);
		$this->fields['FKID_SALES_Customer'] =& new DB_Field_FK('FKID_SALES_Customer',false,'SALES_Customer','PKID_SALES_Customer','Name',$db,'DB_Field_Int');
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['Address'] =& new DB_Field_Varchar('Address',true);
		$this->fields['FKID_City'] =& new DB_Field_FK('FKID_City',true,'City','PKID_City','Name',$db,'DB_Field_Int');
		$this->fields['Zip'] =& new DB_Field_Int('Zip',true);
		$this->fields['CreatedOn'] =& new DB_Field_DateTime('CreatedOn',false);
		$this->fields['ModifiedOn'] =& new DB_Field_DateTime('ModifiedOn',false);
		$this->fields['LastAccessedOn'] =& new DB_Field_DateTime('LastAccessedOn',false);
		$this->fields['FKID_SALES_Step'] =& new DB_Field_FK('FKID_SALES_Step',false,'SALES_Step','PKID_SALES_Step','Name',$db,'DB_Field_Int');
		$this->fields['FKID_MS_Installation'] =& new DB_Field_Int('FKID_MS_Installation',true);
		$this->fields['FKID_MS_Users'] =& new DB_Field_Int('FKID_MS_Users',true);

		parent::PRT_add_pk_ref('SALES_Device_Installation','FKID_SALES_Installation',false);
		parent::PRT_add_pk_ref('SALES_ExistingEquipment','FKID_SALES_Installation',false);
		parent::PRT_add_pk_ref('SALES_OtherExistingEquipment','FKID_SALES_Installation',false);
		parent::PRT_add_pk_ref('SALES_Room','FKID_SALES_Installation',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Installation' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Installation';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Installation'=>1,'FKID_SALES_Customer'=>1,'Name'=>1,'Address'=>1,'FKID_City'=>1,'Zip'=>1,'CreatedOn'=>1,'ModifiedOn'=>1,'LastAccessedOn'=>1,'FKID_SALES_Step'=>1,'FKID_MS_Installation'=>1,'FKID_MS_Users'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Installation column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Installation($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Installation',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Installation in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Installation($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Installation',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Installation Required ?
	 */
	function ISREQ_PKID_SALES_Installation()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_SALES_Customer column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Customer($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Customer',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Customer in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Customer($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Customer',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Customer to $match_arr
	 */
	function SETFLTR_FKID_SALES_Customer($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Customer',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Customer Required ?
	 */
	function ISREQ_FKID_SALES_Customer()
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
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Address column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Address($id_keys=false)
	{
		return parent::PRT_fetch_col('Address',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Address in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Address($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Address',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Address Required ?
	 */
	function ISREQ_Address()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_City column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_City($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_City',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_City in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_City($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_City',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_City to $match_arr
	 */
	function SETFLTR_FKID_City($match_arr)
	{
		parent::PRT_set_filter('FKID_City',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_City Required ?
	 */
	function ISREQ_FKID_City()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Zip column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Zip($id_keys=false)
	{
		return parent::PRT_fetch_col('Zip',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Zip in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Zip($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Zip',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Zip Required ?
	 */
	function ISREQ_Zip()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CreatedOn column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CreatedOn($id_keys=false)
	{
		return parent::PRT_fetch_col('CreatedOn',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CreatedOn in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CreatedOn($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CreatedOn',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CreatedOn Required ?
	 */
	function ISREQ_CreatedOn()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ModifiedOn column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ModifiedOn($id_keys=false)
	{
		return parent::PRT_fetch_col('ModifiedOn',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ModifiedOn in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ModifiedOn($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ModifiedOn',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ModifiedOn Required ?
	 */
	function ISREQ_ModifiedOn()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch LastAccessedOn column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_LastAccessedOn($id_keys=false)
	{
		return parent::PRT_fetch_col('LastAccessedOn',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where LastAccessedOn in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_LastAccessedOn($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('LastAccessedOn',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is LastAccessedOn Required ?
	 */
	function ISREQ_LastAccessedOn()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_SALES_Step column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Step($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Step',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Step in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Step($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Step',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Step to $match_arr
	 */
	function SETFLTR_FKID_SALES_Step($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Step',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Step Required ?
	 */
	function ISREQ_FKID_SALES_Step()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_MS_Installation column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_MS_Installation($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_MS_Installation',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_MS_Installation in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_MS_Installation($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_MS_Installation',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_MS_Installation Required ?
	 */
	function ISREQ_FKID_MS_Installation()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_MS_Users column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_MS_Users($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_MS_Users',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_MS_Users in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_MS_Users($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_MS_Users',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_MS_Users Required ?
	 */
	function ISREQ_FKID_MS_Users()
	{
		return false;
	}
}
