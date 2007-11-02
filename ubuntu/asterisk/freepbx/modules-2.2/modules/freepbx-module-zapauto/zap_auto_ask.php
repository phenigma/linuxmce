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

<h2>
	<?php echo _("ZAPTEL auto-configuration"); ?>
</h2>

<p>
	<table cellpadding="15">
	<tr >
		<td valign="top"><b>Warning</b></td>
		
		<td style="border: #FF2020 1px dotted; background-color: #FFEAEA;">
			<ol>
				<li>
				<?php echo _("This action will shut down and restart your PBX, and any existing calls will be lost."); ?>
				</li>
				
				<li>
<?php
echo _("ZAPTEL auto-configuration will assign consecutive extention numbers starting with 
400 to each ZAPTEL extention it detects.
If those numbers are already in use by non ZAPTEL
extension auto-configuration will fail to work properly.");
?>
				</li>

				<li>
<?php
echo _("Depending on your HW configuration, this process might take several minutes
in which the system seems unresponsive."
);
?>
				</li>
			</ol>
		</td>
	</tr>
	</table>
</p>
	
<p>
	<br>

	
	<?php echo _("Click here to run ZAPTEL auto detection and configuration."); ?>
	<form action="" name="form_detect" method="post">
		<input type="hidden" name="detect" value="yes">
		<input name="Submit" type="submit" value="Run ZAPTEL detection">
	</form>

	<br>
	<br>
	<br>
	<br>
</p>
