<?
require_once('g/control.php');
class Control_Numbered extends Control 
{
	var $ctrl_num;
	
	function Control_Numbered()
	{
		$this->P_reset_ctrl_num();
	}
	
	function P_reset_ctrl_num($first=false)
	{
		static $num = 0;
		if ($first) $num = 0;
		$this->ctrl_num = $num++;
	}
	
	/**
	 * @return int
	 */
	function P_get_ctrl_num()
	{
		return $this->ctrl_num;
	}

	/**
	 * @return Control_Numbered
	 * @desc PUBLIC STATIC
	 * $ctrl_arr should contain all $ctrls of type
	 * returns Control if it finds (and removes) it, null otherwise
	 */
	function & PS_remove_numbered_ctrl(&$ctrl_arr, $ctrl_num)
	{
		$ctrl_found = null;
		$found = false;
		if (is_numeric($ctrl_num))
		{
			foreach (array_keys($ctrl_arr) as $ctrl_key)
			{
				$ctrl =& $ctrl_arr[$ctrl_key]; /* @var $ctrl Control_Numbered */
				if ($ctrl->P_get_ctrl_num()===(int)$ctrl_num)
				{
					$found = true;
					$ctrl_found =& $ctrl;
					unset($ctrl_arr[$ctrl_key]);
					break;
				}
			}
			if ($found)
			{
				$first = true;
				foreach (array_keys($ctrl_arr) as $ctrl_key)
				{
					$ctrl_arr[$ctrl_key]->P_reset_ctrl_num($first);
					$first = false;
				}
			}
		}
		return $ctrl_found;
	}
}