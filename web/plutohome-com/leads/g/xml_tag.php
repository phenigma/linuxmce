<?
class XML_Tag
{
	var $parent;
	var $parent_key;
	var $name;       // XML name of tag
	var $properties = array(); // key=>value
	var $content;    // Array: strings & XML_Tags within this XML_Tag
	
	/**
	 * @desc $content: false for <tag />
	 */
	function XML_Tag($name, $properties=array(), $content=array())
	{
		if (is_a($name,'XML_Tag')) $this = $name;
		else 
		{
			$this->name = strtolower($name);
			foreach ($properties as $prop_name=>$prop_value)
			{
				if (is_int($prop_name)) $this->set_property($prop_value);
				else $this->set_property($prop_name,$prop_value);
			}
			$this->content = $content;
		}
	}
	
	function set_parent(&$parent)
	{
		$this->parent =& $parent;
	}
	
	function & get_parent()
	{
		return $this->parent;
	}
	
	/**
	 * @return str
	 */
	function get_tag_name()
	{
		return $this->name;
	}
	
	/**
	 * @return str|null
	 * @param name str
	 * @desc Get the value of $name for this XML tag
	 */
	function get_property($name)
	{
		return (isset($this->properties[$name])) ? $this->properties[$name] : null;
	}
	
	/**
	 * @return str|null
	 * @param name str
	 * @param value str
	 * @desc Set a property for the XML tag
	 * If $value is left null, will assign the name as the value
	 * returns the old value for the property
	 */
	function set_property($name,$value=null)
	{
		$old_property = $this->get_property($name);
		if (is_null($value)) $value = $name;
		$this->properties[strtolower($name)] = $value;
		return $old_property;
	}
	
	/**
	 * @return void
	 * @param $content XML_Tag|str
	 * @param $prepend bool
	 * @desc Add content between this tag
	 */
	function add_content($content, $prepend=false)
	{
		if (!is_array($this->content)) $this->content = array();
		if ($prepend) array_unshift($this->content,$content);
		else $this->content[] = $content;
	}

	/**
	 * @return void
	 * @param $content XML_Tag
	 * @desc Add content between this tag
	 */
	function add_content_ref(&$content)
	{
		if (is_a($content,'XML_Tag'))
		{ 
			$content->set_parent($this);
		}
		$this->content[] =& $content;
		if (is_a($content,'XML_Tag'))
		{
			end($this->content);
			$content->parent_key = key($this->content);
		}
	}
	
	function remove()
	{
		if (is_a($this->parent,'XML_Tag') && isset($this->parent_key))
		{
			if (isset($this->parent->content[$this->parent_key]))
			{
				unset($this->parent->content[$this->parent_key]);
				return true;
			}
		}
		return false;
	}
	
	/**
	 * @return str
	 */
	function get_xml()
	{
		$xml = $this->get_start();
		if ($this->content!==false)
		{
			$xml.= $this->get_content();
			$xml.= $this->get_end();
		}
		return $xml;
	}
	
	function display()
	{
		echo $this->get_xml();
	}
	
	/**
	 * @return str
	 */
	function get_start()
	{
		$return = "<$this->name";
		foreach ($this->properties as $key=>$val) $return .= " $key=\"$val\"";
		$return .= ($this->content===false) ? ' />' : '>';
		return $return;
	}

	/**
	 * @return str
	 */	
	function get_content()
	{
		return is_array($this->content) ? $this->PRV_get_content($this->content) : '';
	}
	
	function & get_content_arr()
	{
		return is_array($this->content) ? $this->content : array();
	}
	
	/**
	 * @return str
	 */
	function get_end()
	{
		return ($this->content===false) ? '' : "</$this->name>";
	}
	
	/**
	 * @return int
	 */
	function content_count()
	{
		return is_array($this->content) ? count($this->content) : 0;
	}
	
	/**
	 * @return str
	 */
	function PRV_get_content(&$content)
	{
		$xml = '';
		if (is_array($content))
		{
			foreach ($content as $content_element)
			{
				$xml.= $this->PRV_get_content($content_element);
			}
		}
		elseif (is_a($content,'XML_Tag')) 
		{	/* @var $content XML_Tag */
			$xml.= $content->get_xml(); 
		}
		else $xml.= $content;
		return $xml;
	}
}