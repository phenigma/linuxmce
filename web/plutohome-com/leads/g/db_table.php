<?
// DB ABSTRACTION: Layer 2
require_once('g/db_field.php');
require_once('g/db_row.php');
require_once('g/this.php');

// load_(): From DB Into DB_Table
// get_(): Load & Fetch
// fetch_(): return from DB_Table, may have bool to load too.

// NOTE: PRT functions in db_table.php, db_row.php, & db_field.php may call each other even though there is no inheritance

// NOTE: there is currently no way to get a form field outside of a row context (in a table context: 4 a select row)
// Can create a NEW row object on a table with FK to what is desired & use GETFF or GETFFO accordingly

// TODO: show a select based on the table keys, should FK select fields be based on this ?

// TODO: delete without loading ? still do cascade ?

class DB_Table
{
	// some vars accessed directly from db_row
	var $db;
	var $tablename;
	var $rows = array();    // $rows[$id] = $row, in case of multi-field-id: $id = '1_2_3'
	var $fields = array();  // extending classes will set this: $fields[$fname] =& new DB_Field();

	// TODO: Set true for cascade delete (in autogen)
	var $table_pk_refs = array(); // Set by extending classes: $table_pk_refs[$table_name][$field_name] = $cascade_delete (bool)
	var $pk_fields; // keeps track of PKs, is array for multi-field PKs
	var $pk_is_autoinc;
	
	var $select = '*'; // MS-SQL tables will change if they have varchar columns bigger than 255
	
	/**
	 * @return DB_Table
	 * @param $db Database
	 * @param tablename string
	 * @param pk_fields string_or_array
	 * @param pk_is_autoinc bool
	 * @desc Create a DB_Table Object
	 * pk_fields should be set for any table without 'PKID_', can be string or array
	 * Should be extended to set fields for validation & PK refs for FK constraints
	 * Represents what's actually in the database.
	 */
	function DB_Table(&$db, $tablename, $pk_fields='', $pk_is_autoinc=true)
	{
		$this->db =& $db;
		$this->tablename = $tablename;
		$this->pk_is_autoinc = $pk_is_autoinc;
		if (is_array($pk_fields)) $this->pk_fields = $pk_fields;
		elseif (is_string($pk_fields) && $pk_fields!=='') $this->pk_fields = $pk_fields;
		else $this->pk_fields = "PKID_$tablename";
	}
	
	/**
	 * @desc PROTECTED
	 */
	function PRT_add_pk_ref($tablename,$fieldname,$cascade_delete = false)
	{
		$this->table_pk_refs[$tablename][$fieldname] = $cascade_delete;
	}
	
	/**
	 * @return array
	 * @param load_table bool
	 * @desc PUBLIC: Get ids of all rows in table 
	 * Used to create new DBT_*_Row() objects
	 * Will return in order the rows were originally set
	 */
	function P_fetch_ids($load_table = false)
	{
		if ($load_table) $this->P_load();
		return array_keys($this->rows);
	}
	
	/**
	 * @return arr
	 * @desc PUBLIC
	 */
	function P_get_fieldnames()
	{
		return array_keys($this->fields);
	}

	/**
	 * @return arr
	 * @param id_keys arr
	 * @desc PUBLIC: Get Array of rows matching $id_keys or false=current, true=all
	 */
	function P_fetch_rows($id_keys = false)
	{
		if ($id_keys===true) $this->P_load();
		if (!is_array($id_keys)) return $this->rows;
		$rows = array();
		foreach ($id_keys as $id_key)
		{
			$rows[$id_key] = $this->P_get_row($id_key); // TODO: =& ???
		}
		return $rows;
	}
	
	/**
	 * @desc PUBLIC: flush, empty rows
	 */
	function P_flush()
	{
		$this->rows = array();
	}
	
	/**
	 * @return arr
	 * @param col_name str
	 * @param id_keys arr
	 * @desc PROTECTED: fetch a column from the table, rows matching $id_keys or false=current, true=all
	 */
	function PRT_fetch_col($col_name, $id_keys = false)
	{
		$col = array();
		if ($id_keys===true) $this->P_load();
		if (!is_array($id_keys)) $id_keys = array_keys($this->rows);
		foreach ($id_keys as $id_key)
		{
			$row =& $this->P_get_row($id_key);
			$col[$id_key] = $row->values[$col_name];
		}
		return $col;
	}
	
