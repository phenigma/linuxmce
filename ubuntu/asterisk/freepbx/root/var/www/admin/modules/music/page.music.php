<?php /* $Id: page.music.php 4913 2007-08-21 04:49:30Z p_lindheimer $ */
//Copyright (C) 2004 Coalescent Systems Inc. (info@coalescentsystems.ca)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
?>

<?php
$action = isset($_REQUEST['action'])?$_REQUEST['action']:'';
$randon = isset($_REQUEST['randon'])?$_REQUEST['randon']:'';
$randoff = isset($_REQUEST['randoff'])?$_REQUEST['randoff']:'';
$category = strtr(isset($_REQUEST['category'])?$_REQUEST['category']:''," ./\"\'\`", "------");
if ($category == null) $category = 'default';
$display='music';

global $asterisk_conf;

if ($category == "default")
	$path_to_dir = $asterisk_conf['astvarlibdir']."/mohmp3"; //path to directory u want to read.
else
	$path_to_dir = $asterisk_conf['astvarlibdir']."/mohmp3/$category"; //path to directory u want to read.


if (strlen($randon)) {
	touch($path_to_dir."/.random");
	createmusicconf();
}
if (strlen($randoff)) {
	unlink($path_to_dir."/.random");
	createmusicconf();
}
switch ($action) {
	case "addednew":
		makemusiccategory($path_to_dir,$category); 
		createmusicconf();
		needreload();
		redirect_standard();
	break;
	case "addedfile":
		createmusicconf();
		needreload();
//		redirect_standard();
	break;
	case "delete":
		music_rmdirr("$path_to_dir"); 
		$path_to_dir = $asterisk_conf['astvarlibdir']."/mohmp3"; //path to directory u want to read.
		$category='default';
		createmusicconf();
		needreload();
		redirect_standard();
	break;
}


?>
</div>
<div class="rnav"><ul>
    <li><a href="config.php?display=<?php echo urlencode($display)?>&action=add"><?php echo _("Add Music Category")?></a></li>

<?php
//get existing trunk info
$tresults = music_list($asterisk_conf['astvarlibdir']."/mohmp3");
if (isset($tresults)) {
	foreach ($tresults as $tresult) {
		if ($tresult != "none") {
			echo "<li><a id=\"".($category==$tresult ? 'current':'')."\" href=\"config.php?display=".urlencode($display)."&category=".urlencode($tresult)."&action=edit\">{$tresult}</a></li>";
		}
	}
}
?>
</ul></div>


<?php
function createmusicconf()
{
	global $asterisk_conf;

	$File_Write="";
	$tresults = music_list($asterisk_conf['astvarlibdir']."/mohmp3");
	if (isset($tresults)) {
		foreach ($tresults as $tresult)  {
			// hack - but his is all a hack until redone, in functions, etc.
			// this puts a none category to allow no music to be chosen
			//
			if ($tresult == "none") {
				$dir = "/dev/null";
				$File_Write.="[{$tresult}]\nmode=files\ndirectory={$dir}\n";
				continue;
			}
			if ($tresult != "default" ) {
				$dir = $asterisk_conf['astvarlibdir']."/mohmp3/{$tresult}/";
			} else {
				$dir = $asterisk_conf['astvarlibdir']."/mohmp3/";
			}
			if (file_exists("{$dir}.random")) {
				$File_Write.="[{$tresult}]\nmode=files\ndirectory={$dir}\nrandom=yes\n";
			} else {
				$File_Write.="[{$tresult}]\nmode=files\ndirectory={$dir}\n";
			}
		}
	}


	$handle = fopen("/etc/asterisk/musiconhold_additional.conf", "w");

	if (fwrite($handle, $File_Write) === FALSE) {
		echo _("Cannot write to file")." ($tmpfname)";
		exit;
	}

	fclose($handle);

	needreload();
}

function makemusiccategory($category)
{
	mkdir("$path_to_dir/$category", 0755); 
}
 
