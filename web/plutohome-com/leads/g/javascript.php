<?
class javascript
{
	/**
	 * @return str
	 * @param php_array arr
	 * @param single_quotes bool
	 * @desc STATIC: Get a Javascript array from a PHP array
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
	
	/**
	* @return str
	* @param str $source
	* @desc STATIC, return &lt;script&gt; tag
	*/
	function source_tag($source)
	{
		return "<script language=\"javascript\" src=\"$source\" type=\"text/javascript\"></script>";
	}
	
	/**
	* @return str
	*/
	function open_tag()
	{
		return "<script language=\"javascript\" type=\"text/javascript\">\n<!--\n";
	}
	
	/**
	* @return str
	*/
	function close_tag()
	{
		return "//-->\n</script>\n";
	}
}