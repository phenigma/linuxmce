<?
// NOTE: By default, $empty_string_2_null = true, only relevant for string types, sets '' to null when validating
// Also, empty strings are set to null if field allows null.

// TODO: P_get_value($value) for fields that want to format the $value in a special way before displaying

// TODO: BETTER CHECKBOX SOLUTION !

require_once('g/this.php');
require_once('g/html_tag.php');

class DB_Field
{
	var $allow_null; // accessed from row
	var $field_name;
	var $empty_string_2_null = true;
	var $messages;
	
	/**
	 * @return DB_Field
	 * @param field_name str
	 * @param allow_null bool
	 * @desc DB_Field is ABSTRACT
	 */
	function DB_Field($field_name, $allow_null = false)
	{
		$this->field_name = $field_name;
		$this->allow_null = $allow_null;
		$this->messages =& new Messages();
	}
	
	/**
	 * @return Messages
	 * @desc Get the messages for this field.
	 * TODO: message related Functions identical in DB_Row (re-use code ?)
	 */
	function & P_get_messages()
	{
		return $this->messages;
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
	function PRT_set_notice($notice, $id_notice='')
	{
		$this->messages->setNotice($notice,$id_notice);
	}
	
	/**
	 * @return bool
	 * @param value str
	 * @desc Validate field according to this object's properties.
	 */
	function validate(&$value)
	{
		return $this->validate_null($value);
	}
	
	/**
	 * @return bool
	 * @param value mix
	 * @desc Validate field according to "allow_null" 
	 * Implemented seperately to give inherited objects easy access
	 * empty strings returned as 'null'
	 */
	function validate_null(&$value)
	{
		if ($value==='') // DB rules: strings can be empty and not null
		{
			if ($this->empty_string_2_null || $this->get_php_type()!=='string') $value = null; 
		}
		if (is_null($value) && !$this->allow_null) return $this->PRT_set_error($this->field_name.' is Required'); // ,$this->field_name
		return true;
	}
	
	/**
	 * @return str
	 * @param name str
	 * @param prefix str
	 * @param value str
	 * @desc Get an HTML form element for this field
	 */
	function get_form_field($name,$prefix,$value='')
	{
		$html =& $this->get_form_field_obj($name,$prefix,$value); /* @var $html HTML_Tag */
		return $html->get_html();
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name,$prefix,$value='')
	{
		this::abstract_function('DB_Field','get_form_field_obj');
	}
	
	/**
	 * @return void
	 * @param name str
	 * @param value str
	 * @desc Display an HTML form element for this field
	 */
	function display_form_field($name,$value)
	{
		echo $this->get_form_field($name,$value);
	}
	
	/**
	 * @return str
	 * @param name str
	 * @param prefix str
	 * @desc PROTECTED: Get the name property for a form field
	 */
	function PRT_get_ff_name($name, $prefix)
	{
		return ($prefix==='') ? $name : $prefix."[$name]";
	}
	
	/**
	 * @return str
	 * @desc ABSTRACT: Get the PHP type of this field
	 */
	function get_php_type()
	{
		this::abstract_function('DB_Field','get_php_type');
	}
}

class DB_Field_Varchar extends DB_Field 
{
	var $size;
	var $size_display;
	
	function DB_Field_Varchar($field_name, $allow_null=false, $size='', $size_display='')
	{
		parent::DB_Field($field_name, $allow_null);
		$this->size = $size;
		$this->size_display = $size_display;
	}
	
	function validate(&$value)
	{
		if (parent::validate($value))
		{
			if (is_null($value)) return true;
			if ($this->size==='' || strlen($value) <= $this->size) return true;
			$this->PRT_set_error("$this->field_name must be less than $this->size characters wide"); // ,$this->field_name
		}
		return false;
	}

	/**
	 * @return str
	 */
	function get_form_field($name, $prefix, $value='')
	{
		$html =& $this->get_form_field_obj($name,$prefix,$value); /* @var $html HTML_Tag */
		return $html->get_html();
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name, $prefix, $value='')
	{
		$input =& new HTML_Tag('input',array('type'=>'text'),false);
		$input->set_property('name',$this->PRT_get_ff_name($name,$prefix));
		$input->set_property('value',$value);
		if ($this->size!=='') 
		{
			$input->set_property('maxlength',$this->size);
			if ($this->size_display==='') $input->set_property('size',$this->size);
		}
		if ($this->size_display!=='') $input->set_property('size',$this->size_display);
		return $input;
	}
	
