<?
/**
 * @return string
 * @param $length int
 * @desc DEPRECATED: use Random::generate_pwd() Generates a random password (lowercase letters)
 */
function generate_pwd($length=6)
{
	$pwd='';
	for ($i=0; $i < $length; $i++)
	{
		$pwd .= chr(rand(97,122));
	}
	return $pwd;
}