<?php
/**
 * Initialization routines for the MythWeb TV module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/tv/init.php $
 * @date        $Date: 2006-01-01 02:00:31 +0200 (Sun, 01 Jan 2006) $
 * @version     $Revision: 8460 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

// The TV module is always enabled.
    $Modules['tv'] = array('path'        => 'tv',
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

