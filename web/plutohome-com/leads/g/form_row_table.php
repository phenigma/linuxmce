<?
require_once('g/form.php');
require_once('g/messages.php');

class Form_Row_Table extends Form 
{
	var $title;
	var $db_tablename;
	
	var $id_field;
	var $desc_field;
	
	var $can_add;
	var $can_delete;
	
	var $fields = array();
	var $fields_static = array(); // not in $fields
	var $fields_unique = array(); // also in $fields, $fields_unique[$field_name][$processed_value] = true
	
	var $fields_long_descriptions = array();
	
	var $db_rows = array(); // TODO: is just keys better ?
	var $on_delete_warnings = array(); // Array of $id=>string
	var $form_field_properties = array(); // Array of $fieldname[$id][$propname]=>$value // $id can be 'ALL'
	
	var $new_row_id;
	
	/**
	 * @return Form_Row_Table
	 * @param $fields arr
	 * @param $db_tablename str
	 * @param $id_field str
	 * @param $desc_field str
	 * @param $title str
	 * @param $can_add bool
	 * @param $can_delete bool
	 * @desc A Form where each row represents a DB row
	 * $fields[$fieldname]=$description
	 * or $fields[$fieldname]['static']=$description
	 * or $fields[$fieldname]['unique']=$description
	 * NOTE: currently requires form to be named 'form1'
	 */
	function Form_Row_Table($fields, $db_tablename, $id_field, $desc_field, $title='', $can_add = true, $can_delete = true)
	{
		foreach ($fields as $field_name=>$field_value) // Find fixed fields
		{
			if (is_array($field_value))
			{
				if (isset($field_value['static']))
				{
					$this->fields_static[$field_name] = $field_value['static'];
				}
				elseif (isset($field_value['unique']))
				{
					$this->fields[$field_name] = $field_value['unique'];
					$this->fields_unique[$field_name] = array();
				}
			}
			else $this->fields[$field_name] = $field_value;
		}
		
		$this->db_tablename = $db_tablename;
		$this->id_field = $id_field;
		$this->desc_field = $desc_field;
		
		$this->title = $title;
		$this->can_add = $can_add;
		$this->can_delete = $can_delete;
	}
	
	/**
	 * @return arr
	 * @param bool $include_new_row
	 * @desc Get the keys of the current rows
	 */
	function get_row_keys($include_new_row = false)
	{
		$keys = array_keys($this->db_rows); 
		if (!$include_new_row && isset($keys['NEW']))
		{
			unset($keys['NEW']);
		}
		return $keys;
	}
	
	/**
	 * @return void
	 * @param field_name str
	 * @param description str
	 * @desc Add an "on hover" description to the colun title
	 * NOTE: " is  escaped for you, but you need to escape everything else as necessary
	 */
	function add_long_description($field_name, $description)
	{
		$this->fields_long_descriptions[$field_name] = str_replace('"','&quot;',$description);
	}
	
	/**
	 * @return void
	 * @param $field_name str
	 * @param $filter arr|str
	 * @param $row_key bool|str
	 * @desc Add a filter to a column or a particular field
	 */
	function set_filter($field_name,$filter,$row_key=false)
	{
		if ($row_key!==false) // filter row with that id
		{
			if (!isset($this->db_rows[$row_key])) exit('ERROR: key not found while setting filter');
			$db_row =& $this->db_rows[$row_key]; /* @var $db_row DB_Row */
			$db_row->P_SETFLTR_($field_name,$filter,false);
		}
		else // filter whole table
		{
			$db_table =& $GLOBALS["DB_{$this->db_tablename}_Table"]; /* @var $db_table DB_Table */
			$db_table->P_SETFLTR_($field_name,$filter);
		}
	}
	
	/**
	 * @return void
	 * @param id int
	 * @param warning str
	 * @desc Add an extra on delete warning to row with $id
	 * NO need to escape $warning in any way before calling
	 */
	function set_on_delete_warning($id, $warning)
	{
		$this->on_delete_warnings[$id] = str_replace(array("'",'"'),array("\'",'&quot;'),$warning);
	}
	
	/**
	 * @return void
	 * @param fieldname str
	 * @param propname str
	 * @param action str
	 * @param id int
	 * @desc Set a property on a particular form field, in a particular row or all rows
	 * $propname="$action" 
	 * NOTE: " is  escaped for you, but you need to escape everything else as necessary
	 */
	function set_form_field_property($fieldname, $propname, $action, $id='ALL')
	{
		$this->form_field_properties[$fieldname][$id][$propname] = str_replace('"','&quot;',$action);
	}
	
