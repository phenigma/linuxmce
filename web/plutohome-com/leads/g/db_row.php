<? 
// TODO: Better error handling in db classes
// TODO: Better Ability to continue without regenerating
// TODO: autogen: varchar length in mysql ?

// TODO: BETTER CHECKBOX SOLUTION !

require_once('g/messages.php');
require_once('g/this.php');

class DB_Row_Const
{
	var $dbt_path = '';
	var $db_table;
	var $values = array();
	var $fields = array();  // $fields[$fname] = & DB_Field;
	var $messages; // Errors & notices

	/**
	 * @return DB_Row_Const
	 * @param $dbtable_varname str
	 * @param $row mix
	 * @param $match_arr bool
	 * @desc Creates a DB_Row_Const
	 * TODO: restructure to take &$DB_Table instead of $dbtable_varname, (FIX in autogen L3, autogen all)
	 */
	function DB_Row_Const($dbtable_varname, $row, $match_arr=false) 
	{
		if (!$match_arr && (is_array($row) || is_object($row)))
		{
			$this->PRT_init_props($dbtable_varname);
			$this->PRT_set_fields($row);
		}
		else $this->PRV_set_to($GLOBALS[$dbtable_varname]->P_get_row($row)); // $this will be false if row doesn't exist
//		else $this = $GLOBALS[$dbtable_varname]->P_get_row($row); // OLD WAY: copy not ref
	}
	
	/**
	 * @desc PRIVATE: equivalent of $this =& $that
	 */
	function PRV_set_to(&$that)
	{
		if (is_a($that,'DB_Row')) this::set_to($that); // $that needs empty constructor
		else $this = $that; // $that could also be DB_Row_Const, false, null
	}
	
	/**
	 * @return int
	 * @desc Get the number of messages
	 */
	function P_count_messages()
	{
		return $this->messages->count();
	}
	
	/**
	 * @return int
	 * @desc Get the number of error messages
	 */
	function P_count_errors()
	{
		return $this->messages->count_errors();
	}
	
	/**
	 * @return Messages
	 * @desc Get the messages for this row.
	 * NOTE: calling $m =& $row->P_get_messages(); may change the messages for this Object.
	 * may want to call $m->pass_in($row->P_get_messages());
	 */
	function & P_get_messages()
	{
		return $this->messages;
	}

	function PRT_init_props($dbtable_varname)
	{
		$this->messages =& new Messages();
		$this->db_table =& $GLOBALS[$dbtable_varname];			
		foreach (array_keys($this->db_table->fields) as $fname)
		{
			$this->fields[$fname] =& $this->db_table->fields[$fname];
		}
	}
	
	/**
	 * @return bool
	 * @param $fname str
	 * @param $field_ref DB_Field
	 * @desc Set the DB_Field object for $fname
	 * This affects this particular row only.
	 */
	function P_set_dbfield($fname, &$field_ref)
	{
		$this->fields[$fname] =& $field_ref;
		
		$type = $field_ref->get_php_type();
		if (isset($this->values[$fname]))
		{
			if (!settype($this->values[$fname],$type))
			{
				return $this->PRT_set_error("ERROR converting {$this->values[$fname]} ($fname) to $type");
			}
		}
		if (isset($this->orig[$fname])) // only for DB_Row
		{
			if (!settype($this->orig[$fname],$type))
			{
				return $this->PRT_set_error("ERROR converting {$this->orig[$fname]} ($fname) to $type");
			}
		}
	}

	/**
	 * @return void
	 * @param row obj|arr
	 * @desc PROTECTED
	 */
	function PRT_set_fields($row)
	{
		$array = $this->PRT_get_field_array($row);
		foreach ($array as $field=>$value) $this->PRT_set_field($field, $value);
	}
	
