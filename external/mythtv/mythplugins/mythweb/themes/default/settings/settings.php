<?php
/**
 * Main settings index
 *
 * @url         $URL$
 * @date        $Date: 2006-01-25 05:41:36 +0200 (Wed, 25 Jan 2006) $
 * @version     $Revision: 8709 $
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
        echo '    <li><a href="', root, 'settings/', $link, '">', html_entities($name), "</a></li>\n";
    }
    echo '</ul>';
?>
</div>
<?php

// Print the page footer
    require_once theme_dir.'/footer.php';

