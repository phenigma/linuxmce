<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_Lead_Row extends DB_Row
{
	/**
	 * @return DB3_Lead_Row
	 * @param $PKID_Lead str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_Lead can also be an Array, an Object, or an ID-key
	 */
	function DB3_Lead_Row($PKID_Lead='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_Lead_Table',$PKID_Lead);
		if ($PKID_Lead==='') parent::PRT_set_fields(array('FKID_LeadType'=>0,'FKID_Status'=>0,'TechSupport'=>0));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from ActionHistory by FKID_Lead
	 */
	function GETCHLD_ActionHistory__FKID_Lead()
	{
		return parent::PRT_get_field_children('FKID_Lead','ActionHistory');
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from ActionPending by FKID_Lead
	 */
	function GETCHLD_ActionPending__FKID_Lead()
	{
		return parent::PRT_get_field_children('FKID_Lead','ActionPending');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_Lead
	 */
	function GET_PKID_Lead($as_string=false)
	{
		return parent::PRT_get_field('PKID_Lead',$as_string);
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
	 * @desc Get Value of CustomerID
	 */
	function GET_CustomerID($as_string=false)
	{
		return parent::PRT_get_field('CustomerID',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CustomerID
	 */
	function GETFF_CustomerID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CustomerID',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CustomerID as an HTML_Tag object
	 */
	function & GETFFO_CustomerID($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CustomerID',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CustomerID
	 */
	function SET_CustomerID($value,$from_form=true)
	{
		return parent::PRT_set_field('CustomerID',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CustomerID
	 */
	function ORIG_CustomerID($as_string=false)
	{
		return parent::PRT_get_orig('CustomerID',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Address1
	 */
	function GET_Address1($as_string=false)
	{
		return parent::PRT_get_field('Address1',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address1
	 */
	function GETFF_Address1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Address1',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address1 as an HTML_Tag object
	 */
	function & GETFFO_Address1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Address1',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Address1
	 */
	function SET_Address1($value,$from_form=true)
	{
		return parent::PRT_set_field('Address1',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Address1
	 */
	function ORIG_Address1($as_string=false)
	{
		return parent::PRT_get_orig('Address1',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Address2
	 */
	function GET_Address2($as_string=false)
	{
		return parent::PRT_get_field('Address2',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address2
	 */
	function GETFF_Address2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Address2',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Address2 as an HTML_Tag object
	 */
	function & GETFFO_Address2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Address2',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Address2
	 */
	function SET_Address2($value,$from_form=true)
	{
		return parent::PRT_set_field('Address2',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Address2
	 */
	function ORIG_Address2($as_string=false)
	{
		return parent::PRT_get_orig('Address2',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of City
	 */
	function GET_City($as_string=false)
	{
		return parent::PRT_get_field('City',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for City
	 */
	function GETFF_City($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('City',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for City as an HTML_Tag object
	 */
	function & GETFFO_City($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('City',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of City
	 */
	function SET_City($value,$from_form=true)
	{
		return parent::PRT_set_field('City',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of City
	 */
	function ORIG_City($as_string=false)
	{
		return parent::PRT_get_orig('City',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of State
	 */
	function GET_State($as_string=false)
	{
		return parent::PRT_get_field('State',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for State
	 */
	function GETFF_State($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('State',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for State as an HTML_Tag object
	 */
	function & GETFFO_State($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('State',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of State
	 */
	function SET_State($value,$from_form=true)
	{
		return parent::PRT_set_field('State',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of State
	 */
	function ORIG_State($as_string=false)
	{
		return parent::PRT_get_orig('State',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Postal_Code
	 */
	function GET_Postal_Code($as_string=false)
	{
		return parent::PRT_get_field('Postal_Code',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Postal_Code
	 */
	function GETFF_Postal_Code($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Postal_Code',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Postal_Code as an HTML_Tag object
	 */
	function & GETFFO_Postal_Code($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Postal_Code',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Postal_Code
	 */
	function SET_Postal_Code($value,$from_form=true)
	{
		return parent::PRT_set_field('Postal_Code',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Postal_Code
	 */
	function ORIG_Postal_Code($as_string=false)
	{
		return parent::PRT_get_orig('Postal_Code',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Country
	 */
	function GET_Country($as_string=false)
	{
		return parent::PRT_get_field('Country',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Country
	 */
	function GETFF_Country($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Country',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Country as an HTML_Tag object
	 */
	function & GETFFO_Country($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Country',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Country
	 */
	function SET_Country($value,$from_form=true)
	{
		return parent::PRT_set_field('Country',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Country
	 */
	function ORIG_Country($as_string=false)
	{
		return parent::PRT_get_orig('Country',$as_string);
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
	 * @desc Get Value of Fax
	 */
	function GET_Fax($as_string=false)
	{
		return parent::PRT_get_field('Fax',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Fax
	 */
	function GETFF_Fax($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Fax',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Fax as an HTML_Tag object
	 */
	function & GETFFO_Fax($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Fax',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Fax
	 */
	function SET_Fax($value,$from_form=true)
	{
		return parent::PRT_set_field('Fax',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Fax
	 */
	function ORIG_Fax($as_string=false)
	{
		return parent::PRT_get_orig('Fax',$as_string);
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
	 * @desc Get Value of website
	 */
	function GET_website($as_string=false)
	{
		return parent::PRT_get_field('website',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for website
	 */
	function GETFF_website($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('website',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for website as an HTML_Tag object
	 */
	function & GETFFO_website($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('website',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of website
	 */
	function SET_website($value,$from_form=true)
	{
		return parent::PRT_set_field('website',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of website
	 */
	function ORIG_website($as_string=false)
	{
		return parent::PRT_get_orig('website',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_LeadType
	 */
	function GET_FKID_LeadType($as_string=false)
	{
		return parent::PRT_get_field('FKID_LeadType',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_LeadType
	 */
	function GETFF_FKID_LeadType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_LeadType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_LeadType as an HTML_Tag object
	 */
	function & GETFFO_FKID_LeadType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_LeadType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_LeadType
	 */
	function SET_FKID_LeadType($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_LeadType',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_LeadType
	 */
	function ORIG_FKID_LeadType($as_string=false)
	{
		return parent::PRT_get_orig('FKID_LeadType',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_LeadType to $match_arr
	 */
	function SETFLTR_FKID_LeadType($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_LeadType',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_Status
	 */
	function GET_FKID_Status($as_string=false)
	{
		return parent::PRT_get_field('FKID_Status',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Status
	 */
	function GETFF_FKID_Status($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_Status',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_Status as an HTML_Tag object
	 */
	function & GETFFO_FKID_Status($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_Status',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_Status
	 */
	function SET_FKID_Status($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_Status',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_Status
	 */
	function ORIG_FKID_Status($as_string=false)
	{
		return parent::PRT_get_orig('FKID_Status',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_Status to $match_arr
	 */
	function SETFLTR_FKID_Status($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_Status',$match_arr,$field_wide);
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

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of TechSupport
	 */
	function GET_TechSupport($as_string=false)
	{
		return parent::PRT_get_field('TechSupport',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for TechSupport
	 */
	function GETFF_TechSupport($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('TechSupport',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for TechSupport as an HTML_Tag object
	 */
	function & GETFFO_TechSupport($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('TechSupport',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of TechSupport
	 */
	function SET_TechSupport($value,$from_form=true)
	{
		return parent::PRT_set_field('TechSupport',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of TechSupport
	 */
	function ORIG_TechSupport($as_string=false)
	{
		return parent::PRT_get_orig('TechSupport',$as_string);
	}
}

class DB3_Lead_Table extends DB_Table
{
	function DB3_Lead_Table(&$db)
	{
		parent::DB_Table($db,'Lead','PKID_Lead',true);

		$this->fields['PKID_Lead'] =& new DB_Field_Int('PKID_Lead',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',true);
		$this->fields['CustomerID'] =& new DB_Field_Varchar('CustomerID',true);
		$this->fields['Address1'] =& new DB_Field_Varchar('Address1',true);
		$this->fields['Address2'] =& new DB_Field_Varchar('Address2',true);
		$this->fields['City'] =& new DB_Field_Varchar('City',true);
		$this->fields['State'] =& new DB_Field_Varchar('State',true);
		$this->fields['Postal_Code'] =& new DB_Field_Varchar('Postal_Code',true);
		$this->fields['Country'] =& new DB_Field_Varchar('Country',true);
		$this->fields['Phone'] =& new DB_Field_Varchar('Phone',true);
		$this->fields['Fax'] =& new DB_Field_Varchar('Fax',true);
		$this->fields['Email'] =& new DB_Field_Varchar('Email',true);
		$this->fields['website'] =& new DB_Field_Varchar('website',true);
		$this->fields['FKID_LeadType'] =& new DB_Field_FK('FKID_LeadType',false,'LeadType','PKID_LeadType','Description',$db,'DB_Field_Int');
		$this->fields['FKID_Status'] =& new DB_Field_FK('FKID_Status',false,'Status','PKID_Status','Description',$db,'DB_Field_Int');
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true);
		$this->fields['TechSupport'] =& new DB_Field_Int('TechSupport',false);

		parent::PRT_add_pk_ref('ActionHistory','FKID_Lead',false);
		parent::PRT_add_pk_ref('ActionPending','FKID_Lead',false);
	}

	/**
	 * @desc STATIC: get the tablename 'Lead' 
	 */
	function S_get_tablename()
	{
		return 'Lead';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_Lead'=>1,'Name'=>1,'CustomerID'=>1,'Address1'=>1,'Address2'=>1,'City'=>1,'State'=>1,'Postal_Code'=>1,'Country'=>1,'Phone'=>1,'Fax'=>1,'Email'=>1,'website'=>1,'FKID_LeadType'=>1,'FKID_Status'=>1,'Comments'=>1,'TechSupport'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_Lead column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_Lead($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_Lead',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_Lead in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_Lead($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_Lead',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_Lead Required ?
	 */
	function ISREQ_PKID_Lead()
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
	 * @desc Fetch CustomerID column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CustomerID($id_keys=false)
	{
		return parent::PRT_fetch_col('CustomerID',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CustomerID in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CustomerID($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CustomerID',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CustomerID Required ?
	 */
	function ISREQ_CustomerID()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Address1 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Address1($id_keys=false)
	{
		return parent::PRT_fetch_col('Address1',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Address1 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Address1($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Address1',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Address1 Required ?
	 */
	function ISREQ_Address1()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Address2 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Address2($id_keys=false)
	{
		return parent::PRT_fetch_col('Address2',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Address2 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Address2($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Address2',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Address2 Required ?
	 */
	function ISREQ_Address2()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch City column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_City($id_keys=false)
	{
		return parent::PRT_fetch_col('City',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where City in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_City($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('City',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is City Required ?
	 */
	function ISREQ_City()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch State column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_State($id_keys=false)
	{
		return parent::PRT_fetch_col('State',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where State in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_State($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('State',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is State Required ?
	 */
	function ISREQ_State()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Postal_Code column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Postal_Code($id_keys=false)
	{
		return parent::PRT_fetch_col('Postal_Code',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Postal_Code in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Postal_Code($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Postal_Code',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Postal_Code Required ?
	 */
	function ISREQ_Postal_Code()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Country column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Country($id_keys=false)
	{
		return parent::PRT_fetch_col('Country',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Country in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Country($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Country',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Country Required ?
	 */
	function ISREQ_Country()
	{
		return false;
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
	 * @desc Fetch Fax column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Fax($id_keys=false)
	{
		return parent::PRT_fetch_col('Fax',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Fax in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Fax($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Fax',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Fax Required ?
	 */
	function ISREQ_Fax()
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
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch website column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_website($id_keys=false)
	{
		return parent::PRT_fetch_col('website',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where website in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_website($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('website',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is website Required ?
	 */
	function ISREQ_website()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_LeadType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_LeadType($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_LeadType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_LeadType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_LeadType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_LeadType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_LeadType to $match_arr
	 */
	function SETFLTR_FKID_LeadType($match_arr)
	{
		parent::PRT_set_filter('FKID_LeadType',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_LeadType Required ?
	 */
	function ISREQ_FKID_LeadType()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_Status column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_Status($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_Status',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_Status in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_Status($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_Status',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_Status to $match_arr
	 */
	function SETFLTR_FKID_Status($match_arr)
	{
		parent::PRT_set_filter('FKID_Status',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_Status Required ?
	 */
	function ISREQ_FKID_Status()
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

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch TechSupport column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_TechSupport($id_keys=false)
	{
		return parent::PRT_fetch_col('TechSupport',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where TechSupport in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_TechSupport($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('TechSupport',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is TechSupport Required ?
	 */
	function ISREQ_TechSupport()
	{
		return true;
	}
}
