<?
require_once('g/globals.php');

class Calendar_Entry
{
	var $id;
	var $idname;
	var $title;
	var $linkto;

	var $starttime; // optional in Task
	var $endtime;   // optional in Task

	var $description; // optional
	
	function Calendar_Entry($id, $idname, $title, $linkto, $starttime, $endtime, $description='')
	{
		$this->id = $id;
		$this->idname = $idname;
		$this->title = $title;
		$this->linkto = $linkto;
		$this->starttime = $starttime;
		$this->endtime = $endtime;
		$this->description = $description;
	}
	
	function DisplayLinkSummary($day=NULL)
	{
		echo '<br />ERROR: '.get_class($this).' extends Calendar_Entry but does not implement DisplayLinkSummary()';
	}
	
	/**
	 * @return int
	 * @param $a Calendar_Entry
	 * @param $b Calendar_Entry
	 * @desc STATIC: For use by uasort($array_of_Calendar_Entries, array('Calendar_Entry','S_Compare'))
	 */
	function S_Compare(&$a, &$b)
	{
		$sort_key_a = $a->get_compare_field();
		$sort_key_b = $b->get_compare_field();
		$sort_rev = false;
		
		if ((is_null($a) || is_null($a->$sort_key_a))&&(is_null($b) || is_null($b->$sort_key_b))) return 0;
		if (is_null($a) || is_null($a->$sort_key_a)) return $sort_rev ?  1 : -1;
		if (is_null($b) || is_null($b->$sort_key_b)) return $sort_rev ? -1 :  1;
		
		if ( $a->$sort_key_a < $b->$sort_key_b ) return $sort_rev ?  1 : -1;
		if ( $a->$sort_key_a > $b->$sort_key_b ) return $sort_rev ? -1 :  1;
		return 0;
	}
	
	function get_compare_field()
	{
		return 'starttime';
	}
}

class Task extends Calendar_Entry
{
	var $priority;
	
	var $duetime; // optional
	
	function Task($id, $idname, $title, $linkto, $priority, $description='', $duetime=null, $starttime=null, $endtime=null)
	{
		parent::Calendar_Entry($id, $idname, $title, $linkto, $starttime, $endtime, $description);
		$this->priority = $priority;
		$this->duetime = $duetime;	
	}
	
	function get_compare_field()
	{
		return 'duetime';
	}
	
	function Display()
	{
	}
	function DisplayLink($day=NULL)
	{
		if (!is_null($this->duetime))
		{
			echo 'DUE: '.date('g:i A',$this->duetime).'<br />';
		}
		echo $this->title.'<br />';
	}

	/**
	 * @desc NOTE: can't get multi-line title in Mozilla, tried \r\n, &#13;&#10;
	 */
	function DisplayLinkSummary($day=NULL)
	{
		?>
		<br /><a href="<?="$this->linkto?$this->idname=$this->id"?>" title="<?=$this->description?>"><font size="-1"><?=$this->title?></font></a>
		<?
		if (!isset($this->endtime))
		{
			?> <a href="<?="$this->linkto?$this->idname=$this->id&action=complete"?>" title="Click here if this is done" ><font size="-2">(it's done)</font></a><?
		}
	}
}

class Event extends Calendar_Entry
{
	function Event($id, $idname, $title, $linkto, $starttime, $endtime, $description='')
	{
		parent::Calendar_Entry($id, $idname, $title, $linkto, $starttime, $endtime, $description);
	}
	
	/**
	 * @desc TODO
	 */
	function Display()
	{
	}
	
	function DisplayLink($day=NULL)
	{
		if ($this->linkto=='') $this->linkto = $_SERVER['PHP_SELF'];
		$start = (is_null($day) || date('z Y',$day) == date('z Y',$this->starttime)) ? date('g:i A',$this->starttime) : date('n/j',$this->starttime);
		$end   = (is_null($day) || date('z Y',$day) == date('z Y',$this->endtime))   ? date('g:i A',$this->endtime)   : date('n/j',$this->endtime);
		?>
		<font face="arial" size="-2"><b><?="$start - $end"?></b></font><br />
		<font size="-1"><a href="<?="$this->linkto?$this->idname=$this->id"?>" title="<?=$this->description?>"><?=$this->title?></a></font><br />
		<?
	}
	
	function DisplayLinkSummary($day=NULL)
	{
		if ($this->linkto=='') $this->linkto = $_SERVER['PHP_SELF'];
		$start = (is_null($day) || date('z Y',$day) == date('z Y',$this->starttime)) ? date('g:ia',$this->starttime) : date('n/j',$this->starttime);
		$end   = (is_null($day) || date('z Y',$day) == date('z Y',$this->endtime))   ? date('g:ia',$this->endtime)   : date('n/j',$this->endtime);
		?>
		<br />
		<a href="<?="$this->linkto?$this->idname=$this->id"?>" title="<?=$this->description?>">
			<font size="-1"><?="$start - $end - "?></font><?=$this->title?>
		</a>
		<?
	}
}

class Calendar
{
	var $entries = array();
	var $starttime, $endtime;
	var $display;  // array of display options
	