	/**
	 * @return void
	 * @param $db_rows arr of DB_Row
	 * @desc Set the table to an Array of DB_Rows ( arr[$id_key] = DBT_*_Row )
	 */
	function set_table(&$db_rows)
	{
		$this->db_rows = $db_rows;
	}
	
	function display_header()
	{
		if (count($this->db_rows))
		{
			?><tr><?
			$this->PRT_display_header_fields();
			if ($this->can_delete) echo '<th>&nbsp;</th>';
			?></tr><?
		}
	}
	
	function PRT_display_delete(&$db_row)
	{
		?><th><?
		if ($id_arr = $db_row->P_get_match_arr())
		{
			$del_name = 'delete';
			foreach ($id_arr as $id) $del_name .= "[$id]";
			
			$delete_warning = '';
			if (isset($this->on_delete_warnings[$db_row->P_get_id_key()]))
			{
				$delete_warning.= $this->on_delete_warnings[$db_row->P_get_id_key()].'\n\n';
			}
			$delete_warning .= 'Are you sure you want to delete '.str_replace("'","\'",$db_row->P_GET_($this->desc_field,true)).' ?';
			$onclick = "if(confirm('$delete_warning')){set_value('1','$del_name');document.form1.submit();}";

			?><input type="hidden" name="<?= $del_name ?>" id="<?= $del_name ?>" value="0" /><?
			?><input type="button" name="d_button" value="Delete" onclick="<?= $onclick ?>" /><?
			
			// TODO: If NO Javascript, display this instead: But enter on TB, will delete & with no warning
/*			?><noscript><?
			echo '<input type="submit" name="$del_name.'" value="Delete" '; 
			echo 'onclick="return confirm(\'Are you sure you want to delete '.str_replace("'","\'",$db_row->P_GET_($this->desc_field,)).' ?\');" />';
			?></noscript><? */
		}
		else 
		{
			?><input type="hidden" name="remove_new" id="remove_new" value="0" /><?
			?><input type="button" name="r_button" value="Remove" onclick="set_value('1','remove_new');document.form1.submit();" /><?
		}
		?>&nbsp;</th><?
	}

	function PRT_display_header_fields()
	{
		$db_table =& $GLOBALS["DB_{$this->db_tablename}_Table"]; /* @var $db_table DB_Table */
		foreach ($this->fields_static as $fdesc) echo "<th>$fdesc</th>";
		foreach ($this->fields as $fname=>$fdesc)
		{
			$title = '';
			if (isset($this->fields_long_descriptions[$fname])) $title .= $this->fields_long_descriptions[$fname];
			if ($db_table->P_ISREQ_($fname))
			{
				if ($title != '') $title .= "\n";
				$title .= 'Required'; // $fdesc .= '*';
			}
			?><th title="<?= $title ?>"><?= $fdesc ?></th><?
		}
	}
	
	/**
	 * @param $db_row DB_Row
	 * @desc $row_count might be used by extending classes
	 */
	function PRT_display_fields(&$db_row, $row_count=null)
	{
		$id_key = $db_row->P_get_id_key();
		if (!$id_key) $id_key = 'NEW';
		foreach (array_keys($this->fields_static) as $fname)
		{
			?><td><?= $db_row->P_GET_($fname,true) ?></td><?
		}
		foreach ($this->fields as $fname=>$fdesc)
		{
			$ff_props = array();
			if (isset($this->form_field_properties[$fname]))
			{
				if (isset($this->form_field_properties[$fname]['ALL']))
				{
					foreach ($this->form_field_properties[$fname]['ALL'] as $prop_name=>$prop_value)
					{
						$ff_props[$prop_name] = $prop_value;
					}
				}
				if (isset($this->form_field_properties[$fname][$id_key]))
				{
					foreach ($this->form_field_properties[$fname][$id_key] as $prop_name=>$prop_value)
					{
						$ff_props[$prop_name] = $prop_value;
					}
				}
			}

			?><td align="center"><?
			$html_tag =& $db_row->P_GETFFO_($fname,true);
			foreach ($ff_props as $prop_name=>$prop_value)
			{
				$html_tag->set_property($prop_name,$prop_value);
			}
			$html_tag->display();
			?></td><?
		}
	}
	
	
	function PRT_display_db_row(&$db_row, $alt_shading=false)
	{
		static $row_count = 0;
		
		if ($alt_shading===false) echo '<tr>';
		else
		{
			if (!is_string($alt_shading)) $alt_shading = '#E7E7E7';
			echo ($row_count++ % 2) ? "<tr bgcolor=\"$alt_shading\">" : '<tr>';
		}
		
		$this->PRT_display_fields($db_row, $row_count);
		if ($this->can_delete) $this->PRT_display_delete($db_row);
		echo '</tr>';
	}
	
