<?php
/**
 * Initialization routines for the MythWeb Status module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/status/init.php $
 * @date        $Date: 2006-03-23 09:46:57 +0200 (Thu, 23 Mar 2006) $
 * @version     $Revision: 9478 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Status
 *
/**/

// The TV module is always enabled.
    $Modules['status'] = array('path'        => 'status',
                               'sort'        => 500,
                               'name'        => t('Status'),
                          );

