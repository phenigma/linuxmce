<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_CurStatus_Row extends DB_Row
{
	/**
	 * @return DB3_CurStatus_Row
	 * @param $FKID_Device str
	 * @desc Will return FALSE if a matching row is not found
	 * FKID_Device can also be an Array, an Object, or an ID-key
	 */
	function DB3_CurStatus_Row($FKID_Device='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_CurStatus_Table',$FKID_Device);
		if ($FKID_Device==='') parent::PRT_set_fields(array('FKID_Device'=>0));
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
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Event_Stop
	 */
	function GET_FKID_Event_Stop($as_string=false)
	{
		return parent::PRT_get_field('FKID_Event_Stop',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Event_Stop
	 */
	function GETFF_FKID_Event_Stop($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_Event_Stop',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Event_Stop as an HTML_Tag object
	 */
	function & GETFFO_FKID_Event_Stop($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_Event_Stop',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Event_Stop
	 */
	function SET_FKID_Event_Stop($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Event_Stop',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Event_Stop
	 */
	function ORIG_FKID_Event_Stop($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Event_Stop',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Event_Start
	 */
	function GET_FKID_Event_Start($as_string=false)
	{
		return parent::PRT_get_field('FKID_Event_Start',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Event_Start
	 */
	function GETFF_FKID_Event_Start($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('FKID_Event_Start',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Event_Start as an HTML_Tag object
	 */
	function & GETFFO_FKID_Event_Start($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('FKID_Event_Start',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Event_Start
	 */
	function SET_FKID_Event_Start($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Event_Start',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Event_Start
	 */
	function ORIG_FKID_Event_Start($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Event_Start',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of HostAddress
	 */
	function GET_HostAddress($as_string=false)
	{
		return parent::PRT_get_field('HostAddress',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HostAddress
	 */
	function GETFF_HostAddress($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('HostAddress',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for HostAddress as an HTML_Tag object
	 */
	function & GETFFO_HostAddress($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('HostAddress',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of HostAddress
	 */
	function SET_HostAddress($value,$from_form=true)
	{
		return parent::PRT_set_field('HostAddress',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of HostAddress
	 */
	function ORIG_HostAddress($as_string=false)
	{
		return parent::PRT_get_orig('HostAddress',$as_string);
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
	 * @desc Get Value of Since
	 */
	function GET_Since($as_string=false)
	{
		return parent::PRT_get_field('Since',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Since
	 */
	function GETFF_Since($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Since',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Since as an HTML_Tag object
	 */
	function & GETFFO_Since($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Since',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Since
	 */
	function SET_Since($value,$from_form=true)
	{
		return parent::PRT_set_field('Since',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Since
	 */
	function ORIG_Since($as_string=false)
	{
		return parent::PRT_get_orig('Since',$as_string);
	}
}

class DB3_CurStatus_Table extends DB_Table
{
	function DB3_CurStatus_Table(&$db)
	{
		parent::DB_Table($db,'CurStatus','FKID_Device',false);

		$this->fields['FKID_Device'] =& new DB_Field_Int('FKID_Device',false);
		$this->fields['FKID_Event_Stop'] =& new DB_Field_Int('FKID_Event_Stop',true);
		$this->fields['FKID_Event_Start'] =& new DB_Field_Int('FKID_Event_Start',true);
		$this->fields['HostAddress'] =& new DB_Field_Varchar('HostAddress',true);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['Since'] =& new DB_Field_DateTime('Since',false);
	}

	/**
	 * @desc STATIC: get the tablename 'CurStatus' 
	 */
	function S_get_tablename()
	{
		return 'CurStatus';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('FKID_Device'=>1,'FKID_Event_Stop'=>1,'FKID_Event_Start'=>1,'HostAddress'=>1,'Name'=>1,'Since'=>1);
		return isset($fields[$fieldname]);
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
	 * @desc Fetch FKID_Event_Stop column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Event_Stop($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Event_Stop',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Event_Stop in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Event_Stop($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Event_Stop',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Event_Stop Required ?
	 */
	function ISREQ_FKID_Event_Stop()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Event_Start column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Event_Start($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Event_Start',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Event_Start in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Event_Start($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Event_Start',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Event_Start Required ?
	 */
	function ISREQ_FKID_Event_Start()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch HostAddress column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_HostAddress($id_keys=false)
	{
		return parent::PRT_fetch_col('HostAddress',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where HostAddress in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_HostAddress($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('HostAddress',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is HostAddress Required ?
	 */
	function ISREQ_HostAddress()
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
	 * @desc Fetch Since column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Since($id_keys=false)
	{
		return parent::PRT_fetch_col('Since',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Since in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Since($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Since',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Since Required ?
	 */
	function ISREQ_Since()
	{
		return true;
	}
}
