<?
class this
{
	/**
	 * @return void
	 * @param $that obj
	 * @desc set props in $this to be a ref to props in $that
	 * Constructor for $that needs to accept no params.
	 * $this becomes like a reference to $that
	 * USAGE: inside another class: this::set_to($that);
	 */
	function set_to(&$that)
	{
		$class_name = get_class($that);
		$this = new $class_name(); // Can't do "$this =& ...", that's the point of this fn
		foreach (array_keys(get_object_vars($that)) as $varname)
		{
			$this->$varname =& $that->$varname;
		}
	}
		
	/**
	 * @return mix
	 * @param $fn_name str
	 * @param $parameters arr
	 * @param $return_on_error mix
	 * @desc Call $fn_name if it exists, otherwise return $return_on_error
	 */
	function call_function($fn_name, $parameters = array(), $return_on_error = false)
	{
		$fn_arr[] =& $this;
		$fn_arr[] = $fn_name;
		return method_exists($this,$fn_name) ? call_user_func_array($fn_arr,$parameters) : $return_on_error;
	}
	
	/*
	TODO: need to test call_user_func() && call_user_func_array() for:
	
	passing by ref.
	returning by ref.
	
	function & call_function($fn_name, &$parameters, $return_on_error = false)
	{
		if (!method_exists($this,$fn_name)) return $return_on_error;
	}
	*/
	
	function abstract_function($abstract_class, $fn_name)
	{
		exit('<br />ERROR: "'.get_class($this)."\" extends \"$abstract_class\" but does NOT implement $fn_name()<br />");
	}
	
	function abstract_constructor($abstract_class)
	{
		exit("<br />ERROR: \"$abstract_class\" is ABSTRACT, it CANNOT be instantiated<br />");
	}
}