	/**
	 * @return false
	 * @param error str
	 * @param id_error str
	 * @desc PROTECTED: Set Error
	 * Returns false, many functions return false to indicate an error has ocurred.
	 */
	function PRT_set_error($error, $id_error='')
	{
		$this->messages->setError($error,$id_error);
		return false;
	}
	/**
	 * @return bool
	 * @param $messages Messages
	 * @desc Returns true if no errors, false if has errors
	 */
	function PRT_pass_in_messages(&$messages, $new_error='')
	{
		$this->messages->pass_in($messages,$new_error);
		return $this->messages->noErrors();
	}
	function PRT_set_notice($notice, $id_notice='')
	{
		$this->messages->setNotice($notice,$id_notice);
	}
	
	/**
	 * @return bool
	 * @param fieldname str
	 * @param value mix
	 * @desc PROTECTED: Sets field to value, converts to corresponding type, returns success
	 */
	function PRT_set_field($fieldname, $value)
	{
		if (!in_array($fieldname,$this->P_get_fieldnames()))
		{
			return $this->PRT_set_error("ERROR: \"$fieldname\" Does not exist in data class for \"{$this->db_table->tablename}\"");
		}
		
		$type = $this->fields[$fieldname]->get_php_type();
		
		if ($value==='')
		{
			if ($type!=='string' || $this->fields[$fieldname]->allow_null) $value = null;
		}
		if (!is_null($value))
		{
			if (!settype($value,$type))
			{
				return $this->PRT_set_error("ERROR converting $value ($fieldname) to $type");
			}
		}
		$this->values[$fieldname] = $value;		
		return true;
	}
	
	/**
	 * @return arr
	 * @desc PUBLIC: get array of $field=>$value for this row, $value may be str, int, float, or bool
	 */
	function P_field_array()
	{
		return $this->PRT_get_field_array($this);
	}
	
	/**
	 * @return arr
	 * @param $row_object obj|arr
	 * @desc PROTECTED: get array of field->value for $row_object  // TODO: check handling of nulls
	 */
	function PRT_get_field_array(&$row_object)
	{
		$array = array();
		$fields = $this->P_get_fieldnames();
		if (!is_array($fields)) 
		{
			return $this->PRT_set_error('ERROR retreiving fieldnames in DB_Row_Const->PRT_get_field_array()');
		}
		if (is_object($row_object))
		{
			foreach ($fields as $field)
			{
				if (isset($row_object->values[$field])) $array[$field] = $row_object->values[$field];
				else $array[$field] = null;
			}
		}
		elseif (is_array($row_object))
		{
			foreach ($fields as $field)
			{
				if (isset($row_object[$field])) $array[$field] = $row_object[$field];
				elseif (array_key_exists($field,$row_object)) $array[$field] = null;
			}
		}
		else 
		{
			return $this->PRT_set_error('ERROR: $row_object is not an array or object in DB_Row_Const->PRT_get_field_array()');
		}
		return $array;
	}
	
	/**
	 * @return arr
	 * @desc PUBLIC: Get Fieldnames available in this ROW
	 */
	function P_get_fieldnames() 
	{
		return $this->db_table->P_get_fieldnames();
	}
	
	/**
	 * @return str
	 * @desc Get the name of the DB table this row belongs to
	 */
	function P_get_tablename()
	{
		return $this->db_table->tablename;
	}
	
	/**
	 * @return mix
	 * @param $fieldname str
	 * @param $as_string bool
	 * @desc Get Value of $fieldname
	 * Calls L4 fn if it exists:  "GET_$fieldname"($value,$from_form)
	 */
	function P_GET_($fieldname, $as_string=false)
	{
		return this::call_function("GET_$fieldname",array($as_string));
	}
	
	/**
	 * @return mix
	 * @param fieldname string
	 * @param as_string bool
	 * @desc PROTECTED: Get field, return type depends on $as_string
	 */
	function PRT_get_field($fieldname, $as_string=false)
	{
		return $this->PRT_get('values',$fieldname,$as_string);
	}
	
