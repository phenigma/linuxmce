<?
require_once('g/database.php');

class Mysql_Database extends Database 
{
	/**
	 * @return Mysql_Database
	 * @param server string
	 * @param user string
	 * @param pwd string
	 * @param db_name string
	 * @desc Connect to a new MySQL Database
	 */
	function Mysql_Database($server, $user, $pwd, $db_name)
	{
		static $db_name_last_selected;
		$this->db_name_last_selected =& $db_name_last_selected;
		
		$this->connection = mysql_pconnect($server, $user, $pwd) or exit('Error Connecting to database: '.mysql_error());
		$this->PRV_select_db($db_name) or exit('Error Selecting Database: '.mysql_error());
		$this->db_name = $db_name;
		$this->server = $server;
	}
	
	function PRV_select_db($db_name)
	{
		$success = mysql_select_db($db_name, $this->connection);
		if ($success) $this->db_name_last_selected = $db_name;
		return $success; // or error: couldn't open DB ?
	}

	function close()
	{
		return mysql_close($this->connection);
	}
	
	/**
	 * @return bool
	 * @param table_name string
	 * @desc NOTE: Code that uses this assumes all auto-inc fields are a single-column PK
	 */
	function autoinc_exists($table_name)
	{
		$exists = false;
		$this->query("SHOW FIELDS FROM $table_name");
		$this->fetch_table($fields_table);
		foreach ($fields_table as $fields_row)
		{
			if ($fields_row['Extra']=='auto_increment') $exists = true;
		}
		return $exists;
	}
	
	/**
	 * @return array_of_object
	 * @param table_name string
	 * @desc Get field information for $table_name
	 */
	function get_fields($table_name)
	{
		$result = mysql_list_fields($this->db_name, $table_name, $this->connection);
		$fields = array();
		while ($field = mysql_fetch_field($result))
		{
			$fields[$field->name] = $field;
		}
		return $fields;
	}
	
	/**
	 * @return arr
	 * @param table_name str
	 * @desc Get field information for $table_name, Table where each row has array keys: Field  Type  Null  Key  Default  Extra  
	 */
	function get_fields_info($table_name)
	{
		$this->query("SHOW FIELDS FROM $table_name");
		$this->fetch_table_id($fields_table,'Field');
		return $fields_table;
	}
	
	function error_message()
	{
		return mysql_error($this->connection);
	}
	
	/**
	 * @return str
	 * @param $php_timestamp int
	 * @desc convert $php_timestamp into a string in the Mysql Date format: 'YYYY-MM-DD'
	 * TODO: rename this function into something re-useable by MS-SQL & deprecate current name
	 */
	function mysql_date($php_timestamp = '')
	{
		if ($php_timestamp=='') $php_timestamp = mktime();
		return date('Y-m-d',$php_timestamp);
	}
	
	function mysql_datetime_from_timestamp($mysql_timestamp)
	{
		return substr($mysql_timestamp,0,4).'-'.substr($mysql_timestamp,4,2).'-'.substr($mysql_timestamp,6,2).' '.substr($mysql_timestamp,8,2).':'.substr($mysql_timestamp,10,2).':'.substr($mysql_timestamp,12,2);
	}
	
	/**
	* @return str
	* @param mix $php_date
	* @desc $php_date: timestamp or array -> mysql date-time string
	*/
	function mysql_datetime($php_date = '')
	{
		if (is_array($php_date))
		{
			if ($php_date['Month']==0) return null; // month: 0 = no date;			
			
			if (!isset($php_date['Second'])) $php_date['Second'] = '00';
			
			if (isset($php_date['AM']))
			{
				if ($php_date['AM']==1 && $php_date['Hour']==12) $php_date['Hour'] = 0;
				elseif ($php_date['AM']==0 && $php_date['Hour']<12) $php_date['Hour'] += 12;
			}
			
			return "$php_date[Year]-$php_date[Month]-$php_date[Day] $php_date[Hour]:$php_date[Minute]:$php_date[Second]";
		}
		if ($php_date=='')$php_date = mktime();
		return date('Y-m-d H:i:s',$php_date);
	}
	
	function mysql_timestamp()
	{
		return date('YmdHis');
	}
	
	function php_timestamp($mysql_datetime)
	{
		$d =& $mysql_datetime;
		if (is_numeric($d)) $d = $this->mysql_datetime_from_timestamp($d);
		$d = $this->datetime_explode($d,false);
		return mktime($d['Hour'],$d['Minute'],$d['Second'],$d['Month'],$d['Day'],$d['Year']);
	}

	// mysql date-time string: 2003-04-16 20:15:00 -> array
	function datetime_explode($mysql_date_string='', $hr_12 = true)
	{
		if ($mysql_date_string=='') // null==''
		{
			 $date = array('Year'=>'','Month'=>0,'Day'=>'','Hour'=>'','Minute'=>'','Second'=>'');
			 if ($hr_12) $date['AM'] = -1;
			 return $date;
		}
		$date = array();
		$date['Year'] = substr($mysql_date_string,0,4);
		$date['Month'] = (int)substr($mysql_date_string,5,2); // (int) for compatibility with fn_form -> html select
		$date['Day'] = substr($mysql_date_string,8,2);
		$date['Hour'] = substr($mysql_date_string,11,2);
		$date['Minute'] = substr($mysql_date_string,14,2);
		$date['Second'] = substr($mysql_date_string,17,2);
		if ($hr_12)
		{
			$date['AM'] = (int)($date['Hour'] < 12); // (int) for easy xfer on forms 0 or 1
			if ($date['Hour'] > 12) $date['Hour'] -= 12;
			elseif ($date['Hour'] == 0) $date['Hour'] = 12;
			
			$date['Hour'] = (int)$date['Hour'];
			if ($date['Hour'] < 10) $date['Hour'] = "0$date[Hour]";
		}
		return $date;
	}
	
