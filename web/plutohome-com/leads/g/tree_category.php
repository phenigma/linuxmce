<?
require_once('g/tree.php');

class Category_Node extends Node 
{
	var $FKID_Category_Parent;
	var $Define;
	var $Description;

	function Category_Node($id, $Description='', $FKID_Category_Parent='', $Define='', $open=true, $nodes='')
	{
		if (is_a($id,'Node')) $this = $id;
		else 
		{
			parent::Node($id,$open,$nodes);
			$this->Description = $Description;
			$this->FKID_Category_Parent = $FKID_Category_Parent;
			$this->Define = $Define;
		}
	}	
	
	function display_node_data()
	{
		echo $this->Description;
	}
}

class Category_Node_Populate
{
	function & get_table(&$db_my, $cat_type_id, $user_id)
	{
		$match_arr = array();
		$match_arr['FKID_Users'][] = null;
		$match_arr['FKID_Users'][] = $user_id;
		$match_arr['FKID_C_CategoryType'] = $cat_type_id;
		$db_my->select_query('*','Category',$match_arr,'Level'); // Order by level to ensure parent nodes will already exist
		$db_my->fetch_table_id($category_table,'PKID_Category');
		return $category_table;
	}
	
	function & get_nodes_from_table(&$category_table)
	{
		$nodes_final = $nodes = array();
		foreach ($category_table as $category_id=>$category_row)
		{
			$nodes[$category_id] =& new Category_Node($category_id,$category_row['Description']);
			if ($category_row['FKID_Category_Parent']!='')
			{
				$nodes[$category_row['FKID_Category_Parent']]->add($nodes[$category_id]);
			}
			else $nodes_final[$category_id] =& $nodes[$category_id];
		}
		return $nodes_final;
	}
	
	function & get_nodes(&$db_my, $cat_type_id, $user_id)
	{
		return Category_Node_Populate::get_nodes_from_table(Category_Node_Populate::get_table($db_my, $cat_type_id, $user_id));
	}
}