	function PRT_get($from, $fieldname, $as_string)
	{
		$fields =& $this->$from;
		$remove_field = !array_key_exists($fieldname,$fields);
		$value =& $fields[$fieldname]; // This will make the array key exist TODO: look for similar bugs
		
		if (!isset($value))
		{
			if ($remove_field) unset($fields[$fieldname]);
			return ($as_string) ? '' : null; // ( null or simply not set )
		}
		if ($as_string)
		{
			if ($value===false) return '0'; // avoid '' for false
			
			$field =& $this->fields[$fieldname]; 
			if (is_a($field,'DB_Field_FK') || is_a($field,'DB_Field_Select')) return $field->get_description($value); // Get Description from PK Table
			
			return (string) $value; // null->''
		}
		else return $value;
	}
	
	/**
	 * @return arr
	 * @param fieldname str
	 * @desc PROTECTED: Follow $fieldname to get array of IDs
	 * NOTE: this will only work with 1 col PKs
	 */
	function PRT_get_field_ancestors($fieldname)
	{
		$i = 0;
		$ids[$i] = $this->values[$this->db_table->pk_fields];
		$row_class = get_class($this);
		while ($ids[$i] !== null)
		{
			$rows[$i] =& new $row_class($ids[$i]);
			$ids[$i+1] = $rows[$i]->PRT_get_field($fieldname);
			$i++;
		}
		unset($ids[$i]);
		return $ids;
	}
	
	/**
	 * @return arr
	 * @param fieldname str
	 * @desc PROTECTED: Get array of chidren based on $fieldname
	 * NOTE: this will only work with 1 col PKs
	 */
	function PRT_get_field_children($fieldname, $tablename='')
	{
		if (!isset($this->values[$this->db_table->pk_fields])) return array();
		return $this->PRV_get_field_children($fieldname,$this->values[$this->db_table->pk_fields],$tablename);
	}
	
	/**
	 * @desc PRIVATE
	 * NOTE: calls DB_Table->PRT_load_by_field(), should be optimized
	 */
	function PRV_get_field_children($fieldname, $value, $tablename='')
	{
		if ($tablename==='') return $this->db_table->PRT_load_by_field($fieldname,$value);
		require_once($this->dbt_path.strtolower($tablename).'.php');
		return $GLOBALS["DB_{$tablename}_Table"]->PRT_load_by_field($fieldname,$value);
	}
	
	/**
	 * @return arr|false
	 * @param fieldname str
	 * @param sub_arr bool
	 * @desc PROTECTED: Get descendants of this row based on $fieldname
	 * First descendent is self.
	 * if($sub_arr) returns an array of arrays with the top-level keys being the parent (or 'TOP' if no parent)
	 * NOTE: this will only work with 1 col PKs
	 * Returns empty array if PK not set, returns false on error
	 * TODO: optimize to minimize queries
	 */
	function PRT_get_field_descendants($fieldname, $sub_arr = false)
	{
		if (is_array($this->db_table->pk_fields)) return false;
		if (!isset($this->values[$this->db_table->pk_fields])) return array();
		$decs[] = $this->values[$this->db_table->pk_fields];
		if ($sub_arr) 
		{
			if (is_int($this->values[$fieldname])) $decs_sub[$this->values[$fieldname]][] = $decs[0];
			else $decs_sub['TOP'][] = $decs[0]; // No Parent -> 'TOP'
		}
		while (list($key,$dec) = each($decs))
		{
			foreach ($this->PRV_get_field_children($fieldname,$dec) as $new_dec)
			{
				if (!in_array($new_dec,$decs)) 
				{
					$decs[] = $new_dec;
					if ($sub_arr) $decs_sub[$dec][] = $new_dec;
				}
			}
		}
		return ($sub_arr) ? $decs_sub : $decs;
	}
		
