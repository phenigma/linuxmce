<?
// converts object to another type but maintaining same values in its vars
// Returns BOOL based on success
function convert_to(&$object, $new_type) //, $parameters=NULL)
{
	$array = get_object_vars($object);
	if (class_exists($new_type))
	{
		$object = new $new_type(); // TODO: test =& will probably make it lose ref to object passed in
		foreach ($array as $key=>$value)
		{
			$object->$key = $value;
		}
		return true;
	}
	else return false;
}