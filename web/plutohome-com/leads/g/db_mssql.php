<?
require_once('g/database.php');

class Mssql_Database extends Database 
{
	var $select_query_table;
	var $select_query_table_resource;
	var $num_queries = 0;
	
	/**
	 * @return Mssql_Database
	 * @param server string
	 * @param user string
	 * @param pwd string
	 * @param db_name string
	 * @desc Connect to a new MS-SQL Database
	 */
	function Mssql_Database($server, $user, $pwd, $db_name, $exit_on_error = true)
	{
		static $db_name_last_selected;
		$this->db_name_last_selected =& $db_name_last_selected;
		
		$this->connection = mssql_connect($server, $user, $pwd);
		if (!$this->connection)
		{
			if ($exit_on_error) exit('Error Connecting to database');
			return $this = null;
		}
		if (!$this->PRV_select_db($db_name, $exit_on_error)) return $this = null;
		$this->db_name = $db_name;
		$this->server = $server;
	}
	
	function PRV_select_db($db_name, $exit_on_error = true)
	{
		$success = mssql_select_db($db_name, $this->connection);
		if ($success) $this->db_name_last_selected = $db_name;
		elseif ($exit_on_error) exit('Error Selecting Database');
		return $success;
	}
	
	function close()
	{
		return mssql_close($this->connection);
	}
	
	function query($sql)
	{
		if ($this->db_name !== $this->db_name_last_selected) $this->PRV_select_db($this->db_name);
		$this->last_query = $sql;
		$this->result = mssql_query($sql, $this->connection);
		if (!$this->result)
		{
			exit("Error with query: <br />\"$sql\"<br />".mysql_error()); // TODO: better error handling //  on ($this->server,$this->db_name)
		}
		if ($this->display_queries)
		{
			echo str_replace("\n",'<br />',$sql).'<br /><br />';
		}
		$this->num_queries++;
		return $this->result;
	}

	/**
	 * @return int|res|false
	 * @param $tablename str
	 * @param $field2val arr
	 * @param $auto_field str
	 * @param $pwd_field str
	 * @param $noquote_fields arr
	 * @desc Page calling this function should require fn_form.php
	 * returns insert id if $auto_field set, otherwise returns $result
 	 */
	function insert_query($tablename, &$field2val, $auto_field='', $pwd_field='', $noquote_fields=array())
	{
		$result = parent::insert_query($tablename,$field2val,$auto_field,$pwd_field,$noquote_fields);
		if ($result === false) return false;
		
		if ($auto_field !== false) // NOTE: was '', but '' defaults to PKID_$tablename
		{
			$this->select_row($row,'SCOPE_IDENTITY() AS si');
			return ($row['si']==='') ? $result : (int)$row['si'];
		}
		return $result;
	}
	
	/**
	 * @return resource
	 * @param select str
	 * @param from str
	 * @param where str|arr
	 * @param orderby str
	 * @param limit str
	 * @param groupby str
	 * @param having str
	 * @desc WARNING: Does NOT prepare data for SQL
	 */
	function select_query($select, $from='', $where='', $orderby='', $limit='', $groupby='', $having='')
	{
		$result = parent::select_query($select, $from, $where, $orderby, $limit, $groupby, $having);
		if (is_resource($result) && $from!=='' && !substr_count($from,' '))
		{
			$this->select_query_table = $from;
			$this->select_query_table_resource = $result;
		}
		return $result;
	}
	
	function free_result($result=null)
	{
		if (!is_resource($result)) $result =& $this->result;
		if (!is_resource($result)) return false;
		return mssql_free_result($result);
	}
	
