<?
require_once('g/form.php');
class Form_X_Table extends Form 
{
	var $table_title_r; // Display title of table that defines rows
	var $table_title_c; // Display title of table that defines cols
	
	var $titles_r; // $titles_r[{ID}] = {Title of row}
	var $titles_c; // $titles_c[{ID}] = {Title of col}
	
	var $table; // $table[{ID_R}][{ID_C}] = true
	
	var $allow_same = true;
	
	/**
	 * @return Form_X_Table
	 * @param table_title_r str
	 * @param table_title_c str
	 * @param titles_r arr
	 * @param titles_c arr
	 * @param table arr
	 * @desc Specialized Form that's a table of checkboxes 
	 * $titles[{ID}] = {Title} 
	 * $table[{ID_R}][{ID_C}] = true
	 */
	function Form_X_Table($table_title_r,$table_title_c,$titles_r=null,$titles_c=null,$table=null)
	{
		$this->table_title_r = $table_title_r;
		$this->table_title_c = $table_title_c;
		$this->titles_r = $titles_r;
		$this->titles_c = $titles_c;
		if (is_array($table)) $this->set_table($table);
	}
	
	function set_table($table)
	{
		$real_table = array();
		foreach ($table as $id_r=>$row)
		{
			foreach ($row as $id_c=>$col)
			{
				if (isset($this->titles_r[$id_r], $this->titles_c[$id_c])) $real_table[$id_r][$id_c] = true;
			}
		}
		$this->table = $real_table;
	}
	
	function get_table_from_db(&$db, $table_name, $r_col_name, $c_col_name)
	{
		$match_arr_current = array($r_col_name=>array_keys($this->titles_r),$c_col_name=>array_keys($this->titles_c));
		$db->select_query('*',$table_name,$match_arr_current);
		$db->fetch_table_id_sub_arr($db_table,array($r_col_name,$c_col_name));
		foreach (array_keys($db_table) as $id_r)
		{
			foreach (array_keys($db_table[$id_r]) as $id_c) $db_table[$id_r][$id_c] = true;
		}
		return $db_table;
	}
	
	function fetch_table(&$db, $table_name, $r_col_name, $c_col_name)
	{
		$this->table = $this->get_table_from_db($db, $table_name, $r_col_name, $c_col_name);
	}
		
	/**
	 * @return void
	 * @param r0c1 unknown
	 * @param $db unknown
	 * @param tablename unknown
	 * @param id_col unknown
	 * @param description_col unknown
	 * @param filter_arr unknown
	 * @desc PRIVATE - called by fetch_rows() or fetch_col()
	 */
	function PRV_fetch_rc($r0c1, &$db, $tablename, $id_in_desc, $id_col, $description_col, $filter_arr)
	{
		if ($id_col=='') $id_col = "PKID_$tablename";
		
		if ($id_in_desc) $description = "$description_col + ' (' + CAST($id_col AS NVARCHAR(11)) + ')'";
		else $description = $description_col;
		
		$id_col .= ' AS id';
		$description .= ' AS description';
		
		$db->select_query("$id_col, $description",$tablename,$filter_arr);
		$db->fetch_map($map,'id','description');
		
		if ($r0c1) $this->titles_c = $map;
		else $this->titles_r = $map;
	}
	
	/**
	 * @return void
	 * @param $db Database
	 * @param tablename str
	 * @param filter_arr arr
	 * @param id_in_desc bool
	 * @param id_col str
	 * @param description_col str
	 * @desc Fetch Rows for table
	 */
	function fetch_rows(&$db, $tablename, $filter_arr=array(), $id_in_desc=false, $id_col='', $description_col='Description')
	{
		$this->PRV_fetch_rc(0, $db, $tablename, $id_in_desc, $id_col, $description_col, $filter_arr);
	}
	
	function fetch_cols(&$db, $tablename, $filter_arr = array(), $id_in_desc=false, $id_col='', $description_col='Description')
	{
		$this->PRV_fetch_rc(1, $db, $tablename, $id_in_desc, $id_col, $description_col, $filter_arr);
	}
	
