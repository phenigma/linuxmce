<?php
/**
 * Initialization routines for the MythWeb streaming module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/trunk/mythplugins/mythweb/modules/stream/init.php $
 * @date        $Date: 2005-12-08 07:00:04 +0200 (Thu, 08 Dec 2005) $
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