	var $long_col_name_map = array();
	function fetch_row(&$row, $result=null, $free_result=false)
	{
		if (!is_resource($result)) $result =& $this->result;
		if (!is_resource($result)) return false;
		
		$row_num = mssql_fetch_row($result);
		if (is_array($row_num)) // GET COLUMN NAMES FOR RESULT
		{
			$row = array();
			foreach ($row_num as $key=>$value)
			{
				$fname = mssql_fetch_field($result, $key); 
				$fname = $fname->name;				
				//$fname = mssql_field_name($result,$key); // NOT SUPPORTED IN LINUX
				if (strlen($fname)===30) // Get real field name
				{
					// WARNING: this approach will fail if multiple columns query-wide have same 1st 30 chars in their names.
					if ($this->select_query_table_resource === $result)
					{
						if (isset($this->long_col_name_map[$this->select_query_table][$fname]))
						{
							$fname = $this->long_col_name_map[$this->select_query_table][$fname];
						}
						else 
						{
							$col_sql = 'SELECT DISTINCT sc.name FROM syscolumns sc JOIN sysobjects so ON so.id=sc.id WHERE so.name=\'';
							$col_sql.= $this->select_query_table."' AND (sc.name LIKE '$fname%')";
							
							if ($this->display_queries) echo $col_sql.'<br />';
							$col_result = mssql_query($col_sql,$this->connection);
							$col_row = mssql_fetch_row($col_result);
							$fname_orig = $fname;
							$fname = $col_row[0];
							$this->long_col_name_map[$this->select_query_table][$fname_orig] = $fname;
						}
					}
					else // don't know table, from had many tables
					{
						$col_sql = "SELECT DISTINCT name FROM syscolumns WHERE (name LIKE '$fname%')";
						if ($this->display_queries) echo $col_sql.'<br />';
						$col_result = mssql_query($col_sql,$this->connection);
						$col_row = mssql_fetch_row($col_result);
						$fname = $col_row[0];
					}
				}
				$row[$fname] = htmlspecialchars(trim($value)); // TODO: this is not a goog place for this, converts to string
			}
		}
		else $row = $row_num;
		if ($free_result) $this->free_result($result);
		return is_array($row);
	}
	
	function check_column_names()
	{
		$this->select_table($table,'so.name AS oname, sc.name','syscolumns sc INNER JOIN sysobjects so ON sc.id=so.id',"so.xtype='U' AND len(sc.name)>29",'sc.name');
		echo '<table border="1" cellpadding="5" cellspacing="0"><tr><th>Table</th><th>Column</th><th>&nbsp;</th></tr>';
		
		$prev_first_30 = '';
		$prev_rest = '';
		foreach ($table as $row)
		{
			$first_30 = substr($row['name'],0,30);
			$rest = substr($row['name'],30);
			echo "<tr><td>$row[oname]</td>";
			echo "<td><font color=\"blue\">$first_30</font>$rest</td><td>";
			
			if ($first_30 === $prev_first_30 && $rest !== $prev_rest) echo '<font color="red">COLUMN NAME WILL CAUSE PROBLEMS</font>';
			else echo 'OK';
			
			echo '</td></tr>';
			$prev_first_30 = $first_30;
			$prev_rest = $rest;
		}
	}
			
	function num_rows()
	{
		return mssql_num_rows($this->result);
	}
	
	/**
	 * @return int
	 * @param arr $table
	 * @param bool $reset_table
	 * @desc Fetch Result Rows into $table, return count
	 */
	function fetch_table(&$table, $reset_table=true)
	{
		if ($reset_table) $table = array();
		while ($this->fetch_row($row)) $table[] = $row;
		$this->free_result();
		return count($table);
	}
	
	function P_prepare_item($item)
	{
		return str_replace("'","''",trim($item));
	}
	
	function PRV_retry_item(&$item)
	{
		$item = htmlspecialchars(str_replace("''","'",$item));
	}
	
	function get_fields(&$fields)
	{
		$fields = array();
		$num_fields = mssql_num_fields($this->result);
		for ($field_num = 0; $field_num < $num_fields; $field_num++ )
		{
			$fields[] = mssql_field_name($this->result, $field_num);
		}
	}
	
	function table_exists($table_name) // TODO: pass in 'dbt_ms_corp' to look for table.php files with col_names
	{
		return (bool) $this->select_row($row,'id','sysobjects',"type='U' AND name='$table_name'");
	}
	
	function column_exists($table_name, $col_name)
	{
		return (bool) $this->select_row($row,'sc.colid','sysobjects so INNER JOIN syscolumns sc ON so.id=sc.id',"so.type='U' AND so.name='$table_name' AND sc.name='$col_name'");
	}
}