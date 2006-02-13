<?php
/**
 * Initialization routines for the MythWeb Music module
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/music/init.php $
 * @date        $Date: 2006-01-28 22:19:02 -0500 (Sat, 28 Jan 2006) $
 * @version     $Revision: 8758 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Music
 *
/**/

// First, we should check to see that MythMusic is configured.
// Make sure to turn off fatal errors in case the table isn't there.
    $db->disable_fatal_errors();
    $has_music = $db->query_col('SELECT COUNT(*) FROM musicmetadata');
    $db->enable_fatal_errors();
    $db->error(false);

// If music is enabled, add it to the list.
    if ($has_music)
        $Modules['music'] = array('path'        => 'music',
                                  'name'        => t('Music'),
                                  'description' => t('')
                                 );

