<?php
/**
 * Initialization routines for the MythWeb streaming module.
 *
 * @url         $URL: svn+ssh://ijr@cvs.mythtv.org/var/lib/svn/tags/release-0-19/mythplugins/mythweb/modules/stream/init.php $
 * @date        $Date: 2005-12-08 00:00:04 -0500 (Thu, 08 Dec 2005) $
 * @version     $Revision: 8171 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Stream
 *
/**/

// The TV module is always enabled.
    $Modules['stream'] = array('path'        => 'stream',
                               'hidden'      => true,
                               'name'        => t('Streaming'),
                              );

