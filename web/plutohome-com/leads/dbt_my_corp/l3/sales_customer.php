<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_SALES_Customer_Row extends DB_Row
{
	/**
	 * @return DB3_SALES_Customer_Row
	 * @param $PKID_SALES_Customer str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_SALES_Customer can also be an Array, an Object, or an ID-key
	 */
	function DB3_SALES_Customer_Row($PKID_SALES_Customer='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_SALES_Customer_Table',$PKID_SALES_Customer);
		if ($PKID_SALES_Customer==='') parent::PRT_set_fields(array('PwdIsTemp'=>0,'CreatedOn'=>'0000-00-00 00:00:00','ModifiedOn'=>'0000-00-00 00:00:00','LastLoggedIn'=>'0000-00-00 00:00:00'));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from SALES_Installation by FKID_SALES_Customer
	 */
	function GETCHLD_SALES_Installation__FKID_SALES_Customer()
	{
		return parent::PRT_get_field_children('FKID_SALES_Customer','SALES_Installation');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_SALES_Customer
	 */
	function GET_PKID_SALES_Customer($as_string=false)
	{
		return parent::PRT_get_field('PKID_SALES_Customer',$as_string);
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
	 * @desc Get Value of Email
	 */
	function GET_Email($as_string=false)
	{
		return parent::PRT_get_field('Email',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Email
	 */
	function GETFF_Email($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Email',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Email as an HTML_Tag object
	 */
	function & GETFFO_Email($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Email',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Email
	 */
	function SET_Email($value,$from_form=true)
	{
		return parent::PRT_set_field('Email',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Email
	 */
	function ORIG_Email($as_string=false)
	{
		return parent::PRT_get_orig('Email',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Phone
	 */
	function GET_Phone($as_string=false)
	{
		return parent::PRT_get_field('Phone',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Phone
	 */
	function GETFF_Phone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Phone',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Phone as an HTML_Tag object
	 */
	function & GETFFO_Phone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Phone',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Phone
	 */
	function SET_Phone($value,$from_form=true)
	{
		return parent::PRT_set_field('Phone',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Phone
	 */
	function ORIG_Phone($as_string=false)
	{
		return parent::PRT_get_orig('Phone',$as_string);
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
	 * @desc Get Value of PwdIsTemp
	 */
	function GET_PwdIsTemp($as_string=false)
	{
		return parent::PRT_get_field('PwdIsTemp',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PwdIsTemp
	 */
	function GETFF_PwdIsTemp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('PwdIsTemp',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for PwdIsTemp as an HTML_Tag object
	 */
	function & GETFFO_PwdIsTemp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('PwdIsTemp',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of PwdIsTemp
	 */
	function SET_PwdIsTemp($value,$from_form=true)
	{
		return parent::PRT_set_field('PwdIsTemp',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of PwdIsTemp
	 */
	function ORIG_PwdIsTemp($as_string=false)
	{
		return parent::PRT_get_orig('PwdIsTemp',$as_string);
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
	 * @desc Get Value of LastLoggedIn
	 */
	function GET_LastLoggedIn($as_string=false)
	{
		return parent::PRT_get_field('LastLoggedIn',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastLoggedIn
	 */
	function GETFF_LastLoggedIn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('LastLoggedIn',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for LastLoggedIn as an HTML_Tag object
	 */
	function & GETFFO_LastLoggedIn($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('LastLoggedIn',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of LastLoggedIn
	 */
	function SET_LastLoggedIn($value,$from_form=true)
	{
		return parent::PRT_set_field('LastLoggedIn',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of LastLoggedIn
	 */
	function ORIG_LastLoggedIn($as_string=false)
	{
		return parent::PRT_get_orig('LastLoggedIn',$as_string);
	}
}

class DB3_SALES_Customer_Table extends DB_Table
{
	function DB3_SALES_Customer_Table(&$db)
	{
		parent::DB_Table($db,'SALES_Customer','PKID_SALES_Customer',true);

		$this->fields['PKID_SALES_Customer'] =& new DB_Field_Int('PKID_SALES_Customer',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['Email'] =& new DB_Field_Varchar('Email',false);
		$this->fields['Phone'] =& new DB_Field_Varchar('Phone',true);
		$this->fields['Password'] =& new DB_Field_Pwd('Password',false);
		$this->fields['PwdIsTemp'] =& new DB_Field_Int('PwdIsTemp',false);
		$this->fields['CreatedOn'] =& new DB_Field_DateTime('CreatedOn',false);
		$this->fields['ModifiedOn'] =& new DB_Field_DateTime('ModifiedOn',false);
		$this->fields['LastLoggedIn'] =& new DB_Field_DateTime('LastLoggedIn',false);

		parent::PRT_add_pk_ref('SALES_Installation','FKID_SALES_Customer',false);
	}

	/**
	 * @desc STATIC: get the tablename 'SALES_Customer' 
	 */
	function S_get_tablename()
	{
		return 'SALES_Customer';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_SALES_Customer'=>1,'Name'=>1,'Email'=>1,'Phone'=>1,'Password'=>1,'PwdIsTemp'=>1,'CreatedOn'=>1,'ModifiedOn'=>1,'LastLoggedIn'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_SALES_Customer column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_SALES_Customer($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_SALES_Customer',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_SALES_Customer in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_SALES_Customer($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_SALES_Customer',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_SALES_Customer Required ?
	 */
	function ISREQ_PKID_SALES_Customer()
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
	 * @desc Fetch Email column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Email($id_keys=false)
	{
		return parent::PRT_fetch_col('Email',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Email in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Email($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Email',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Email Required ?
	 */
	function ISREQ_Email()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Phone column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Phone($id_keys=false)
	{
		return parent::PRT_fetch_col('Phone',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Phone in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Phone($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Phone',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Phone Required ?
	 */
	function ISREQ_Phone()
	{
		return false;
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
	 * @desc Fetch PwdIsTemp column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PwdIsTemp($id_keys=false)
	{
		return parent::PRT_fetch_col('PwdIsTemp',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PwdIsTemp in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PwdIsTemp($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PwdIsTemp',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PwdIsTemp Required ?
	 */
	function ISREQ_PwdIsTemp()
	{
		return true;
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
	 * @desc Fetch LastLoggedIn column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_LastLoggedIn($id_keys=false)
	{
		return parent::PRT_fetch_col('LastLoggedIn',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where LastLoggedIn in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_LastLoggedIn($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('LastLoggedIn',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is LastLoggedIn Required ?
	 */
	function ISREQ_LastLoggedIn()
	{
		return true;
	}
}