	function get_php_type()
	{
		return 'string';
	}
}

class DB_Field_Pwd extends DB_Field_Varchar 
{
	var $show_value;
	
	function DB_Field_Pwd($field_name, $allow_null=false, $size='', $size_display='', $show_value=false)
	{
		parent::DB_Field_Varchar($field_name,$allow_null,$size,$size_display);
		$this->show_value = $show_value;
	}
	
	/**
	 * @return HTML_Tag
	 * @desc Doesn't show an existing password
	 */
	function & get_form_field_obj($name, $prefix, $value='')
	{
		if (!$this->show_value) $value = '';
		$input =& parent::get_form_field_obj($name,$prefix,$value); /* @var $input HTML_Tag */
		$input->set_property('type','password');
		return $input;
	}
}

class DB_Field_Text extends DB_Field 
{
	var $rows;
	var $cols;
	
	function DB_Field_Text($field_name, $allow_null=false, $rows='', $cols='')
	{
		parent::DB_Field($field_name, $allow_null);
		$this->rows = $rows;
		$this->cols = $cols;
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name, $prefix, $value='')
	{
		$textarea =& new HTML_Tag('textarea');
		$textarea->set_property('name',$this->PRT_get_ff_name($name,$prefix));
		if ($this->rows!=='') $textarea->set_property('rows',$this->rows);
		if ($this->cols!=='') $textarea->set_property('cols',$this->cols);
		$textarea->add_content($value);
		return $textarea;
	}
	
	function get_php_type()
	{
		return 'string';
	}
}

class DB_Field_Bool extends DB_Field 
{
	/**
	 * @return DB_Field_Bool
	 * @param field_name string
	 * @param allow_null bool
	 * @desc Create a DB_Field_Bool object
	 */
	function DB_Field_Bool($field_name, $allow_null=false)
	{
		parent::DB_Field($field_name, $allow_null);
	}
	
	/**
	 * @return bool
	 * @param value string
	 * @desc NOTE: A form checkbox will not be set if false (can detect with is_null)
	 */
	function validate(&$value)
	{
		if (parent::validate($value))
		{
			if (is_null($value)) return true;
			if (is_bool($value) || is_int($value) || $value==='0' || $value==='1') return true;
			$this->PRT_set_error($this->field_name.' must be true or false'); // ,$this->field_name
		}
		return false;
	}
	
	/**
	 * @return str
	 * @param name str
	 * @param prefix str
	 * @param tablename str
	 * @param value str
	 * @desc Get's hidden input to check for existence of unchecked checkbox
	 */
	function get_form_field($name, $prefix, $tablename, $value='')
	{
		$html =& $this->get_form_field_obj($name,$prefix,$tablename,$value); /* @var $html HTML_Tag */
		return $html->get_html();
	}
	
	/**
	 * @return HTML_Tag
	 * @desc NOTE: used to have hidden field named: $tablename."_cb[$name]"
	 * Now Hidden Field has real name and cb sets it when it changes
	 */
	function & get_form_field_obj($name, $prefix, $tablename, $value='')
	{
		$value = $value ? 1 : 0;
		$name = $this->PRT_get_ff_name($name,$prefix);
		$id = str_replace(array('[',']'),'_',$name);
		
		$input =& new HTML_Tag('input',array('type'=>'hidden','id'=>$id,'name'=>$name,'value'=>$value),false);
		$cb =& new HTML_Tag('input',array('type'=>'checkbox','value'=>1),false);
		if ($value) $cb->set_property('checked');
		$cb->set_property('onclick',"document.getElementById('$id').value=this.checked?'1':'0';");

		$input->tie_after($cb);
		return $input;
		
		/* --- ORIGINAL CODE BELOW --- : also see DB_Row::PRT_set_fields();
		$input =& new HTML_Tag('input',array('type'=>'checkbox','value'=>1),false);
		$input->set_property('name',$this->PRT_get_ff_name($name,$prefix));
		if ($value===true || $value==='1') $input->set_property('checked');
		$t_name = $tablename."_cb[$name]"; // keep out of prefix array
		$input->tie_before('<input type="hidden" name="'.$t_name.'" value="1" />');
		return $input;
		*/
	}
	
	function get_php_type()
	{
		return 'bool';
	}
}

class DB_Field_Select extends DB_Field
{
	var $options;
	var $real_type;
	var $default_option;
	
