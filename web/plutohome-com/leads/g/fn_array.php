<?
/**
 * @return array
 * @param $table array
 * @param column_name string
 * @desc Get a Column from a Table
 */
function get_column(&$table, $column_name='')
{
	$column = array();
	if ($column_name=='') foreach ($table as $row) foreach ($row as $field) $column[] = $field;
	else foreach ($table as $row) $column[] = $row[$column_name]; 
	return $column;
}

/**
 * @return arr
 * @param $map arr
 * @desc old code, deprecated, find & replace, this is what array_keys() does
 */
function get_ids_from_map(&$map)
{
	return array_keys($map);
}

/**
 * @return arr
 * @param $table arr
 * @desc Given $table[$id_row][$id_column] return $table[$id_column][$id_row]
 */
function array_row_col_flip($table)
{
	$new_table = array();
	foreach ($table as $id_row=>$row)
	{
		foreach ($row as $id_col=>$content)
		{
			$new_table[$id_col][$id_row] = $content;
		}
	}
	return $new_table;
}

/**
 * @return array
 * @param $table array_array
 * @param column_name string
 * @desc Extract a column from a table
 */
function get_column_unique(&$table, $column_name='')
{
	$column = array();
	$keys_arr = array();
	if ($column_name=='') foreach ($table as $row) foreach ($row as $field)
	{
		if (!isset($keys_arr[$field]))
		{
			$keys_arr[$field] = true;
			$column[] = $field;
		}
	}
	else foreach ($table as $row)
	{
		if (!isset($keys_arr[$row[$column_name]]))
		{
			$keys_arr[$row[$column_name]] = true;
			$column[] = $row[$column_name];
		}
	}
	return $column;
}

if (!function_exists('get_key')) // also defined in fn_form
{
	function get_key($array)
	{
		reset($array);
		$each_arr = each($array);
		return $each_arr['key'];
	}
}

function get_fields(&$table)
{
	$key = get_key($table);
	$fields = array();
	if (is_null($key)) return $fields;
	foreach ($table[$key] as $field=>$value)
	{
		$fields[$field] = $field;
	}
	return $fields;
}

/**
 * @return void
 * @param $table1 unknown
 * @param $table2 unknown
 * @desc Will add rows of table2 to table1
 * Will add any non-existing fields in 1 table to the other with empty string values
 */
function combine_tables(&$table1, &$table2)
{
	$fields1 = get_fields($table1);
	$fields2 = get_fields($table2);
	$fields12 = array();
	
	foreach ($fields2 as $field2)
	{
		if (isset($fields1[$field2]))
		{
			$fields12[$field2] = $field2;
			unset($fields1[$field2]);
			unset($fields2[$field2]);
		}
	}
	
	foreach ($table1 as $index1=>$row1)
	{
		foreach ($fields2 as $field2)
		{
			$table1[$index1][$field2] = '';
		}
		ksort($table1[$index1]); // to display properly in a table
	}
	foreach ($table2 as $row2)
	{
		foreach ($fields1 as $field1)
		{
			$row2[$field1] = '';
		}
		ksort($row2); // to display properly in a table
		$table1[] = $row2;
	}
}

/**
 * @return bool
 * @param $array1 arr
 * @param $array2 arr
 * @param $fields arr
 * @param field1 str
 * @param field2 str
 * @desc Combines table arrays on specific fields, ( Like a DB Join )
 * Returns combined array in $array1
 * Combine $field1 on $array1 with $field2 on $array2
 * Both Arrays should be sorted on this field.
 * $fields indicates fields to transfer: $fields['oldfield'] = 'newfield'
 */
function join_arrays(&$array1, &$array2, &$fields, $field1, $field2)
{
	$size1 = count($array1);
	$size2 = count($array2);
	$row1 = $row2 = 0;

	while ($row1 < $size1 && ($array1[$row1][$field1]=='' || $array1[$row1][$field1]=='0'))
	{
		foreach ($fields as $newfield)
		{
			$array1[$row1][$newfield]='';
		}
		$row1++;	
	}
	
	while($row1 < $size1) // combining to $array1, don't care where $array2 is
	{
		for (;$array1[$row1][$field1]!=$array2[$row2][$field2];$row2++)
		{
			if ($row2 >= $size2) return true; 
		}
		foreach ($fields as $oldfield=>$newfield)
		{
			$array1[$row1][$newfield] = $array2[$row2][$oldfield];
		}
		$row1++;
	} 
	return true;
}