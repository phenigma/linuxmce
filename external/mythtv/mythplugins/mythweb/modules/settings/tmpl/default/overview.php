<?php
/**
 * Main settings index
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/settings/tmpl/default/overview.php $
 * @date        $Date: 2006-06-24 22:03:10 +0300 (Sat, 24 Jun 2006) $
 * @version     $Revision: 10290 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - '.t('Settings');

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';

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
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';

