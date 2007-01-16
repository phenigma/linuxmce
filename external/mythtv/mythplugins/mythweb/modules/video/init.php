<?php
/**
 * Initialization routines for the MythWeb Video module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/video/init.php $
 * @date        $Date: 2006-03-23 09:46:57 +0200 (Thu, 23 Mar 2006) $
 * @version     $Revision: 9478 $
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
    $db->error(false);

// If video is enabled, add it to the list.
    if ($has_video)
        $Modules['video'] = array('path'        => 'video',
                                  'sort'        => 3,
                                  'name'        => t('Video'),
                                  'description' => t('')
                                 );