	/**
	 * @return arr
	 * @desc PUBLIC: Get array of PK cols=>vals to match a DB row, returns false on error
	 * Matches Original values if DB_Row
	 */
	function P_get_match_arr()
	{
		$match_arr = array();
		if (is_array($this->db_table->pk_fields)) $pk_fields =& $this->db_table->pk_fields;
		else $pk_fields = array($this->db_table->pk_fields);
		
		if (is_a($this,'DB_Row') && $this->orig!==array()) $values =& $this->orig;
		else $values =& $this->values;
		
		foreach ($pk_fields as $pk_field)
		{
			if (!isset($values[$pk_field]) || $values[$pk_field] === '') return false;
			$match_arr[$pk_field] = $values[$pk_field];
		}
		if (count($match_arr)===0) return false;
		return $match_arr;
	}
	
	/**
	 * @return str
	 * @desc Get the id_key for this row
	 */
	function P_get_id_key()
	{
		return $this->db_table->P_get_id_key($this->P_get_match_arr());
	}
}

class DB_Row extends DB_Row_Const
{
	var $orig = array();
	var $filters = array(); // $filters[$field_name] = $match_arr // filters here override those od db_field
	
	/**
	 * @return DB_Row
	 * @param $dbtable_varname str
	 * @param $row mix
	 * @param $match_arr bool
	 * @desc leave $row empty for 'New' Rows
	 * Row that can be edited
	 * TODO: take &$DB_Table instead of $dbtable_varname
	 */
	function DB_Row($dbtable_varname, $row='', $match_arr=false)
	{
		if ($row==='' || is_null($row)) $this->PRT_init_props($dbtable_varname);
		else 
		{
			if ($match_arr || !(is_array($row) || is_object($row)))
			{
				$dbtable =& $GLOBALS[$dbtable_varname]; /* @var $dbtable DB_Table */
				$loaded = $dbtable->P_row_loaded($row); // Determine BEFORE calling parent::DB_Row_Const()
			}
			else $loaded = false;

			parent::DB_Row_Const($dbtable_varname,$row,$match_arr);
			
			if (!$loaded && is_a($this,'DB_Row')) 
			{
				$this->PRV_reset_orig(); // $this could be null or false
			}
		}
	}
	
	function PRV_reset_orig()
	{
		foreach (array_keys($this->fields) as $fieldname)
		{
			$this->orig[$fieldname] = isset($this->values[$fieldname]) ? $this->values[$fieldname] : null;
		}
	}
	
	/**
	 * @return mix
	 * @param fieldname string
	 * @param as_string bool
	 * @desc PROTECTED: Get original value for field, return type depends on $as_string
	 */
	function PRT_get_orig($fieldname, $as_string=false)
	{
		return $this->PRT_get('orig',$fieldname,$as_string);
	}
	
	/**
	 * @return bool
	 * @param $row arr
	 * @param $from_form bool
	 * @param $call_custom_set_fn bool
	 * @param $allowed_fields arr
	 * @desc PROTECTED: Set existing fields to values in row
	 * NOTE: default $from_form different from PUBLIC version
	 */
	function PRT_set_fields($row, $from_form = false, $call_custom_set_fn = false, $allowed_fields = null)
	{
		$error = false;
		$array = $this->PRT_get_field_array($row); // COMMENTED: old way not good enough, there are fields we can't set
		if ($call_custom_set_fn)
		{
			if (isset_array($allowed_fields)) // remove invalid ones
			{
				$allowed_fields = array_flip($allowed_fields);
			}
			foreach ($array as $field=>$value)
			{
				if (isset($allowed_fields[$field]) || !isset_array($allowed_fields))
				{
					if (!$this->P_SET_($field,$value,$from_form)) $error = true;
				}
			}
		}
		else 
		{
			foreach ($array as $field=>$value)
			{
				if (!$this->PRT_set_field($field,$value,$from_form)) $error = true;
			}
		}
		return !$error;
	}
	
	/**
	 * @return bool
	 * @param $row arr
	 * @param $from_form bool
	 * @param $allowed_fields arr
	 * @desc PUBLIC: Set existing fields to values in row, return success of all fields
	 */
	function P_set_fields($row, $from_form = true, $allowed_fields=null)
	{
		return $this->PRT_SET_fields($row,$from_form,true,$allowed_fields);
	}
	