	/**
	 * @param $options arr
	 */
	function DB_Field_Select($field_name, $options, $real_type='DB_Field_Int', $allow_null=false, $default_option=null)
	{
		parent::DB_Field($field_name,$allow_null);
		$this->options = $options;
		$this->real_type = $real_type;
		$this->default_option = $default_option;
	}
	function validate(&$value)
	{
		return isset($this->options[(string)$value]) ? true : $this->PRT_set_error("Invalid $this->field_name");
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name,$prefix,$value='')
	{
		$select =& new HTML_Tag('select');
		$select->set_property('name',$this->PRT_get_ff_name($name,$prefix));
		
		if (!isset($this->options[$value]) && isset($this->default_option)) $value = $this->default_option;
		
		foreach ($this->options as $option_val=>$option_desc)
		{
			$option =& new HTML_Tag('option');
			$option->set_property('value',$option_val);
			if ((string)$option_val===(string)$value) $option->set_property('selected');
			$option->add_content($option_desc);
			$select->add_content($option);
		}
		return $select;
	}

	function get_php_type()
	{
		return eval("return $this->real_type::get_php_type();");
	}

	function get_description($value)
	{
		return isset($this->options[$value]) ? (string)$this->options[$value] : '';
	}
}

class DB_Field_Int extends DB_Field 
{
	var $min;
	var $max;
	var $display_size;
	
	function DB_Field_Int($field_name, $allow_null=false, $min='', $max='', $display_size='')
	{
		parent::DB_Field($field_name, $allow_null);
		
		$this->min = $min;
		$this->max = $max;
		$this->display_size = $display_size;
	}
	
	function validate(&$value)
	{
		if (parent::validate($value))
		{
			if (is_null($value)) return true; // Both MYSQL & MS-SQL WILL ROUND FLOATS
			if (is_numeric($value) && ($this->min==='' || $value >= $this->min) && ($this->max==='' || $value <= $this->max)) return true;

			$error_str = "$this->field_name should be a number";
			if ($this->min !== '' && $this->max !== '') $error_str .= " between $this->min and $this->max";
			elseif ($this->min !== '') $error_str .= " greater than $this->min";
			elseif ($this->max !== '') $error_str .= " less than $this->max";
			
			$this->PRT_set_error($error_str); // ,$this->field_name
		}
		return false;
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name,$prefix,$value='')
	{
		$input =& new HTML_Tag('input',array('type'=>'text'),false);
		$input->set_property('name',$this->PRT_get_ff_name($name,$prefix));
		$input->set_property('value',$value);
		
		$size = $this->size();
		if ($size!=='') $input->set_property('maxlength',$size);
		
		$display_size = $this->display_size();
		if ($display_size!=='') $input->set_property('size',$display_size);
		
		return $input;
	}
	
	function display_size()
	{
		return ($this->display_size === '') ? $this->size() : $this->display_size;
	}
	
	function size()
	{
		return ($this->min==='' || $this->max==='') ? '' : max(strlen($this->min),strlen($this->max));
	}
	
	function get_php_type()
	{
		return 'int';
	}
}

class DB_Field_Float extends DB_Field_Int 
{
	function get_php_type() // Same except for casting !
	{
		return 'float';
	}
}

// TODO: !@#$%^&*  May not be urgent: MS-SQL db (ovo, ov, image), MySQL has more
class DB_Field_DateTime extends DB_Field_Varchar
{	
}

// TODO: Could now extend DB_Field_Select
class DB_Field_FK extends DB_Field
{
	var $db;
	var $pk_table;
	var $pk_field;
	var $desc_field;
	var $filter;
	
	var $map_current = false; // can be the particular row filter it is current to.
	var $last_map;
	var $map_sorted;
	
	var $real_type;
	
	/**
	 * @return DB_Field_FK
	 * @param $field_name str
	 * @param $allow_null bool
	 * @desc FK Field
	 */
	function DB_Field_FK($field_name, $allow_null, $pk_table, $pk_field, $desc_field, &$db, $real_type)
	{
		parent::DB_Field($field_name, $allow_null);
		
		$this->pk_table = $pk_table;  // =& $GLOBALS["DB_{$pk_table}_Table"] // MAYBE NOT A GOOD IDEA
		$this->pk_field = $pk_field;
		$this->desc_field = $desc_field;
		$this->db =& $db;
		
		$this->real_type = $real_type;
	}
	
