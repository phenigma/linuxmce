<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/status/tmpl/default/welcome.php $
 * @date        $Date: 2005-12-13 09:23:18 +0200 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_status" class="hidden">',
         '<img src="', skin_url, '/img/status.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: status'),

// Close the div
         "</div>\n";