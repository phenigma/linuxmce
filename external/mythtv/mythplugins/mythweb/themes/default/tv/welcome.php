<?php
/**
 * Welcome page description of the TV module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/tv/welcome.php $
 * @date        $Date: 2005-12-13 09:23:18 +0200 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

// Open with a div and an image
    echo '<div id="info_tv">',
         '<img src="', skin_url, '/img/tv.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: tv'),

// Next, print a list of possible subsectons
         '<ul>';
    foreach ($Modules['tv']['links'] as $link => $name) {
        echo '    <li><a href="', root, 'tv/', $link, '">', htmlentities($name), "</a></li>\n";
    }
    echo '</ul>',

// Close the div
         "</div>\n";