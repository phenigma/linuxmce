<?php
/**
 * Initialization routines for the MythWeb Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/weather/init.php $
 * @date        $Date: 2006-03-23 09:46:57 +0200 (Thu, 23 Mar 2006) $
 * @version     $Revision: 9478 $
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
                                    'sort'        => 4,
                                    'name'        => t('Weather'),
                                    'description' => t('')
                                   );

