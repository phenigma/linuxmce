<?
require_once('g/this.php');
require_once('g/fn_globals.php');

class Database
{
	var $server;
	var $db_name;
	var $db_name_last_selected;
	
	var $connection = false;
	var $result = false;
	var $display_queries = false;
	var $last_query = '';
	
	/**
	 * @return Database
	 * @desc ABSTRACT: Extending class should implement: query(), fetch_row(), fetch_table()
	 * DB ABSTRACTION: Layer 1
	 */
	function Database()
	{
		this::abstract_constructor('Database');
	}
	
	/**
	 * @return void
	 * @param sql string
	 * @desc ABSTRACT: Query an SQL DB
	 */
	function query($sql)
	{
		this::abstract_function('Database','query');
	}
	
	/**
	 * @return void
	 * @param $row Array
	 * @desc ABSTRACT: Fetch row into $row
	 */
	function fetch_row(&$row, $result=null, $free_result=false)
	{
		this::abstract_function('Database','fetch_row');
	}
	
	/**
	 * @return void
	 * @param $table arr_arr
	 * @desc ABSTRACT: Fetch Result Rows into $table, returns count
	 */
	function fetch_table(&$table)
	{
		this::abstract_function('Database','fetch_table');
	}
	
	/**
	 * @return str
	 * @param str $item
	 * @desc ABSTRACT: prepare a string for use in a query
	 */
	function P_prepare_item($item)
	{
		this::abstract_function('Database','P_prepare_item');
	}
	
	function PRV_retry_item(&$item)
	{
		this::abstract_function('Database','PRV_retry_item');
	}
	
	function table_exists($table_name)
	{
		this::abstract_function('Database','table_exists');
	}

	function column_exists($table_name, $col_name)
	{
		this::abstract_function('Database','column_exists');
	}
	
	function free_result($result=null)
	{
		this::abstract_function('Database','free_result');
	}
	
	function PRV_prepare_item(&$item)
	{
		$item = $this->P_prepare_item($item);
	}

	/**
	 * @return string
	 * @desc Get the last sql executed
	 */
	function get_last_query()
	{
		return $this->last_query;
	}
	
	/**
	 * @return str|false
	 * @desc select a single value
	 */
	function select_value($select,$from='',$where='')
	{
		if ($this->select_row($row,$select.' AS my_result',$from,$where)) return $row['my_result'];
		else return false;
	}
	
	/**
	 * @return str|false
	 * @param $fn_name str
	 * @param $params str
	 * @desc TODO: DEPRECATE: use select_value() instead Execute $fn_name with $params
	 */
	function sql_function($fn_name,$params='',$from='',$where='')
	{
		return $this->select_value("$fn_name($params)",$from,$where);
	}
	
	/**
	 * @return int
	 * @param $table arr
	 * @param $id_field str
	 * @param $reset_table bool
	 * @param $null_key str
	 * @desc Fetch table into $table, indexed by $id_field
	 * ( $table[$row[$id_field]] = $row )
	 * returns count()
	 */
	function fetch_table_id(&$table, $id_field='id', $reset_table=true, $null_key='NULL')
	{
		if ($reset_table) $table = array();
		while ($this->fetch_row($row))
		{
			if ($row[$id_field]=='') $row[$id_field] = $null_key;
			$table[$row[$id_field]] = $row;
		}
		$this->free_result();
		return count($table);
	}
	
	/**
	 * @return int
	 * @param $table arr
	 * @param $id_field str
	 * @param $reset_table bool
	 * @param $null_key str
	 * @desc Fetch table into $table, indexed by $id_field with an array of rows for each index
	 * ( $table[$row[$id_field]][] = $row )
	 * returns count()
	 */
	function fetch_table_id_arr(&$table, $id_field='id', $reset_table=true, $null_key='NULL')
	{
		if ($reset_table) $table = array();
		while ($this->fetch_row($row))
		{
			if ($row[$id_field]=='') $row[$id_field] = $null_key;
			$table[$row[$id_field]][] = $row;
		}
		$this->free_result();
		return count($table);
	}
	
