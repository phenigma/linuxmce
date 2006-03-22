<?php
/**
 * Initialization routines for the MythWeb Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/weather/init.php $
 * @date        $Date: 2005-12-16 08:55:02 +0200 (Fri, 16 Dec 2005) $
 * @version     $Revision: 8285 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/

// First, we should check to see that MythWeather is configured.
    $has_weather = $db->query_col('SELECT COUNT(data) FROM settings WHERE value="locale"');

// If weather is enabled, add it to the list.
    if ($has_weather)
        $Modules['weather'] = array('path'        => 'weather',
                                    'name'        => t('Weather'),
                                    'description' => t('')
                                   );

