<?
class XML_Parser
{
	var $parser;
	function XML_Parser($encoding = null)
	{
		$this->parser = xml_parser_create($encoding);
		xml_set_object($this->parser,$this);
	}
	function set_element_handler($shdl,$ehdl)
	{
		return xml_set_element_handler($this->parser,$shdl,$ehdl);
	}
	function set_character_data_handler($hdl)
	{
		return xml_set_character_data_handler($this->parser, $hdl);
	}
	function parse($data,$isFinal=null)
	{
		return xml_parse($this->parser,$data,$isFinal);
	}
	function parse_into_struct($data,&$struct,&$index)
	{
		return xml_parse_into_struct($this->parser,$data,$struct,$index);
	}
	function get_error_code()
	{
		return xml_get_error_code($this->parser);
	}
	function error_string($code=null)
	{
		if (is_null($code)) $code = $this->get_error_code();
		return xml_error_string($code);
	}
	function get_current_line_number()
	{
		return xml_get_current_line_number($this->parser);
	}
	function free()
	{
		return xml_parser_free($this->parser);
	}
}