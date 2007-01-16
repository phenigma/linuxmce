<?php
/**
 * The display code for the main welcome page that lists the available mythweb
 * sections.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/status/tmpl/default/status.php $
 * @date        $Date: 2006-03-26 13:09:14 +0300 (Sun, 26 Mar 2006) $
 * @version     $Revision: 9493 $
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
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';

// Print the content itself
    echo "<div id=\"content_wrapper\">\n"
        .$status
        ."\n</div>";

// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';

