<?
/**
 * @desc Option can be an array of options
 * String representations must be the same
 * &$selected by ref because might not be set
 */
function selected_option($option, &$selected)
{
	if (is_null($selected)) return '';
	if (is_array($option))
	{
		foreach ($option as $an_option)
		{
			if (((string)$an_option)===((string)$selected)) return ' selected="selected"';
		}
	}
	elseif (((string)$option)===((string)$selected)) return ' selected="selected"';
	else return '';
}

/**
 * @desc Will convert null values or empty strings to 0
 */
function selected_bool(&$selected)
{
	if (!isset($selected) || is_null($selected) || $selected=='') $selected = 0;
	return $selected ? 'selected="selected"' : '';
}

/**
 * @desc String representations must be the same
 */
function checked_option($option, $checked)
{
	if (is_array($option))
	{
		foreach ($option as $an_option)
		{
			if (((string)$an_option)===((string)$checked)) return 'checked="checked"';
		}
	}
	elseif (((string)$option)===((string)$checked)) return 'checked="checked"';
	else return '';
}

function get_radiobox_array($name, $values, $checked)
{
	$rb_arr = array();
	foreach ($values as $key=>$value)
	{
		$rb_arr[$key] = "<input type=\"radio\" name=\"$name\" value=\"$value\" ".checked_option($value,$checked).' />';
	}
	return $rb_arr;
}

/**
 * @desc Will convert null values or empty strings to 0, $escape useful for Javascript
 */
function checked_bool(&$checked, $escape=false)
{
	if (!isset($checked) || is_null($checked) || $checked=='') $checked = 0;
	return $checked ? ($escape ? 'checked=\"checked\"' : 'checked="checked"') : '';
}

/**
 * @return void
 * @param $val2display arr
 * @param $selected_option str
 * @desc Display Option tags for Form select element
 */
function display_options($val2display, $selected_option=null)
{
	echo get_options($val2display, $selected_option);
}

/**
 * @return str
 * @param val2display arr
 * @param selected_option str
 * @desc Get Option tags for Form select element
 */
function get_options($val2display, $selected_option=null)
{
	$html = '';
	foreach ($val2display as $val=>$display)
	{
		$html .= "<option value=\"$val\"".selected_option($val,$selected_option).">$display</option>";
	}
	return $html;
}

function &get_checkbox_array($input_name, &$key2display, &$key2checked, $tab_index=1, $display_field='')
{
	$check_boxes = array();
	foreach ($key2display as $key=>$display)
	{
		if ($display_field != '') $display = $display[$display_field];
		$check_boxes[$key] = '<input type="checkbox" name="'.$input_name."[$key]\" value=\"1\" ".checked_bool($key2checked[$key])." tabindex=\"$tab_index\" />$display";
	}
	return $check_boxes;
}

function reset_form() // for use with add, alternative: use different form with submit button: Reset Form
{
	foreach ($_POST as $key=>$val)
	{
		unset($_POST[$key]);
	}
}

/**
 * @return void
 * @desc set $_POST or $_POST[$set_in_array](when string) or $_POST[$set][$in][$array](when array) to $db_row
 * for use with edit, set to db row
 * set in array may not be working properly
 */
function set_form(&$db_row, $set_in_array='')
{
	if ($set_in_array=='') $form = &$_POST;
	elseif (!is_array($set_in_array)) $form = &$_POST[$set_in_array];
	else
	{
		$first = true;
		foreach ($set_in_array as $sub_array)
		{
			if ($first) $form = &$_POST[$sub_array];
			else $form = &$form[$sub_array];
			$first = false;
		}
	}
	$form = array();
	foreach ($db_row as $field=>$val)
	{
		$form[$field] = str_replace('"','&quot;',$val);
	}
}

function set_form_ref(&$db_row, &$set_var, $reset = true)
{
	if ($reset) $set_var = array();
	foreach ($db_row as $field=>$val)
	{
		$set_var[$field] = str_replace('"','&quot;',$val);
	}
}

function bool_data_type(&$item, $allow_null = true)
{
	if (db_null_data($item))
	{
		if ($allow_null) $item = 0;
		return $allow_null;
	}
	
	if (is_numeric($item)) $item = (int)$item;
	else return false;
	return ($item===0 || $item===1);
}

