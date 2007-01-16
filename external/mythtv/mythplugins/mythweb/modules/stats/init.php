<?php
/**
 * Initialization routines for the MythWeb Stats module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/stats/init.php $
 * @date        $Date: 2006-03-27 01:26:24 +0300 (Mon, 27 Mar 2006) $
 * @version     $Revision: 9507 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Stats
 *
/**/

// The stats module is always enabled.
    $Modules['stats'] = array('path' => 'stats',
                              'sort' => 501,
                              'name' => t('Statistics'),
                             );

