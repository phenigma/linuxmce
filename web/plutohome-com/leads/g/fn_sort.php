<?
/**
 * @return int
 * @param $a arr
 * @param $b arr
 * @desc For use by uasort($tablename, compare)
 * SET $sort_key & $sort_rev before using
 * $sort_rev should be a boolean, optional if false
 */
function Compare_Array($a, $b)
{
	global $sort_key;
	global $sort_rev;
	if (isset($a[$sort_key],$b[$sort_key]))
	{
		if ( $a[$sort_key] < $b[$sort_key] ) return $sort_rev ?  1 : -1;
		if ( $a[$sort_key] > $b[$sort_key] ) return $sort_rev ? -1 :  1;
	}
	else 
	{
		if (isset($a[$sort_key])) return $sort_rev ? -1 :  1;
		if (isset($b[$sort_key])) return $sort_rev ?  1 : -1;
	}
	return 0;
}

/**
 * @return int
 * @param $a arr
 * @param $b arr
 * @desc For use by uasort($tablename, compare)
 * SET $sort_keys before using as array of $sort_key => $sort_rev
 * $sort_rev should be a boolean, optional if false
 */
function Compare_Array_Mult($a, $b)
{
	global $sort_keys;
	foreach ($sort_keys as $sort_key => $sort_rev)
	{
		if (isset($a[$sort_key],$b[$sort_key]))
		{
			if (is_string($a[$sort_key])) $a[$sort_key] = strtolower($a[$sort_key]);
			if (is_string($b[$sort_key])) $b[$sort_key] = strtolower($b[$sort_key]);
			
			if ( $a[$sort_key] < $b[$sort_key] ) return $sort_rev ?  1 : -1;
			if ( $a[$sort_key] > $b[$sort_key] ) return $sort_rev ? -1 :  1;
		}
		else 
		{
			if (isset($a[$sort_key])) return $sort_rev ? -1 :  1;
			if (isset($b[$sort_key])) return $sort_rev ?  1 : -1;
		}
	}
	return 0;
}

/**
 * @return int
 * @param $a obj
 * @param $b obj
 * @desc For use by uasort($object_array_name, compareObject)
 * SET $sort_key && $sort_rev before using
 * $sort_rev should be a boolean, optional if false
 */
function Compare_Object(&$a, &$b)
{
	global $sort_key;
	global $sort_rev;
	
	if (is_null($a) && is_null($b)) return 0;
	if (is_null($a)) return $sort_rev ?  1 : -1;
	if (is_null($b)) return $sort_rev ? -1 :  1;
	
	if ( $a->$sort_key < $b->$sort_key ) return $sort_rev ?  1 : -1;
	if ( $a->$sort_key > $b->$sort_key ) return $sort_rev ? -1 :  1;
	return 0;
}