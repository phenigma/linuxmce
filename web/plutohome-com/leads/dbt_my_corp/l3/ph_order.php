<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_PH_Order_Row extends DB_Row
{
	/**
	 * @return DB3_PH_Order_Row
	 * @param $PKID_PH_Order str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_PH_Order can also be an Array, an Object, or an ID-key
	 */
	function DB3_PH_Order_Row($PKID_PH_Order='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_PH_Order_Table',$PKID_PH_Order);
		if ($PKID_PH_Order==='') parent::PRT_set_fields(array('Total'=>0.00));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from PH_Order_Item by FKID_PH_Order
	 */
	function GETCHLD_PH_Order_Item__FKID_PH_Order()
	{
		return parent::PRT_get_field_children('FKID_PH_Order','PH_Order_Item');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_PH_Order
	 */
	function GET_PKID_PH_Order($as_string=false)
	{
		return parent::PRT_get_field('PKID_PH_Order',$as_string);
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
	 * @desc Get Value of BillFirstName
	 */
	function GET_BillFirstName($as_string=false)
	{
		return parent::PRT_get_field('BillFirstName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillFirstName
	 */
	function GETFF_BillFirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillFirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillFirstName as an HTML_Tag object
	 */
	function & GETFFO_BillFirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillFirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillFirstName
	 */
	function SET_BillFirstName($value,$from_form=true)
	{
		return parent::PRT_set_field('BillFirstName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillFirstName
	 */
	function ORIG_BillFirstName($as_string=false)
	{
		return parent::PRT_get_orig('BillFirstName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillLastName
	 */
	function GET_BillLastName($as_string=false)
	{
		return parent::PRT_get_field('BillLastName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillLastName
	 */
	function GETFF_BillLastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillLastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillLastName as an HTML_Tag object
	 */
	function & GETFFO_BillLastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillLastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillLastName
	 */
	function SET_BillLastName($value,$from_form=true)
	{
		return parent::PRT_set_field('BillLastName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillLastName
	 */
	function ORIG_BillLastName($as_string=false)
	{
		return parent::PRT_get_orig('BillLastName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillPhone
	 */
	function GET_BillPhone($as_string=false)
	{
		return parent::PRT_get_field('BillPhone',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillPhone
	 */
	function GETFF_BillPhone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillPhone',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillPhone as an HTML_Tag object
	 */
	function & GETFFO_BillPhone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillPhone',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillPhone
	 */
	function SET_BillPhone($value,$from_form=true)
	{
		return parent::PRT_set_field('BillPhone',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillPhone
	 */
	function ORIG_BillPhone($as_string=false)
	{
		return parent::PRT_get_orig('BillPhone',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillAddress1
	 */
	function GET_BillAddress1($as_string=false)
	{
		return parent::PRT_get_field('BillAddress1',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillAddress1
	 */
	function GETFF_BillAddress1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillAddress1',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillAddress1 as an HTML_Tag object
	 */
	function & GETFFO_BillAddress1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillAddress1',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillAddress1
	 */
	function SET_BillAddress1($value,$from_form=true)
	{
		return parent::PRT_set_field('BillAddress1',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillAddress1
	 */
	function ORIG_BillAddress1($as_string=false)
	{
		return parent::PRT_get_orig('BillAddress1',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillAddress2
	 */
	function GET_BillAddress2($as_string=false)
	{
		return parent::PRT_get_field('BillAddress2',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillAddress2
	 */
	function GETFF_BillAddress2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillAddress2',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillAddress2 as an HTML_Tag object
	 */
	function & GETFFO_BillAddress2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillAddress2',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillAddress2
	 */
	function SET_BillAddress2($value,$from_form=true)
	{
		return parent::PRT_set_field('BillAddress2',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillAddress2
	 */
	function ORIG_BillAddress2($as_string=false)
	{
		return parent::PRT_get_orig('BillAddress2',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillCity
	 */
	function GET_BillCity($as_string=false)
	{
		return parent::PRT_get_field('BillCity',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillCity
	 */
	function GETFF_BillCity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillCity',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillCity as an HTML_Tag object
	 */
	function & GETFFO_BillCity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillCity',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillCity
	 */
	function SET_BillCity($value,$from_form=true)
	{
		return parent::PRT_set_field('BillCity',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillCity
	 */
	function ORIG_BillCity($as_string=false)
	{
		return parent::PRT_get_orig('BillCity',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillState
	 */
	function GET_BillState($as_string=false)
	{
		return parent::PRT_get_field('BillState',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillState
	 */
	function GETFF_BillState($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillState',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillState as an HTML_Tag object
	 */
	function & GETFFO_BillState($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillState',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillState
	 */
	function SET_BillState($value,$from_form=true)
	{
		return parent::PRT_set_field('BillState',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillState
	 */
	function ORIG_BillState($as_string=false)
	{
		return parent::PRT_get_orig('BillState',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillZip
	 */
	function GET_BillZip($as_string=false)
	{
		return parent::PRT_get_field('BillZip',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillZip
	 */
	function GETFF_BillZip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillZip',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillZip as an HTML_Tag object
	 */
	function & GETFFO_BillZip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillZip',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillZip
	 */
	function SET_BillZip($value,$from_form=true)
	{
		return parent::PRT_set_field('BillZip',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillZip
	 */
	function ORIG_BillZip($as_string=false)
	{
		return parent::PRT_get_orig('BillZip',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipFirstName
	 */
	function GET_ShipFirstName($as_string=false)
	{
		return parent::PRT_get_field('ShipFirstName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipFirstName
	 */
	function GETFF_ShipFirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipFirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipFirstName as an HTML_Tag object
	 */
	function & GETFFO_ShipFirstName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipFirstName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipFirstName
	 */
	function SET_ShipFirstName($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipFirstName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipFirstName
	 */
	function ORIG_ShipFirstName($as_string=false)
	{
		return parent::PRT_get_orig('ShipFirstName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipLastName
	 */
	function GET_ShipLastName($as_string=false)
	{
		return parent::PRT_get_field('ShipLastName',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipLastName
	 */
	function GETFF_ShipLastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipLastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipLastName as an HTML_Tag object
	 */
	function & GETFFO_ShipLastName($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipLastName',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipLastName
	 */
	function SET_ShipLastName($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipLastName',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipLastName
	 */
	function ORIG_ShipLastName($as_string=false)
	{
		return parent::PRT_get_orig('ShipLastName',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipPhone
	 */
	function GET_ShipPhone($as_string=false)
	{
		return parent::PRT_get_field('ShipPhone',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipPhone
	 */
	function GETFF_ShipPhone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipPhone',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipPhone as an HTML_Tag object
	 */
	function & GETFFO_ShipPhone($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipPhone',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipPhone
	 */
	function SET_ShipPhone($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipPhone',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipPhone
	 */
	function ORIG_ShipPhone($as_string=false)
	{
		return parent::PRT_get_orig('ShipPhone',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipAddress1
	 */
	function GET_ShipAddress1($as_string=false)
	{
		return parent::PRT_get_field('ShipAddress1',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipAddress1
	 */
	function GETFF_ShipAddress1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipAddress1',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipAddress1 as an HTML_Tag object
	 */
	function & GETFFO_ShipAddress1($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipAddress1',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipAddress1
	 */
	function SET_ShipAddress1($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipAddress1',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipAddress1
	 */
	function ORIG_ShipAddress1($as_string=false)
	{
		return parent::PRT_get_orig('ShipAddress1',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipAddress2
	 */
	function GET_ShipAddress2($as_string=false)
	{
		return parent::PRT_get_field('ShipAddress2',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipAddress2
	 */
	function GETFF_ShipAddress2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipAddress2',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipAddress2 as an HTML_Tag object
	 */
	function & GETFFO_ShipAddress2($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipAddress2',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipAddress2
	 */
	function SET_ShipAddress2($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipAddress2',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipAddress2
	 */
	function ORIG_ShipAddress2($as_string=false)
	{
		return parent::PRT_get_orig('ShipAddress2',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipCity
	 */
	function GET_ShipCity($as_string=false)
	{
		return parent::PRT_get_field('ShipCity',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipCity
	 */
	function GETFF_ShipCity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipCity',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipCity as an HTML_Tag object
	 */
	function & GETFFO_ShipCity($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipCity',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipCity
	 */
	function SET_ShipCity($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipCity',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipCity
	 */
	function ORIG_ShipCity($as_string=false)
	{
		return parent::PRT_get_orig('ShipCity',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipState
	 */
	function GET_ShipState($as_string=false)
	{
		return parent::PRT_get_field('ShipState',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipState
	 */
	function GETFF_ShipState($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipState',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipState as an HTML_Tag object
	 */
	function & GETFFO_ShipState($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipState',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipState
	 */
	function SET_ShipState($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipState',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipState
	 */
	function ORIG_ShipState($as_string=false)
	{
		return parent::PRT_get_orig('ShipState',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipZip
	 */
	function GET_ShipZip($as_string=false)
	{
		return parent::PRT_get_field('ShipZip',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipZip
	 */
	function GETFF_ShipZip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipZip',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipZip as an HTML_Tag object
	 */
	function & GETFFO_ShipZip($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipZip',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipZip
	 */
	function SET_ShipZip($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipZip',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipZip
	 */
	function ORIG_ShipZip($as_string=false)
	{
		return parent::PRT_get_orig('ShipZip',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_PH_CCType
	 */
	function GET_FKID_PH_CCType($as_string=false)
	{
		return parent::PRT_get_field('FKID_PH_CCType',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_CCType
	 */
	function GETFF_FKID_PH_CCType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_PH_CCType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_CCType as an HTML_Tag object
	 */
	function & GETFFO_FKID_PH_CCType($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_PH_CCType',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_PH_CCType
	 */
	function SET_FKID_PH_CCType($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_PH_CCType',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_PH_CCType
	 */
	function ORIG_FKID_PH_CCType($as_string=false)
	{
		return parent::PRT_get_orig('FKID_PH_CCType',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_PH_CCType to $match_arr
	 */
	function SETFLTR_FKID_PH_CCType($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_PH_CCType',$match_arr,$field_wide);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of CC
	 */
	function GET_CC($as_string=false)
	{
		return parent::PRT_get_field('CC',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CC
	 */
	function GETFF_CC($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CC',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CC as an HTML_Tag object
	 */
	function & GETFFO_CC($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CC',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CC
	 */
	function SET_CC($value,$from_form=true)
	{
		return parent::PRT_set_field('CC',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CC
	 */
	function ORIG_CC($as_string=false)
	{
		return parent::PRT_get_orig('CC',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of CCExp
	 */
	function GET_CCExp($as_string=false)
	{
		return parent::PRT_get_field('CCExp',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CCExp
	 */
	function GETFF_CCExp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CCExp',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CCExp as an HTML_Tag object
	 */
	function & GETFFO_CCExp($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CCExp',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of CCExp
	 */
	function SET_CCExp($value,$from_form=true)
	{
		return parent::PRT_set_field('CCExp',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of CCExp
	 */
	function ORIG_CCExp($as_string=false)
	{
		return parent::PRT_get_orig('CCExp',$as_string);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of CCCode
	 */
	function GET_CCCode($as_string=false)
	{
		return parent::PRT_get_field('CCCode',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CCCode
	 */
	function GETFF_CCCode($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('CCCode',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for CCCode as an HTML_Tag object
	 */
	function & GETFFO_CCCode($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('CCCode',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of CCCode
	 */
	function SET_CCCode($value,$from_form=true)
	{
		return parent::PRT_set_field('CCCode',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of CCCode
	 */
	function ORIG_CCCode($as_string=false)
	{
		return parent::PRT_get_orig('CCCode',$as_string);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of Total
	 */
	function GET_Total($as_string=false)
	{
		return parent::PRT_get_field('Total',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Total
	 */
	function GETFF_Total($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Total',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Total as an HTML_Tag object
	 */
	function & GETFFO_Total($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Total',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of Total
	 */
	function SET_Total($value,$from_form=true)
	{
		return parent::PRT_set_field('Total',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of Total
	 */
	function ORIG_Total($as_string=false)
	{
		return parent::PRT_get_orig('Total',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of BillCountry
	 */
	function GET_BillCountry($as_string=false)
	{
		return parent::PRT_get_field('BillCountry',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillCountry
	 */
	function GETFF_BillCountry($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('BillCountry',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for BillCountry as an HTML_Tag object
	 */
	function & GETFFO_BillCountry($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('BillCountry',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of BillCountry
	 */
	function SET_BillCountry($value,$from_form=true)
	{
		return parent::PRT_set_field('BillCountry',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of BillCountry
	 */
	function ORIG_BillCountry($as_string=false)
	{
		return parent::PRT_get_orig('BillCountry',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of ShipCountry
	 */
	function GET_ShipCountry($as_string=false)
	{
		return parent::PRT_get_field('ShipCountry',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipCountry
	 */
	function GETFF_ShipCountry($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('ShipCountry',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for ShipCountry as an HTML_Tag object
	 */
	function & GETFFO_ShipCountry($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('ShipCountry',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of ShipCountry
	 */
	function SET_ShipCountry($value,$from_form=true)
	{
		return parent::PRT_set_field('ShipCountry',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of ShipCountry
	 */
	function ORIG_ShipCountry($as_string=false)
	{
		return parent::PRT_get_orig('ShipCountry',$as_string);
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

class DB3_PH_Order_Table extends DB_Table
{
	function DB3_PH_Order_Table(&$db)
	{
		parent::DB_Table($db,'PH_Order','PKID_PH_Order',true);

		$this->fields['PKID_PH_Order'] =& new DB_Field_Int('PKID_PH_Order',false);
		$this->fields['Email'] =& new DB_Field_Varchar('Email',false);
		$this->fields['BillFirstName'] =& new DB_Field_Varchar('BillFirstName',false);
		$this->fields['BillLastName'] =& new DB_Field_Varchar('BillLastName',false);
		$this->fields['BillPhone'] =& new DB_Field_Varchar('BillPhone',false);
		$this->fields['BillAddress1'] =& new DB_Field_Varchar('BillAddress1',false);
		$this->fields['BillAddress2'] =& new DB_Field_Varchar('BillAddress2',true);
		$this->fields['BillCity'] =& new DB_Field_Varchar('BillCity',false);
		$this->fields['BillState'] =& new DB_Field_Varchar('BillState',true);
		$this->fields['BillZip'] =& new DB_Field_Varchar('BillZip',true);
		$this->fields['ShipFirstName'] =& new DB_Field_Varchar('ShipFirstName',true);
		$this->fields['ShipLastName'] =& new DB_Field_Varchar('ShipLastName',true);
		$this->fields['ShipPhone'] =& new DB_Field_Varchar('ShipPhone',true);
		$this->fields['ShipAddress1'] =& new DB_Field_Varchar('ShipAddress1',true);
		$this->fields['ShipAddress2'] =& new DB_Field_Varchar('ShipAddress2',true);
		$this->fields['ShipCity'] =& new DB_Field_Varchar('ShipCity',true);
		$this->fields['ShipState'] =& new DB_Field_Varchar('ShipState',true);
		$this->fields['ShipZip'] =& new DB_Field_Varchar('ShipZip',true);
		$this->fields['FKID_PH_CCType'] =& new DB_Field_FK('FKID_PH_CCType',true,'PH_CCType','PKID_PH_CCType','Name',$db,'DB_Field_Int');
		$this->fields['CC'] =& new DB_Field_Varchar('CC',true);
		$this->fields['CCExp'] =& new DB_Field_Varchar('CCExp',true);
		$this->fields['CCCode'] =& new DB_Field_Int('CCCode',true);
		$this->fields['Total'] =& new DB_Field_Float('Total',false);
		$this->fields['BillCountry'] =& new DB_Field_Varchar('BillCountry',true);
		$this->fields['ShipCountry'] =& new DB_Field_Varchar('ShipCountry',true);
		$this->fields['Comments'] =& new DB_Field_Text('Comments',true);

		parent::PRT_add_pk_ref('PH_Order_Item','FKID_PH_Order',false);
	}

	/**
	 * @desc STATIC: get the tablename 'PH_Order' 
	 */
	function S_get_tablename()
	{
		return 'PH_Order';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_PH_Order'=>1,'Email'=>1,'BillFirstName'=>1,'BillLastName'=>1,'BillPhone'=>1,'BillAddress1'=>1,'BillAddress2'=>1,'BillCity'=>1,'BillState'=>1,'BillZip'=>1,'ShipFirstName'=>1,'ShipLastName'=>1,'ShipPhone'=>1,'ShipAddress1'=>1,'ShipAddress2'=>1,'ShipCity'=>1,'ShipState'=>1,'ShipZip'=>1,'FKID_PH_CCType'=>1,'CC'=>1,'CCExp'=>1,'CCCode'=>1,'Total'=>1,'BillCountry'=>1,'ShipCountry'=>1,'Comments'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_PH_Order column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_PH_Order($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_PH_Order',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_PH_Order in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_PH_Order($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_PH_Order',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_PH_Order Required ?
	 */
	function ISREQ_PKID_PH_Order()
	{
		return true;
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
	 * @desc Fetch BillFirstName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillFirstName($id_keys=false)
	{
		return parent::PRT_fetch_col('BillFirstName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillFirstName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillFirstName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillFirstName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillFirstName Required ?
	 */
	function ISREQ_BillFirstName()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillLastName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillLastName($id_keys=false)
	{
		return parent::PRT_fetch_col('BillLastName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillLastName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillLastName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillLastName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillLastName Required ?
	 */
	function ISREQ_BillLastName()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillPhone column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillPhone($id_keys=false)
	{
		return parent::PRT_fetch_col('BillPhone',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillPhone in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillPhone($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillPhone',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillPhone Required ?
	 */
	function ISREQ_BillPhone()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillAddress1 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillAddress1($id_keys=false)
	{
		return parent::PRT_fetch_col('BillAddress1',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillAddress1 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillAddress1($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillAddress1',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillAddress1 Required ?
	 */
	function ISREQ_BillAddress1()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillAddress2 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillAddress2($id_keys=false)
	{
		return parent::PRT_fetch_col('BillAddress2',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillAddress2 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillAddress2($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillAddress2',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillAddress2 Required ?
	 */
	function ISREQ_BillAddress2()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillCity column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillCity($id_keys=false)
	{
		return parent::PRT_fetch_col('BillCity',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillCity in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillCity($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillCity',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillCity Required ?
	 */
	function ISREQ_BillCity()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillState column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillState($id_keys=false)
	{
		return parent::PRT_fetch_col('BillState',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillState in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillState($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillState',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillState Required ?
	 */
	function ISREQ_BillState()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillZip column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillZip($id_keys=false)
	{
		return parent::PRT_fetch_col('BillZip',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillZip in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillZip($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillZip',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillZip Required ?
	 */
	function ISREQ_BillZip()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipFirstName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipFirstName($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipFirstName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipFirstName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipFirstName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipFirstName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipFirstName Required ?
	 */
	function ISREQ_ShipFirstName()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipLastName column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipLastName($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipLastName',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipLastName in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipLastName($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipLastName',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipLastName Required ?
	 */
	function ISREQ_ShipLastName()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipPhone column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipPhone($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipPhone',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipPhone in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipPhone($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipPhone',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipPhone Required ?
	 */
	function ISREQ_ShipPhone()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipAddress1 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipAddress1($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipAddress1',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipAddress1 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipAddress1($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipAddress1',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipAddress1 Required ?
	 */
	function ISREQ_ShipAddress1()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipAddress2 column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipAddress2($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipAddress2',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipAddress2 in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipAddress2($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipAddress2',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipAddress2 Required ?
	 */
	function ISREQ_ShipAddress2()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipCity column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipCity($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipCity',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipCity in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipCity($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipCity',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipCity Required ?
	 */
	function ISREQ_ShipCity()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipState column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipState($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipState',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipState in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipState($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipState',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipState Required ?
	 */
	function ISREQ_ShipState()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipZip column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipZip($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipZip',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipZip in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipZip($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipZip',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipZip Required ?
	 */
	function ISREQ_ShipZip()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_PH_CCType column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_PH_CCType($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_PH_CCType',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_PH_CCType in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_PH_CCType($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_PH_CCType',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_PH_CCType to $match_arr
	 */
	function SETFLTR_FKID_PH_CCType($match_arr)
	{
		parent::PRT_set_filter('FKID_PH_CCType',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_PH_CCType Required ?
	 */
	function ISREQ_FKID_PH_CCType()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CC column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CC($id_keys=false)
	{
		return parent::PRT_fetch_col('CC',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CC in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CC($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CC',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CC Required ?
	 */
	function ISREQ_CC()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CCExp column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CCExp($id_keys=false)
	{
		return parent::PRT_fetch_col('CCExp',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CCExp in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CCExp($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CCExp',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CCExp Required ?
	 */
	function ISREQ_CCExp()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch CCCode column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_CCCode($id_keys=false)
	{
		return parent::PRT_fetch_col('CCCode',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where CCCode in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_CCCode($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('CCCode',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is CCCode Required ?
	 */
	function ISREQ_CCCode()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Total column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Total($id_keys=false)
	{
		return parent::PRT_fetch_col('Total',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Total in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Total($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Total',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Total Required ?
	 */
	function ISREQ_Total()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch BillCountry column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_BillCountry($id_keys=false)
	{
		return parent::PRT_fetch_col('BillCountry',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where BillCountry in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_BillCountry($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('BillCountry',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is BillCountry Required ?
	 */
	function ISREQ_BillCountry()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch ShipCountry column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_ShipCountry($id_keys=false)
	{
		return parent::PRT_fetch_col('ShipCountry',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where ShipCountry in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_ShipCountry($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('ShipCountry',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is ShipCountry Required ?
	 */
	function ISREQ_ShipCountry()
	{
		return false;
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
