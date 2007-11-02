<?php   
/*
 *  Written by Diego Iastrubni <diego.iastrubni@xorcom.com>
 *  Copyright (C) 2005,2006,2007 Xorcom
 * 
 *  All rights reserved.
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  
 *  New page, for autodetection of ZAPTEL configuration.
 *  depends on genzaptelconf, and special SUDO setup.
 *  
 */ 
?>

</div>

<div class="content">
	<?php
		$detect = $_REQUEST['detect'];
		
		// to make this page more readable, the logic has been splited into 2 pages:
		// one asks for confirmation
		// the other does the logic
		if (strtolower($detect) != "yes") {
			include "zap_auto_ask.php";
		}
		else {
			include "zap_auto_detect.php";
		}
	?>
</div>
