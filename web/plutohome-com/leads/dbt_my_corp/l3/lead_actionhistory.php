<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_ActionHistory_Row extends DB_Row
{
	/**
	 * @return DB3_ActionHistory_Row
	 * @param $PKID_ActionHistory str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_ActionHistory can also be an Array, an Object, or an ID-key
	 */
	function DB3_ActionHistory_Row($PKID_ActionHistory='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_ActionHistory_Table',$PKID_ActionHistory);
		if ($PKID_ActionHistory==='') parent::PRT_set_fields(array('FKID_Lead'=>0,'FKID_Action'=>0,'CompleteDate'=>'0000-00-00'));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_ActionHistory
	 */
	function GET_PKID_ActionHistory($as_string=false)
	{
		return parent::PRT_get_field('PKID_ActionHistory',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Lead
	 */
	function GET_FKID_Lead($as_string=false)
	{
		return parent::PRT_get_field('FKID_Lead',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Lead
	 */
	function GETFF_FKID_Lead($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_Lead',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Lead as an HTML_Tag object
	 */
	function & GETFFO_FKID_Lead($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_Lead',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Lead
	 */
	function SET_FKID_Lead($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Lead',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Lead
	 */
	function ORIG_FKID_Lead($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Lead',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_Lead to $match_arr
	 */
	function SETFLTR_FKID_Lead($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_Lead',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Action
	 */
	function GET_FKID_Action($as_string=false)
	{
		return parent::PRT_get_field('FKID_Action',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Action
	 */
	function GETFF_FKID_Action($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_Action',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Action as an HTML_Tag object
	 */
	function & GETFFO_FKID_Action($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_Action',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Action
	 */
	function SET_FKID_Action($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Action',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Action
	 */
	function ORIG_FKID_Action($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Action',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_Action to $match_arr
	 */
	function SETFLTR_FKID_Action($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_Action',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Employee
	 */
	function GET_FKID_Employee($as_string=false)
	{
		return parent::PRT_get_field('FKID_Employee',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Employee
	 */
	function GETFF_FKID_Employee($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_Employee',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Employee as an HTML_Tag object
	 */
	function & GETFFO_FKID_Employee($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_Employee',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Employee
	 */
	function SET_FKID_Employee($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Employee',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Employee
	 */
	function ORIG_FKID_Employee($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Employee',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_Employee to $match_arr
	 */
	function SETFLTR_FKID_Employee($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_Employee',$match_arr,$field_wide);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of StartDate
	 */
	function GET_StartDate($as_string=false)
	{
		return parent::PRT_get_field('StartDate',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for StartDate
	 */
	function GETFF_StartDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('StartDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for StartDate as an HTML_Tag object
	 */
	function & GETFFO_StartDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('StartDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of StartDate
	 */
	function SET_StartDate($value,$from_form=true)
	{
		return parent::PRT_set_field('StartDate',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of StartDate
	 */
	function ORIG_StartDate($as_string=false)
	{
		return parent::PRT_get_orig('StartDate',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of DueDate
	 */
	function GET_DueDate($as_string=false)
	{
		return parent::PRT_get_field('DueDate',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for DueDate
	 */
	function GETFF_DueDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('DueDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for DueDate as an HTML_Tag object
	 */
	function & GETFFO_DueDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('DueDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of DueDate
	 */
	function SET_DueDate($value,$from_form=true)
	{
		return parent::PRT_set_field('DueDate',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of DueDate
	 */
	function ORIG_DueDate($as_string=false)
	{
		return parent::PRT_get_orig('DueDate',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of CompleteDate
	 */
	function GET_CompleteDate($as_string=false)
	{
		return parent::PRT_get_field('CompleteDate',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CompleteDate
	 */
	function GETFF_CompleteDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CompleteDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CompleteDate as an HTML_Tag object
	 */
	function & GETFFO_CompleteDate($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CompleteDate',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CompleteDate
	 */
	function SET_CompleteDate($value,$from_form=true)
	{
		return parent::PRT_set_field('CompleteDate',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CompleteDate
	 */
	function ORIG_CompleteDate($as_string=false)
	{
		return parent::PRT_get_orig('CompleteDate',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Comments
	 */
	function GET_Comments($as_string=false)
	{
		return parent::PRT_get_field('Comments',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Comments
	 */
	function GETFF_Comments($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Comments',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Comments as an HTML_Tag object
	 */
	function & GETFFO_Comments($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Comments',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Comments
	 */
	function SET_Comments($value,$from_form=true)
	{
		return parent::PRT_set_field('Comments',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Comments
	 */
	function ORIG_Comments($as_string=false)
	{
		return parent::PRT_get_orig('Comments',$as_string);
	}
}

class DB3_ActionHistory_Table extends DB_Table
{
	function DB3_ActionHistory_Table(&$db)
	{
		parent::DB_Table($db,'ActionHistory','PKID_ActionHistory',true);

		$this->fields['PKID_ActionHistory'] =& new DB_Field_Int('PKID_ActionHistory',false);
		$this->fields['FKID_Lead'] =& new DB_Field_FK('FKID_Lead',false,'Lead','PKID_Lead','Name',$db,'DB_Field_Int');
		$this->fields['FKID_Action'] =& new DB_Field_FK('FKID_Action',false,'Action','PKID_Action','Description',$db,'DB_Field_Int');
		$this->fields['FKID_Employee'] =& new DB_Field_FK('FKID_Employee',true,'Employee','PKID_Employee','PKID_Employee',$db,'DB_Field_Int');
		$this->fields['StartDate'] =& new DB_Field_DateTime('StartDate',true);
		$this->fields['DueDate'] =& new DB_Field_DateTime('DueDate',true);
		$this->fields['CompleteDate'] =& new DB_Field_DateTime('CompleteDate',false);
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true);
	}

	/**
	 * @desc STATIC: get the tablename 'ActionHistory' 
	 */
	function S_get_tablename()
	{
		return 'ActionHistory';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_ActionHistory'=>1,'FKID_Lead'=>1,'FKID_Action'=>1,'FKID_Employee'=>1,'StartDate'=>1,'DueDate'=>1,'CompleteDate'=>1,'Comments'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_ActionHistory column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_ActionHistory($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_ActionHistory',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_ActionHistory in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_ActionHistory($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_ActionHistory',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_ActionHistory Required ?
	 */
	function ISREQ_PKID_ActionHistory()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Lead column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Lead($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Lead',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Lead in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Lead($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Lead',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_Lead to $match_arr
	 */
	function SETFLTR_FKID_Lead($match_arr)
	{
		parent::PRT_set_filter('FKID_Lead',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Lead Required ?
	 */
	function ISREQ_FKID_Lead()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Action column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Action($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Action',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Action in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Action($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Action',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_Action to $match_arr
	 */
	function SETFLTR_FKID_Action($match_arr)
	{
		parent::PRT_set_filter('FKID_Action',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Action Required ?
	 */
	function ISREQ_FKID_Action()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Employee column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Employee($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Employee',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Employee in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Employee($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Employee',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_Employee to $match_arr
	 */
	function SETFLTR_FKID_Employee($match_arr)
	{
		parent::PRT_set_filter('FKID_Employee',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Employee Required ?
	 */
	function ISREQ_FKID_Employee()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch StartDate column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_StartDate($id_keys=false)
	{
		return parent::PRT_fetch_col('StartDate',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where StartDate in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_StartDate($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('StartDate',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is StartDate Required ?
	 */
	function ISREQ_StartDate()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch DueDate column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_DueDate($id_keys=false)
	{
		return parent::PRT_fetch_col('DueDate',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where DueDate in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_DueDate($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('DueDate',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is DueDate Required ?
	 */
	function ISREQ_DueDate()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CompleteDate column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CompleteDate($id_keys=false)
	{
		return parent::PRT_fetch_col('CompleteDate',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CompleteDate in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CompleteDate($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CompleteDate',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CompleteDate Required ?
	 */
	function ISREQ_CompleteDate()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Comments column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Comments($id_keys=false)
	{
		return parent::PRT_fetch_col('Comments',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Comments in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Comments($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Comments',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Comments Required ?
	 */
	function ISREQ_Comments()
	{
		return false;
	}
}