	/**
	 * @return int
	 * @param $table arr
	 * @param $id_field_arr arr
	 * @param $reset_table bool
	 * @param $null_key str
	 * @desc Fetch table into $table, indexed by each $id_field in $id_field_arr
	 * ( $table[$row[$id_field[0]]][$row[$id_field[1]]] = $row )
	 * returns count()
	 */
	function fetch_table_id_sub_arr(&$table, $id_field_arr=array(), $reset_table=true, $null_key='NULL')
	{
		if ($reset_table) $table = array();
		while ($this->fetch_row($row))
		{
			$first = true;
			foreach ($id_field_arr as $id_field)
			{
				if ($row[$id_field]=='') $row[$id_field] = $null_key;
				if ($first) $set_in =& $table[$row[$id_field]];
				else $set_in =& $set_in[$row[$id_field]];
				$first = false;
			}
			$set_in = $row;
		}
		$this->free_result();
		return count($table);
	}
	
	/**
	 * @return int
	 * @param $column arr
	 * @param fieldname str
	 * @desc Fetch a column into $column
	 * ( $column[] = $value )
	 * NOTE: an empty $fieldname will assume you only selected 1 row
	 */
	function fetch_column(&$column, $fieldname='', $index_by_val=false)
	{
		$column = array();
		$count = $this->fetch_table($table);
		foreach ($table as $row)
		{
			if ($fieldname=='')
			{
				foreach ($row as $field)
				{
					if ($index_by_val) $column[$field] = $field;
					else $column[] = $field;
				}
			}
			else
			{
				if ($index_by_val) $column[$row[$fieldname]] = $row[$fieldname];
				else $column[] = $row[$fieldname];
			}
		}
		return $count;
	}
	
	/**
	 * @return int
	 * @param $column arr
	 * @param $fieldname str
	 * @desc Fetch a column into $column, placing values as keys
	 * ( $column[$value] = true )
	 * NOTE: an empty $fieldname will assume you only selected 1 row
	 */
	function fetch_column_key(&$column, $fieldname='')
	{
		$column = array();
		$count = $this->fetch_table($table);
		foreach ($table as $row)
		{
			if ($fieldname=='')
			{
				foreach ($row as $field)
				{
					$column[$field] = true;
				}
			}
			else $column[$row[$fieldname]] = true;
		}
		return $count;
	}
	
	function PRV_get_row_index($key)
	{
		$key = trim($key);
		$as_pos = strpos(strtolower($key),' as ');
		if ($as_pos !== false) $key = substr($key,$as_pos+4);
		else 
		{
			$dot_pos = strrpos($key,'.');
			if ($dot_pos !== false) $key = substr($key,$dot_pos+1);
		}
		return $key;
	}
	
	/**
	 * @return int
	 * @param $map arr
	 * @param $key_name str
	 * @param $value_name str
	 * @desc $map[$key] = $value, returns count.
	 */
	function fetch_map(&$map, $key_name, $value_name, $reset_map=true)
	{
		if ($reset_map) $map = array();
		$count = $this->fetch_table($table);
		
		$key_name = $this->PRV_get_row_index($key_name);
		$value_name = $this->PRV_get_row_index($value_name);
		
		foreach ($table as $row)
		{
			if (is_numeric($row[$key_name]) && !substr_count($row[$key_name],'.')) $row[$key_name] = (int)$row[$key_name]; // work-around PHP-BUG #21918 Negative int Keys
			$map[$row[$key_name]] = $row[$value_name];
		}
		return $count;
	}
	
