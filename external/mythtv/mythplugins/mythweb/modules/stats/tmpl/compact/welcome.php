<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/stats/tmpl/compact/welcome.php $
 * @date        $Date: 2006-03-27 01:26:24 +0300 (Mon, 27 Mar 2006) $
 * @version     $Revision: 9507 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Stats
 *
/**/

// Open with a div and an image
    echo '<div id="info_stats" class="hidden">',
         '<img src="', skin_url, '/img/stats.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: stats'),

// Close the div
         "</div>\n";