<?php
/**
 * Initialization routines for the MythWeb TV module.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/tv/init.php $
 * @date        $Date: 2005-12-31 19:00:31 -0500 (Sat, 31 Dec 2005) $
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