	function get_map_from_table(&$table, $key_name, $value_name)
	{
		$map = array();
		foreach ($table as $row)
		{
			if (is_numeric($row[$key_name]) && !substr_count($row[$key_name],'.')) $row[$key_name] = (int)$row[$key_name]; // work-around PHP-BUG #21918 Negative int Keys
			$map[$row[$key_name]] = $row[$value_name];
		}
		return $map;
	}
	
	/**
	 * @return resource
	 * @param select str|arr
	 * @param from str
	 * @param where str|arr
	 * @param orderby str
	 * @param limit str
	 * @param groupby str
	 * @param having str
	 * @desc WARNING: Does NOT prepare data for SQL
	 * $where as array: $where[$fieldname] = $value or $where[$fieldname] = $values(array)
	 * $where: For all fields, match any value in field.
	 */
	function select_query($select, $from='', $where='', $orderby='', $limit='', $groupby='', $having='')
	{
		if (is_array($select)) $select = implode(', ',$select);
		if (is_array($where)) $where = $this->PRV_match_arr($where);
		
		$sql = 'SELECT ';
		if ($limit!=='' && is_a($this,'Mssql_Database')) $sql.=" TOP $limit"; // NOTE: Doing here for simplicity
		$sql.= $select;
		if ($from!=='') $sql.=" FROM $from";
		if ($where!=='') $sql.=" WHERE $where";
		if ($groupby!=='') $sql.=" GROUP BY $groupby";
		if ($having!=='') $sql.=" HAVING $having";
		if ($orderby!=='') $sql.=" ORDER BY $orderby";
		if ($limit!=='' && !is_a($this,'Mssql_Database')) $sql.=" LIMIT $limit"; // NOTE: Doing here for simplicity
		return  $this->query($sql);
	}
	
	/**
	 * @return result
	 * @param select str
	 * @param from str
	 * @param id_values str|arr
	 * @param id_field str
	 * @param orderby str
	 * @desc NOTE: fixing S_ tables so PKID doesn't have 'S_' (in MySQL db, S_ tables are imported from MS)
	 * can enter more than 1 value
	 */
	function select_by_id($select, $from, $id_values, $id_field='', $orderby='')
	{
		if ($id_field == '')
		{
			if (substr($from,0,2)=='S_') $id_field = 'PKID_'.substr($from,2);
			else $id_field = "PKID_$from";
		}
		return $this->select_by_field($select, $from, $id_field, $id_values, $orderby);
	}
	
	function select_by_unique($select, $from, $unique_values, $unique_field='Define', $orderby='')
	{
		return $this->select_by_field($select,$from,$unique_field,$unique_values,$orderby);
	}
	
	/**
	 * @return result
	 * @param select str
	 * @param from str
	 * @param field str
	 * @param values str|arr
	 * @param orderby str
	 * @param limit str
	 * @desc Match any value in field
	 */
	function select_by_field($select, $from, $field, $values, $orderby='', $limit='')
	{
		$where = $this->PRV_match_any_value($field, $values);
		return $this->select_query($select,$from,$where,$orderby,$limit);
	}

	/**
	 * @return bool
	 * @param arr $map
	 * @param str $key
	 * @param str $value
	 * @param str $from
	 * @param str|arr $where
	 * @param str $orderby
	 * @param str $limit
	 * @param str $groupby
	 * @param str $having
	 * @desc
	 */
	function select_map(&$map, $key, $value, $from='', $where='', $orderby='', $limit='', $groupby='', $having='', $reset_map=true)
	{
		if ($this->select_query("$key,$value", $from, $where, $orderby, $limit, $groupby, $having)) return $this->fetch_map($map,$key,$value,$reset_map);
		else return false;
	}
	
	/**
	 * @return int|false
	 */
	function select_column(&$column, $select, $from='', $where='', $orderby='', $limit='', $groupby='', $having='', $index_by_val=false)
	{
		if ($this->select_query($select, $from, $where, $orderby, $limit, $groupby, $having)) return $this->fetch_column($column,'',$index_by_val);
		else return false;
	}
	
