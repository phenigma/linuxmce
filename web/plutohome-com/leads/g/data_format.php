<?
/*
// THIS SOLUTION IS PROBABLY OVERKILL FOR NOW

$DF_HTML_READY =& new Data_Format('',"'",'&quot;');
$DF_FORM_SUBMITTED =& new Data_Format('',"'",'"');

// MYSQL_READY: (null,"\\\'",'\"') // Preparing for db is more complicated

class Data_Format
{
	var $NULL_DATA;
	var $SINGLE_QUOTE;
	var $DOUBLE_QUOTE;
	
	function Data_Format($NULL_DATA, $SINGLE_QUOTE, $DOUBLE_QUOTE)
	{
		$this->NULL_DATA = $NULL_DATA;
		$this->SINGLE_QUOTE = $SINGLE_QUOTE;
		$this->DOUBLE_QUOTE = $DOUBLE_QUOTE;
	}
	
	function Transform($value, $new_format)
	{
	}
}
*/