function build_list() 
{
	global $path_to_dir;
	$pattern = '';
	$handle=opendir($path_to_dir) ;
	$extensions = array('mp3','wav'); // list of extensions to match
	
	//generate the pattern to look for.
	$pattern = '/(\.'.implode('|\.',$extensions).')$/i';
	
	//store file names that match pattern in an array
	$i = 0;
	while (($file = readdir($handle))!==false) 
	{
		if ($file != "." && $file != "..") 
		{ 
		
			if(preg_match($pattern,$file))
			{
				$file_array[$i] = $file; //pattern is matched store it in file_array.
				$i++;		
			}
		} 
	
	}
	closedir($handle); 
	
	return (isset($file_array))?$file_array:null;  //return the size of the array
	
}

function draw_list($file_array, $path_to_dir, $category) 
{
	global $display;
	//list existing mp3s and provide delete buttons
	if ($file_array) {
		foreach ($file_array as $thisfile) {
			print "<div style=\"text-align:right;width:350px;border: 1px solid;padding:2px;\">";
			//print "<a style=\"float:left;margin-left:5px;\" href=\"file:". $path_to_dir ."". $thisfile ."\">".$thisfile."</a>";
			print "<b style=\"float:left;margin-left:5px;\" >".$thisfile."</b>";
			print "<a style=\"margin-right:5px;\" href=\"".$_SERVER['SCRIPT_NAME']."?display=";
			print (isset($display)?$display:'')."&del=".$thisfile."&category=".$category."\">"._("Delete")."</a>";
			print "</div><br>";
		}
	}
}

function process_mohfile($mohfile)
{
	global $path_to_dir;
	$output = 0;
	$returncode = 0;
	$origmohfile=$path_to_dir."/orig_".$mohfile;
	$newname = strtr($mohfile,"&", "_");
	$newmohfile=$path_to_dir."/". ((strpos($newname,'.mp3') === false) ? $newname.".mp3" : $newname);
	$lamecmd="lame --cbr -m m -t -F \"".$origmohfile."\" \"".$newmohfile."\" 2>&1 ";
	if (strpos($newmohfile,'.mp3') !== false) 
		exec($lamecmd, $output, $returncode);
	if ($returncode != 0) {
		return join("<br>\n", $output);
	}
	$rmcmd="rm -f \"". $origmohfile."\"";
	exec($rmcmd);
	return null;
}

/*function kill_mpg123()
{
	$killcmd="killall -9 mpg123";
	exec($killcmd);
}*/
?>

<div class="content">
<h2><?php echo _("On Hold Music")?></h2>

