<?
require_once('g/xml_tag_parser.php');

class XML_File
{
	var $path;
	var $version;
	var $xml_tags = array();
		
	function XML_File($version = '1.0')
	{
		$this->version = $version;
	}
	
	function set_path($path)
	{
		$this->path = $path;
	}
	
	function add_xml_tag(&$xml_tag)
	{
		$this->xml_tags[] =& $xml_tag;
	}
	
	function & get_xml_tags()
	{
		return $this->xml_tags;
	}
	
	function read($path = null)
	{
		if (is_null($path)) $path = $this->path;
		if (isset($path) && file_exists($path))
		{
			$xml_tag_parser =& new XML_Tag_Parser();
			$this->xml_tags =& $xml_tag_parser->get_xml_tags(file_get_contents($path));
			if (is_array($this->xml_tags)) return true;
			$this->xml_tags = array();
		}
		return false;
	}
	
	function write($path = null)
	{
		if (is_null($path)) $path = $this->path;
		if (isset($path)) //  && file_exists($path)
		{
			if ($handle = fopen($path, 'w'))
			{
				$return = fwrite($handle, $this->get_xml());
				fclose($handle);
				return $return;
			}
		}
		return false;
	}
	
	function get_xml()
	{
		$file = "<?xml version=\"$this->version\" ?>";
		foreach (array_keys($this->xml_tags) as $key)
		{
			$xml_tag =& $this->xml_tags[$key]; /* @var $xml_tag XML_Tag */
			$file .= $xml_tag->get_xml();
		}
		return $file;
	}
}