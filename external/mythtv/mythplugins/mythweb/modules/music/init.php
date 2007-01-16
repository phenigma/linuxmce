<?php
/**
 * Initialization routines for the MythWeb Music module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/music/init.php $
 * @date        $Date: 2006-09-15 08:08:12 +0300 (Fri, 15 Sep 2006) $
 * @version     $Revision: 11197 $
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
    $has_music = $db->query_col('SELECT COUNT(*) FROM music_songs');
    $db->enable_fatal_errors();
    $db->error(false);

// If music is enabled, add it to the list.
    if ($has_music)
        $Modules['music'] = array('path'        => 'music',
                                  'sort'        => 2,
                                  'name'        => t('Music'),
                                  'description' => t('')
                                 );

