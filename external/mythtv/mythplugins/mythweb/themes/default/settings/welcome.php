<?php
/**
 * Welcome page description of the Settings module.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/default/settings/welcome.php $
 * @date        $Date: 2006-01-24 22:41:36 -0500 (Tue, 24 Jan 2006) $
 * @version     $Revision: 8709 $
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
        echo '    <li><a href="', root, 'settings/', $link, '">', html_entities($name), "</a></li>\n";
    }
    echo '</ul>',

// Close the div
         "</div>\n";