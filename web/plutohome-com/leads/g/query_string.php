<? 
class Query_String
{
	var $query_array;

	function Query_String($key2val = array())
	{
		$this->reset($key2val);
	}
	
	/**
	 * @return str
	 */
	function set_var($key, $value)
	{
		$old_val = $this->get_var($key);
		$this->query_array[$key] = $value;
		return $old_val;
	}
	
	/**
	 * @return str
	 */
	function get_var($key)
	{
		return isset($this->query_array[$key]) ? $this->query_array[$key] : null;
	}
	
	/**
	 * @return str
	 */
	function get($for_redirect = false)
	{
		$qs = '?';
		foreach (array_keys($this->query_array) as $i=>$key)
		{
			if ($i) $qs .= '&';
			$qs .= $key.'='.($for_redirect ? urlencode($this->query_array[$key])  : htmlentities(urlencode($this->query_array[$key])));
		}
		return $qs;
	}
	
	function reset($key2val = array())
	{
		$this->query_array = array();
		foreach ($key2val as $key=>$val)
		{
			$this->set_var($key,$val);
		}
	}
	
	/**
	 * @return str
	 */
	function unset_var($key)
	{
		$old_val = $this->get_var($key);
		if (isset($this->query_array[$key])) unset($this->query_array[$key]);
		return $old_val;
	}
}