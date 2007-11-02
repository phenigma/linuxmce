<?php   
/*
 *  Written by Diego Iastrubni <diego.iastrubni@xorcom.com>
 *  Copyright (C) 2005,2006 Xorcom
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

<?php
// small detection, to prevent malicious users from running this file 
// directly and messing up the system
$detect = $_REQUEST['detect'];
if (strtolower($detect) != "yes") {
	return;
}
?>


<h2>Performing HW detection and configuration</h2>
<style>
	.clsWorkingCode{ border: #0000FF 1px solid; background-color: #41AAFF; }
	.clsFailedCode { border: #BB0A0A 1px solid; background-color: #ffc0c0; }
	.clsDoneCode   { border: green 1px solid; background-color: #a0FFa0; }
	.clsHidden     { display: none; border: 0px solid; }
</style>

<span id="idWaitMessage" class="clsWorkingCode">
	<?php echo _("Please wait, this might take up to several minutes."); ?>
</span>

<code>
	<pre id="idScriptOutput" class="clsWorkingCode">
<?php
	// the autodetection is done by a set of bash/perl/php scripts
	// we just call them using sudo, as they need root permissions
	system( "sudo /usr/share/freepbx-common/detect_zap", $detect_zap_value );
	if ($detect_zap_value == 0)
		$new_class = "clsDoneCode";
	else
		$new_class = "clsFailedCode";
?>
	</pre>
</code>

<script>
	document.getElementById("idWaitMessage").className = "clsHidden";
	setTimeout( "change_output_color()", 1 );

function change_output_color()
{
	document.getElementById("idScriptOutput").className = "<?php echo $new_class; ?>";
	// script ended with value: <?php echo "$detect_zap_value\n"; ?>
}
</script>
