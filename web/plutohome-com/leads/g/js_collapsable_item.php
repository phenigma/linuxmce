<?
require_once('g/javascript.php');
class js_collapsable_item
{
	var $name;
	var $desc;
	var $is_open;
	
	function js_collapsable_item($name, $description, $is_open)
	{
		$this->name = $name;
		$this->desc = $description;
		$this->is_open = $is_open;
	}
	
	function display_header()
	{
		echo javascript::source_tag('/'.APPLICATION_PATH.'/inc/javascript/collapsable_item.php');
	}
	
	function display_js_create_item()
	{
		$js_open = ($this->is_open) ? 'true' : 'false';
		echo "$this->name = new CollapsableItem(\"$this->name\",\"$this->desc\",$js_open);\n";
	}
	
	function display_start()
	{
		echo javascript::open_tag().$this->name.'.writeHTML(0);'.javascript::close_tag();
		?><noscript><table width="100%" cellpadding="4" cellspacing="0" border="0"><tr bgcolor="#D1D9EA">
			<td><b><?= $this->desc ?></b>&nbsp;</td>
		</tr></table></noscript><?
	}
	
	function display_end()
	{
		echo javascript::open_tag().$this->name.'.writeHTML(1);'.javascript::close_tag();
	}
}