<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_OtherExistingEquipment_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_OtherExistingEquipment_Row
	 * @param $PKID_SALES_OtherExistingEquipment str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_OtherExistingEquipment can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_OtherExistingEquipment_Row($PKID_SALES_OtherExistingEquipment='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_OtherExistingEquipment_Table',$PKID_SALES_OtherExistingEquipment);
		if ($PKID_SALES_OtherExistingEquipment==='') parent::PRT_set_fields(array('FKID_SALES_Installation'=>0,'FKID_SALES_EquipmentCategory'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_OtherExistingEquipment
	 */
	function GET_PKID_SALES_OtherExistingEquipment($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_OtherExistingEquipment',$as_string);
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
	 * @desc Get Value of FKID_SALES_EquipmentCategory
	 */
	function GET_FKID_SALES_EquipmentCategory($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_EquipmentCategory',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_EquipmentCategory
	 */
	function GETFF_FKID_SALES_EquipmentCategory($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_EquipmentCategory',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_EquipmentCategory as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_EquipmentCategory($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_EquipmentCategory',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_EquipmentCategory
	 */
	function SET_FKID_SALES_EquipmentCategory($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_EquipmentCategory',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_EquipmentCategory
	 */
	function ORIG_FKID_SALES_EquipmentCategory($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_EquipmentCategory',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_EquipmentCategory to $match_arr
	 */
	function SETFLTR_FKID_SALES_EquipmentCategory($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_EquipmentCategory',$match_arr,$field_wide);
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

class DB3_SALES_OtherExistingEquipment_Table extends DB_Table
{
	function DB3_SALES_OtherExistingEquipment_Table(&$db)
	{
		parent::DB_Table($db,'SALES_OtherExistingEquipment','PKID_SALES_OtherExistingEquipment',true);

		$this->fields['PKID_SALES_OtherExistingEquipment'] =& new DB_Field_Int('PKID_SALES_OtherExistingEquipment',false);
		$this->fields['FKID_SALES_Installation'] =& new DB_Field_FK('FKID_SALES_Installation',false,'SALES_Installation','PKID_SALES_Installation','Name',$db,'DB_Field_Int');
		$this->fields['FKID_SALES_EquipmentCategory'] =& new DB_Field_FK('FKID_SALES_EquipmentCategory',false,'SALES_EquipmentCategory','PKID_SALES_EquipmentCategory','Name',$db,'DB_Field_Int');
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_OtherExistingEquipment' 
	 */
	function S_get_tablename()
	{
		return 'SALES_OtherExistingEquipment';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_OtherExistingEquipment'=>1,'FKID_SALES_Installation'=>1,'FKID_SALES_EquipmentCategory'=>1,'Name'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_OtherExistingEquipment column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_OtherExistingEquipment($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_OtherExistingEquipment',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_OtherExistingEquipment in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_OtherExistingEquipment($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_OtherExistingEquipment',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_OtherExistingEquipment Required ?
	 */
	function ISREQ_PKID_SALES_OtherExistingEquipment()
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
	 * @desc Fetch FKID_SALES_EquipmentCategory column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_EquipmentCategory($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_EquipmentCategory',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_EquipmentCategory in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_EquipmentCategory($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_EquipmentCategory',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_EquipmentCategory to $match_arr
	 */
	function SETFLTR_FKID_SALES_EquipmentCategory($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_EquipmentCategory',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_EquipmentCategory Required ?
	 */
	function ISREQ_FKID_SALES_EquipmentCategory()
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