	/**
	 * @return Calendar
	 * @param starttime timestamp
	 * @param endtime timestamp
	 * @desc Create a Calendar Object
	 */
	function Calendar($starttime, $endtime)
	{
		if ($starttime > $endtime) swap($starttime, $endtime);
		$this->starttime = $starttime;
		$this->endtime = $endtime;
	}
	
	/**
	 * @return bool
	 * @param $entries Calendar_Entry_or_Array
	 * @desc Add events & tasks only if in time range of calendar
	 */
	function Add_Entries(&$entries)
	{
		if (is_array($entries))
		{
			$success = false;
			foreach ($entries as $key=>$entry)
			{
				if ($this->Add_Entry($entries[$key],false)) $success = true;
			}
			if ($success) $this->Sort_Entries();
			return $success; // @ least 1 event added
		}
		return $this->Add_Entry($entries, true);
	}
	
	/**
	 * @return bool
	 * @param $entry Calendar_Entry
	 * @param sort bool
	 * @desc Add event or task only if in time range of calendar
	 */
	function Add_Entry(&$entry, $sort=true)
	{
		if (is_a($entry,'Event') && in_time_range($this->starttime, $this->endtime, $entry->starttime, $entry->endtime))
		{
			$this->entries[] = &$entry;
			if ($sort) $this->Sort_Entries();
			return true;
		}
		elseif (is_a($entry,'Task'))// && in_time_range($this->starttime,$this->endtime,$entry->duetime))
		{
			$this->entries[] = &$entry;
			if ($sort) $this->Sort_Entries();
			return true;
		}
		return false;
	}

	/**
	 * @return void
	 * @desc Sort Calendar Entries using Calendar_Entry::S_Compare()
	 */
	function Sort_Entries()
	{
		uasort($this->entries, array('Calendar_Entry','S_Compare'));
	}
}

class Month extends Calendar
{
	function Month($month = -1, $year = -1)
	{
		if ($month == -1 || $year == -1)
		{
			$date = getdate();
			$month = $date['mon'];
			$year = $date['year'];
		}
		
		$starttime = mktime(0,0,0, $month, 1, $year);
		$endtime   = mktime(0,0,0, $month+1, 1, $year)-1;
		
		parent::Calendar($starttime, $endtime);
	}
	
	function get_description()
	{
		$date = getdate($this->starttime);
		return "$date[month] $date[year]";
	}
	
	/**
	 * @return Month
	 */
	function & previous()
	{
		$date = getdate($this->starttime);
		if (--$date['mon'] == 0)
		{
			$date['mon'] = 12;
			$date['year']--;
		}
		return new Month($date['mon'],$date['year']);
	}
	
	/**
	 * @return Month
	 */
	function & next()
	{
		$date = getdate($this->starttime);
		if (++$date['mon'] == 13)
		{
			$date['mon'] = 1;
			$date['year']++;
		}
		return new Month($date['mon'],$date['year']);
	}
	
	/**
	 * @desc displays month
	 */
	function display()
	{
		$date = getdate($this->starttime);
		
		?><table width="100%" align="center" border="1" cellpadding="7" cellspacing="0"><?
		
		// End of next Saturday (start of next sunday - 1)
		$endweek = mktime(0,0,0,$date['mon'],$date['mday']+7-$date['wday'],$date['year'])-1;
		$week =& new Week($this->starttime, $endweek);
		$week->Add_Entries($this->entries);
		$week->displayWeekdays();
		$week->displayRow();
		for ($startweek = $endweek + 1; $startweek <= $this->endtime; $startweek = $endweek + 1)
		{
			$date = getdate($startweek);
			$endweek = mktime(0,0,0,$date['mon'],$date['mday']+7,$date['year'])-1;
			if ($endweek > $this->endtime) $endweek = $this->endtime;
			
			$week =& new Week($startweek, $endweek);
			$week->Add_Entries($this->entries);
			$week->displayRow();
		}
		?></table><?
	}
	
	function display_small()
	{
		$date = getdate($this->starttime);
		?>
		<table width="125" border="0" cellpadding="0" cellspacing="0">
			<tr><th colspan="7"><a href="<?=$_SERVER['PHP_SELF']."?month=$date[mon]&year=$date[year]"?>"><?="$date[month] $date[year]"?></a></th></tr>
			<?
			// End of next Saturday (start of next sunday - 1)
			$endweek = mktime(0,0,0,$date['mon'],$date['mday']+7-$date['wday'],$date['year'])-1;
			$week =& new Week($this->starttime, $endweek);
			$week->displayWeekdays_small();
			$week->displayRow_small();
			for ($startweek = $endweek + 1; $startweek <= $this->endtime; $startweek = $endweek + 1)
			{
				$date = getdate($startweek);
				$endweek = mktime(0,0,0,$date['mon'],$date['mday']+7,$date['year'])-1;
				if ($endweek > $this->endtime) $endweek = $this->endtime;
				
				$week =& new Week($startweek, $endweek);
				$week->displayRow_small();
			}
		?>
		</table>
		<?
	}
}

class Week extends Calendar
{
	var $startweek;
	
