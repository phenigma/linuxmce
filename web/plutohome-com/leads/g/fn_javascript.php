<?

/**
 * @return unknown
 * @param php_array unknown
 * @param single_quotes unknown
 * @desc DEPRECATED: use Javascript class instead
 * TODO: Find uses & replace
 */
function js_array($php_array, $single_quotes = false)
{
	$js_arr = 'Array(';
	
	$comma = false;
	foreach ($php_array as $php_item)
	{
		if ($comma) $js_arr.=',';
		if ($single_quotes) $js_arr.="'$php_item'";
		else $js_arr.="\"$php_item\"";
		$comma = true;
	}
	$js_arr.= ')';
	return $js_arr;
}