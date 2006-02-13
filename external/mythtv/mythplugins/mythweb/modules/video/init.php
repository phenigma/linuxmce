<?php
/**
 * Initialization routines for the MythWeb Video module
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/video/init.php $
 * @date        $Date: 2006-01-28 22:19:02 -0500 (Sat, 28 Jan 2006) $
 * @version     $Revision: 8758 $
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
                                  'name'        => t('Video'),
                                  'description' => t('')
                                 );

