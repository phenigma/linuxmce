<?
class Random
{
	/**
	 * @return string
	 * @param $length int
	 * @desc Generates a random password (lowercase letters)
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
	
	/**
	 * @param $length int
	 * @desc Generates a confirmation code to use on a link (32 char, 0-9 a-z A-Z)
	 */
	function generate_conf_code($length=32)
	{
		$code = '';
		for ($i=0; $i < $length; $i++)
		{
			$rand = rand(1,62);
			if ($rand < 11)		$rand += 47; // 48-57
			elseif ($rand < 37)	$rand += 54; // 65-90
			else				$rand += 60; // 97-122
			$code .= chr($rand);
		}
		return $code;
	}
}