	/**
	 * @return bool
	 * @param $fieldname str
	 * @param $value unk
	 * @param $from_form bool
	 * @desc Set Value of $fieldname
	 * Calls L4 fn if it exists:  "SET_$fieldname"($value,$from_form)
	 */
	function P_SET_($fieldname, $value, $from_form = true)
	{
		return this::call_function("SET_$fieldname", array($value,$from_form));
	}

	/**
	 * @return void
	 * @param $fieldname str
	 * @param match_arr arr
	 * @param field_wide bool
	 * @desc Filter options for $fieldname to $match_arr
	 * Calls L4 fn if it exists: "SETFLTR_$fieldname"($match_arr,$field_wide)
	 */
	function P_SETFLTR_($fieldname,$match_arr,$field_wide=true)
	{
		return this::call_function("SETFLTR_$fieldname", array($match_arr,$field_wide));
	}
	
	/**
	 * @return str|false
	 * @param $fieldname str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for $fieldname
	 * Calls L4 fn if it exists:  "GETFF_$fieldname"($precol,$prefix,$input_name)
	 * NOTE: currently, GETFF_*() may have 2 extra fields not included here
	 */
	function P_GETFF_($fieldname, $precol=false, $prefix=true, $input_name='')
	{
		return this::call_function("GETFF_$fieldname", array($precol,$prefix,$input_name));
	}
	
	/**
	 * @return HTML_Tag
	 * @param $fieldname str
	 * @param $precol bool|str
	 * @param $prefix bool|str
	 * @param $input_name str
	 * @desc Get Form Field for $fieldname
	 * Calls L4 fn if it exists:  "GETFFO_$fieldname"($precol,$prefix,$input_name)
	 * returns false on error
	 * NOTE: currently, GETFFO_*() may have 2 extra fields not included here
	 */
	function & P_GETFFO_($fieldname, $precol=false, $prefix=true, $input_name='')
	{
		return this::call_function("GETFFO_$fieldname", array($precol,$prefix,$input_name));
	}


	/**
	 * @return bool
	 * @param fieldname str
	 * @param value mix
	 * @param from_form bool
	 * @desc PROTECTED: returns success
	 */
	function PRT_set_field($fieldname, $value, $from_form = false)
	{
		if ($from_form) // TODO: empty strings from form should be null if field allows null
		{
			$type = $this->fields[$fieldname]->get_php_type();
			if ($type==='bool' && is_null($value)) $value = false; // Need Before Validate, checkboxes leave value null
			// if ($type==='string' || $type==='text' && $value==='' )
			if (!$this->PRT_validate_field($fieldname,$value)) return false;
			if (is_string($value) && $value!=='') $value = htmlspecialchars(trim($value)); // HTML fmt. like DB select
		}
		return parent::PRT_set_field($fieldname,$value);
	}
	
