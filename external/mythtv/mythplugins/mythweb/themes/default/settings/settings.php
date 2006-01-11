<?php
/**
 * Main settings index
 *
 * @url         $URL$
 * @date        $Date: 2005-12-15 09:33:52 +0200 (Thu, 15 Dec 2005) $
 * @version     $Revision: 8273 $
 * @author      $Author: xris $
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - '.t('Settings');

// Print the page header
    require_once theme_dir.'/header.php';

?>

<div style="padding: 20px">
<?php

    echo t('settings: overview'),
         '<ul>';
    foreach ($Modules['settings']['links'] as $link => $name) {
        echo '    <li><a href="', root, 'settings/', $link, '">', htmlentities($name), "</a></li>\n";
    }
    echo '</ul>';
?>
</div>
<?php

// Print the page footer
    require_once theme_dir.'/footer.php';

