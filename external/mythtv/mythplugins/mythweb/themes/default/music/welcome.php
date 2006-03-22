<?php
/**
 * Welcome page description of the Music module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/music/welcome.php $
 * @date        $Date: 2005-12-13 09:23:18 +0200 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_music" class="hidden">',
         '<img src="', skin_url, '/img/music.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: music'),

// Next, print a list of possible subsectons
    ####

// Close the div
         "</div>\n";