	/**
	 * @return bool
	 * @desc PUBLIC: Save row to DB, (insert or update)
	 * returns success
	 * NOTE: will NOT update PK columns because of layer 1 implementation
	 */
	function update()
	{
		// if ($this->P_count_errors()) return false; // TRY TO UPDATE WITH ERRORS ?
		
		$db =& $this->db_table->db;
		$table_name = $this->db_table->tablename;
		
		$match_arr = $this->P_get_match_arr();
		$auto_inc = $this->db_table->pk_is_autoinc;
		$insert = ($this->orig===array()); // $insert or update
		
		if (!($insert && $auto_inc) && !is_array($match_arr))
		{
			return $this->PRT_set_error('ERROR: no PK match and not auto-incrementing');
		}

		$row = $this->P_field_array();
	
		// Validate NON-PK fields According to allow_null
		$pk_fields = $this->db_table->pk_fields;
		if (!is_array($pk_fields)) $pk_fields = array($pk_fields);
		foreach ($row as $fieldname=>$value)
		{
			if (!in_array($fieldname,$pk_fields))
			{
				if (!$insert && !array_key_exists($fieldname,$this->orig))
				{
					unset($row[$fieldname]);
					$this->PRT_set_notice("Notice: '$fieldname' has been removed from '$table_name'");
				}
				elseif (!$insert && $value===$this->orig[$fieldname])
				{
					unset($row[$fieldname]); // update only changed fields
				}
				elseif (!$this->fields[$fieldname]->validate_null($value))
				{
					$this->messages->pass_in($this->fields[$fieldname]->P_get_messages());
					return false;
				}
			}
			elseif (is_array($match_arr)) $row[$fieldname] = $match_arr[$fieldname]; // revert any changed PKs
		}
		
		$this->PRV_prepare_data($row); // HTML fmt. -> SQL fmt.
		
		if ($insert)
		{
			foreach ($row as $row_field=>$row_value) // unset null values of non-pk_fields, so insert will take default
			{
				if (!in_array($row_field,$pk_fields) && is_null($row_value)) unset($row[$row_field]);
			}
			
			if ($auto_inc) 
			{
				if (count($pk_fields)!==1) 
				{
					return $this->PRT_set_error('ERROR: can\'t auto-inc a multi-column PK');
				}
				$pk_field = $pk_fields[0];
				$this->values[$pk_field] = $db->insert_query($table_name, $row, $pk_field);
				if ($this->values[$pk_field]===false)
				{
					$this->values[$pk_field] = null;
					return $this->PRT_set_error('ERROR inserting row to DB');
				}
				$match_arr[$pk_field] = $this->values[$pk_field];
			}
			elseif ($db->insert_query($table_name,$row,false)===false)
			{
				return $this->PRT_set_error('ERROR inserting row to DB');
			}
			
			if (count($pk_fields)===1) // Inserted new row, update pk-refs' maps
			{
				$pk_val = $match_arr[$pk_fields[0]];
				foreach ($this->db_table->table_pk_refs as $ref_table_name=>$table_pk_ref) // keep each $ref_field's map current
				{
					foreach (array_keys($table_pk_ref) as $ref_field_name)
					{
						$ref_table_var_name = "DB_{$ref_table_name}_Table";
						if (isset($GLOBALS[$ref_table_var_name]))
						{
							$field =& $GLOBALS[$ref_table_var_name]->fields[$ref_field_name];
							if ($field->map_current !== false && is_array($field->last_map))
							{
								$map_filter = (is_array($field->map_current)) ? $field->map_current : $field->filter;
								$add_to_map = true;
								if (is_array($map_filter))
								{
									foreach ($map_filter as $filter_field=>$filter_values)
									{
										if (!is_array($filter_values)) $filter_values = array($filter_values);
										if (!in_array($this->values[$filter_field],$filter_values)) $add_to_map = false;
									}
								}
								if ($add_to_map) $field->last_map[$pk_val] = $this->values[$field->desc_field];
							}
						}
					}
				}
			}
		}
		elseif ($db->update_query($table_name, $row, array_keys($match_arr))===false) 
		{
			return $this->PRT_set_error('ERROR updating row in DB');
		}

		$this->PRV_reset_orig();
		$this->db_table->P_set_row($this, $match_arr);
		return true;
	}
	
