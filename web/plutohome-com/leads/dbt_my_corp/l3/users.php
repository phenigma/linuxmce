<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Users_Row extends DB_Row
{
	/**
	 * @return DB3_Users_Row
	 * @param $FKID_S_Users str
	 * @desc Will return FALSE if a matching row is not found
	 * FKID_S_Users can also be an Array, an Object, or an ID-key
	 */
	function DB3_Users_Row($FKID_S_Users='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Users_Table',$FKID_S_Users);
		if ($FKID_S_Users==='') parent::PRT_set_fields(array('FKID_S_Users'=>0,'FKID_Installation'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_S_Users
	 */
	function GET_FKID_S_Users($as_string=false)
	{
		return parent::PRT_get_field('FKID_S_Users',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_S_Users
	 */
	function GETFF_FKID_S_Users($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_S_Users',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_S_Users as an HTML_Tag object
	 */
	function & GETFFO_FKID_S_Users($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_S_Users',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_S_Users
	 */
	function SET_FKID_S_Users($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_S_Users',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_S_Users
	 */
	function ORIG_FKID_S_Users($as_string=false)
	{
		return parent::PRT_get_orig('FKID_S_Users',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of UserName
	 */
	function GET_UserName($as_string=false)
	{
		return parent::PRT_get_field('UserName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for UserName
	 */
	function GETFF_UserName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('UserName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for UserName as an HTML_Tag object
	 */
	function & GETFFO_UserName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('UserName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of UserName
	 */
	function SET_UserName($value,$from_form=true)
	{
		return parent::PRT_set_field('UserName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of UserName
	 */
	function ORIG_UserName($as_string=false)
	{
		return parent::PRT_get_orig('UserName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Password
	 */
	function GET_Password($as_string=false)
	{
		return parent::PRT_get_field('Password',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password
	 */
	function GETFF_Password($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Password',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password as an HTML_Tag object
	 */
	function & GETFFO_Password($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Password',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Password
	 */
	function SET_Password($value,$from_form=true)
	{
		return parent::PRT_set_field('Password',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Password
	 */
	function ORIG_Password($as_string=false)
	{
		return parent::PRT_get_orig('Password',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Installation
	 */
	function GET_FKID_Installation($as_string=false)
	{
		return parent::PRT_get_field('FKID_Installation',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Installation
	 */
	function GETFF_FKID_Installation($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_Installation',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Installation as an HTML_Tag object
	 */
	function & GETFFO_FKID_Installation($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_Installation',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Installation
	 */
	function SET_FKID_Installation($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Installation',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Installation
	 */
	function ORIG_FKID_Installation($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Installation',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Password_Unix
	 */
	function GET_Password_Unix($as_string=false)
	{
		return parent::PRT_get_field('Password_Unix',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password_Unix
	 */
	function GETFF_Password_Unix($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Password_Unix',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password_Unix as an HTML_Tag object
	 */
	function & GETFFO_Password_Unix($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Password_Unix',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Password_Unix
	 */
	function SET_Password_Unix($value,$from_form=true)
	{
		return parent::PRT_set_field('Password_Unix',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Password_Unix
	 */
	function ORIG_Password_Unix($as_string=false)
	{
		return parent::PRT_get_orig('Password_Unix',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Password_Samba
	 */
	function GET_Password_Samba($as_string=false)
	{
		return parent::PRT_get_field('Password_Samba',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password_Samba
	 */
	function GETFF_Password_Samba($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Password_Samba',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Password_Samba as an HTML_Tag object
	 */
	function & GETFFO_Password_Samba($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Password_Samba',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Password_Samba
	 */
	function SET_Password_Samba($value,$from_form=true)
	{
		return parent::PRT_set_field('Password_Samba',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Password_Samba
	 */
	function ORIG_Password_Samba($as_string=false)
	{
		return parent::PRT_get_orig('Password_Samba',$as_string);
	}
}

class DB3_Users_Table extends DB_Table
{
	function DB3_Users_Table(&$db)
	{
		parent::DB_Table($db,'Users','FKID_S_Users',false);

		$this->fields['FKID_S_Users'] =& new DB_Field_Int('FKID_S_Users',false);
		$this->fields['UserName'] =& new DB_Field_Varchar('UserName',false);
		$this->fields['Password'] =& new DB_Field_Pwd('Password',false);
		$this->fields['FKID_Installation'] =& new DB_Field_Int('FKID_Installation',false);
		$this->fields['Password_Unix'] =& new DB_Field_Pwd('Password_Unix',false);
		$this->fields['Password_Samba'] =& new DB_Field_Pwd('Password_Samba',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Users' 
	 */
	function S_get_tablename()
	{
		return 'Users';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('FKID_S_Users'=>1,'UserName'=>1,'Password'=>1,'FKID_Installation'=>1,'Password_Unix'=>1,'Password_Samba'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_S_Users column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_S_Users($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_S_Users',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_S_Users in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_S_Users($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_S_Users',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_S_Users Required ?
	 */
	function ISREQ_FKID_S_Users()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch UserName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_UserName($id_keys=false)
	{
		return parent::PRT_fetch_col('UserName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where UserName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_UserName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('UserName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is UserName Required ?
	 */
	function ISREQ_UserName()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Password column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Password($id_keys=false)
	{
		return parent::PRT_fetch_col('Password',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Password in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Password($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Password',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Password Required ?
	 */
	function ISREQ_Password()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Installation column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Installation($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Installation',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Installation in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Installation($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Installation',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Installation Required ?
	 */
	function ISREQ_FKID_Installation()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Password_Unix column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Password_Unix($id_keys=false)
	{
		return parent::PRT_fetch_col('Password_Unix',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Password_Unix in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Password_Unix($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Password_Unix',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Password_Unix Required ?
	 */
	function ISREQ_Password_Unix()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Password_Samba column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Password_Samba($id_keys=false)
	{
		return parent::PRT_fetch_col('Password_Samba',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Password_Samba in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Password_Samba($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Password_Samba',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Password_Samba Required ?
	 */
	function ISREQ_Password_Samba()
	{
		return true;
	}
}