	/**
	 * @desc Partial weeks may be created too. (To gray out rest of week)
	 */
	function Week($starttime, $endtime)
	{
		$date = getdate($starttime);
		// Beginning of Preceding Sunday
		$startweek = mktime(0,0,0, $date['mon'], $date['mday']-$date['wday'], $date['year']);
		$sdate = getdate($startweek);
		// End of next Saturday
		$endweek = mktime(0,0,0,$sdate['mon'],$sdate['mday']+7-$sdate['wday'],$sdate['year'])-1;
		if ($endtime > $endweek) $endtime = $endweek;
		
		$this->startweek = $startweek;
		parent::Calendar($starttime, $endtime);
	}
	
	/**
	 * @desc TODO
	 */
	function display()
	{
	}
	
	function displayWeekdays()
	{
		?><tr><th>Sunday</th><th>Monday</th><th>Tuesday</th><th>Wednesday</th><th>Thursday</th><th>Friday</th><th>Saturday</th><?
	}
	function displayWeekdays_small()
	{
		?>
		<tr>
			<th><font size="-2">S</font></th>
			<th><font size="-2">M</font></th>
			<th><font size="-2">T</font></th>
			<th><font size="-2">W</font></th>
			<th><font size="-2">R</font></th>
			<th><font size="-2">F</font></th>
			<th><font size="-2">S</font></th>
		</tr>
		<?
	}
	
	/**
	 * @desc shades out days not included in incomplete week
	 */
	function displayRow()
	{
		?>
		<tr>
			<?
			for ($time=$this->startweek, $count=0; $count<7; $count++)
			{
				$day =& new Day($time);
				$day->Add_Entries($this->entries);
				if ($this->starttime <= $time && $time <= $this->endtime)
				{
					if ($day->today) $day->displayCell('#FFFFCC');
					elseif ($day->starttime < $day->start_today) $day->displayCell('#EEEEEE');
					else $day->displayCell();
				}
				else $day->displayCell('#BBBBBB'); // Grayed out cell
				
				$date = getdate($time); // add 1 day to time
				$time = mktime(0,0,0,$date['mon'], $date['mday']+1, $date['year']);
			}
			?>
		</tr>
		<?
	}
	
	function displayRow_small()
	{
		?>
		<tr>
			<?
			for ($time=$this->startweek, $count=0; $count<7; $count++)
			{
				$day =& new Day($time);
				if ($this->starttime <= $time && $time <= $this->endtime)
				{
					$day->displayCell_small();
				}
				else 
				{
					?><td align="center" valign="middle" bgcolor="#FFFFFF" width="14.3%">&nbsp;</td><?
				}
				$date = getdate($time); // add 1 day to time
				$time = mktime(0,0,0,$date['mon'], $date['mday']+1, $date['year']);
			}
			?>
		</tr>
		<?
	}
}

class Day extends Calendar
{
	var $today = false;
	var $start_today = false;
	
	/**
	 * @desc $time is any timestamp within day
	 * Must be exactly 1 day
	 */
	function Day($time)
	{
		$day = getdate($time);
			
		$starttime = mktime(0,0,0, $day['mon'], $day['mday'], $day['year']);
		$endtime = mktime(0,0,0, $day['mon'], $day['mday']+1, $day['year'])-1;
			
		$today = getdate();
		$this->start_today = mktime(0,0,0, $today['mon'], $today['mday'], $today['year']);
		if ($this->start_today == $starttime) $this->today = true;

		parent::Calendar($starttime, $endtime);
	}
	
	/**
	 * @desc TODO: displays day for use on its own
	 */
	function display()
	{
	}
	
	/**
	 * @desc displays day in a cell
	 */
	function displayCell($bgcolor = '#ffffff')
	{
		?>
		<td align="left" bgcolor="<?=$bgcolor?>" valign="top" width="14.3%">
			<font size="-1">
				<?=date('j', $this->starttime)?>
				<a href="event.php?id=0&day=<?=$this->starttime?>" title="New Event">(+)</a>
			</font><br />
			<?
			foreach ($this->entries as $entry)
			{
				$entry->DisplayLink($this->starttime);
			}
			?>
		</td>
		<?
	}
	
	function displayCell_small($bgcolor = '#ffffff')
	{
		?>
		<td align="center" valign="middle" bgcolor="<?=$bgcolor?>" width="14.3%">
			<font size="-2"><?= date('j', $this->starttime) ?></font>
		</td>
		<?
	}
	
	function displaySummary($entry_type = '',$title = '',$past_in_red=false)
	{
		
		if (count($this->entries) > 0)
		{
			if ($title=='') $title = $this->today ? 'Today' : date('l - n / d / Y',$this->starttime);
			$color = ($past_in_red && $this->starttime < $this->start_today) ? 'color="red"' : '';
			?>
			<p>
				<font size="+1" <?=$color?>><b><?=$title?></b></font>
				<?
				foreach ($this->entries as $entry)
				{
					if ($entry_type=='' || is_a($entry,$entry_type)) $entry->DisplayLinkSummary($this->starttime);
				}
				?>
			</p>
			<?
		}
	}
}