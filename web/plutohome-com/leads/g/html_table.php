<?
require_once('g/html_tag.php');

class HTML_Table extends HTML_Tag
{
	var $rows;       // Array of row objects
	
	function Table($properties=array())
	{
		parent::Tag('table',$properties);
		$this->rows = array();
	}
	
	function display()
	{
		parent::display_start();
		foreach ($this->rows as $row) $row->display();
		parent::display_end();
	}
	
	function get_row($id_row)
	{
		if (isset($this->rows[$id_row])) return $this->rows[$id_row];
		else return false;
	}
	
	function set_row($row, $id_row='') // add or edit
	{
		if ($id_row=='') $this->rows[] = $row;
		else $this->rows[$id_row] = $row;
	}
}

class HTML_Row extends HTML_Tag
{
	var $cells;      // Array of cell objects
	
	function Row($properties=array())
	{
		parent::Tag('tr',$properties);
		$this->cells = array();
	}
	
	function display()
	{
		parent::display_start();
		foreach ($this->cells as $cell) $cell->display();
		parent::display_end();
	}
	
	function get_cell($id_cell)
	{
		if (isset($this->cells[$id_cell])) return $this->cells[$id_cell];
		else return false;
	}
	
	function set_cell($cell, $id_cell='') // add or edit
	{
		if ($id_cell=='') $this->cells[] = $cell;
		else $this->cells[$id_cell] = $cell;
	}
	
}

class HTML_Cell extends HTML_Tag
{
	var $content;      // content of cell object
	
	function Cell($content, $properties=array())
	{
		parent::Tag('td',$properties);
		$this->content = $content;
	}
	
	function display()
	{
		parent::display_start();
		echo $content;
		parent::display_end();
	}
}