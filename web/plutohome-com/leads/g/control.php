<?
require_once('g/this.php');

class Control
{
	/**
	 * @desc ABSTRACT CONSTRUCTOR
	 */
	function Control()
	{
		this::abstract_constructor('Control');
	}
	
	/**
	 * @desc TODO: DEPRECATE: ABSTRACT: make PRT_display & have constructor call
	 */
	function display()
	{
		this::abstract_function('Control','display');
	}
	
	/**
	 * @desc TODO: DEPRECATE ? make PRT_process & have constructor call
	 * NOT Abstract, May Be empty if JS control
	 */
	function process()
	{
	}

	/**
	 * @return str
	 * @param $disabled bool
	 * @desc PROTECTED: return $disabled ? ' disabled="disabled"' : '';
	 */
	function PRT_disabled_string($disabled = true)
	{
		return $disabled ? ' disabled="disabled"' : '';
	}

	/**
	 * @return str
	 * @param $checked bool
	 * @desc PROTECTED: return $checked ? ' checked="checked"' : '';
	 */
	function PRT_checked_string($checked = true)
	{
		return $checked ? ' checked="checked"' : '';
	}
	
	/**
	 * @return str
	 * @param $selected bool
	 * @desc PROTECTED: return $selected ? ' selected="selected"' : '';
	 * Deprecated, use PS_selected_string()
	 */
	function PRT_selected_string($selected = true)
	{
		return $selected ? ' selected="selected"' : '';
	}

	/**
	 * @return str
	 * @param $selected bool
	 * @desc STATIC: return $selected ? ' selected="selected"' : '';
	 */
	function PS_selected_string($selected = true)
	{
		return $selected ? ' selected="selected"' : '';
	}
	
	/**
	 * @return str
	 * @param $checked bool
	 * @desc STATIC: return $checked ? ' checked="checked"' : '';
	 */
	function PS_checked_string($checked = true)
	{
		return $checked ? ' checked="checked"' : '';
	}
	
	/**
	 * @return str
	 * @param $display bool
	 * @desc STATIC: return $display ? '' : " style='display:none'";
	 * NOTE: returns style attribute, don't use if you are setting this attribute.
	 */
	function PS_display_string($display = false)
	{
		return $display ? '' : " style='display:none'";
	}
	
	/**
	 * @desc DEPRECATED, use PS_display_select_options()
	 */
	function PRT_display_select_options(&$options,$selection)
	{
		foreach ($options as $key=>$val)
		{
			?><option value="<?=$key?>"<?=$this->PRT_selected_string(((string)$key)===((string)$selection))?>><?=$val?></option><?
		}
	}
	function PS_display_select_options(&$options,$selection='')
	{
		foreach ($options as $key=>$val)
		{
			?><option value="<?=$key?>"<?=Control::PRT_selected_string(((string)$key)===((string)$selection))?>><?=$val?></option><?
		}
	}
}
