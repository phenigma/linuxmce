<?

// Under Construction
// Primarily to easily translate dates & times between different formats

// Formats: PHP timestamp, PHP date array, Mysql DateTime, Mysql timestamp, 

class G_Date
{
	var $year;  // 4 digit
	var $month; // 1-12
	var $day;	// 1-31
	
	function G_Date($year=null,$month=null,$day=null) // Default to Today
	{
	}
}

class G_Time
{
	var $hour;	 // 0-23
	var $minute; // 0-59
	var $second; // 0-59
	
	function G_Time($hour=0, $minute=0, $second=0)
	{
		if (!int_in_range($hour,0,23)) $hour=0;
		if (!int_in_range($minute,0,59)) $minute=0;
		if (!int_in_range($second,0,59)) $second=0;
		
		$this->hour = $hour;
		$this->minute = $minute;
		$this->second = $second;
	}
}

class DateTime
{
	var $g_date;
	var $g_time;
	
	function DateTime()
	{
		$this->g_date =& new G_Date();
		$this->g_time =& new G_Time();
	}
}