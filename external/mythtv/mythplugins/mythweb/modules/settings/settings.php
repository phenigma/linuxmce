<?php
/**
 * Configure MythTV Settings table
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-20-fixes/mythplugins/mythweb/modules/settings/settings.php $
 * @date        $Date: 2006-04-11 20:30:50 +0300 (Tue, 11 Apr 2006) $
 * @version     $Revision: 9670 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Specific host?
    isset($_GET['host']) or $_GET['host'] = $_GET['host'];
    if (empty($_GET['host']))
        $_GET['host'] = null;

// Save?
    if ($_POST['save']) {
        foreach ($_POST['settings'] as $value => $data) {
            setting($value, $_GET['host'], $data);
        }
        if (is_array($_POST['delete'])) {
            foreach ($_POST['delete'] as $value => $data) {
                if (!$data)
                    continue;
                if (is_null($_GET['host']))
                    $sh = $db->query('DELETE FROM settings
                                       WHERE value=? AND hostname IS NULL',
                                     $value
                                    );
                else
                    $sh = $db->query('DELETE FROM settings
                                       WHERE value=? AND hostname=?',
                                     $value,
                                     $_GET['host']
                                    );
            }
        }
    }

// Load all of the known mythtv hosts
    $Hosts = array();
    $sh = $db->query('SELECT DISTINCT hostname FROM settings ORDER BY hostname');
    while (list($host) = $sh->fetch_row()) {
        if (empty($host))
            continue;
        $Hosts[] = $host;
    }
    $sh->finish();

// Load all of the settings for the requested host
    $Settings = array();
    if (is_null($_GET['host']))
        $sh = $db->query('SELECT value, data
                            FROM settings
                           WHERE hostname IS NULL
                        ORDER BY value');
    else
        $sh = $db->query('SELECT value, data
                            FROM settings
                           WHERE hostname=?
                        ORDER BY value',
                         $_GET['host']);
    while (list($value, $data) = $sh->fetch_row()) {
        $Settings[$value] = $data;
    }
    $sh->finish();

// Load the class for this page
    require_once tmpl_dir.'settings.php';

// Exit
    exit;