	function fetch_rc(&$db, $tablename, $filter_arr = array(), $allow_same=false, $id_in_desc=false, $id_col='', $description_col='Description')
	{
		$this->fetch_rows($db,$tablename,$filter_arr,$id_in_desc,$id_col,$description_col);
		$this->titles_c = $this->titles_r;
		//$this->titles_c = array_reverse($this->titles_r,true);
		$this->allow_same = $allow_same;
	}
	
	
	/**
	 * @return int
	 * @param cb_title str
	 * @desc Display rows with checkboxes for form
	 * Does NOT echo &lt;table&gt; tags, returns # of cols in table
	 */
	function display_cb_tablerows($cb_title)
	{
		?><tr><th bgcolor="#ffffcc"><?= "$this->table_title_r \ $this->table_title_c" ?></th><?
		foreach ($this->titles_c as $title_c)
		{
			?><th><?= $title_c ?></th><?
		}
		?></tr><?
		foreach ($this->titles_r as $id_r=>$title_r)
		{
			?><tr><th align="right"><?= $title_r ?></th><?
			foreach ($this->titles_c as $id_c=>$title_c)
			{
				?><th><?
				if ($id_r===$id_c && !$this->allow_same) echo '&nbsp;';
				else 
				{
					?><input type="checkbox" name="<?=$cb_title."[$id_r][$id_c]"?>" value="1"<?=$this->PRT_checked_string(isset($this->table[$id_r][$id_c]))?> /><?
				}
				?></th><?
			}
			?></tr><?
		}
		return count($this->titles_c)+1; // Number of Columns in table
	}
	
	/**	
	 * @return void
	 * @param cb_title str
	 * @param action bool|str
	 * @param submit_text str
	 * @desc Set $action to false in order to avoid displaying &lt;form&gt&lt;/form&gt;
	 * TODO: match Form's parameter order
	 */
	function display($cb_title, $action='', $submit_text='Update')
	{
		if (is_string($action)) $this->display_tag_top($action);
		
		?><table align="center" border="1" cellspacing="0" cellpadding="5"><?
			$num_cols = $this->display_cb_tablerows($cb_title);
			?><tr><td align="center" colspan="<?= $num_cols ?>"><input type="submit" value="<?= $submit_text ?>" /></td></tr><?
		?></table><?
		
		if (is_string($action)) $this->display_tag_bottom();
	}
	
	/**
	 * @return void
	 * @param $db Database
	 * @param $table_name str
	 * @param $r_col_name str
	 * @param $c_col_name str
	 * @param $other_col_vals arr
	 * @desc Save current table to database
	 * $other_col_vals[$colname] = $value, used when inserting new rows.
	 * $other_col_vals[$colname]['fn'] = $fn_name calls function $fn_name($id_row,$id_col) to get value.
	 * TODO: DEPRECATE $other_col_vals (used in controller_scenario.php) ?
	 */
	function save_to_db(&$db, $table_name, $r_col_name, $c_col_name, $other_col_vals=array())
	{
		$db_table = $this->get_table_from_db($db, $table_name, $r_col_name, $c_col_name);
		foreach (array_keys($this->titles_r) as $id_r)
		{
			foreach (array_keys($this->titles_c) as $id_c)
			{
				if (isset($this->table[$id_r][$id_c]))
				{
					if (!isset($db_table[$id_r][$id_c]))
					{
						$this->PRT_insert($db,$table_name,$id_r,$id_c,$r_col_name,$c_col_name,$other_col_vals);
					}
				}
				elseif (isset($db_table[$id_r][$id_c]))
				{
					$this->PRT_delete($db,$table_name,$id_r,$id_c,$r_col_name,$c_col_name);
				}
			}
		}
	}
	
	/**
	 * @return void
	 * @param $db Database
	 * @param $table_name str
	 * @param $id_r int
	 * @param $id_c int
	 * @param $r_col_name str
	 * @param $c_col_name str
	 * @param $other_col_vals arr
	 */
	function PRT_insert(&$db,$table_name,$id_r,$id_c,$r_col_name,$c_col_name,$other_col_vals)
	{
		foreach ($other_col_vals as $column=>$column_value)
		{
			if (is_array($column_value) && isset($column_value['fn']))
			{
				$column_value_fn = $column_value['fn'];
				$other_col_vals[$column] = $column_value_fn($id_r,$id_c);
			}
		}
		$other_col_vals[$r_col_name] = $id_r;
		$other_col_vals[$c_col_name] = $id_c;
		$db->insert_query($table_name,$other_col_vals);
	}
	
	/**
	 * @return void
	 * @param $db Database
	 * @param $table_name str
	 * @param $id_r int
	 * @param $id_c int
	 * @param $r_col_name str
	 * @param $c_col_name str
	 */
	function PRT_delete(&$db,$table_name,$id_r,$id_c,$r_col_name,$c_col_name)
	{
		$db->delete_where($table_name,array($r_col_name=>$id_r,$c_col_name=>$id_c));
	}
}