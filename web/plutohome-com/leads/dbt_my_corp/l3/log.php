<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Log_Row extends DB_Row
{
	/**
	 * @return DB3_Log_Row
	 * @param $PKID_Log str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Log can also be an Array, an Object, or an ID-key
	 */
	function DB3_Log_Row($PKID_Log='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Log_Table',$PKID_Log);
		if ($PKID_Log==='') parent::PRT_set_fields(array('ServerID'=>0,'DeviceID'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Log
	 */
	function GET_PKID_Log($as_string=false)
	{
		return parent::PRT_get_field('PKID_Log',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of ServerID
	 */
	function GET_ServerID($as_string=false)
	{
		return parent::PRT_get_field('ServerID',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ServerID
	 */
	function GETFF_ServerID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ServerID',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ServerID as an HTML_Tag object
	 */
	function & GETFFO_ServerID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ServerID',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of ServerID
	 */
	function SET_ServerID($value,$from_form=true)
	{
		return parent::PRT_set_field('ServerID',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of ServerID
	 */
	function ORIG_ServerID($as_string=false)
	{
		return parent::PRT_get_orig('ServerID',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of EntryTimeStamp
	 */
	function GET_EntryTimeStamp($as_string=false)
	{
		return parent::PRT_get_field('EntryTimeStamp',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EntryTimeStamp
	 */
	function GETFF_EntryTimeStamp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('EntryTimeStamp',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EntryTimeStamp as an HTML_Tag object
	 */
	function & GETFFO_EntryTimeStamp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('EntryTimeStamp',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of EntryTimeStamp
	 */
	function SET_EntryTimeStamp($value,$from_form=true)
	{
		return parent::PRT_set_field('EntryTimeStamp',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of EntryTimeStamp
	 */
	function ORIG_EntryTimeStamp($as_string=false)
	{
		return parent::PRT_get_orig('EntryTimeStamp',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Level
	 */
	function GET_Level($as_string=false)
	{
		return parent::PRT_get_field('Level',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Level
	 */
	function GETFF_Level($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Level',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Level as an HTML_Tag object
	 */
	function & GETFFO_Level($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Level',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Level
	 */
	function SET_Level($value,$from_form=true)
	{
		return parent::PRT_set_field('Level',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Level
	 */
	function ORIG_Level($as_string=false)
	{
		return parent::PRT_get_orig('Level',$as_string);
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
	 * @desc Get Value of DeviceID
	 */
	function GET_DeviceID($as_string=false)
	{
		return parent::PRT_get_field('DeviceID',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for DeviceID
	 */
	function GETFF_DeviceID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('DeviceID',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for DeviceID as an HTML_Tag object
	 */
	function & GETFFO_DeviceID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('DeviceID',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of DeviceID
	 */
	function SET_DeviceID($value,$from_form=true)
	{
		return parent::PRT_set_field('DeviceID',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of DeviceID
	 */
	function ORIG_DeviceID($as_string=false)
	{
		return parent::PRT_get_orig('DeviceID',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Data
	 */
	function GET_Data($as_string=false)
	{
		return parent::PRT_get_field('Data',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Data
	 */
	function GETFF_Data($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Data',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Data as an HTML_Tag object
	 */
	function & GETFFO_Data($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Data',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Data
	 */
	function SET_Data($value,$from_form=true)
	{
		return parent::PRT_set_field('Data',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Data
	 */
	function ORIG_Data($as_string=false)
	{
		return parent::PRT_get_orig('Data',$as_string);
	}
}

class DB3_Log_Table extends DB_Table
{
	function DB3_Log_Table(&$db)
	{
		parent::DB_Table($db,'Log','PKID_Log',true);

		$this->fields['PKID_Log'] =& new DB_Field_Int('PKID_Log',false);
		$this->fields['ServerID'] =& new DB_Field_Int('ServerID',false);
		$this->fields['EntryTimeStamp'] =& new DB_Field_Int('EntryTimeStamp',true);
		$this->fields['Level'] =& new DB_Field_Int('Level',true);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['DeviceID'] =& new DB_Field_Int('DeviceID',false);
		$this->fields['Data'] =& new DB_Field_Text('Data',true);
	}

	/**
	 * @desc STATIC: get the tablename 'Log' 
	 */
	function S_get_tablename()
	{
		return 'Log';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Log'=>1,'ServerID'=>1,'EntryTimeStamp'=>1,'Level'=>1,'Name'=>1,'DeviceID'=>1,'Data'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Log column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Log($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Log',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Log in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Log($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Log',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Log Required ?
	 */
	function ISREQ_PKID_Log()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ServerID column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ServerID($id_keys=false)
	{
		return parent::PRT_fetch_col('ServerID',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ServerID in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ServerID($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ServerID',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ServerID Required ?
	 */
	function ISREQ_ServerID()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch EntryTimeStamp column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_EntryTimeStamp($id_keys=false)
	{
		return parent::PRT_fetch_col('EntryTimeStamp',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where EntryTimeStamp in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_EntryTimeStamp($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('EntryTimeStamp',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is EntryTimeStamp Required ?
	 */
	function ISREQ_EntryTimeStamp()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Level column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Level($id_keys=false)
	{
		return parent::PRT_fetch_col('Level',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Level in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Level($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Level',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Level Required ?
	 */
	function ISREQ_Level()
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
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch DeviceID column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_DeviceID($id_keys=false)
	{
		return parent::PRT_fetch_col('DeviceID',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where DeviceID in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_DeviceID($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('DeviceID',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is DeviceID Required ?
	 */
	function ISREQ_DeviceID()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Data column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Data($id_keys=false)
	{
		return parent::PRT_fetch_col('Data',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Data in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Data($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Data',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Data Required ?
	 */
	function ISREQ_Data()
	{
		return false;
	}
}