	function set_description_field($description_field)
	{
		$this->desc_field = $description_field;
		$this->map_current = false;
	}
	
	function set_filter($match_arr)
	{
		if ($this->filter!==$match_arr)
		{
			$this->filter = $match_arr;
			$this->map_current = false;
		}
	}
	
	function get_php_type()
	{
		return eval("return $this->real_type::get_php_type();");
	}

	function get_form_field($name,$prefix,$value='',$sorted=false,$row_filter=false,$show_null=true)
	{
		// TODO: find '->GETFFO_FKID_' & see how it is used.
//		$html =& $this->get_form_field_obj($name,$prefix,$value,$sorted,$row_filter,$show_null); /* @var $html HTML_Tag */
//		return $html->get_html();

		require_once('g/fn_form.php'); // for get_options
		$options = array();
		if ($show_null && $this->allow_null) $options[''] = '';
		foreach ($this->PRV_option_arr($sorted,$row_filter) as $key=>$val) $options[$key] = $val;
		switch (count($options))
		{
			case 0: return '&nbsp;'; // TODO: this presents challenge to get_form_field_obj, search for '->GETFFO_FKID_' to find uses
			case 1: return reset($options).'<input type="hidden" name="'.$this->PRT_get_ff_name($name,$prefix).'" value="'.key($options).'" />';
			default: return '<select name="'.$this->PRT_get_ff_name($name,$prefix).'">'.get_options($options, $value).'</select>';
		}
	}
	
	/**
	 * @return HTML_Tag
	 */
	function & get_form_field_obj($name,$prefix,$value='',$sorted=false,$row_filter=false,$show_null=true)
	{
		$select =& new HTML_Tag('select');
		$select->set_property('name',$this->PRT_get_ff_name($name,$prefix));
				
		if ($show_null && $this->allow_null) $select->add_content(new HTML_Tag('option',array('value'=>'')));
		
		foreach ($this->PRV_option_arr($sorted,$row_filter) as $id=>$description)
		{
			$option =& new HTML_Tag('option',array('value'=>$id),array($description));
			if (((string)$id)===((string)$value)) $option->set_property('selected');
			$select->add_content($option);
		}
		return $select;
	}

	function validate(&$value,$row_filter=false)
	{
		if (parent::validate($value))
		{
			if (is_null($value)) return true;
			if (is_string($value) || is_int($value))
			{
				$map = $this->PRV_option_arr(false,$row_filter);
				if (array_key_exists($value,$map)) return true;
				$this->PRT_set_error("$this->field_name not in $this->pk_table"); // ,$this->field_name
			}
			else $this->PRT_set_error('ERROR: expecting integer or string');
		}
		return false;
	}
	
	/**
	 * @desc $sorted: false = don't care, true = description, or string
	 * TODO: 'other' first
	 * Using PK refs in table to set $map_current to false when rows are added or deleted
	 */
	function PRV_option_arr($sorted=false, $row_filter=false)
	{
		$map_current = is_bool($this->map_current) ? $this->map_current : ($this->map_current===$row_filter);
		$short_desc = 'Description';
		$desc_field = "$this->desc_field AS $short_desc";
		if (!$map_current || ($sorted!==false && $this->map_sorted!==$sorted))
		{
			$sort_str = $sorted ? (($sorted===true) ? $short_desc : $sorted) : '';
			$filter = (is_array($row_filter) || is_string($row_filter)) ? $row_filter : $this->filter;
			$filter = (is_array($filter) || is_string($filter)) ? $filter : '';
			$this->db->select_query("$this->pk_field,$desc_field",$this->pk_table,$filter,$sort_str);
			$this->db->fetch_map($this->last_map,$this->pk_field,$short_desc);
			$this->map_current = (is_array($row_filter) || is_string($row_filter)) ? $row_filter : true;
			$this->map_sorted = $sorted;
			
			foreach ($this->last_map as $key=>$val)
			{
				if ($val==='Other')
				{
					unset($this->last_map[$key]);
					$last_map = $this->last_map;
					$this->last_map = array($key=>$val);
					foreach ($last_map as $key=>$val) $this->last_map[$key] = $val;
					break;
				}
			}
		}
		return $this->last_map;
	}
	
	function get_description($value)
	{
		$options = $this->PRV_option_arr();
		return isset($options[$value]) ? (string)$options[$value] : '';
	}
}
