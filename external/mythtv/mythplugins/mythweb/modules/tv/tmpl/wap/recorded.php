<?php
/**
 * Recorded programs
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/tv/tmpl/wap/recorded.php $
 * @date        $Date: 2006-06-24 22:03:10 +0300 (Sat, 24 Jun 2006) $
 * @version     $Revision: 10290 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Print the page title and load the header
    $page_title = "MythWeb - ".t('Recorded Programs');
    require_once 'modules/_shared/tmpl/'.tmpl.'/header.php';

// Print the page contents
    global $All_Shows;
?>

<p>
<form class="form" id="program_titles" action="<?php echo root ?>tv/recorded" method="get">
<center>
    Recordings<br />
    <select name="title">
        <option value="">All recordings</option><?php
        global $Program_Titles;
        foreach($Program_Titles as $title => $count) {
            echo '<option value="'.$title.'"';
            if ($_GET['title'] == $title)
                echo ' SELECTED';
            echo '>'.$title.($count > 1 ? " ($count episodes)" : "").'</option>';
        }
        ?>
    </select><br />
    <input type="submit" value="Go"><br /><br />
</center>
</form>
</p>

<?php
    $row = 0;
    foreach ($All_Shows as $show) {
        echo $show->title."<br />";
        echo $show->channame."<br />";
        if(strlen($show->subtitle)>1)
            echo "<b>".$show->subtitle."</b><br />";
        if(strlen($show->description)>1)
            echo $show->description."<br />";
        echo date('D, M j,Y (g:i A)', $show->starttime)."<br />";
        echo nice_length($show->length)." ".nice_filesize($show->filesize)."<br />";

        if ($show->endtime > time()) { ?>
            <font color="#FF0000">currently recording - </font>
<?php   } ?>
            <b></b><a id="delete_<?php echo $row ?>" href="<?php echo root ?>tv/recorded?delete=yes&file=<?php echo urlencode($show->filename) ?>">Delete </a></b>
            <b></b><a id="delete_<?php echo $row ?>" href="<?php echo root ?>tv/recorded?delete=yes&file=<?php echo urlencode($show->filename) ?>&forget_old">Rerecord</a></b><br />
<?php
        echo "<br />";

        $row++;
    }
?>

<?php
    echo $GLOBALS['Total_Programs'].' programs, using '.nice_filesize(disk_used).' out of '.nice_filesize(disk_size);

    // Print the main page footer
        require_once 'modules/_shared/tmpl/'.tmpl.'/footer.php';