	function select_row(&$row, $select, $from='', $where='', $orderby='', $limit='', $groupby='', $having='')
	{
		return ($this->select_query($select, $from, $where, $orderby, $limit, $groupby, $having)) ? $this->fetch_row($row,null,true) : false;
	}
	
	function select_row_by_id(&$row, $select, $from, $id_value, $id_field='', $orderby='')
	{
		return ($this->select_by_id($select, $from, $id_value, $id_field, $orderby)) ? $this->fetch_row($row,null,true) : false;
	}
	
	function select_row_by_unique(&$row, $select, $from, $unique_values, $unique_field='Define', $orderby='')
	{
		return ($this->select_by_unique($select, $from, $unique_values, $unique_field, $orderby)) ? $this->fetch_row($row,null,true) : false;
	}
	
	function select_row_by_field(&$row, $select, $from, $field, $values, $orderby='', $limit='')
	{
		return ($this->select_by_field($select, $from, $field, $values, $orderby, $limit)) ? $this->fetch_row($row,null,true) : false;
	}
	
	function select_table(&$table, $select, $from, $where='', $orderby='', $limit='', $groupby='', $having='')
	{
		return ($this->select_query($select, $from, $where, $orderby, $limit, $groupby, $having)) ? $this->fetch_table($table) : false;
	}

	/**
	 * @return int
	 * @param $from str
	 * @param $where str|arr
	 * @desc Get # of rows in $from, $where
	 */
	function select_count($from, $where='')
	{
		return $this->select_value('COUNT(*)',$from,$where);
	}
	
	/**
	 * @return str
	 * @param tablename str
	 * @param $field2val arr
	 * @param auto_field str|false
	 * @param pwd_field str
	 * @param noquote_fields arr
	 * @desc PUBLIC: get SQL for an insert query
	 * PKID_$tablename not always auto-increment, use false in $auto_field to skip
	 */
	function P_insert_sql($tablename, &$field2val, $auto_field='', $pwd_field='', $noquote_fields=array())
	{
		if ($auto_field==='') $auto_field = "PKID_$tablename";
		$sql_into = "INSERT INTO $tablename(";
		$sql_values = ' VALUES (';
		$comma = false;
		foreach ($field2val as $field=>$val)
		{
			if ($field!==$auto_field && !is_array($val))
			{
				if ($comma)
				{
					$sql_into.=',';
					$sql_values.=',';
				}
				if (is_null($val)) $val = 'NULL';
				elseif ($field == $pwd_field) $val = "'".md5($val)."'";
				elseif (is_string($val) && !in_array($field,$noquote_fields)) $val = "'$val'";
				$sql_into.=$field;
				$sql_values.=$val;
				$comma = true;
			}
		}
		$sql_into.=')';
		$sql_values.=')';
		return $sql_into.$sql_values;
	}
	
	/**
	 * @return resource
	 * @param tablename str
	 * @param $field2val arr
	 * @param auto_field str|false
	 * @param pwd_field str
	 * @param noquote_fields arr
	 * @desc PUBLIC: Insert a row into the DB
	 * NOTE: Page calling this function should require fn_form.php
	 * PKID_$tablename not always auto-increment, use false in $auto_field to skip
	 */
	function insert_query($tablename, &$field2val, $auto_field='', $pwd_field='', $noquote_fields=array())
	{
		return $this->query($this->P_insert_sql($tablename, $field2val, $auto_field, $pwd_field, $noquote_fields));
	}
	
