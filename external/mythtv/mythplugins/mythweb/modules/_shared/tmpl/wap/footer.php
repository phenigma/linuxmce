<?php
/**
 * WAP footer
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/_shared/tmpl/wap/footer.php $
 * @date        $Date: 2006-03-26 13:09:14 +0300 (Sun, 26 Mar 2006) $
 * @version     $Revision: 9493 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Display footnotes
    global $Footnotes;
    if (is_array($Footnotes)) {
        foreach ($Footnotes as $note) {
            echo $note;
        }
    }
?>

</body>
</html>