	/**
	 * @return bool
	 * @desc PUBLIC: Delete row, returns success 
	 * Will fail unless all PK fields are set
	 * cascade deletes where specified (in custom table)
	 * doesn't delete ANYTHING unless all constraints pass
	 * TODO: allow_cascade has to be set in FK field.  delete($cascade=false) -> set null instead of deleting referenced row.
	 * TODO: cascade set NULL, let referenced field choose to allow cascade delete & set null
	 */
	function delete()
	{
		$db =& $this->db_table->db; /* @var $db Database */
		$table_name = $this->db_table->tablename;
		$match_arr = $this->P_get_match_arr();

		if (!$this->can_delete(true)) return false;
		if (!$this->PRV_cascade_delete()) return false;
		
		$db->delete_match_arr($table_name, $match_arr);
		$this->db_table->P_del_row($match_arr);
		
		foreach (array_keys($this->values) as $fieldname)
		{
			$this->values[$fieldname] = null;
		}
		$this->PRV_reset_orig();
		
		if (count($match_arr)===1)
		{
			$pk_val = reset($match_arr);
			foreach ($this->db_table->table_pk_refs as $ref_table_name=>$table_pk_ref) // keep each $ref_field's map current
			{
				foreach (array_keys($table_pk_ref) as $ref_field_name)
				{
					$ref_table_var_name = "DB_{$ref_table_name}_Table";
					if (isset($GLOBALS[$ref_table_var_name]))
					{
						$field =& $GLOBALS[$ref_table_var_name]->fields[$ref_field_name];
						if ($field->map_current !== false && is_array($field->last_map) && array_key_exists($pk_val,$field->last_map))
						{
							unset($field->last_map[$pk_val]);
						}
					}
				}
			}
		}
		return true;
	}
	
	/**
	 * @desc Check all $pk_refs recursively
	 */
	function can_delete($set_error = false)
	{
		$table_name = $this->db_table->tablename;
		if (!$this->P_get_match_arr()) return $set_error ? $this->PRT_set_error('ERROR: cannot match row to delete from DB') : false;
		
		if (!is_array($this->db_table->pk_fields)) // Can't have refs on a multi-field pk
		{
			foreach ($this->db_table->table_pk_refs as $ref_table_name=>$table_pk_ref)
			{
				foreach ($table_pk_ref as $ref_field_name=>$cascade_delete)
				{
					if ($chld_keys = $this->PRT_get_field_children($ref_field_name,$ref_table_name))
					{
						if ($cascade_delete)
						{
							foreach ($chld_keys as $chld_key)
							{
								$chld_row_class = "DBT_{$ref_table_name}_Row";
								$chld_row =& new $chld_row_class($chld_key);
								
								$req_fn = "ISREQ_$ref_field_name";
								if ($chld_row->db_table->$req_fn())
								{							
									if (!$chld_row->can_delete($set_error))
									{
										if ($set_error) $this->messages->pass_in($chld_row->messages);
										return false;
									}
								}
							}
						}
						else return $set_error ? $this->PRT_set_error("ERROR: delete from $ref_table_name before deleting from $table_name") : false;
					}
				}
			}
		}
		return true;
	}
	
	/**
	 * @desc PRIVATE: can_delete() should be called 1st, by default: sets referring to null if allowed
	 */
	function PRV_cascade_delete()
	{
		if (!is_array($this->db_table->pk_fields)) // Can't have refs on a multi-field pk
		{
			foreach ($this->db_table->table_pk_refs as $ref_table_name=>$table_pk_ref)
			{
				foreach ($table_pk_ref as $ref_field_name=>$cascade_delete)
				{
					if ($chld_keys = $this->PRT_get_field_children($ref_field_name,$ref_table_name))
					{
						if ($cascade_delete)
						{
							foreach ($chld_keys as $chld_key)
							{
								$chld_row_class = "DBT_{$ref_table_name}_Row";
								$chld_row =& new $chld_row_class($chld_key);
								
								$req_fn = "ISREQ_$ref_field_name";
								if ($chld_row->db_table->$req_fn())
								{					
									if (!$chld_row->delete())
									{
										return $this->PRT_pass_in_messages($chld_row->messages);
									}
								}
								else 
								{
									if (!$chld_row->P_SET_($ref_field_name,null))
									{
										return $this->PRT_pass_in_messages($chld_row->messages);
									}
									if (!$chld_row->update())
									{
										return $this->PRT_pass_in_messages($chld_row->messages);
									}
								}
							}
						}
					}
				}
			}
		}
		return true;
	}
	
