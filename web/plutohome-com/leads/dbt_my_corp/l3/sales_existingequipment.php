<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_ExistingEquipment_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_ExistingEquipment_Row
	 * @param $PKID_SALES_ExistingEquipment str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_ExistingEquipment can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_ExistingEquipment_Row($PKID_SALES_ExistingEquipment='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_ExistingEquipment_Table',$PKID_SALES_ExistingEquipment);
		if ($PKID_SALES_ExistingEquipment==='') parent::PRT_set_fields(array('FKID_SALES_Installation'=>0,'FKID_SALES_Equipment'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_ExistingEquipment
	 */
	function GET_PKID_SALES_ExistingEquipment($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_ExistingEquipment',$as_string);
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
	 * @desc Get Value of FKID_SALES_Equipment
	 */
	function GET_FKID_SALES_Equipment($as_string=false)
	{
		return parent::PRT_get_field('FKID_SALES_Equipment',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Equipment
	 */
	function GETFF_FKID_SALES_Equipment($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_SALES_Equipment',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_SALES_Equipment as an HTML_Tag object
	 */
	function & GETFFO_FKID_SALES_Equipment($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_SALES_Equipment',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_SALES_Equipment
	 */
	function SET_FKID_SALES_Equipment($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_SALES_Equipment',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_SALES_Equipment
	 */
	function ORIG_FKID_SALES_Equipment($as_string=false)
	{
		return parent::PRT_get_orig('FKID_SALES_Equipment',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_SALES_Equipment to $match_arr
	 */
	function SETFLTR_FKID_SALES_Equipment($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_SALES_Equipment',$match_arr,$field_wide);
	}
}

class DB3_SALES_ExistingEquipment_Table extends DB_Table
{
	function DB3_SALES_ExistingEquipment_Table(&$db)
	{
		parent::DB_Table($db,'SALES_ExistingEquipment','PKID_SALES_ExistingEquipment',true);

		$this->fields['PKID_SALES_ExistingEquipment'] =& new DB_Field_Int('PKID_SALES_ExistingEquipment',false);
		$this->fields['FKID_SALES_Installation'] =& new DB_Field_FK('FKID_SALES_Installation',false,'SALES_Installation','PKID_SALES_Installation','Name',$db,'DB_Field_Int');
		$this->fields['FKID_SALES_Equipment'] =& new DB_Field_FK('FKID_SALES_Equipment',false,'SALES_Equipment','PKID_SALES_Equipment','Name',$db,'DB_Field_Int');
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_ExistingEquipment' 
	 */
	function S_get_tablename()
	{
		return 'SALES_ExistingEquipment';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_ExistingEquipment'=>1,'FKID_SALES_Installation'=>1,'FKID_SALES_Equipment'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_ExistingEquipment column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_ExistingEquipment($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_ExistingEquipment',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_ExistingEquipment in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_ExistingEquipment($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_ExistingEquipment',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_ExistingEquipment Required ?
	 */
	function ISREQ_PKID_SALES_ExistingEquipment()
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
	 * @desc Fetch FKID_SALES_Equipment column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_SALES_Equipment($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_SALES_Equipment',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_SALES_Equipment in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_SALES_Equipment($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_SALES_Equipment',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_SALES_Equipment to $match_arr
	 */
	function SETFLTR_FKID_SALES_Equipment($match_arr)
	{
		parent::PRT_set_filter('FKID_SALES_Equipment',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_SALES_Equipment Required ?
	 */
	function ISREQ_FKID_SALES_Equipment()
	{
		return true;
	}
}
