<?php
/**
 * The display code for the main welcome page that lists the available mythweb
 * sections.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/themes/wap/status/status.php $
 * @date        $Date: 2006-01-25 21:43:22 -0500 (Wed, 25 Jan 2006) $
 * @version     $Revision: 8726 $
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

