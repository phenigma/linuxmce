<?
/**
 * @return bool
 */
function running_on_windows()
{
	if (isset($_SERVER['SystemRoot']) && $_SERVER['SystemRoot']==='C:\WINDOWS') return true;
	if (isset($_SERVER['SERVER_SOFTWARE']) && substr($_SERVER['SERVER_SOFTWARE'],0,9)==='Microsoft') return true;
	return false;
}

function check_for_inactivity($seconds_before_logout = 900)
{
	if (isset($_SESSION['last_activity']) && mktime() - $_SESSION['last_activity'] > $seconds_before_logout)
	{
		if (!substr_count($_SERVER['PHP_SELF'],'login.php')) logout('',true);
	}
	$_SESSION['last_activity'] = mktime();
}

function came_from_this_page()
{
	if (isset($_SESSION['this_page'],$_SESSION['last_page']))
	{
		if ($_SESSION['this_page'] === $_SESSION['last_page']) return true;
		if (substr($_SESSION['this_page'],0,8)==='https://' && substr($_SESSION['last_page'],0,7)==='http://')
		{
			return (substr($_SESSION['this_page'],8) === substr($_SESSION['last_page'],7));
		}
	}
	return false;
}

/*
function fullpath($link)
{
	if (is_string(strstr($link, 'http://'))) return $link;
	$dir  = dirname($_SERVER['PHP_SELF']);
	$file = basename($link);
	$query_string = isset_string($_ENV['QUERY_STRING']) ? "?$_ENV[QUERY_STRING]" : '';
	return "http://$_SERVER[HTTP_HOST]$dir/$file$query_string";
}
*/

/**
 * @return mix
 * @desc Used to avoid warning if $variable has not been set
 * Equivalent of preceding statement with: if (!isset($variable)) $variable = null;
 */
function value_of(&$variable)
{
	return $variable;
}

function port443()
{
	return (isset($_SERVER['SERVER_PORT']) && $_SERVER['SERVER_PORT']==443);
}

function full_host()
{
	return 'http'.(port443() ? 's' : '')."://$_SERVER[HTTP_HOST]";
}

/**
 * @return void
 * @param query_string string
 * @desc Refresh the current page. 
 * include '?' in query_string
 */
function refresh($query_string='')
{
	redirect("$_SESSION[this_page]$query_string");
}

function redirect($address, $use_application_path = true)
{
	if ($use_application_path===true && defined('APPLICATION_PATH') && $address{0}=='/')
	{
		if (APPLICATION_PATH!=='' && strpos($address,APPLICATION_PATH)!==1 && strpos($address,$_SERVER['HTTP_HOST'])!==1)
		{
			$address = '/'.APPLICATION_PATH.$address;
		}
		$address = full_host().$address;
	}
	header("Location: $address");
	exit();
}

function redirect_last_page()
{
	if ($_SESSION['last_page'] != $_SESSION['this_page'])	redirect($_SESSION['last_page']);
	else redirect(full_host()); // was '/'
}

function logout($redirect='', $timeout=false, $login_redirect='')
{
	if ($redirect==='') $redirect = defined('APPLICATION_PATH') ? '/login.php' : 'login.php';
		
	if ($login_redirect==='') $login_redirect = $_SERVER['REQUEST_URI'];
	session_unset();
	//session_destroy();
	if (is_string($login_redirect)) $_SESSION['login_redirect'] = $login_redirect;
	
	if ($timeout)
	{
		$_SESSION['messages'] = new Messages(); // =& here dangerous would need to test
		$_SESSION['messages']->setNotice('Your session has timed out, Please Log-In');
		redirect($redirect);
	}
	if (is_string($redirect)) redirect($redirect);
}

function isset_array(&$array)
{
	return (isset($array) && is_array($array));
}

function isset_int(&$int)
{
	return (isset($int) && is_int($int));
}

function isset_string(&$string, $allow_empty = true)
{
	return (isset($string) && is_string($string) && ($allow_empty || $string !== ''));
}

function isset_true(&$bool)
{
	return (isset($bool) && $bool===true);
}

function isset_object(&$object, $class_name='')
{
	return (isset($object) && is_object($object)) ? (($class_name==='') ? true : is_a($object,$class_name)) : false;
}

/**
 * @return bool
 * @param $int int
 * @desc casts $int to an int and returns success
 */
function isset_to_int(&$int)
{
	if (isset($int) && is_numeric($int))
	{
		$int = (int) $int;
		return true;
	}
	else return false;
}

function isset_to_float(&$float)
{
	if (isset($float) && is_numeric($float))
	{
		$float = (float) $float;
		return true;
	}
	else return false;
}

/**
 * @return void
 * @param $a mixed
 * @param $b mixed
 * @desc $a <-> $b
 */
function swap(&$a, &$b)
{
	$temp = $b;
	$b = $a;
	$a = $temp;
}

