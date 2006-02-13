<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/default/backend_log/welcome.php $
 * @date        $Date: 2005-12-13 02:23:18 -0500 (Tue, 13 Dec 2005) $
 * @version     $Revision: 8252 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_backend_log" class="hidden">',
         '<img src="', skin_url, '/img/backend_log.png" class="module_icon" />',

// Print a basic overview of what this module does
         t('welcome: backend_log'),

// Close the div
         "</div>\n";