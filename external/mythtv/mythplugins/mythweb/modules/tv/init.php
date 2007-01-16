<?php
/**
 * Initialization routines for the MythWeb TV module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/tv/init.php $
 * @date        $Date: 2006-03-23 09:46:57 +0200 (Thu, 23 Mar 2006) $
 * @version     $Revision: 9478 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

// The TV module is always enabled.
    $Modules['tv'] = array('path'        => 'tv',
                           'sort'        => 1,
                           'name'        => t('TV'),
                           'links'       => array('list'             => t('Program Listing'),
                                                  'searches'         => t('Special Searches'),
                                                  'upcoming'         => t('Upcoming Recordings'),
                                                  'schedules'        => t('Recording Schedules'),
                                                  'schedules/manual' => t('Schedule Manually'),
                                                  'schedules/custom' => t('Custom Schedule'),
                                                  'recorded'         => t('Recorded Programs'),
                                                 ),
                          );