	/**
	 * @return void
	 * @param col_name str
	 * @param desc_col str
	 * @desc PROTECTED: set the description column for the select box of $col_name 
	 * TODO: call from autogen files
	 */
	function PRT_set_desc_field($col_name, $desc_col)
	{
		$this->fields[$col_name]->set_description_field($desc_col);
	}
	
	/**
	 * @return DB_Row
	 * @param id mixed
	 * @desc Get a DB Row
	 * $id can be int, string, or array of pk_field => value
	 * returns false if matching row not found, null if there is an error
	 */
	function & P_get_row($id)
	{
		$id_key = $this->P_get_id_key($id);
		$row_name = "DBT_{$this->tablename}_Row";
		if (!isset($this->rows[$id_key]) || !is_a($this->rows[$id_key],$row_name))
		{
			if (!is_array($this->pk_fields))
			{
				if (is_array($id)) return null; // ERROR: unexpected array
				$this->db->select_row_by_id($row,$this->select,$this->tablename, $id, $this->pk_fields);	
			}
			else 
			{
				if (!is_array($id)) // Expecting Array, but $id could be an $id_key, try to fix
				{
					$id = $this->P_get_id($id);
					if ($id===false) return null; // ERROR: with $id as $id_key
				}
				elseif (array_diff_count(array_keys($id),$this->pk_fields) > 0) return null; // ERROR: Array keys in $id don't match PK
				$this->db->select_row($row,$this->select,$this->tablename,$id);
			}
			if ($row===false) return false; // Row not found
			else $this->rows[$id_key] =& new $row_name($row);
		}
		return $this->rows[$id_key];
	}
	
	function P_row_loaded($id)
	{
		$id_key = $this->P_get_id_key($id);
		$row_name = "DBT_{$this->tablename}_Row";
		return (isset($this->rows[$id_key]) && is_a($this->rows[$id_key],$row_name));
	}
	
	/**
	 * @return arr
	 * @param $where str|arr
	 * @param $orderby str
	 * @param $limit int
	 * @param $start int
	 * @desc Load table from Database 
	 * NOTE: $start not supported by MS-SQL
	 */
	function P_load($where='', $orderby='', $limit = -1, $start = 0)
	{
		$limit = $this->PRV_get_limit($limit,$start);
		$this->db->select_table($table,$this->select,$this->tablename,$where,$orderby,$limit);
		return $this->PRV_load($table);
	}
	
	/**
	 * @desc PROTECTED
	 */
	function PRT_load_by_field($field, $values, $orderby='', $limit = -1, $start = 0)
	{
		$limit = $this->PRV_get_limit($limit,$start);
		$this->db->select_by_field($this->select,$this->tablename,$field,$values,$orderby,$limit);
		$this->db->fetch_table($table);
		return $this->PRV_load($table);
	}
	
	/**
	 * @return bool|null
	 * @param $fieldname str
	 * @desc Is $fieldname Required ?
	 * Calls L4 fn if it exists:  "ISREQ_$fieldname"()
	 */
	function P_ISREQ_($fieldname)
	{
		return this::call_function("ISREQ_$fieldname",array(),null);
	}

	/**
	 * @return void
	 * @param $fieldname str
	 * @param $match_arr arr
	 * @desc Filter options for $fieldname to $match_arr
	 * Calls L4 fn if it exists: "SETFLTR_$fieldname"($match_arr)
	 */
	function P_SETFLTR_($fieldname,$match_arr)
	{
		return this::call_function("SETFLTR_$fieldname", array($match_arr));
	}
	
	/**
	 * @desc PRIVATE
	 */
	function PRV_get_limit($limit, $start)
	{
		if ($start != 0) $limit = "$start,$limit";
		elseif ($limit == -1) $limit = '';
		return $limit;
	}
	