	/**
	 * @return void
	 * @param $row arr
	 * @desc PRIVATE: Called by update(), Prepare for DB, bool->int 
	 */
	function PRV_prepare_data(&$row)
	{
		foreach (array_keys($row) as $key)
		{
			if (is_string($row[$key]) && $row[$key]!=='') $row[$key] = htmlspecialchars_decode($row[$key]);
			elseif (is_bool($row[$key])) $row[$key] = (int)$row[$key];
		}
		$this->db_table->db->prepare_data($row);
	}

	/**
	 * @return bool
	 * @param fieldname str
	 * @param value mix
	 * @desc PROTECTED: validate a value entered in a form.
	 */
	function PRT_validate_field($fieldname, $value)
	{
		$field =& $this->fields[$fieldname];
		if (is_a($field,'DB_Field_FK') && isset($this->filters[$fieldname]))
		{
			if ($field->validate($value,$this->filters[$fieldname])) return true;
		}
		elseif ($field->validate($value)) return true;

		$this->messages->pass_in($field->P_get_messages());
		return false;
	}
	
	/**
	 * @return str
	 * @param field_name str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @param extra_field bool
	 * @desc PROTECTED: Get HTML for a form field, pwd_field for varchar
	 * Default $precol for a NEW row is 'NEW'
	 */
	function PRT_get_form_field($field_name, $precol=false, $prefix=true, $input_name='', $extra_field=false, $extra_2=false)
	{
		return $this->PRV_get_form_field('get_form_field',$field_name,$precol,$prefix,$input_name,$extra_field,$extra_2);
	}

	/**
	 * @return str
	 * @param field_fn_name str
	 * @param field_name str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @param extra_field bool
	 * @desc PRIVATE: Get HTML for a form field, pwd_field for varchar
	 */
	function & PRT_get_form_field_obj($field_name, $precol=false, $prefix=true, $input_name='', $extra_field=false, $extra_2=false)
	{
		return $this->PRV_get_form_field('get_form_field_obj',$field_name,$precol,$prefix,$input_name,$extra_field,$extra_2);
	}
	
	/**
	 * @return str
	 * @param field_fn_name str
	 * @param field_name str
	 * @param precol bool|str
	 * @param prefix bool|str
	 * @param input_name str
	 * @param extra_field bool
	 * @desc PRIVATE: Get HTML for a form field, pwd_field for varchar
	 */
	function & PRV_get_form_field($field_fn_name, $field_name, $precol=false, $prefix=true, $input_name='', $extra_field=false, $extra_2=false)
	{
		if ($prefix===true) $prefix = $this->db_table->tablename;
		if ($prefix===false) $prefix = '';
		elseif ($precol!==false)
		{
			if ($precol===true)
			{
				$pk_arr = $this->P_get_match_arr();
				$precol = is_array($pk_arr) ? $this->db_table->P_get_id_key($pk_arr) : 'NEW';
			}
			$prefix.="[$precol]";
		}
		
		if ($input_name==='') $input_name = $field_name;
		
		$field =& $this->fields[$field_name];
		
		if (is_a($field,'DB_Field_Bool'))
		{
			return $field->$field_fn_name($input_name,$prefix,$this->db_table->tablename,$this->PRT_get_field($field_name,true));
		}
		if (is_a($field,'DB_Field_FK'))
		{
			$row_filter = isset($this->filters[$field_name]) ? $this->filters[$field_name] : false;
			return $field->$field_fn_name($input_name,$prefix,$this->PRT_get_field($field_name),$extra_field,$row_filter,$extra_2);
		}
		if (is_a($field,'DB_Field_Select'))
		{
			return $field->$field_fn_name($input_name,$prefix,$this->PRT_get_field($field_name));			
		}
		return $field->$field_fn_name($input_name,$prefix,$this->PRT_get_field($field_name,true));
	}
	
	function PRT_set_filter($field_name, $match_arr, $field_wide=true)
	{
		if ($field_wide) $this->fields[$field_name]->set_filter($match_arr);
		else $this->filters[$field_name] = $match_arr;
	}
}