	/**
	 * @return result
	 * @param sql string
	 * @desc Execute a MySQL Query
	 */
	function query($sql)
	{
		if ($this->db_name !== $this->db_name_last_selected) $this->PRV_select_db($this->db_name);
		$this->last_query = $sql;
		$this->result = mysql_query($sql, $this->connection) or exit("Error with query: $sql<br />".mysql_error());
		if ($this->display_queries) echo "$sql<br />";
		return $this->result;
	}

	function free_result($result=null)
	{
		if (!is_resource($result)) $result =& $this->result;
		if (!is_resource($result)) return false;
		return mysql_free_result($result);
	}

	/**
	 * @return bool
	 * @param $row arr
	 * @desc Fetch a row into $row
	 */
	function fetch_row(&$row, $result=null, $free_result=false)
	{
		if (!is_resource($result)) $result =& $this->result;
		if (!is_resource($result)) return false;
		
		$row = mysql_fetch_assoc($result);
		if ($free_result) $this->free_result($result);
		if (is_array($row)) foreach ($row as $key=>$value) $row[$key] = htmlspecialchars($value);
		return is_array($row);
	}

	function reset_result_pointer()
	{
		if (mysql_num_rows($this->result)) return mysql_data_seek($this->result,0);
		else return false;
	}
	
	/**
	 * @return int
	 * @param $table array
	 * @param $reset_table bool
	 * @desc Fetch table into $table
	 * returns count()
	 */
	function fetch_table(&$table, $reset_table=true)
	{
		if ($reset_table) $table = array();
		while ($row = mysql_fetch_assoc($this->result))
		{
			foreach ($row as $field_key=>$field) $row[$field_key] = htmlspecialchars($field);
			$table[] = $row;
		}
		$this->free_result();
		return count($table);

//		for ($rows=0;$table[] = mysql_fetch_assoc($this->result);$rows++);
//		unset($table[$rows]);
//		foreach ($table as $row_key=>$row) foreach ($row as $field_key=>$field) $table[$row_key][$field_key] = htmlspecialchars($field);
//		return $rows;
	}
	
	function fetch_fields(&$field_arr)
	{
		$field_arr = array();
		for ($i = 0; $i < mysql_num_fields($this->result); $i++)
		{
			$field_arr[] = mysql_field_name($this->result, $i);
		}
		return $i;
	}
	
	/**
	 * @return str
	 * @param str $item
	 */
	function P_prepare_item($item)
	{
		return mysql_escape_string(trim($item));
	}
	
	function PRV_retry_item(&$item)
	{
		$item = htmlspecialchars(stripslashes($item));
	}

	/**
	 * @return mix
	 * @param tablename str
	 * @param $field2val arr
	 * @param auto_field str
	 * @param pwd_field str
	 * @param noquote_fields arr
	 * @desc Page calling this function should require fn_form.php
	 */
	function insert_query($tablename, &$field2val, $auto_field='', $pwd_field='', $noquote_fields=array())
	{
		$result = parent::insert_query($tablename, $field2val, $auto_field, $pwd_field, $noquote_fields);
		if ($result === false) return false;
		$id = mysql_insert_id($this->connection);
		if ($id !== 0) return $id;
//		if ($auto_field!=='') return false; // $auto-field can be false && '' defaults to PKID_$tablename, $id should take care of it
		return $result;
	}
		
	function select_max($table, $fieldname)
	{
		$sql = "SELECT MAX($fieldname) FROM $table";
		$result = $this->query($sql);
		$row = mysql_fetch_row($result);
		return $row[0];
	}
	
	function getSum($table, $fieldname, $where='')
	{
		$sql = "SELECT SUM($fieldname) FROM $table";
		if ($where!='') $sql .= " WHERE $where";
		$result = $this->query($sql);
		$row = mysql_fetch_row($result);
		return $row[0];
	}
	
	/**
	 * @desc DEPRECATED: use select_count()
	 */
	function exists_match_arr($table, &$field2val, $where='', $count='*')
	{
		$sql = "SELECT COUNT($count) FROM $table WHERE ";
		$and = false;
		foreach ($field2val as $field=>$value)
		{
			if ($and) $sql.= ' AND ';
			$sql .= "$field='$value'";
			$and = true;
		}
		if ($where!='') $sql .= " AND $where";
		$result = $this->query($sql);
		$row = mysql_fetch_row($result);
		return $row[0];
	}
		
	/**
	 * @return int
	 * @param table str
	 * @param field str
	 * @param value str
	 * @param idfield str
	 * @param idvalue str
	 * @param where str
	 * @desc check if $table.$field contains a row with $value other than the row where $idfield matches $idvalue, AND optional $where clause is met
	 * Returns number of matching rows
	 * can be used for editting to make sure a field is not duplicated
	 */
	function existsDifferentId($table, $field, $value, $idfield, $idvalue, $where='')
	{
		$sql = "SELECT COUNT(*) FROM $table WHERE $field='$value' AND $idfield!='$idvalue'";
		if ($where!='') $sql .= " AND $where";
		$result = $this->query($sql);
		$row = mysql_fetch_row($result);
		return $row[0];
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
		if ($auto_field==='') $auto_field = "PKID_$tablename"; // avoid ``
		return parent::P_insert_sql("`$tablename`",$field2val,$auto_field,$pwd_field,$noquote_fields);
	}
}