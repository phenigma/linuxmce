<?
if (!function_exists('odbc_fetch_array'))
{
	function odbc_fetch_array($result, $rownumber=-1)
	{
		if ($rownumber < 0) @odbc_fetch_into($result, &$rs);
		else odbc_fetch_into($result, &$rs, $rownumber);
		if (count($rs) > 0)
		{
			foreach ($rs as $key => $value)
				$rs_assoc[odbc_field_name($result, $key+1)] = $value;
		}
		else $rs_assoc = false;
		return $rs_assoc;
	}
}

function odbc_fetch_table($db_result, &$table)
{
	$table = array();
	for ($count=0; is_array($table[] = odbc_fetch_array($db_result)); $count++);
	unset($table[$count]);
	return $count;	
}

class ODBC_Database extends Database
{
//	var $connection_id = false;
//	var $result = false;
	
	function ODBC_Database($dsn, $uid='', $pwd='')
	{
		if (is_string($dsn)) $this->connection = odbc_pconnect($dsn, $uid, $pwd);
		elseif (is_array($dsn))
			$this->connection = odbc_pconnect($dsn['dsn'], $dsn['uid'], $dsn['pwd']);
		return $this->connection;
	}

	function query($sql)
	{
		if ($this->connection) $this->result = odbc_exec($this->connection, $sql);
		return $this->result;
	}
	
	function close()
	{
		@odbc_close($this->connection);
	}

	function free_result()
	{
		@odbc_free_result($this->result);
	}
	
	function fetch_table(&$table)
	{
		if ($this->result) return odbc_fetch_table($this->result, $table);
		else return false;
	}	
	
	function fetch_table_id(&$table, $id_field='id')
	{
		$table = array();
		while ($this->fetch_row($row))
		{
			$table[$row[$id_field]] = $row;
		}
		return count($table);
	}
	
	function fetch_table_id_arr(&$table, $id_field='id')
	{
		$table = array();
		while ($this->fetch_row($row))
		{
			$table[$row[$id_field]][] = $row;
		}
		return count($table);
	}
	
	function fetch_table_id_sub_arr(&$table, $id_field_arr=array())
	{
		$table = array();
		while ($this->fetch_row($row))
		{
			$first = true;
			foreach ($id_field_arr as $id_field)
			{
				if ($first) $set_in = &$table[$row[$id_field]];
				else $set_in = &$set_in[$row[$id_field]];
				$first = false;
			}
			$set_in = $row;
		}
		return count($table);
	}
	
	function fetch_row(&$row)
	{
		if ($this->result)
		{
			$row = odbc_fetch_array($this->result);
			if (!is_array($row))
			{
				$row = false;
				return false;
			}
			else return true;
		}
		else
		{
			$row = false;
			return false;
		}
	}
	
	function get_fields(&$fields)
	{
		$fields = array();
		$num_fields = odbc_num_fields($this->result);
		for ($field_num = 1; $field_num <= $num_fields; $field_num++ )
		{
			$fields[] = odbc_field_name($this->result, $field_num);
		}		
	}
}