function date_long_string()  // Monday April 7, 2003
{
	if (!isset_string($_SESSION['date_long_string']))
	{
		$_SESSION['date_long_string'] = date('l F j, Y');
	}
	return $_SESSION['date_long_string'];
}

function date_display_long()
{
	echo date_long_string();
}

/**
 * @return bool
 * @param rangestart time
 * @param rangeend time
 * @param timestart time
 * @param timeend time
 * @desc Takes 2-4 timestamps
 * Checks if current time (or 3rd time) is between first 2 times
 * if endtime specified, checks for any overlap
 * NOTE: start time may be > than end time
 */
function in_time_range($rangestart, $rangeend, $timestart=NULL, $timeend=NULL)
{
	if ($rangestart > $rangeend) swap($rangestart, $rangeend);
	if (is_null($timestart)) $timestart=mktime();
	if (is_null($timeend)) $timeend = $timestart;
	elseif ($timestart > $timeend) swap($timestart, $timeend);
	
	return ($timestart<=$rangeend && $rangestart<=$timeend);
}

/**
 * @return bool
 * @param $int int
 * @param start int
 * @param end int
 * @desc casts $int to an int, and verifies that $start <= $int <= $end
 */
function int_in_range(&$int, $start, $end)
{
	return (isset_to_int($int) && in_range($int,$start,$end));
}

/**
 * @return bool
 * @param number num
 * @param start num
 * @param end num
 * @desc inclusive
 */
function in_range($number, $start, $end)
{
	return ($start <= $number && $number <= $end);
}

/**
 * @return bool
 * @param number num
 * @param start2end array
 * @desc any of the ranges
 */
function in_ranges($number, $start2end)
{
	foreach ($start2end as $start=>$end)
	{
		if (in_range($number,$start,$end)) return true;
	}
	return false;
}

function array_true_diff($array1,$array2)
{
	return array_merge(array_diff($array1,$array2),array_diff($array2,$array1));
}

function array_diff_count($array1,$array2)
{
	return count(array_diff($array1,$array2)) + count(array_diff($array2,$array1));
}

function & array_extract($array_to_extract_from, $keys_to_extract)
{
	$extracted = array();
	foreach ($keys_to_extract as $key)
	{
		if (array_key_exists($key,$array_to_extract_from)) $extracted[$key] = $array_to_extract_from[$key];
	}
	return $extracted;
}

/**
 * @return int
 * @param $array array
 * @param $value mixed
 * @desc Prepend a reference to an element to the beginning of an array. Renumbers numeric keys, so $value always inserted to $array[0]
 */
function array_unshift_ref(&$array, &$value)
{
	$return = array_unshift($array,'');
	$array[0] =& $value;
	return $return;
}

/**
 * @return arr
 * @param array arr
 * @desc $array[$val] = $val
 */
function & array_key_by_val($array)
{
	$key_arr = array();
	foreach ($array as $val)
	{
		$key_arr[$val] = $val;
	}
	return $key_arr;
}

/**
 * @return str
 * @param string str
 * @desc Convert HTML entities back to special chars
 */
function htmlspecialchars_decode($string)
{
	return strtr($string, array_flip(get_html_translation_table(HTML_SPECIALCHARS)));
}

function clone_object(&$new, &$old)
{
	if (isset_object($old) || is_a($old,'__PHP_Incomplete_Class'))
	{
		$old_props = get_object_vars($old);
		foreach (array_keys(get_object_vars($new)) as $property)
		{
			if (isset($old_props[$property])) $new->$property = $old_props[$property];
		}
		return true;
	}
	return false;
}

if (!function_exists('file_get_contents')) // PHP 4.3.0
{
	function file_get_contents($filename, $use_include_path=0)
	{
		$file = @fopen($filename, 'rb', $use_include_path);
		if ($file !== false) 
		{
			$data = '';
			while (!feof($file)) $data .= fread($file, 1024);
			fclose($file);
			return $data;
		}
		return false;
	}
}

/*
function str_ireplace($find,$replace,$string)
{
	if(!is_array($find)) $find = array($find);
	if(!is_array($replace))
	{
		// this will duplicate the string into an array the size of $find
		$rString = $replace;
		$replace = array();
		for ($i = 0; $i < count($find); $i++)
		{
			$replace[$i] = $rString;
		}
	}
	foreach($find as $fKey => $fItem)
	{
		$between = explode(strtolower($fItem),strtolower($string));
		$pos = 0;
		foreach($between as $bKey => $bItem)
		{
			$between[$bKey] = substr($string,$pos,strlen($bItem));
			$pos += strlen($bItem) + strlen($fItem);
		}
		$string = implode($replace[$fKey],$between);
	}
	return $string;
}
/*
if (!function_exists('html_entity_decode'))
{
	function html_entity_decode($string)
	{
		$string = strtr($string, array_flip(get_html_translation_table(HTML_ENTITIES)));
		return preg_replace("/&#([0-9]+);/me", "chr('\\1')", $string);
	}
}
*/