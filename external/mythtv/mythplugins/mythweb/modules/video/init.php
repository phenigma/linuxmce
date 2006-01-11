<?php
/**
 * Initialization routines for the MythWeb Video module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/video/init.php $
 * @date        $Date: 2005-12-22 07:13:45 +0200 (Thu, 22 Dec 2005) $
 * @version     $Revision: 8344 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Video
 *
/**/

// First, we should check to see that MythVideo is configured.
// Make sure to turn off fatal errors in case the table isn't there.
    $db->disable_fatal_errors();
    $has_video = $db->query_col('SELECT COUNT(*) FROM videometadata');
    $db->enable_fatal_errors();

// If video is enabled, add it to the list.
    if ($has_video)
        $Modules['video'] = array('path'        => 'video',
                                  'name'        => t('Video'),
                                  'description' => t('')
                                 );

