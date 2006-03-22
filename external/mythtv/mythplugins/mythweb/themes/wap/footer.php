<?php
/**
 * This footer file is shared by all MythWeb modules.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/wap/footer.php $
 * @date        $Date: 2006-01-26 04:43:22 +0200 (Thu, 26 Jan 2006) $
 * @version     $Revision: 8726 $
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
