<?
require_once('g/xml_tag.php');
require_once('g/xml_parser.php');
class XML_Tag_Parser extends XML_Parser  
{
	var $tags = array();
	var $tag; // current tag
	var $depth = 0;
	
	function XML_Tag_Parser($encoding = null)
	{
		parent::XML_Parser($encoding);
		$this->set_element_handler('start_element','end_element');
		$this->set_character_data_handler('content');
	}
	function start_element($parser, $name, $attrs)
	{
		$tag =& new XML_Tag($name,$attrs,false);
		if ($this->depth) $this->tag->add_content_ref($tag);
		else $this->tags[] =& $tag;
		$this->tag =& $tag;
		$this->depth++;
	}
	function content($parser, $data)
	{
		$data = trim($data);
		if ($data !== '') $this->tag->add_content($data);
	}
	function end_element($parser, $name)
	{
		$this->tag =& $this->tag->get_parent();
		$this->depth--;
	}
	/**
	 * @return arr|false
	 * @desc Parse $data & return array of XML_Tag objects
	 */
	function & get_xml_tags($data, $isFinal = null)
	{
		return parent::parse($data,$isFinal) ? $this->tags : false;
	}
}