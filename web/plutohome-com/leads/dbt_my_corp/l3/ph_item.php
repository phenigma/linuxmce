<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DBT_PH_Item
{
	// DEFINES REMOVED OR RENAMED IN DB:
	var $CONFCAM = 14;
	var $CONFMIC = 15;
	var $ETHSW = 18;
	var $INST = 20;
	var $INTMOD = 5;
	var $JUKEBOX = 17;
	var $MDNW = 10;
	var $MOBSOFT = 13;
	var $ORB = 11;
	var $WIFI = 19;

	// CURRENT DEFINES:
	var $_84ORB = 11;
	var $CORE = 1;
	var $HYBRID = 8;
	var $INSTEU = 21;
	var $INSTOTH = 22;
	var $INSTUSA = 20;
	var $IPHONE = 16;
	var $ISDN = 6;
	var $JUKE = 17;
	var $LOADCD = 23;
	var $LOADDVD = 24;
	var $MD = 9;
	var $MDNOHD = 10;
	var $MPSW = 13;
	var $NOK3660 = 12;
	var $POTS = 5;
	var $SWITCH = 18;
	var $T1 = 7;
	var $UP1200 = 3;
	var $UP2000 = 4;
	var $UP600 = 2;
	var $VCCAM = 14;
	var $VCMIC = 15;
	var $WIFIAP = 19;
}
$DB_PH_Item =& new DBT_PH_Item();
$GLOBALS['DB_PH_Item'] =& $DB_PH_Item;

class DB3_PH_Item_Row extends DB_Row
{
	/**
	 * @return DB3_PH_Item_Row
	 * @param $PKID_PH_Item str
	 * @desc Will return FALSE if a matching row is not found
	 * PKID_PH_Item can also be an Array, an Object, or an ID-key
	 */
	function DB3_PH_Item_Row($PKID_PH_Item='')
	{
		$this->dbt_path='dbt_my_corp/';
		parent::DB_Row('DB_PH_Item_Table',$PKID_PH_Item);
		if ($PKID_PH_Item==='') parent::PRT_set_fields(array('Price'=>0.00,'SalePrice'=>0.00));
	}

	/**
	 * @return arr
	 * @desc Get Array of child ids from PH_Order_Item by FKID_PH_Item
	 */
	function GETCHLD_PH_Order_Item__FKID_PH_Item()
	{
		return parent::PRT_get_field_children('FKID_PH_Item','PH_Order_Item');
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of PKID_PH_Item
	 */
	function GET_PKID_PH_Item($as_string=false)
	{
		return parent::PRT_get_field('PKID_PH_Item',$as_string);
	}

	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Value of Define
	 */
	function GET_Define($as_string=false)
	{
		return parent::PRT_get_field('Define',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define
	 */
	function GETFF_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Define as an HTML_Tag object
	 */
	function & GETFFO_Define($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Define',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Define
	 */
	function SET_Define($value,$from_form=true)
	{
		return parent::PRT_set_field('Define',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Define
	 */
	function ORIG_Define($as_string=false)
	{
		return parent::PRT_get_orig('Define',$as_string);
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
	 * @desc Get Value of Description
	 */
	function GET_Description($as_string=false)
	{
		return parent::PRT_get_field('Description',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Description
	 */
	function GETFF_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Description as an HTML_Tag object
	 */
	function & GETFFO_Description($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Description',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value str
	 * @param $from_form bool
	 * @desc Set Value of Description
	 */
	function SET_Description($value,$from_form=true)
	{
		return parent::PRT_set_field('Description',$value,$from_form);
	}
	/**
	 * @return str
	 * @param $as_string bool
	 * @desc Get Original Value of Description
	 */
	function ORIG_Description($as_string=false)
	{
		return parent::PRT_get_orig('Description',$as_string);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of Price
	 */
	function GET_Price($as_string=false)
	{
		return parent::PRT_get_field('Price',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Price
	 */
	function GETFF_Price($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Price',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Price as an HTML_Tag object
	 */
	function & GETFFO_Price($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Price',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of Price
	 */
	function SET_Price($value,$from_form=true)
	{
		return parent::PRT_set_field('Price',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of Price
	 */
	function ORIG_Price($as_string=false)
	{
		return parent::PRT_get_orig('Price',$as_string);
	}

	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Value of SalePrice
	 */
	function GET_SalePrice($as_string=false)
	{
		return parent::PRT_get_field('SalePrice',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for SalePrice
	 */
	function GETFF_SalePrice($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('SalePrice',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for SalePrice as an HTML_Tag object
	 */
	function & GETFFO_SalePrice($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('SalePrice',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value float
	 * @param $from_form bool
	 * @desc Set Value of SalePrice
	 */
	function SET_SalePrice($value,$from_form=true)
	{
		return parent::PRT_set_field('SalePrice',$value,$from_form);
	}
	/**
	 * @return float
	 * @param $as_string bool
	 * @desc Get Original Value of SalePrice
	 */
	function ORIG_SalePrice($as_string=false)
	{
		return parent::PRT_get_orig('SalePrice',$as_string);
	}
}

class DB3_PH_Item_Table extends DB_Table
{
	function DB3_PH_Item_Table(&$db)
	{
		parent::DB_Table($db,'PH_Item','PKID_PH_Item',true);

		$this->fields['PKID_PH_Item'] =& new DB_Field_Int('PKID_PH_Item',false);
		$this->fields['Define'] =& new DB_Field_Varchar('Define',false);
		$this->fields['Name'] =& new DB_Field_Varchar('Name',false);
		$this->fields['Description'] =& new DB_Field_Text('Description',true);
		$this->fields['Price'] =& new DB_Field_Float('Price',false);
		$this->fields['SalePrice'] =& new DB_Field_Float('SalePrice',false);

		parent::PRT_add_pk_ref('PH_Order_Item','FKID_PH_Item',false);
	}

	/**
	 * @desc STATIC: get the tablename 'PH_Item' 
	 */
	function S_get_tablename()
	{
		return 'PH_Item';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('PKID_PH_Item'=>1,'Define'=>1,'Name'=>1,'Description'=>1,'Price'=>1,'SalePrice'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch PKID_PH_Item column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_PKID_PH_Item($id_keys=false)
	{
		return parent::PRT_fetch_col('PKID_PH_Item',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where PKID_PH_Item in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_PKID_PH_Item($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('PKID_PH_Item',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is PKID_PH_Item Required ?
	 */
	function ISREQ_PKID_PH_Item()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Define column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Define($id_keys=false)
	{
		return parent::PRT_fetch_col('Define',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Define in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Define($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Define',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Define Required ?
	 */
	function ISREQ_Define()
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
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Description column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Description($id_keys=false)
	{
		return parent::PRT_fetch_col('Description',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Description in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Description($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Description',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Description Required ?
	 */
	function ISREQ_Description()
	{
		return false;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch Price column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Price($id_keys=false)
	{
		return parent::PRT_fetch_col('Price',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Price in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Price($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Price',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Price Required ?
	 */
	function ISREQ_Price()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch SalePrice column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_SalePrice($id_keys=false)
	{
		return parent::PRT_fetch_col('SalePrice',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where SalePrice in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_SalePrice($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('SalePrice',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is SalePrice Required ?
	 */
	function ISREQ_SalePrice()
	{
		return true;
	}
}
