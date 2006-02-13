<?php
/**
 * The display code for the main welcome page that lists the available mythweb
 * sections.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/default/status/status.php $
 * @date        $Date: 2005-11-07 22:37:20 -0500 (Mon, 07 Nov 2005) $
 * @version     $Revision: 7792 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Set the desired page title
    $page_title = 'MythTV Backend Status';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'/status.css" />';

// Print the page header
    require_once theme_dir.'/header.php';

// Print the content itself
    echo "<div id=\"content_wrapper\">\n"
        .$status
        ."\n</div>";

// Print the page footer
    require_once theme_dir.'/footer.php';