	function display_content()
	{
		$this->display_header();
		foreach (array_keys($this->db_rows) as $db_row_key)
		{
			$this->PRT_display_db_row($this->db_rows[$db_row_key],true);
		}
	}
	
	/**
	 * @return int
	 * @desc Get the number of colums in this table
	 * Extending classes should add any additional columns to the value returned by this function
	 */
	function count_display_cols()
	{
		$count = count($this->fields) + count($this->fields_static);
		return ($this->can_delete) ? $count + 1 : $count;
	}
	
	/**
	 * @return void
	 * @param action str|bool
	 * @param submit_text str
	 * @desc Set $action to false to avoid displaying form tags
	 */
	function display($action='', $submit_text='Update')
	{
		$this->PRT_fn_before_display();
		if (is_string($action)) $this->display_tag_top($action);
		?><table align="center" cellpadding="4" cellspacing="3" border="0"><?
		if ($this->can_delete) 
		{
			require_once('g/javascript.php');
			echo javascript::source_tag('/'.APPLICATION_PATH.'/inc/javascript/toggle.js');
		}
		$this->display_content();
		$this->PRT_display_update_row($submit_text);
		?></table><?
		if (is_string($action)) $this->display_tag_bottom();
	}
	
	/**
	 * @return void
	 * @param submit_text str
	 * @desc PROTECTED
	 */
	function PRT_display_update_row($submit_text='Update')
	{
		?><tr><th colspan="<?= $this->count_display_cols() ?>"><input type="submit" name="update" value="<?= $submit_text ?>" /><?
		if ($this->can_add)
		{
			?> &nbsp; <?
			$this->PRT_display_add();
		}
		?></th></tr><?
	}
	
	function PRT_display_add()
	{
		?><input type="submit" name="add" value="Add" /><?
	}
	
	/**
	 * @return Messages
	 * @param $del_post arr
	 * @desc PROTECTED: Delete rows from db according to $delpost
	 */
	function & PRT_delete_from_db($del_post)
	{
		$messages =& new Messages();
		if (!$this->can_delete)
		{
			$messages->setError("Error: can't delete {$this->title}s ");
			return $messages;
		}
		$all_messages =& new Messages();
		$key_map = $this->PRV_id_map();
		foreach ($del_post as $id=>$value)
		{
			if ($value==='1')
			{
				if (isset($key_map[$id]))
				{
					$db_row =& $this->db_rows[$key_map[$id]];
					$messages->pass_in($this->PRT_fn_before_delete($db_row, $id));
					if ($messages->noErrors()) // delete from users
					{
						if ($db_row->delete())
						{
							$messages->pass_in($this->PRT_fn_after_delete($id));
							unset($this->db_rows[$key_map[$id]]);
						}
						else 
						{
							$messages->setError('Error Deleting '.$db_row->P_GET_($this->desc_field,true));
							$messages->pass_in($db_row->P_get_messages());
						}
					}
					if (isset($_POST[$this->db_tablename][$id])) unset($_POST[$this->db_tablename][$id]); // CHANGED: was only unsetting when delete was successful
				}
				else logout(); // Trying to delete row not shown in form (could be from other installation)
				$all_messages->pass_in($messages);
			}
		}
		return $all_messages;
	}
	
	/**
	 * @param $db_row DB_Row
	 */
	function add_row(&$db_row)
	{
		$id = $db_row->P_GET_($this->id_field);
		$this->db_rows[$id] =& $db_row;
		return $id;
	}
	
	/**
	 * @return Messages
	 * @param $db_row DB_Row
	 * @param $id int
	 * @param $post arr
	 * @desc (can delete? other tables: cascade delete)
	 */
	function & PRT_fn_before_update(&$db_row, $id, &$post)
	{
		return new Messages();
	}
	
	/**
	 * @return Messages
	 * @param $db_row DB_Row
	 * @desc if adding, executed after PRT_fn_after_add()
	 */
	function & PRT_fn_after_update(&$db_row)
	{
		return new Messages();
	}

	/**
	 * @return Messages
	 * @param $db_row DB_Row
	 * @param $id int
	 * @desc (can delete? other tables: cascade delete)
	 */
	function & PRT_fn_before_delete(&$db_row, $id)
	{
		return new Messages();
	}
	
	/**
	 * @return Messages
	 */
	function & PRT_fn_after_delete($id)
	{
		return new Messages();
	}

	/**
	 * @return Messages
	 * @desc (default fields)
	 */
	function & PRT_fn_before_add(&$db_row)
	{
		return new Messages();
	}
	
	/**
	 * @return Messages
	 * @param $db_row DB_Row
	 * @desc (other tables)
	 */
	function & PRT_fn_after_add(&$db_row)
	{
		return new Messages();
	}
	
