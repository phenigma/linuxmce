<?php
/**
 * This footer file is shared by all MythWeb modules.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/default/footer.php $
 * @date        $Date: 2005-11-08 01:43:55 -0500 (Tue, 08 Nov 2005) $
 * @version     $Revision: 7795 $
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
