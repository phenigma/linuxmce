<?
require_once('g/control.php');
require_once('g/fn_form.php'); // TODO: make OO

class Ctrl_Double_Select extends Control 
{
	var $static = false;
	
	var $name;
	var $options_available = array();
	var $options_selected = array();
	var $selected_option = '';
	
	var $js_onload = '';
	var $add_disabled = true;
	var $remove_disabled = true;
	
	function Ctrl_Double_Select($name)
	{
		$this->name = $name;
	}
	
	function display()
	{
		?><table cellpadding="2" cellspacing="0" border="0"><tr><?
		$this->PRT_display_cells();
		?></tr></table><?		
	}
	
	function PRT_display_cells()
	{
		?><td><?
		if ($this->static)
		{
			foreach ($this->options_selected as $option) echo $option.'<br />';
		}
		else 
		{
			?><select name="<?= $this->name ?>__available" size="10" onchange="<?= $this->PRT_js_available_changed(); ?>"><?
				display_options($this->options_available,$this->selected_option); 
			?></select><?
			?></td><td>
				<input type="hidden" name="<?= $this->name ?>__button" value="" />
				<input type="button" name="<?= $this->name ?>__add" value="&gt;"<?= $this->PRT_disabled_string($this->add_disabled) ?> onclick="<?= $this->PRV_js_click('add'); ?>" /><br />
				<input type="button" name="<?= $this->name ?>__remove" value="&lt;"<?= $this->PRT_disabled_string($this->remove_disabled) ?> onclick="<?= $this->PRV_js_click('remove'); ?>" />
			</td><td>
				<select name="<?= $this->name ?>__selected" size="10" onchange="<?= $this->PRT_js_selected_changed(); ?>"><? 
				display_options($this->options_selected,$this->selected_option);
			?></select><?
		}
		?></td><?
	}
	
	function get_js_onload()
	{
		return $this->js_onload;
	}
	
	function PRV_js_click($name)
	{
		return $this->PRT_js_item('button').".value='$name';document.form1.submit();";
	}
	
	function PRT_js_available_changed()
	{
		return $this->PRT_js_item('selected').'.selectedIndex=-1;'
			  .$this->PRT_js_item('add').'.disabled=0;'
			  .$this->PRT_js_item('remove').'.disabled=1;';
	}
	
	function PRT_js_selected_changed()
	{
		return $this->PRT_js_item('available').'.selectedIndex=-1;'
			  .$this->PRT_js_item('add').'.disabled=1;'
			  .$this->PRT_js_item('remove').'.disabled=0;';
	}
	
	function PRT_js_item($name)
	{
		return 'document.form1.'.$this->PRT_item_name($name);
	}
	function & PRT_item_post($name)
	{
		return $_POST[$this->PRT_item_name($name)];
	}
	function PRT_item_name($name)
	{
		return $this->name."__$name";
	}
	
	/**
	 * @return Messages
	 */
	function & process()
	{
		$messages =& new Messages();
		if ($_SERVER['REQUEST_METHOD']==='POST')
		{
			if ($this->PRT_item_post('button') === 'add')
			{
				if ($this->PRT_add($this->PRT_item_post('available')))
				{
					$this->PRT_added($this->PRT_item_post('available'));
				}
			}
			if ($this->PRT_item_post('button') === 'remove')
			{
				if ($this->PRT_remove($this->PRT_item_post('selected')))
				{
					$this->PRT_removed($this->PRT_item_post('selected'));
				}
			}
		}
		return $messages;
	}
	
	function PRT_added($key)
	{
		$this->options_selected[$key] = $this->options_available[$key];
		unset($this->options_available[$key]);
		$this->remove_disabled = false;
		$this->selected_option = $key;
		$this->js_onload = $this->PRT_js_item('selected').'.focus();';
	}
	
	function PRT_removed($key)
	{
		$this->options_available[$key] = $this->options_selected[$key];
		unset($this->options_selected[$key]);
		$this->add_disabled = false;
		$this->selected_option = $key;
		$this->js_onload = $this->PRT_js_item('available').'.focus();';
	}
	
	/**
	 * @return bool
	 */
	function PRT_add($key)
	{
		this::abstract_function('Ctrl_Double_Select','PRT_add');
	}
	
	/**
	 * @return bool
	 */
	function PRT_remove($key)
	{
		this::abstract_function('Ctrl_Double_Select','PRT_remove');
	}
}