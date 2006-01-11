<?php
/**
 * Handler for all MythWeb Settings routines
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/settings/handler.php $
 * @date        $Date: 2005-12-15 09:33:52 +0200 (Thu, 15 Dec 2005) $
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
