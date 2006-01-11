<?php
/**
 * Initialization routines for the MythWeb Backend Logs module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/backend_log/init.php $
 * @date        $Date: 2005-12-16 08:55:02 +0200 (Fri, 16 Dec 2005) $
 * @version     $Revision: 8285 $
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
                                        'name'        => t('Logs'),
                                       );

