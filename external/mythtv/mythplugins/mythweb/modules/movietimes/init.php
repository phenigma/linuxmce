<?php
/**
 * Initialization routines for the MythWeb Movie Times module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/movietimes/init.php $
 * @date        $Date: 2005-12-16 08:55:02 +0200 (Fri, 16 Dec 2005) $
 * @version     $Revision: 8285 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  MovieTimes
 *
/**/

// First, we should check to see that MythVideo is configured.
    $has_movietimes = false;


// If video is enabled, add it to the list.
    if ($has_movietimes)
        $Modules['movietimes'] = array('path'        => 'movietimes',
                                       'name'        => t('Movie Times'),
                                       'description' => t('')
                                      );

