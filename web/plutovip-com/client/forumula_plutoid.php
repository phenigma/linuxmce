<?
	function PRV_plutoid_calculate()
	{
		$int = $this->GET_PK_MasterUsers();
		$str = $this->PRV_plutoid_base35_char($mod = $int%35);
		while ($int = ($int-$mod)/35)
		{
			$str .= $this->PRV_plutoid_base35_char($mod = $int%35);
		}
		return strrev(str_pad($str,6,'0'));
	}

	function PRV_plutoid_base35_char($int)
	{
		if ($int < 10) return $int;
		if ($int < 24) return chr($int+55); // A-N: 65-78
		return chr($int+56); // P-Z: 80-90
	}

?>