	/**
	 * @return resource
	 * @param tablename str
	 * @param $field2val arr
	 * @param id_field str|arr
	 * @param pwd_field str
	 * @param noquote_fields arr
	 * @desc $id_field can be Array of id_fields to match
	 * Meant for 1 row @ a time, update_where() is more flexible :)
	 */
	function update_query($tablename, &$field2val, $id_field='', $pwd_field='', $noquote_fields=array())
	{
		if ($id_field==='') $id_field = "PKID_$tablename";
		if (!is_array($id_field)) $id_field = array($id_field);
		
		$match_arr = array();
		$sql = "UPDATE $tablename SET ";
		$comma = false;
		foreach ($field2val as $field=>$val)
		{
			if (!is_array($val))
			{
				if (is_null($val)) $val = 'NULL';
				elseif ($field == $pwd_field) $val = "'".md5($val)."'";
				elseif (is_string($val) && !in_array($field,$noquote_fields)) $val = "'$val'"; // TODO: depends on type of $field not $val
				
				if (in_array($field,$id_field)) $match_arr[$field] = $val;
				else
				{
					if ($comma) $sql.=',';
					$sql.="$field=$val";
					$comma = true;
				}
			}
		}
		if (!$comma) return true; // no need to update row
		$sql.=' WHERE '.$this->PRV_match_arr($match_arr,'',true);
		return $this->query($sql);
	}

	/**
	 * @return resource
	 * @param $tablename str
	 * @param $field2val str|arr
	 * @param $where str|arr
	 * @param $noquote_fields arr
	 * @desc use $field2val for new value, $where for old values
	 */
	function update_where($tablename, $field2val, $where, $noquote_fields=array())
	{
		if (is_array($where)) $where = $this->PRV_match_arr($where,'',$noquote_fields);
		$sql = "UPDATE $tablename SET ";
		$comma = false;
		if (is_array($field2val))
		{
			foreach ($field2val as $field=>$val)
			{
				if (!is_array($val))
				{
					if (is_null($val)) $val = 'NULL';
					elseif (is_string($val) && !in_array($field,$noquote_fields)) $val = "'$val'"; // TODO: depends on type of $field not $val
					if ($comma) $sql .= ',';
					$sql .= "$field=$val";
					$comma = true;
				}
			}
			if (!$comma) return true; // no need to update row
		}
		else $sql .= $field2val;
		$sql .= " WHERE $where";
		return $this->query($sql);
	}
	
	
	/**
	 * @return result
	 * @param tablename str
	 * @param id_value str
	 * @param id_field str
	 * @desc Delete rows matching $id_value
	 */
	function delete_by_id($tablename, $id_value, $id_field='')
	{
		if ($id_field=='') $id_field = "PKID_$tablename";
		return $this->delete_query($tablename,$id_field,$id_value);
	}
	
	/**
	 * @return result
	 * @param tablename str
	 * @param field str
	 * @param values str|arr
	 * @desc Delete rows Matching any value in field
	 */
	function delete_query($tablename, $field, $values)
	{
		$where = $this->PRV_match_any_value($field, $values);
		return $this->query("DELETE FROM $tablename WHERE $where");
	}

	/**
	 * @return result
	 * @param tablename str
	 * @param field2vals arr
	 * @desc DEPERECATED: use delete_where()
	 * Delete rows where all fields, match any value in field
	 */
	function delete_match_arr($tablename, $field2vals)
	{
		$where = $this->PRV_match_arr($field2vals);
		return $this->query("DELETE FROM $tablename WHERE $where");
	}
	
	/**
	 * @return result
	 * @param $tablename str
	 * @param $where str|arr
	 * @desc Delete rows where all fields, match any value in field
	 */
	function delete_where($tablename, $where)
	{
		if (is_array($where)) $where = $this->PRV_match_arr($where);
		return $this->query("DELETE FROM $tablename WHERE $where");
	}
	
	/**
	 * @return void
	 * @param $tablenames arr
	 * @param field str
	 * @param values arr
	 * @desc Delete from $tablenames where $field IN $values
	 * Order of tables is important !
	 */
	function delete_from_tables(&$tablenames, $field, $values)
	{
		foreach ($tablenames as $tablename)
		{
			$this->delete_query($tablename, $field, $values);
		}
	}

