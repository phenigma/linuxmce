<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Package_Row extends DB_Row
{
	/**
	 * @return DB3_Package_Row
	 * @param $PKID_Package str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Package can also be an Array, an Object, or an ID-key
	 */
	function DB3_Package_Row($PKID_Package='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Package_Table',$PKID_Package);
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from PackageHistory by FKID_Package
	 */
	function GETCHLD_PackageHistory__FKID_Package()
	{
		return parent::PRT_get_field_children('FKID_Package','PackageHistory');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from StartupPackages by FKID_Package
	 */
	function GETCHLD_StartupPackages__FKID_Package()
	{
		return parent::PRT_get_field_children('FKID_Package','StartupPackages');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Package
	 */
	function GET_PKID_Package($as_string=false)
	{
		return parent::PRT_get_field('PKID_Package',$as_string);
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
	 * @desc Get Value of CurVersion
	 */
	function GET_CurVersion($as_string=false)
	{
		return parent::PRT_get_field('CurVersion',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CurVersion
	 */
	function GETFF_CurVersion($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CurVersion',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CurVersion as an HTML_Tag object
	 */
	function & GETFFO_CurVersion($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CurVersion',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CurVersion
	 */
	function SET_CurVersion($value,$from_form=true)
	{
		return parent::PRT_set_field('CurVersion',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CurVersion
	 */
	function ORIG_CurVersion($as_string=false)
	{
		return parent::PRT_get_orig('CurVersion',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of CurMD5
	 */
	function GET_CurMD5($as_string=false)
	{
		return parent::PRT_get_field('CurMD5',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CurMD5
	 */
	function GETFF_CurMD5($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CurMD5',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CurMD5 as an HTML_Tag object
	 */
	function & GETFFO_CurMD5($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CurMD5',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CurMD5
	 */
	function SET_CurMD5($value,$from_form=true)
	{
		return parent::PRT_set_field('CurMD5',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CurMD5
	 */
	function ORIG_CurMD5($as_string=false)
	{
		return parent::PRT_get_orig('CurMD5',$as_string);
	}
}

class DB3_Package_Table extends DB_Table
{
	function DB3_Package_Table(&$db)
	{
		parent::DB_Table($db,'Package','PKID_Package',true);

		$this->fields['PKID_Package'] =& new DB_Field_Int('PKID_Package',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['CurVersion'] =& new DB_Field_Varchar('CurVersion',true);
		$this->fields['CurMD5'] =& new DB_Field_Varchar('CurMD5',true);

		parent::PRT_add_pk_ref('PackageHistory','FKID_Package',false);
		parent::PRT_add_pk_ref('StartupPackages','FKID_Package',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Package' 
	 */
	function S_get_tablename()
	{
		return 'Package';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Package'=>1,'Name'=>1,'CurVersion'=>1,'CurMD5'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Package column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Package($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Package',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Package in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Package($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Package',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Package Required ?
	 */
	function ISREQ_PKID_Package()
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
	 * @desc Fetch CurVersion column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CurVersion($id_keys=false)
	{
		return parent::PRT_fetch_col('CurVersion',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CurVersion in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CurVersion($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CurVersion',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CurVersion Required ?
	 */
	function ISREQ_CurVersion()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CurMD5 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CurMD5($id_keys=false)
	{
		return parent::PRT_fetch_col('CurMD5',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CurMD5 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CurMD5($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CurMD5',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CurMD5 Required ?
	 */
	function ISREQ_CurMD5()
	{
		return false;
	}
}
