<?
require_once('g/ctrl_double_select.php');
class Ctrl_Double_Select_Ordered extends Ctrl_Double_Select
{
	var $ordered = false;
	var $up_disabled = true;
	var $down_disabled = true;
	
	function Ctrl_Double_Select_Ordered($name)
	{
		parent::Ctrl_Double_Select($name);
	}
	
	function display($static = false)
	{
		$this->static = $static;
		parent::display();
	}
	
	/**
	 * @return Messages
	 */
	function & process($ordered=true)
	{
		$messages =& new Messages();
		$this->ordered = $ordered; // BEFORE parent::process()
		$messages->pass_in(parent::process());
		if ($ordered)
		{
			if ($this->PRT_item_post('button') === 'up') $this->PRT_up((int)$this->PRT_item_post('selected'));
			if ($this->PRT_item_post('button') === 'down') $this->PRT_down((int)$this->PRT_item_post('selected'));
		}
		$this->PRT_set_sort();
		return $messages;
	}
	
	/**
	 * @desc overwrite to use, set the sort order in the db
	 */
	function PRT_set_sort()
	{
	}
	
	function PRT_up($key)
	{
		$up_disabled = true;
		foreach (array_keys($this->options_selected) as $i=>$o_key)
		{
			if (($o_key === $key) && isset($o_key_prev))
			{
				if ($i > 1) $up_disabled = false;
				$this->PRT_down($o_key_prev);
				break;
			}
			$o_key_prev = $o_key;
		}
		$this->selected_option = $key;
		$this->down_disabled = false;
		$this->up_disabled = $up_disabled;
	}

	function PRT_down($key)
	{
		$options_selected_orig = $this->options_selected;
		$this->options_selected = array();
		$passing = $passed = $more = false;
		foreach ($options_selected_orig as $o_key=>$o_val)
		{
			if ($passed) $more = true;
			if ($passing)
			{
				$this->options_selected[$o_key] = $o_val;
				$this->options_selected[$key] = $val;
				$passed = true;
			}
			if ($passing = ($o_key === $key)) $val = $o_val;
			else $this->options_selected[$o_key] = $o_val;
		}
		$this->selected_option = $key;
		$this->js_onload = $this->PRT_js_item('selected').'.focus();';
		$this->remove_disabled = false;
		$this->up_disabled = false;
		$this->down_disabled = !$more;
	}

	function get_keys()
	{
		return array_keys($this->options_selected);
	}
	
	function PRT_added($key)
	{
		parent::PRT_added($key);
		if ($this->ordered) $this->up_disabled = (count($this->options_selected) < 2);
	}	
	
	function PRT_display_cells()
	{
		parent::PRT_display_cells();
		if (!$this->static)
		{
			?><td>
			<input type="button" name="<?= $this->name ?>__up" value="u"<?= $this->PRT_disabled_string($this->up_disabled) ?> onclick="<?= $this->PRV_js_click('up'); ?>" /><br />
			<input type="button" name="<?= $this->name ?>__down" value="d"<?= $this->PRT_disabled_string($this->down_disabled) ?> onclick="<?= $this->PRV_js_click('down'); ?>" />
			</td><?
		}
	}
	
	function PRT_js_available_changed()
	{
		return parent::PRT_js_available_changed()
			.$this->PRT_js_item('up').'.disabled=1;'
			.$this->PRT_js_item('down').'.disabled=1;';
	}
	
	function PRT_js_selected_changed()
	{
		$js = parent::PRT_js_selected_changed();
		if ($this->ordered)
		{
			$js.= $this->PRT_js_item('up').'.disabled=('.$this->PRT_js_item('selected').'.selectedIndex==0);'
				 .$this->PRT_js_item('down').'.disabled=('.$this->PRT_js_item('selected').'.selectedIndex==('.$this->PRT_js_item('selected').'.length-1));';
		}
		return $js;
	}	
}