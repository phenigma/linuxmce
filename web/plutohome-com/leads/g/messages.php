<?
// TODO: Display All Messages in order of creation
// TODO: Ability to attach Messages to other objects

// require_once('g/globals.php'); // Requiring here will cause a MAJOR ERROR:
// g/globals.php requires g/session_start.php which in customer_setup/login.php has been required.
// when this page loads g/globals.php, g/session_start.php has started to load but not finished.

class Messages
{
	var $prg_errors = array(); // program errors
	var $errors = array();  // user errors
	var $notices = array(); // user notices
	
	/**
	 * @return Messages
	 * @param messages Messages
	 * @desc Keep track of notices and errors
	 */
	function Messages($messages = null)
	{
		if (isset($messages))
		{
			if (is_object($messages))
			{
				if (function_exists('clone_object')) clone_object($this,$messages);
				else $this->setError('Require APPLICATION_PATH/globals.php to clone $messages');
			}
			if (is_string($messages)) $this->setError($messages);
		}
	}
	
	/**
	 * @return void
	 * @param $more_messages Messages
	 * @desc combine messages in $this with messages in $more_messages
	 */
	function combine(&$more_messages)
	{
		$this->prg_errors = array_merge($this->prg_errors,$more_messages->prg_errors);
		$this->errors = array_merge($this->errors,$more_messages->errors);
		$this->notices = array_merge($this->notices,$more_messages->notices);
	}
	
	/**
	 * @return void
	 * @param $messages Messages
	 * @desc Pass in $messages to $this
	 */
	function pass_in(&$messages, $new_error='')
	{
		if ($new_error !== '') $this->setError($new_error);
		$this->combine($messages);
		$messages->clear();
	}
	
	function display($clear = true)
	{
		echo $this->get($clear);
	}
	
	function display_prg_errors($clear = true)
	{
		echo $this->get_prg_errors($clear);
	}
	
	function displayErrors($clear = true)
	{
		echo $this->getErrors($clear);
	}

	function displayNotices($clear = true)
	{
		echo $this->getNotices($clear);
	}
	
	function get($clear = true)
	{
		return $this->get_prg_errors($clear).$this->getErrors($clear).$this->getNotices($clear);
	}
	
	function get_prg_errors($clear = true)
	{
		return $this->PRV_get('prg_errors',$clear,'red');
	}
	
	function getErrors($clear = true)
	{
		return $this->PRV_get('errors',$clear,'red');
	}
	
	function getNotices($clear = true)
	{
		return $this->PRV_get('notices',$clear,'blue');
	}
	
	function PRV_get($type, $clear, $color)
	{
		$error_str = '';
		foreach ($this->$type as $error)
		{
			$error_str .= "<font color=\"$color\"><b>$error</b></font><br />";
		}
		if ($clear) $this->PRV_clear($type);
		return $error_str;
	}
	
	function clear()
	{
		$this->clear_PRG_Errors();
		$this->clearErrors();
		$this->clearNotices();
	}
	function clear_PRG_Errors()
	{
		$this->PRV_clear('prg_errors');
	}
	
	function clearNotices()
	{
		$this->PRV_clear('notices');
	}
	
	function clearErrors()
	{
		$this->PRV_clear('errors');
	}
	
	function PRV_clear($type)
	{
		$this->$type = array();
	}
	
	function setNotice($notice, $id_notice='')
	{
		if (is_string($notice) && $notice!='')
		{
			if ($id_notice=='') $this->notices[] = $notice;
			else $this->notices[$id_notice] = $notice;
		}
	}
	
	/**
	 * @return false
	 * @desc add or edit error
	 */
	function setError($error, $id_error='')
	{
		if (is_string($error) && $error!='')
		{
			if ($id_error=='') $this->errors[] = $error;
			else $this->errors[$id_error] = $error;
		}
		return false;
	}
	
	function set_prg_error($error, $id_error='')
	{
		if (is_string($error) && $error!='')
		{
			if ($id_error=='') $this->prg_errors[] = $error;
			else $this->prg_errors[$id_error] = $error;
		}
	}
	
	/**
	 * @return int
	 * @desc Get Number of Errors and Notices
 	 */
	function count()
	{
		return count($this->prg_errors) + count($this->errors) + count($this->notices);
	}
	
	/**
	 * @return int
	 */
	function count_errors()
	{
		return count($this->errors) + count($this->prg_errors);
	}
	
	/**
	 * @return bool
	 */
	function hasErrors()
	{
		return (bool)$this->count_errors();
	}
	
	/**
	 * @return bool
	 */
	function noErrors()
	{
		return ($this->count_errors()===0);
	}
}