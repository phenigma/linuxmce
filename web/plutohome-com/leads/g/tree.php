<?
require_once('g/globals.php');

// TODO: Direct reference to a node (from tree) by its id
// TODO: Mechanism for opening & closing nodes
// TODO: display node status: (+) (-) (.)

class Node // this class should be extended for sophisticated nodes
{
	var $id;
	var $open = true;
	var $nodes = array();
	var $parent;
	var $tree; // Tree the node belongs to. Set when added to tree.
	
	function Node($id, $open=true, $nodes='')
	{
		if (is_a($id,'Node')) $this = $id;
		else
		{
			$this->id = $id;
			$this->open = $open;
			if (is_array($nodes)) $this->nodes = $nodes;
		}
	}
	
	function add(&$node)
	{
		$node->parent = &$this;
		$node->set_tree($this->tree);
		$this->nodes[$node->id] = &$node;
	}
	
	function set_tree(&$tree)
	{
		$this->tree = &$tree;
		foreach (array_keys($this->nodes) as $key) $this->nodes[$key]->set_tree($tree);
	}
	
	function display($level = 1)
	{
		$this->display_node($level);
		if ($this->open) $this->display_nodes($level);
	}
	
	function display_option($selected='')
	{
		require_once('g/fn_form.php');
		echo '<option value="'.$this->id.'" '.selected_option($this->id,$selected).'>';
		$this->display_node_path();
		echo '</option>';
		if ($this->open) foreach ($this->nodes as $node) $node->display_option($selected);
	}
	
	function display_node($level)
	{
		$this->display_node_path();
		echo '<br />';
	}

	function display_node_data()
	{
		echo $this->id;
	}

	function display_node_path($separator=' -> ')
	{
		if ($this->parent != null)
		{
			$this->parent->display_node_path();
			echo $separator;
		}
		$this->display_node_data();
	}
	
	function display_nodes($level)
	{
		foreach ($this->nodes as $node) $node->display($level+1);
	}
	
	function get_node_path()
	{
		$node_path = array();
		if ($this->parent != null)
		{
			$node_path = $this->parent->get_node_path();
		}
		$node_path[$this->id] = $this->id;
		return $node_path;
	}
	
	function &get_node($id)
	{
		if ($this->id == $id) return $this;
		foreach (array_keys($this->nodes) as $a_node_key)
		{
			$a_node =& $this->nodes[$a_node_key];
			$node =& $a_node->get_node($id);
			if (is_a($node,'Node')) return $node;
		}
		return null;
	}
	
	function contains($id)
	{
		if ($this->id == $id) return true;
		foreach ($this->nodes as $a_node)
		{
			if ($a_node->contains($id)) return true;
		}
		return false;
	}
	
	function count()
	{
		return count($this->nodes);
	}
	
	function get_num_levels()
	{
		$num_levels = 1;
		foreach ($this->nodes as $node)
		{
			$num_levels = max($num_levels, $node->get_num_levels()+1);
		}
		return $num_levels;		
	}
}

class Tree
{
	var $nodes = array();
	
	function Tree()
	{
	}
	
	function add(&$node)
	{
		$node->set_tree($this);
		$this->nodes[$node->id] = &$node;
	}
	
	function add_array(&$nodes)
	{
		foreach (array_keys($nodes) as $key) $this->add($nodes[$key]);
	}
	
	/**
	 * @param html_tag HTML_Tag
	 * @desc set $html_tag to HTML_tag to enclose nodes in this tag
	 */
	function display($html_tag = false)
	{
		if (is_a($html_tag,'HTML_Tag')) echo $html_tag->get_start();
		foreach ($this->nodes as $node) $node->display();
		if (is_a($html_tag,'HTML_Tag')) echo $html_tag->get_end();
	}
	
	function display_select_options($selected='')
	{
		foreach ($this->nodes as $node) $node->display_option($selected);
	}
	
	function &get_node($id)
	{
		foreach (array_keys($this->nodes) as $a_node_key)
		{
			$a_node =& $this->nodes[$a_node_key];
			$node =& $a_node->get_node($id);
			if (is_a($node,'Node')) return $node;
		}
		return null;
	}
	
	function get_num_levels()
	{
		$num_levels = 0;
		foreach ($this->nodes as $node)
		{
			$num_levels = max($num_levels, $node->get_num_levels());
		}
		return $num_levels;		
	}
}