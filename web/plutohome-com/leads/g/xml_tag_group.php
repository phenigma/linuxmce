<?
require_once('g/fn_globals.php');

class XML_Tag_Group
{
	var $xml_tags = array();
	
	function XML_Tag_Group()
	{
	}
	
	/**
	 * @return void
	 * @param str|XML_Tag|XML_Tag_Group $xml
	 * @desc add $xml to the beginning of this group
	 */
	function add_before(&$xml)
	{
		array_unshift_ref($this->xml_tags,$xml);
	}

	/**
	 * @return void
	 * @param str|XML_Tag|XML_Tag_Group $xml
	 * @desc add $xml to the end of this group
	 */
	function add_after(&$xml)
	{
		$this->xml_tags[] =& $xml;
	}
	
	/**
	 * @return str
	 */
	function get_xml()
	{
		$xml = '';
		foreach ($this->xml_tags as $xml_tag) /* @var $xml_tag XML_Tag */
		{
			$xml .= (is_a($xml_tag,'XML_Tag') || is_a($xml_tag,'XML_Tag_Group')) ? $xml_tag->get_xml() : $xml_tag;
		}
		return $xml;
	}
}