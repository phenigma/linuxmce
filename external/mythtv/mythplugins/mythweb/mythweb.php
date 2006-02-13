<?php
/**
 * The main brain script for all of MythWeb.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/mythweb.php $
 * @date        $Date: 2006-01-30 19:05:51 -0500 (Mon, 30 Jan 2006) $
 * @version     $Revision: 8793 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Add a custom include path?
    if (!empty($_SERVER['include_path']) && $_SERVER['include_path'] != '.')
        ini_set('include_path', $_SERVER['include_path'].PATH_SEPARATOR.ini_get('include_path'));

// Init
    require_once 'includes/init.php';

// Standard module?  Pass along the
    if ($Modules[$Path[0]]) {
        require_once 'modules/'.$Path[0].'/handler.php';
    }
    elseif (!empty($Path[0]) && preg_match('/\w/', $Path[0])) {
        require_once 'templates/_unknown_module.php';
    }
    else {
        require_once 'modules/welcome.php';
    }

// Exit gracefully
    exit;

