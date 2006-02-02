<?php
/**
 * Initialization routines for the MythWeb Music module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/music/init.php $
 * @date        $Date: 2006-01-29 05:19:02 +0200 (Sun, 29 Jan 2006) $
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

