<?
require_once('g/form.php');
class Form_Standard extends Form
{
	var $can_delete;
	var $db_row;
	var $db_tablename;
	var $fields;
	
	var $_deleted_db_row = false;
	var $_cancelled = false;
	
	/**
	 * @return Form_Standard
	 * @param $db_row DB_Row
	 * @param $fields arr
	 * @desc $fields[$field_name] = $field_description
	 */
	function Form_Standard(&$db_row, $fields='*', $can_delete=false)
	{
		$this->db_row =& $db_row;
		$this->fields = ($fields === '*') ? array_key_by_val($db_row->P_get_fieldnames()) : $fields;
//		$this->fields = ($fields === '*') ? $db_row->P_get_fieldnames() : $fields;
		$this->db_tablename = $db_row->P_get_tablename();
		$this->can_delete = $can_delete;
	}
	
	/**
	 * @desc PROTECTED
	 */
	function PRT_display_fields()
	{
		foreach ($this->fields as $fieldname=>$description)
		{
			?><tr><th align="right"><?= $description ?>:</th><td><?= $this->db_row->P_GETFF_($fieldname) ?></td></tr><?
		}
	}
	
	/**
	 * @return void
	 * @param action str|bool
	 * @param submit_text str
	 * @desc Set <font color="#990000">$action</font> to false to display just the content without &lt;form&gt&lt;/form&gt;
	 */
	function display($action='', $submit_text='Update', $show_cancel=false)
	{
		if (is_string($action)) $this->display_tag_top($action);
		?><table align="center" cellpadding="10" cellspacing="0" border="0"><?
		$this->PRT_display_fields();
		?><tr><td colspan="2" align="center"><? $this->PRT_display_update_cell_content($submit_text, $show_cancel); ?></td></tr></table><?
		if (is_string($action)) $this->display_tag_bottom();
	}
	
	function PRT_display_update_cell_content($submit_text, $show_cancel)
	{
		?><input type="submit" name="update" value="<?= $submit_text ?>" /><?
		if ($show_cancel)
		{
			?> <input type="submit" name="cancel" value="Cancel" /><?
		}
		if ($this->can_delete)
		{
			?> <input type="submit" name="delete" value="Delete" onclick="return confirm('Are you sure you want to delete this lead?')" /><?
		}
	}
	
	/**
	 * @return Messages
	 * @desc Process the form and save to DB
	 */
	function & process()
	{
		require_once('g/messages.php'); // Just in case, if it hasn't been included, it isn't being used
		$messages =& new Messages();
		
		if (isset($_POST['cancel']))
		{
			$this->_cancelled = true;
		}
		else 
		{
			if (isset($_POST['delete']))
			{
				$this->db_row->delete();
				$this->_deleted_db_row = true;
			}
			else 
			{
				if (!$this->db_row->P_set_fields($_POST[$this->db_tablename]))  
				{
					$messages->setError('Error with data');
				}
				if ($messages->noErrors())
				{
					if (!$this->db_row->update())
					{
						$messages->setError("Error updating $this->db_tablename");
					}
				}
			}
			$messages->pass_in($this->db_row->P_get_messages());
		}
		return $messages;
	}
	
	function deleted_db_row()
	{
		return $this->_deleted_db_row;
	}
	
	function cancelled()
	{
		return $this->_cancelled;
	}
}