	/**
	 * @return void
	 * @param $item arr|str
	 * @desc Recursively prepare all data submitted for sql.
	 */
	function prepare_data(&$item)
	{
		if (is_array($item))
		{
			foreach($item as $key=>$val)
			{
				$this->prepare_data($item[$key]);
			}
		}
		elseif (is_string($item)) $this->PRV_prepare_item($item);
	}
	
	/**
	 * @return void
	 * @param $item arr|str
	 * @desc Prepare data that was sql-ready for form, reverse of prepare_data() which prepares for sql
	 */
	function form_retry(&$item)
	{
		if (is_array($item))
		{
			foreach($item as $key=>$val)
			{
				$this->form_retry($item[$key]);
			}
		}
		elseif (is_string($item) && $item!=='') $this->PRV_retry_item($item);
	}
	
	
	/**
	 * @return unknown
	 * @param field unknown
	 * @param value unknown
	 * @param pwd_field unknown
	 * @param quote unknown
	 * @desc PRIVATE: Match a field with one value 
	 * NOTE: 'NULL' deprecated
	 */
	function PRV_match_value($field, $value, $pwd_field=false, $quote=true)
	{
		if ($pwd_field) return "$field='".md5($value)."'";
		elseif (is_null($value)) return "$field IS NULL";
		elseif ($quote) return "$field='$value'";
		else return "$field=$value";
	}
	
	/**
	 * @return unknown
	 * @param field unknown
	 * @param values unknown
	 * @param pwd_field unknown
	 * @param quote unknown
	 * @desc PRIVATE: Match any value in field
	 */
	function PRV_match_any_value($field, $values, $pwd_field=false, $quote=true)
	{
		if (is_array($values))
		{
			if (!count($values)) return '0=1';
			
			if (count($values) > 1 && !$pwd_field)
			{
				$where = $field.' IN (';
				$comma = $has_null = false;
				foreach ($values as $value)
				{
					if (is_null($value)) $has_null = true;
					else 
					{
						if ($comma) $where.=',';
						$where .= ($quote) ? "'$value'" : $value;
						$comma = true;
					}
				}
				$where .= ')';
				if ($has_null) $where.= " OR $field IS NULL";
				return $where;
			}
			
			$where = '';
			$or = false;
			foreach ($values as $value)
			{
				if ($or) $where.=' OR ';
				$where.= $this->PRV_match_value($field,$value,$pwd_field,$quote);
				$or = true;
			}
			return $where;
		}
		else return $this->PRV_match_value($field,$values,$pwd_field,$quote);
	}

	/**
	 * @return str
	 * @param $field2vals arr
	 * @param $pwd_field atr
	 * @param $no_quote_fields bool|arr
	 * @desc PRIVATE: For all fields, match any value in field 
	 * Use: $no_quote_fields['field'] = true;
	 */
	function PRV_match_arr(&$field2vals, $pwd_field='', $no_quote_fields=false)
	{
		$where = '';
		$and = false;
		foreach ($field2vals as $field=>$values)
		{
			if ($and) $where.=' AND ';
			if (is_bool($no_quote_fields)) $quote = !$no_quote_fields;
			else $quote = !isset_true($no_quote_fields[$field]);
			$where.='('.$this->PRV_match_any_value($field,$values,$pwd_field==$field,$quote).')';
			$and = true;
		}
		return $where;
	}
	
	/**
	 * @return str
	 * @param $field2vals arr
	 * @param $pwd_field str
	 * @param $no_quote_fields bool|arr
	 * @desc PUBLIC: Get the where clause. For all fields, match any value in field 
	 * Use: $no_quote_fields['field'] = true;
	 * returns with spaces and parenthesis: ' (...) '
	 */	
	function P_get_where_from_arr($field2vals, $pwd_field='', $no_quote_fields=false)
	{
		return ' ('.$this->PRV_match_arr($field2vals,$pwd_field,$no_quote_fields).') ';
	}
}