<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Events_Row extends DB_Row
{
	/**
	 * @return DB3_Events_Row
	 * @param $PKID_Event str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Event can also be an Array, an Object, or an ID-key
	 */
	function DB3_Events_Row($PKID_Event='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Events_Table',$PKID_Event);
		if ($PKID_Event==='') parent::PRT_set_fields(array('FKID_Device'=>0,'EventType'=>0,'FKID_C_Status'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Event
	 */
	function GET_PKID_Event($as_string=false)
	{
		return parent::PRT_get_field('PKID_Event',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Device
	 */
	function GET_FKID_Device($as_string=false)
	{
		return parent::PRT_get_field('FKID_Device',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Device
	 */
	function GETFF_FKID_Device($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_Device',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Device as an HTML_Tag object
	 */
	function & GETFFO_FKID_Device($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_Device',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Device
	 */
	function SET_FKID_Device($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Device',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Device
	 */
	function ORIG_FKID_Device($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Device',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of EventDate
	 */
	function GET_EventDate($as_string=false)
	{
		return parent::PRT_get_field('EventDate',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EventDate
	 */
	function GETFF_EventDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('EventDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EventDate as an HTML_Tag object
	 */
	function & GETFFO_EventDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('EventDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of EventDate
	 */
	function SET_EventDate($value,$from_form=true)
	{
		return parent::PRT_set_field('EventDate',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of EventDate
	 */
	function ORIG_EventDate($as_string=false)
	{
		return parent::PRT_get_orig('EventDate',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of EventType
	 */
	function GET_EventType($as_string=false)
	{
		return parent::PRT_get_field('EventType',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EventType
	 */
	function GETFF_EventType($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('EventType',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for EventType as an HTML_Tag object
	 */
	function & GETFFO_EventType($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('EventType',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of EventType
	 */
	function SET_EventType($value,$from_form=true)
	{
		return parent::PRT_set_field('EventType',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of EventType
	 */
	function ORIG_EventType($as_string=false)
	{
		return parent::PRT_get_orig('EventType',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_C_Status
	 */
	function GET_FKID_C_Status($as_string=false)
	{
		return parent::PRT_get_field('FKID_C_Status',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_C_Status
	 */
	function GETFF_FKID_C_Status($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_C_Status',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_C_Status as an HTML_Tag object
	 */
	function & GETFFO_FKID_C_Status($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_C_Status',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_C_Status
	 */
	function SET_FKID_C_Status($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_C_Status',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_C_Status
	 */
	function ORIG_FKID_C_Status($as_string=false)
	{
		return parent::PRT_get_orig('FKID_C_Status',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_C_Status to $match_arr
	 */
	function SETFLTR_FKID_C_Status($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_C_Status',$match_arr,$field_wide);
	}
}

class DB3_Events_Table extends DB_Table
{
	function DB3_Events_Table(&$db)
	{
		parent::DB_Table($db,'Events','PKID_Event',true);

		$this->fields['PKID_Event'] =& new DB_Field_Int('PKID_Event',false);
		$this->fields['FKID_Device'] =& new DB_Field_Int('FKID_Device',false);
		$this->fields['EventDate'] =& new DB_Field_DateTime('EventDate',true);
		$this->fields['EventType'] =& new DB_Field_Int('EventType',false);
		$this->fields['FKID_C_Status'] =& new DB_Field_FK('FKID_C_Status',false,'C_Status','PKID_Status','Description',$db,'DB_Field_Int');
	}

	/**
	 * @desc STATIC: get the tablename 'Events' 
	 */
	function S_get_tablename()
	{
		return 'Events';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Event'=>1,'FKID_Device'=>1,'EventDate'=>1,'EventType'=>1,'FKID_C_Status'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Event column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Event($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Event',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Event in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Event($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Event',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Event Required ?
	 */
	function ISREQ_PKID_Event()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Device column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Device($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Device',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Device in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Device($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Device',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Device Required ?
	 */
	function ISREQ_FKID_Device()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch EventDate column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_EventDate($id_keys=false)
	{
		return parent::PRT_fetch_col('EventDate',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where EventDate in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_EventDate($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('EventDate',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is EventDate Required ?
	 */
	function ISREQ_EventDate()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch EventType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_EventType($id_keys=false)
	{
		return parent::PRT_fetch_col('EventType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where EventType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_EventType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('EventType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is EventType Required ?
	 */
	function ISREQ_EventType()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_C_Status column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_C_Status($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_C_Status',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_C_Status in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_C_Status($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_C_Status',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_C_Status to $match_arr
	 */
	function SETFLTR_FKID_C_Status($match_arr)
	{
		parent::PRT_set_filter('FKID_C_Status',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_C_Status Required ?
	 */
	function ISREQ_FKID_C_Status()
	{
		return true;
	}
}
