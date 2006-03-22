<?php
/**
 * Welcome page description of the Weather module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/themes/default/weather/welcome.php $
 * @date        $Date: 2005-12-13 09:23:18 +0200 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_weather" class="hidden">',
         '<img src="', skin_url, '/img/weather.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: weather'),

// Next, print a list of possible subsectons
    ####

// Close the div
         "</div>\n";