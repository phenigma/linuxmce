<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_PackageHistory_Row extends DB_Row
{
	/**
	 * @return DB3_PackageHistory_Row
	 * @param $FKID_Package str
	 * @param $Version str
	 * @desc Will return FALSE if a matching row is not found
	 * FKID_Package can also be an Array, an Object, or an ID-key
	 * Set all fields to fetch row with matching PK
	 * Will also fetch row if ID-Key is used
	 */
	function DB3_PackageHistory_Row($FKID_Package='',$Version=null)
	{
		$this->dbt_path='dbt_my_corp/';
		$is_id_key = (is_string($FKID_Package) && substr_count($FKID_Package,'_')==1);
		$match_arr = (!is_null($Version) || $is_id_key);
		if (!$match_arr || is_array($FKID_Package) || $is_id_key) $row = $FKID_Package;
		else $row = array('FKID_Package'=>$FKID_Package,'Version'=>$Version);
		parent::DB_Row('DB_PackageHistory_Table',$row,$match_arr);
		if ($FKID_Package==='') parent::PRT_set_fields(array('FKID_Package'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Package
	 */
	function GET_FKID_Package($as_string=false)
	{
		return parent::PRT_get_field('FKID_Package',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Package
	 */
	function GETFF_FKID_Package($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_Package',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Package as an HTML_Tag object
	 */
	function & GETFFO_FKID_Package($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_Package',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Package
	 */
	function SET_FKID_Package($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Package',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Package
	 */
	function ORIG_FKID_Package($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Package',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_Package to $match_arr
	 */
	function SETFLTR_FKID_Package($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_Package',$match_arr,$field_wide);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Version
	 */
	function GET_Version($as_string=false)
	{
		return parent::PRT_get_field('Version',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Version
	 */
	function GETFF_Version($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Version',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Version as an HTML_Tag object
	 */
	function & GETFFO_Version($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Version',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Version
	 */
	function SET_Version($value,$from_form=true)
	{
		return parent::PRT_set_field('Version',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Version
	 */
	function ORIG_Version($as_string=false)
	{
		return parent::PRT_get_orig('Version',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of MD5
	 */
	function GET_MD5($as_string=false)
	{
		return parent::PRT_get_field('MD5',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for MD5
	 */
	function GETFF_MD5($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('MD5',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for MD5 as an HTML_Tag object
	 */
	function & GETFFO_MD5($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('MD5',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of MD5
	 */
	function SET_MD5($value,$from_form=true)
	{
		return parent::PRT_set_field('MD5',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of MD5
	 */
	function ORIG_MD5($as_string=false)
	{
		return parent::PRT_get_orig('MD5',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of PackageDate
	 */
	function GET_PackageDate($as_string=false)
	{
		return parent::PRT_get_field('PackageDate',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PackageDate
	 */
	function GETFF_PackageDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('PackageDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PackageDate as an HTML_Tag object
	 */
	function & GETFFO_PackageDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('PackageDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of PackageDate
	 */
	function SET_PackageDate($value,$from_form=true)
	{
		return parent::PRT_set_field('PackageDate',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of PackageDate
	 */
	function ORIG_PackageDate($as_string=false)
	{
		return parent::PRT_get_orig('PackageDate',$as_string);
	}
}

class DB3_PackageHistory_Table extends DB_Table
{
	function DB3_PackageHistory_Table(&$db)
	{
		parent::DB_Table($db,'PackageHistory',array('FKID_Package','Version'),false);

		$this->fields['FKID_Package'] =& new DB_Field_FK('FKID_Package',false,'Package','PKID_Package','Name',$db,'DB_Field_Int');
		$this->fields['Version'] =& new DB_Field_Varchar('Version',false);
		$this->fields['MD5'] =& new DB_Field_Varchar('MD5',true);
		$this->fields['PackageDate'] =& new DB_Field_DateTime('PackageDate',true);
	}

	/**
	 * @desc STATIC: get the tablename 'PackageHistory' 
	 */
	function S_get_tablename()
	{
		return 'PackageHistory';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('FKID_Package'=>1,'Version'=>1,'MD5'=>1,'PackageDate'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Package column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Package($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Package',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Package in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Package($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Package',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_Package to $match_arr
	 */
	function SETFLTR_FKID_Package($match_arr)
	{
		parent::PRT_set_filter('FKID_Package',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Package Required ?
	 */
	function ISREQ_FKID_Package()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Version column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Version($id_keys=false)
	{
		return parent::PRT_fetch_col('Version',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Version in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Version($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Version',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Version Required ?
	 */
	function ISREQ_Version()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch MD5 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_MD5($id_keys=false)
	{
		return parent::PRT_fetch_col('MD5',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where MD5 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_MD5($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('MD5',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is MD5 Required ?
	 */
	function ISREQ_MD5()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PackageDate column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PackageDate($id_keys=false)
	{
		return parent::PRT_fetch_col('PackageDate',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PackageDate in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PackageDate($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PackageDate',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PackageDate Required ?
	 */
	function ISREQ_PackageDate()
	{
		return false;
	}
}
