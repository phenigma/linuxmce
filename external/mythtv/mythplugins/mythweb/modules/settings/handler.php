<?php
/**
 * Handler for all MythWeb Settings routines
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/settings/handler.php $
 * @date        $Date: 2006-03-30 06:25:32 +0300 (Thu, 30 Mar 2006) $
 * @version     $Revision: 9548 $
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
    require_once tmpl_dir.'overview.php';
