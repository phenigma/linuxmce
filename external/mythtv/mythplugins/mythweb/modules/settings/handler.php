<?php
/**
 * Handler for all MythWeb Settings routines
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/settings/handler.php $
 * @date        $Date: 2005-12-15 02:33:52 -0500 (Thu, 15 Dec 2005) $
 * @version     $Revision: 8273 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Unknown section?
    if (empty($Modules['settings']['links'][$Path[1]]))
        $Path[1] = NULL;

// Show the requested section
    if ($Path[1])
        require_once 'modules/settings/'.$Path[1].'.php';

// Otherwise, just show the settings index page
    require_once theme_dir.'settings/settings.php';
