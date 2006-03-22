<?php
/**
 * Welcome page description of the TV module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/tv/welcome.php $
 * @date        $Date: 2006-01-25 05:41:36 +0200 (Wed, 25 Jan 2006) $
 * @version     $Revision: 8709 $
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
        echo '    <li><a href="', root, 'tv/', $link, '">', html_entities($name), "</a></li>\n";
    }
    echo '</ul>',

// Close the div
         "</div>\n";