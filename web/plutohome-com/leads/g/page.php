<?
require_once('g/this.php');
require_once('g/control.php');

class Page extends Control 
{
	var $title = '';
	
	function Page()
	{
		$this->PRT_load();
		$this->PRT_display();
	}
	
	function PRT_load()
	{
		this::abstract_function('Page','PRT_load');
	}
	
	function PRT_display()
	{
		?><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
		<html><head><?
		$this->PRT_display_head();
		?></head><body <? $this->PRT_display_body_properties(); ?>><?
		$this->PRT_display_body_start();
		$this->PRT_display_content();
		$this->PRT_display_body_end();
		?></body></html><?
	}
	
	function PRT_display_head(){}
	/**
	 * @return void
	 * @desc &lt;body ...&gt;
	 * TODO: Better to handle in OO way (set body properties instead of overwriting this).
	 */
	function PRT_display_body_properties(){}
	function PRT_display_body_start(){}
	function PRT_display_content(){}
	function PRT_display_body_end(){}
}
