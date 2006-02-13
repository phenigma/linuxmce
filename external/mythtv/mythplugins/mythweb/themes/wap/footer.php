<?php
/**
 * This footer file is shared by all MythWeb modules.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/wap/footer.php $
 * @date        $Date: 2006-01-25 21:43:22 -0500 (Wed, 25 Jan 2006) $
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