	/**
	 * @desc PRIVATE
	 */
	function PRV_load($table)
	{
		$id_keys = array();
		$row_name = "DBT_{$this->tablename}_Row";
		foreach ($table as $row)
		{
			if (is_array($this->pk_fields)) // TODO: For Maintainability, probably better to call P_get_id_key()
			{
				$id_key = '';
				$underscore = false;
				foreach ($this->pk_fields as $pk_field) 
				{
					if ($underscore) $id_key.='_';
					$id_key.=$row[$pk_field];
					$underscore = true;
				}
			}
			else $id_key = $row[$this->pk_fields];
			$this->rows[$id_key] =& new $row_name($row);
			$id_keys[$id_key] = $id_key;
		}
		return $id_keys;
	}
	
	/**
	 * @return void
	 * @param $row DB_Row
	 * @param id mixed
	 * @desc Set row in table object to $row
	 * id can be int, string, or array of pk_field => value
	 */
	function P_set_row(&$row, $id)
	{
		$id = $this->P_get_id_key($id);
		$this->rows[$id] = $row; // a copy of row, which has just been updated
	}
	
	/**
	 * @return void
	 * @param id mixed
	 * @desc DELETES ROW FROM THIS OBJECT, NOT FROM DB
	 * id can be int, string, or array of pk_field => value
	 */
	function P_del_row($id)
	{
		$id = $this->P_get_id_key($id);
		unset($this->rows[$id]);
	}
	
	/**
	 * @return str
	 * @param id mix
	 * @desc PUBLIC: get id_key for accessing $this->rows
	 * id can be int, string, or array of pk_field => value
	 * NOTE: Will have errors if called with id that does not match $this->pk_fields
	 */
	function P_get_id_key($id)
	{
		if (is_array($id))
		{
			if (count($id)==1) return $id[$this->pk_fields];

			$id_key = '';
			$underscore = false;
			
			foreach ($this->pk_fields as $pk_field)
			{
				if ($underscore) $id_key.='_';
				$id_key.=$id[$pk_field];
				$underscore = true;
			}
			return $id_key;
		}
		else return $id;
	}
	
	/**
	 * @return arr|str
	 * @param id_key str
	 * @desc PUBLIC: get id from key 
	 * NOTE: will only work if $id_key is an '_' seperated string 
	 * returns $id[$pk_field] = $value
	 * returns false on failure
	 */
	function P_get_id($id_key)
	{
		$ids_in_key = explode('_',$id_key);
		if (count($this->pk_fields) !== count($ids_in_key)) return false;
		if (count($ids_in_key)===1) return $id_key;
		
		$id = array();
		foreach ($this->pk_fields as $i=>$pk_field)
		{
			$id[$pk_field] = $ids_in_key[$i];
		}
		return $id;
	}
	
	/**
	 * @return arr
	 * @param $id_keys arr
	 * @desc PUBLIC: get array of array of ids given id keys
	 * returns $ids[$id_key][$pk_field] = $value
	 * or $ids[$id_key] = false (on failure)
	 */
	function P_get_ids($id_keys)
	{
		$ids = array();
		foreach ($id_keys as $id_key)
		{
			$ids[$id_key] = $this->P_get_id($id_key);
		}
		return $ids;
	}
	
	/**
	 * @desc NOTE: Selective filtering does not work because rows are copies and not references as I wish they were. can't do $this =& $something :(
	 */
	function PRT_set_filter($field_name, $match_arr) // $selective_id=false
	{
		$this->fields[$field_name]->set_filter($match_arr);
//		COULD WORK WHEN USING IDS, WON'T WORK FOR EXISTING ROWS ( I don't mean loaded, I mean already created with NEW row ), 
//		NEED ROW FILTERING TO WORK 1st
//		if ($selective_id===false) $this->fields[$field_name]->set_filter($match_arr);
//		else
//		{
//			foreach (array_keys($this->rows) as $row_key)
//			{
//				$row =& $this->rows[$row_key];
//				if ($row->PRT_get_field($field_name)===$selective_id) $row->PRT_set_filter($field_name, $match_arr, false);
//			}
//		}
	}
}
