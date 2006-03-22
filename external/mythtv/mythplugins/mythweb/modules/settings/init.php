<?php
/**
 * Initialization routines for the MythWeb settings module
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/settings/init.php $
 * @date        $Date: 2005-12-31 02:08:13 +0200 (Sat, 31 Dec 2005) $
 * @version     $Revision: 8440 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// The settings module is always enabled.
    $Modules['settings'] = array('path'        => 'settings',
                                 'name'        => t('Settings'),
                                 'description' => t('settings'),
                                 'links'       => array('channels'  => t('MythTV channel info'),
                                                        'keys'      => t('MythTV key bindings'),
                                                        'session'   => t('MythWeb session settings'),
                                                       ),
                                );
