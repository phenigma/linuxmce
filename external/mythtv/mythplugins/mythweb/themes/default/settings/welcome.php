<?php
/**
 * Welcome page description of the Settings module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/settings/welcome.php $
 * @date        $Date: 2005-12-13 09:23:18 +0200 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Open with a div and an image
    echo '<div id="info_settings" class="hidden">',
         '<img src="', skin_url, '/img/settings.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: settings'),

// Next, print a list of possible subsectons
         '<ul>';
    foreach ($Modules['settings']['links'] as $link => $name) {
        echo '    <li><a href="', root, 'settings/', $link, '">', htmlentities($name), "</a></li>\n";
    }
    echo '</ul>',

// Close the div
         "</div>\n";