<?
// Including File should have a Database object called $db_my
// Go to bottom of this file for predefined variables

require_once('g/db_table.php');

class DB3_PH_Order_Item_Row extends DB_Row
{
	/**
	 * @return DB3_PH_Order_Item_Row
	 * @param $FKID_PH_Order str
	 * @param $FKID_PH_Item str
	 * @desc Will return FALSE if a matching row is not found
	 * FKID_PH_Order can also be an Array, an Object, or an ID-key
	 * Set all fields to fetch row with matching PK
	 * Will also fetch row if ID-Key is used
	 */
	function DB3_PH_Order_Item_Row($FKID_PH_Order='',$FKID_PH_Item=null)
	{
		$this->dbt_path='dbt_my_corp/';
		$is_id_key = (is_string($FKID_PH_Order) && substr_count($FKID_PH_Order,'_')==1);
		$match_arr = (!is_null($FKID_PH_Item) || $is_id_key);
		if (!$match_arr || is_array($FKID_PH_Order) || $is_id_key) $row = $FKID_PH_Order;
		else $row = array('FKID_PH_Order'=>$FKID_PH_Order,'FKID_PH_Item'=>$FKID_PH_Item);
		parent::DB_Row('DB_PH_Order_Item_Table',$row,$match_arr);
		if ($FKID_PH_Order==='') parent::PRT_set_fields(array('FKID_PH_Order'=>0,'FKID_PH_Item'=>0,'Price'=>0.00,'Qty'=>0));
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_PH_Order
	 */
	function GET_FKID_PH_Order($as_string=false)
	{
		return parent::PRT_get_field('FKID_PH_Order',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_Order
	 */
	function GETFF_FKID_PH_Order($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_PH_Order',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_Order as an HTML_Tag object
	 */
	function & GETFFO_FKID_PH_Order($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_PH_Order',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_PH_Order
	 */
	function SET_FKID_PH_Order($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_PH_Order',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_PH_Order
	 */
	function ORIG_FKID_PH_Order($as_string=false)
	{
		return parent::PRT_get_orig('FKID_PH_Order',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_PH_Order to $match_arr
	 */
	function SETFLTR_FKID_PH_Order($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_PH_Order',$match_arr,$field_wide);
	}

	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of FKID_PH_Item
	 */
	function GET_FKID_PH_Item($as_string=false)
	{
		return parent::PRT_get_field('FKID_PH_Item',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_Item
	 */
	function GETFF_FKID_PH_Item($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field('FKID_PH_Item',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for FKID_PH_Item as an HTML_Tag object
	 */
	function & GETFFO_FKID_PH_Item($precol=false,$prefix=true,$input_name='',$sorted=true,$show_null=true)
	{
		return parent::PRT_get_form_field_obj('FKID_PH_Item',$precol,$prefix,$input_name,$sorted,$show_null);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of FKID_PH_Item
	 */
	function SET_FKID_PH_Item($value,$from_form=true)
	{
		return parent::PRT_set_field('FKID_PH_Item',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of FKID_PH_Item
	 */
	function ORIG_FKID_PH_Item($as_string=false)
	{
		return parent::PRT_get_orig('FKID_PH_Item',$as_string);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @param $field_wide bool
	 * @desc Filter options for FKID_PH_Item to $match_arr
	 */
	function SETFLTR_FKID_PH_Item($match_arr,$field_wide=true)
	{
		parent::PRT_set_filter('FKID_PH_Item',$match_arr,$field_wide);
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
	 * @return int
	 * @param $as_string bool
	 * @desc Get Value of Qty
	 */
	function GET_Qty($as_string=false)
	{
		return parent::PRT_get_field('Qty',$as_string);
	}
	/**
	 * @return str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Qty
	 */
	function GETFF_Qty($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field('Qty',$precol,$prefix,$input_name);
	}
	/**
	 * @return HTML_Tag
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for Qty as an HTML_Tag object
	 */
	function & GETFFO_Qty($precol=false,$prefix=true,$input_name='')
	{
		return parent::PRT_get_form_field_obj('Qty',$precol,$prefix,$input_name);
	}
	/**
	 * @return bool
	 * @param $value int
	 * @param $from_form bool
	 * @desc Set Value of Qty
	 */
	function SET_Qty($value,$from_form=true)
	{
		return parent::PRT_set_field('Qty',$value,$from_form);
	}
	/**
	 * @return int
	 * @param $as_string bool
	 * @desc Get Original Value of Qty
	 */
	function ORIG_Qty($as_string=false)
	{
		return parent::PRT_get_orig('Qty',$as_string);
	}
}

class DB3_PH_Order_Item_Table extends DB_Table
{
	function DB3_PH_Order_Item_Table(&$db)
	{
		parent::DB_Table($db,'PH_Order_Item',array('FKID_PH_Order','FKID_PH_Item'),false);

		$this->fields['FKID_PH_Order'] =& new DB_Field_FK('FKID_PH_Order',false,'PH_Order','PKID_PH_Order','PKID_PH_Order',$db,'DB_Field_Int');
		$this->fields['FKID_PH_Item'] =& new DB_Field_FK('FKID_PH_Item',false,'PH_Item','PKID_PH_Item','Name',$db,'DB_Field_Int');
		$this->fields['Price'] =& new DB_Field_Float('Price',false);
		$this->fields['Qty'] =& new DB_Field_Int('Qty',false);
	}

	/**
	 * @desc STATIC: get the tablename 'PH_Order_Item' 
	 */
	function S_get_tablename()
	{
		return 'PH_Order_Item';
	}
	/**
	 * @desc STATIC: see if $fieldname exists in this table
	 */
	function S_field_exists($fieldname)
	{
		$fields = array('FKID_PH_Order'=>1,'FKID_PH_Item'=>1,'Price'=>1,'Qty'=>1);
		return isset($fields[$fieldname]);
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_PH_Order column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_PH_Order($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_PH_Order',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_PH_Order in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_PH_Order($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_PH_Order',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_PH_Order to $match_arr
	 */
	function SETFLTR_FKID_PH_Order($match_arr)
	{
		parent::PRT_set_filter('FKID_PH_Order',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_PH_Order Required ?
	 */
	function ISREQ_FKID_PH_Order()
	{
		return true;
	}

	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc Fetch FKID_PH_Item column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_FKID_PH_Item($id_keys=false)
	{
		return parent::PRT_fetch_col('FKID_PH_Item',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where FKID_PH_Item in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_FKID_PH_Item($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('FKID_PH_Item',$values,$orderby,$limit,$start);
	}
	/**
	 * @return void
	 * @param $match_arr arr
	 * @desc Filter options for FKID_PH_Item to $match_arr
	 */
	function SETFLTR_FKID_PH_Item($match_arr)
	{
		parent::PRT_set_filter('FKID_PH_Item',$match_arr);
	}
	/**
	 * @return bool
	 * @desc Is FKID_PH_Item Required ?
	 */
	function ISREQ_FKID_PH_Item()
	{
		return true;
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
	 * @desc Fetch Qty column, rows matching $id_keys or false=current, true=all
	 */
	function FETCH_Qty($id_keys=false)
	{
		return parent::PRT_fetch_col('Qty',$id_keys);
	}
	/**
	 * @return arr
	 * @param $values str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load Table where Qty in $values 
	 * returns $id_keys for use in FETCH_*() 
	 * NOTE: $start NOT supported by MS-SQL
	 */
	function LOADBY_Qty($values,$orderby='',$limit=-1,$start=0)
	{
		return parent::PRT_load_by_field('Qty',$values,$orderby,$limit,$start);
	}
	/**
	 * @return bool
	 * @desc Is Qty Required ?
	 */
	function ISREQ_Qty()
	{
		return true;
	}
}
