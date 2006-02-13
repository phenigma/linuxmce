<?php
/**
 * Initialization routines for the MythWeb settings module
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/settings/init.php $
 * @date        $Date: 2005-12-30 19:08:13 -0500 (Fri, 30 Dec 2005) $
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
