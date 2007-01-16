<?php
/**
 * Initialization routines for the MythWeb Backend Logs module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/backend_log/init.php $
 * @date        $Date: 2006-03-24 07:55:11 +0200 (Fri, 24 Mar 2006) $
 * @version     $Revision: 9485 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Logs
 *
/**/

// Check to see if database logging has been enabled
    $enabled = $db->query_col('SELECT SUM(data) FROM settings WHERE value="LogEnabled"');

// The TV module is always enabled.
    if ($enabled > 0)
        $Modules['backend_log'] = array('path'        => 'backend_log',
                                        'sort'        => 550,
                                        'name'        => t('Logs'),
                                       );

