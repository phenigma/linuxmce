<?php
/**
 * This footer file is shared by all MythWeb modules.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/footer.php $
 * @date        $Date: 2005-11-08 08:43:55 +0200 (Tue, 08 Nov 2005) $
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
