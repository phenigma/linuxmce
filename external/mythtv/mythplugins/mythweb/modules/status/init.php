<?php
/**
 * Initialization routines for the MythWeb Status module
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/status/init.php $
 * @date        $Date: 2005-12-16 01:55:02 -0500 (Fri, 16 Dec 2005) $
 * @version     $Revision: 8285 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Status
 *
/**/

// The TV module is always enabled.
    $Modules['status'] = array('path'        => 'status',
                               'name'        => t('Status'),
                          );

