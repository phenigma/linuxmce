<?
require_once('g/page.php');
class Window extends Page 
{
	function Window()
	{
		parent::Page();
		$_SESSION['this_page'] = $_SESSION['last_page']; // Maintain Opening page
	}
}