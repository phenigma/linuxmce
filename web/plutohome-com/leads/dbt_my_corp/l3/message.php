<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Message_Row extends DB_Row
{
	/**
	 * @return DB3_Message_Row
	 * @param $PKID_Message str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Message can also be an Array, an Object, or an ID-key
	 */
	function DB3_Message_Row($PKID_Message='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Message_Table',$PKID_Message);
		if ($PKID_Message==='') parent::PRT_set_fields(array('Severity'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Message
	 */
	function GET_PKID_Message($as_string=false)
	{
		return parent::PRT_get_field('PKID_Message',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Severity
	 */
	function GET_Severity($as_string=false)
	{
		return parent::PRT_get_field('Severity',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Severity
	 */
	function GETFF_Severity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Severity',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Severity as an HTML_Tag object
	 */
	function & GETFFO_Severity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Severity',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Severity
	 */
	function SET_Severity($value,$from_form=true)
	{
		return parent::PRT_set_field('Severity',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Severity
	 */
	function ORIG_Severity($as_string=false)
	{
		return parent::PRT_get_orig('Severity',$as_string);
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
	 * @desc Get Value of Message
	 */
	function GET_Message($as_string=false)
	{
		return parent::PRT_get_field('Message',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Message
	 */
	function GETFF_Message($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Message',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Message as an HTML_Tag object
	 */
	function & GETFFO_Message($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Message',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Message
	 */
	function SET_Message($value,$from_form=true)
	{
		return parent::PRT_set_field('Message',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Message
	 */
	function ORIG_Message($as_string=false)
	{
		return parent::PRT_get_orig('Message',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Timestamp
	 */
	function GET_Timestamp($as_string=false)
	{
		return parent::PRT_get_field('Timestamp',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Timestamp
	 */
	function GETFF_Timestamp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Timestamp',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Timestamp as an HTML_Tag object
	 */
	function & GETFFO_Timestamp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Timestamp',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Timestamp
	 */
	function SET_Timestamp($value,$from_form=true)
	{
		return parent::PRT_set_field('Timestamp',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Timestamp
	 */
	function ORIG_Timestamp($as_string=false)
	{
		return parent::PRT_get_orig('Timestamp',$as_string);
	}
}

class DB3_Message_Table extends DB_Table
{
	function DB3_Message_Table(&$db)
	{
		parent::DB_Table($db,'Message','PKID_Message',true);

		$this->fields['PKID_Message'] =& new DB_Field_Int('PKID_Message',false);
		$this->fields['Severity'] =& new DB_Field_Int('Severity',false);
		$this->fields['FKID_Installation'] =& new DB_Field_Int('FKID_Installation',true);
		$this->fields['Message'] =& new DB_Field_Text('Message',true);
		$this->fields['Timestamp'] =& new DB_Field_DateTime('Timestamp',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Message' 
	 */
	function S_get_tablename()
	{
		return 'Message';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Message'=>1,'Severity'=>1,'FKID_Installation'=>1,'Message'=>1,'Timestamp'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Message column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Message($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Message',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Message in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Message($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Message',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Message Required ?
	 */
	function ISREQ_PKID_Message()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Severity column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Severity($id_keys=false)
	{
		return parent::PRT_fetch_col('Severity',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Severity in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Severity($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Severity',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Severity Required ?
	 */
	function ISREQ_Severity()
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
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Message column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Message($id_keys=false)
	{
		return parent::PRT_fetch_col('Message',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Message in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Message($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Message',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Message Required ?
	 */
	function ISREQ_Message()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Timestamp column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Timestamp($id_keys=false)
	{
		return parent::PRT_fetch_col('Timestamp',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Timestamp in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Timestamp($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Timestamp',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Timestamp Required ?
	 */
	function ISREQ_Timestamp()
	{
		return true;
	}
}
