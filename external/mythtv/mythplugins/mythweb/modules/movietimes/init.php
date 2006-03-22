<?php
/**
 * Initialization routines for the MythWeb Movie Times module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/movietimes/init.php $
 * @date        $Date: 2006-01-29 05:19:02 +0200 (Sun, 29 Jan 2006) $
 * @version     $Revision: 8758 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  MovieTimes
 *
/**/

// First, we should check to see that the movie times plugin is configured.
    $has_movietimes = false;

// If video is enabled, add it to the list.
    if ($has_movietimes)
        $Modules['movietimes'] = array('path'        => 'movietimes',
                                       'name'        => t('Movie Times'),
                                       'description' => t('')
                                      );