function int_data_type(&$item, $allow_null = true, $require_positive = false)
{
	if (db_null_data($item)) return $allow_null;
	elseif (is_numeric($item)) $item = (int)$item;
	else return false;
	
	return ($item < 0) ? $require_positive : true;
}

/**
 * @return bool
 * @param $item str
 * @param $allow_null bool
 * @desc Is $item a valid phone # ?
 */
function phone_data_type(&$item, $allow_null)
{
	$item = str_replace(array('-',' ','#','.','(',')'),'',$item);
	return db_null_data($item) ? $allow_null : is_numeric($item);
}

/**
 * @return str
 * @param str $int
 * @desc Add - for display
 */
function int_to_phone($int)
{
	return (strlen($int) < 4) ? (string)$int : substr($int,0,3).'-'.substr($int,3);
}

function money_data_type(&$item, $allow_null = false, $require_positive = true)
{
	if (db_null_data($item)) return $allow_null;
	else
	{
		if ($item{0}=='$') $item = substr($item,1);
		$item = str_replace(',','',$item);
		if (is_numeric($item))
		{
			$item = (float)$item;
			if ($require_positive && $item < 0) return false;
			return true;
		}
		else return false;
	}
}

/**
 * @return bool
 * @param $item str
 * @desc returns whether or not data is null or empty, and sets to null
 */
function db_null_data(&$item)
{
	if (is_null($item) || $item==='')
	{
		$item = null;
		return true;
	}
	else return false;
}

function isValidEmail($address)
{
	return (eregi(
		"^[-!#$%&\'*+\\./0-9=?A-Z^_`{|}~]+". // username
		"@".								 // @
		"([-0-9A-Z]+\.)+" .					 // host, sub-, and domain names
		"([0-9A-Z]){2,4}$",					 // top-level domain (TLD)
		trim($address)));
}

if (!function_exists('get_key')) // also defined in fn_array
{
	function get_key($array)
	{
		reset($array);
		$each_arr = each($array);
		return $each_arr['key'];
	}
}

function php_timestamp($datetime_arr)
{
	if (!isset($datetime_arr['Second'])) $datetime_arr['Second'] = 0;
	
	if (isset($datetime_arr['AM']))
	{
		if ($datetime_arr['AM']==1 && $datetime_arr['Hour']==12) $datetime_arr['Hour'] = 0;
		elseif ($datetime_arr['AM']==0 && $datetime_arr['Hour']<12) $datetime_arr['Hour'] += 12;
	}
	
	return mktime($datetime_arr['Hour'],$datetime_arr['Minute'],$datetime_arr['Second'],$datetime_arr['Month'],$datetime_arr['Day'],$datetime_arr['Year']);
}

function valid_datetime(&$datetime_arr, $allow_no_date=true, $allow_today=true, $hr_12=true)
{
	if (!int_in_range($datetime_arr['Month'],-1,12)) return false;
	if ($datetime_arr['Month']==0) return $allow_no_date; // No Date
	if ($datetime_arr['Month']==-1) // Today, set date
	{
		if (!$allow_today) return false;
		
		$php_date_arr = getdate();
		$datetime_arr['Year'] = $php_date_arr['year'];
		$datetime_arr['Month'] = $php_date_arr['mon'];
		$datetime_arr['Day'] = $php_date_arr['mday'];		
	}
	
	// validate date & time
	if (!isset($datetime_arr['Second'])) $datetime_arr['Second'] = 0;
	
	
	if (!int_in_range($datetime_arr['Year'],1970,2037)) return false; // $messages->setError('Year out of suported range'); 
	if (!checkdate($datetime_arr['Month'],$datetime_arr['Day'],$datetime_arr['Year'])) return false; //$messages->setError('Invalid Date');
	
	if ($hr_12)
	{
		if (!int_in_range($datetime_arr['AM'], 0, 1)) return false;
		if (!int_in_range($datetime_arr['Hour'], 1, 12)) return false; // $messages->setError('Invalid Time');
	}
	elseif (!int_in_range($datetime_arr['Hour'], 0, 23)) return false;
	
	if (!int_in_range($datetime_arr['Minute'],0,59)) return false; // $messages->setError('Invalid Time');
	if (!int_in_range($datetime_arr['Second'],0,59)) return false;
	
	return true;
}