	/**
	 * @return Messages
	 */
	function & PRT_save_to_db($upd_post)
	{
		$messages =& new Messages();
		$all_messages =& new Messages(); // on a seperate line for future PHP 5, compatibility (objects as references)
		$key_map = $this->PRV_id_map();
		foreach ($upd_post as $id=>$post)
		{
			if ($id==='NEW')
			{
				if ($this->can_add)
				{
					if ($_POST['remove_new']==='1') continue;
					
					$row_class_name = "DBT_{$this->db_tablename}_Row";
					$db_row =& new $row_class_name();
					$key_map[$id] = false; // to continue with add
				}
				else
				{
					$all_messages->setError("Error: can't add $this->title");
					continue;
				}
			}
			if (isset($key_map[$id]))
			{
				/* @var $db_row DB_Row */
				if ($id==='NEW') $messages->pass_in($this->PRT_fn_before_add($db_row));
				else $db_row =& $this->db_rows[$key_map[$id]];

				$messages->pass_in($this->PRT_fn_before_update($db_row,$id,$post));

				foreach ($post as $field=>$value)
				{
					if (isset($this->fields_unique[$field]) && $value!=='')
					{
						if (isset_true($this->fields_unique[$field][$value]))
						{
							$messages->setError("Error: $field must be unique, \"$value\" is duplicated");
						}
						else $this->fields_unique[$field][$value] = true;
					}
				}
				
				if ($id==='NEW' || $messages->noErrors())
				{	
					if (!$db_row->P_set_fields($post)) // Setting values for new row even if errors exist
					{
						if ($id==='NEW') $messages->setError('Error with new row');
						else $messages->setError('Error with '.$db_row->P_GET_($this->desc_field,true));
						$messages->pass_in($db_row->P_get_messages());
					}
				}

				if ($messages->noErrors())
				{
					if ($db_row->update()) 
					{
						if ($id==='NEW')
						{
							$this->new_row_id = $this->add_row($db_row);
							$messages->pass_in($this->PRT_fn_after_add($db_row));
						}
						$messages->pass_in($this->PRT_fn_after_update($db_row));
					}
					else 
					{
						$messages->setError('Error updating '.$db_row->P_GET_($this->desc_field,true));
						$messages->pass_in($db_row->P_get_messages());
					}
				}
				if ($id==='NEW' && $messages->hasErrors()) $this->db_rows['NEW'] =& $db_row; // keep current new row
				$messages->pass_in($db_row->P_get_messages()); // For any Notices
			}
			else logout(); // Trying to edit row not shown in form (could be from other installation) // $messages->setError('ERROR: $this->title.' not found');
			$all_messages->pass_in($messages);
		}
		return $all_messages;
	}
	
	/**
	 * @return arr
	 * @param id_field int
	 * @desc PRIVATE: get a map of $id=>$key where $this->db_rows[$key] = db_row
	 */
	function PRV_id_map()
	{
		$map = array();
		foreach (array_keys($this->db_rows) as $key)
		{
			$map[$this->db_rows[$key]->P_GET_($this->id_field)] = $key;
		}
		return $map;
	}
	
	/**
	 * @return Messages
	 * @desc TODO: standardize parameters for these functions:
	 * PRT_fn_before_add()	// (default fields)
	 * PRT_fn_after_add()		// (other tables)
	 * PRT_fn_before_update()	// (add or edit, check & change data)
	 * PRT_fn_after_update()
	 * PRT_fn_before_delete()	// (can delete? other tables: cascade delete)
	 * PRT_fn_after_delete()
	 */
	function & process()
	{
		$messages =& new Messages();
		if (isset_array($_POST['delete']))
		{
			$messages->pass_in($this->PRT_delete_from_db($_POST['delete']));
		}
		if (isset_array($_POST[$this->db_tablename]))
		{
			$messages->pass_in($this->PRT_save_to_db($_POST[$this->db_tablename]));
		}
		return $messages;
	}
	
	/**
	 * @desc PROTECTED: process a new row before displaying it.
	 */
	function PRT_fn_before_display()
	{
		if (isset($_POST['add']) && $this->can_add && !isset($this->db_rows['NEW']))
		{
			$db_row_name = "DBT_{$this->db_tablename}_Row";
			$this->db_rows['NEW'] =& new $db_row_name();
			
			$keys = array_key_by_val(array_keys($this->db_rows));
			unset($keys['NEW']);
			$this->PRT_fn_before_display_new_row($this->db_rows['NEW'],$keys);
		}
	}
	
	/**
	 * @param $db_row DB_Row
	 * @desc PROTECTED: to overwrite
	 */
	function PRT_fn_before_display_new_row(&$db_row, &$keys)
	{
	}
}
