<?php
/**
 * Initialization routines for the MythWeb settings module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/settings/init.php $
 * @date        $Date: 2006-03-30 11:06:53 +0300 (Thu, 30 Mar 2006) $
 * @version     $Revision: 9558 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// The settings module is always enabled.
    $Modules['settings'] = array('path'        => 'settings',
                                 'sort'        => 999,
                                 'name'        => t('Settings'),
                                 'description' => t('settings'),
                                 'links'       => array('session'   => t('MythWeb session settings'),
                                                        'mythweb'   => t('MythTV global defaults'),
                                                        'channels'  => t('MythTV channel info'),
                                                        'keys'      => t('MythTV key bindings'),
                                                        'settings'  => t('MythTV settings table'),
                                                       ),
                                );