<?php
if ($action == 'add')
{
	?>
	<form name="addcategory" action="<?php $_SERVER['PHP_SELF'] ?>" method="post" onsubmit="return addcategory_onsubmit();">
	<input type="hidden" name="display" value="<?php echo $display?>">
	<input type="hidden" name="action" value="addednew">
	<table>
	<tr><td colspan="2"><h5><?php echo _("Add Music Category")?><hr></h5></td></tr>
	<tr>
		<td><a href="#" class="info"><?php echo _("Category Name:")?><span><?php echo _("Allows you to Set up Different Categories for music on hold.  This is useful if you would like to specify different Hold Music or Commercials for various ACD Queues.")?> </span></a></td>
		<td><input type="text" name="category" value=""></td>
	</tr>
	<tr>
		<td colspan="2"><br><h6><input name="Submit" type="submit" value='<?php echo _("Submit Changes")?>' ></h6></td>		
	</tr>
	</table>
<script language="javascript">
<!--

var theForm = document.addcategory;
theForm.category.focus();

function addcategory_onsubmit() {
	var msgInvalidCategoryName = "<?php echo _('Please enter a valid Category Name'); ?>";
	var msgReservedCategoryName = "<?php echo _('Categories: \"none\" and \"default\" are reserved names. Please enter a different name'); ?>";

	defaultEmptyOK = false;
	if (!isAlphanumeric(theForm.category.value))
		return warnInvalid(theForm.category, msgInvalidCategoryName);
	if (theForm.category.value == "default" || theForm.category.value == "none")
		return warnInvalid(theForm.category, msgReservedCategoryName);
	
	return true;
}

//-->
</script>

	</form>
	<br><br><br><br><br>

<?php
}
else
{
?>

	<h5><?php echo _("Category:")?> <?php echo $category=="default"?_("default"):$category;?></h5>
	<?php  if ($category!="default"){?>
	<p><a href="config.php?display=<?php echo urlencode($display) ?>&action=delete&category=<?php echo urlencode($category) ?>"><?php echo _("Delete Music Category")?> <?php echo $category; ?></a></p><?php }?>

	<form enctype="multipart/form-data" name="upload" action="<?php echo $_SERVER['PHP_SELF'] ?>" method="POST"/>
		<?php echo _("Upload a .wav or .mp3 file:")?><br>
		<input type="hidden" name="display" value="<?php echo $display?>">
		<input type="hidden" name="category" value="<?php echo "$category" ?>">
		<input type="hidden" name="action" value="addedfile">
		<input type="file" name="mohfile"/>
		<input type="button" value="<?php echo _("Upload")?>" onclick="document.upload.submit(upload);alert('<?php echo addslashes(_("Please wait until the page loads. Your file is being processed."))?>');"/>
	</form>
	<br />
	<form name="randomon" action="<?php $_SERVER['PHP_SELF'] ?>" method="post">
	<?php 
		if (file_exists("{$path_to_dir}/.random")) {
			?> <input type="submit" name="randoff" value="Disable Random Play"> <?php
		} else {
			?> <input type="submit" name="randon" value="Enable Random Play"> <?php
		}
	?>
	</form>
	<br />
	<?php

	// Check to see if the upload failed for some reason
	if (isset($_FILES['mohfile']['name']) && !is_uploaded_file($_FILES['mohfile']['tmp_name'])) {
		if (strlen($_FILES['mohfile']['name']) == 0) {
			echo "<h5> PHP "._("Error Processing")."! "._("No file provided")." "._("Please select a file to upload")."</h5>";
		} else {
			echo "<h5> PHP "._("Error Processing")." ".$_FILES['mohfile']['name']."! "._("Check")." upload_max_filesize "._("in")." /etc/php.ini</h5>";
		}
	}
	if (isset($_FILES['mohfile']['tmp_name']) && is_uploaded_file($_FILES['mohfile']['tmp_name'])) {
		//echo $_FILES['mohfile']['name']." uploaded OK";
		move_uploaded_file($_FILES['mohfile']['tmp_name'], $path_to_dir."/orig_".$_FILES['mohfile']['name']);
		$process_err = process_mohfile($_FILES['mohfile']['name']);
		if (isset($process_err)) {
			echo "<h5>"._("Error Processing").": \"$process_err\" for ".$_FILES['mohfile']['name']."!</h5>\n";
			echo "<h5>"._("This is not a fatal error, your Music on Hold may still work.")."</h5>\n";
		} else {
			echo "<h5>"._("Completed processing")." ".$_FILES['mohfile']['name']."!</h5>";
		}
		needreload();
		//kill_mpg123();
	}

	//build the array of files
	$file_array = build_list();
	$numf = count($file_array);


	if (isset($_REQUEST['del'])) {
		$del = $_REQUEST['del'];
		if (strpos($del, "\"") || strpos($del, "\'") || strpos($del, "\;")) {
			print "You're trying to use an invalid character. Please don't.\n"; 
			exit; 
		}
		if (($numf == 1) && ($category == "default") ){
			echo "<h5>"._("You must have at least one file for On Hold Music.  Please upload one before deleting this one.")."</h5>";
		} else {
			$rmcmd="rm -f \"".$path_to_dir."/".$del."\"";
			exec($rmcmd);
			echo "<h5>"._("Deleted")." ".$del."!</h5>";
			//kill_mpg123();
			needreload();
		}
	}
	$file_array = build_list();
	draw_list($file_array, $path_to_dir, $category);
	?>
	<br><br><br><br><br><br>
<?php
}
?>

