<?
require_once('g/this.php');
require_once('g/control.php');
class Form extends Control 
{
	/**
	 * @return Form
	 * @desc ABSTRACT Class that specific types of forms should inherit from
	 */
	function Form()
	{
		this::abstract_constructor('Form');
	}
	
	/**
	 * @return void
	 * @param action str
	 * @desc &lt;form action="$action" method="POST" name="form1"&gt;
	 */
	function display_tag_top($action='')
	{
		if ($action==='') $action = $_SERVER['PHP_SELF'];
		?><form action="<?= $action ?>" method="POST" name="form1"><?
	}
	/**
	 * @return void
	 * @desc &lt;/form&gt; 
	 * For consistency in tag implementation.
	 */
	function display_tag_bottom()
	{
		?></form><?
	}
	
	/**
	 * @return void
	 * @param action str|bool
	 * @param submit_text str
	 * @desc Set <font color="#990000">$action</font> to false to display just the content without &lt;form&gt;&lt;/form&gt;
	 */
	function display($action='', $submit_text='Update')
	{
		if ($action===true) $action = '';
		if (is_string($action)) $this->display_tag_top($action);
		$this->PRT_display($submit_text);		
		if (is_string($action)) $this->display_tag_bottom();
	}
	
	/**
	 * @return void
	 * @param $submit_text str
	 * @desc ABSTRACT: PROTECTED: Display everything between &lt;form&gt;&lt;/form&gt;
	 */
	function PRT_display($submit_text='Update')
	{
		this::abstract_function('Form','PRT_display');
	}

	/**
	 * @return Messages
	 * @desc ABSTRACT: Process the form and save to DB
	 */
	function & process()
	{
		this::abstract_function('Form','process');
	}
}
