<?
require_once('g/xml_tag.php');
class HTML_Tag extends XML_Tag 
{
	var $right_before = array();
	var $right_after = array();
	
	/**
	 * @desc $content: false for <tag /> <br />
	 * DEPRECATE: use XML_Tag & XML_Tag_Group
	 */
	function HTML_Tag($name, $properties=array(), $content=array())
	{
		parent::XML_Tag($name,$properties,$content);
	}

	/**
	 * @return void
	 * @param str|XML_Tag $html
	 * @desc Display $html before $this when displaying
	 */
	function tie_before($html)
	{
		$this->right_before[] = $html;
	}
	
	/**
	 * @return void
	 * @param str|XML_Tag $html
	 * @desc Display $html after $this when displaying
	 */
	function tie_after($html)
	{
		$this->right_after[] = $html;
	}
	
	/**
	 * @return str
	 */
	function get_xml()
	{
		$xml = $this->PRV_get_content($this->right_before);
		$xml.= parent::get_xml();
		$xml.= $this->PRV_get_content($this->right_after);
		return $xml;
	}

	/**
	 * @return str
	 */
	function get_html()
	{
		return $this->get_xml();
	}
}