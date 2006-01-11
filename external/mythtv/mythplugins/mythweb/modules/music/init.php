<?php
/**
 * Initialization routines for the MythWeb Music module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/music/init.php $
 * @date        $Date: 2005-12-22 07:13:45 +0200 (Thu, 22 Dec 2005) $
 * @version     $Revision: 8344 $
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

// If music is enabled, add it to the list.
    if ($has_music)
        $Modules['music'] = array('path'        => 'music',
                                  'name'        => t('Music'),
                                  'description' => t('')
                                 );

