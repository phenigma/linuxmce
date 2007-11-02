<?php /* $Id: functions.inc.php 2188 2006-07-27 02:21:52Z p_lindheimer $ */

function parking_destinations() {
}

/* 	Generates dialplan for parkinglot
	We call this with retrieve_conf
*/
function parking_get_config($engine) {
	global $db;
	global $ext;  // is this the best way to pass this?
	global $asterisk_conf;
	switch($engine) {
		case "asterisk":

		$contextname = 'park-dial';

		$parkinglot_id = 1; // only 1 parking lot, but prepare for future
		$results = parking_getconfig($parkinglot_id);

		// Got the array, let's go work out the required variables
		//
		$parkingenabled	= isset($results['parkingenabled'])?$results['parkingenabled']:'';
		$parkext 	= isset($results['parkext'])?$results['parkext']:70;
		$numslots  	= isset($results['numslots'])?$results['numslots']:8;
		$parkingtime 	= isset($results['parkingtime'])?$results['parkingtime']:'';
		$parkingcontext	= isset($results['parkingcontext'])?$results['parkingcontext']:'parkedcalls';
		$parkalertinfo 	= isset($results['parkalertinfo'])?$results['parkalertinfo']:'';
		$parkcid 	= isset($results['parkcid'])?$results['parkcid']:'';
		$parkingannmsg 	= isset($results['parkingannmsg'])?$results['parkingannmsg']:'';
		$goto	 	= isset($results['goto'])?$results['goto']:'from-pstn,s,1';

		$parkpos1	= $parkext + 1;
		$parkpos2	= $parkpos1 + $numslots;

		//open the file and truncate. If diabled, file will be deleted this way
		//AND GET THE ENV VARIABLES TO CALL THIS BY

		$filename = isset($asterisk_conf["astetcdir"]) && $asterisk_conf["astetcdir"] != '' ? rtrim($asterisk_conf["astetcdir"],DIRECTORY_SEPARATOR) : "/etc/asterisk";
		$filename .= "/parking_additional.inc";
		$fh = fopen($filename, "w+");
		fwrite($fh, ";*** WARNING: DO NOT HAND EDIT THIS FILE IT IS AUTO-GENERATD ***\n;\n");

		if ($parkingenabled) {
			// TODO: lookup ampportal.conf variables for this, don't hard code
			// first write features_additional.inc include file
			//
			fwrite($fh, "parkext => ".$parkext."\n");
			fwrite($fh, "parkpos => ".$parkpos1."-".$parkpos2."\n");
			fwrite($fh, "context => ".$parkingcontext."\n");
			if ($parkingtime) {
				fwrite($fh, "parkingtime => ".$parkingtime."\n");
			}

			// Now generate dialplan
			$ext->add($contextname, "t", '', new ext_noop('Parked Call Timed Out and Got Orphaned'));
			if ($parkalertinfo) {
				$ext->add($contextname, "t", '', new ext_setvar('__ALERT_INFO',str_replace(';', '\;', $parkalertinfo)));
			}
			if ($parkcid) {
				$ext->add($contextname, "t", '', new ext_setvar('CALLERID(name)', $parkcid.'${CALLERID(name)}'));
			}
			if ($parkingannmsg) {
				$ext->add($contextname, "t", '', new ext_playback($parkingannmsg));
			}
			// goto the destination here
			//
			$ext->add($contextname, "t", '', new ext_goto($goto));
		} else {
		fwrite($fh, ";***              PARKING LOT HAS BEEN DISABLED              ***\n");
		}
		fclose($fh);
		chmod($filename, 0660);
		break;
	}
}

function parking_add($parkingenabled, $parkext, $numslots, $parkingtime, $parkingcontext, $parkalertinfo, $parkcid, $parkingannmsg, $goto) {
	global $db;

	$parkinglot_id 	= 1; // only 1 parkinglot but prepare for future

	// in future will do in a parking_del but not needed for now
	//
	$sql = "DELETE FROM parkinglot WHERE id = '$parkinglot_id'";
	$result = $db->query($sql);
	if(DB::IsError($result)) {
		die($result->getMessage().$sql);
	}

	// Check for interger only inputs and set to default if not
	//  and set goto to default if not set for some reason
	//
	$parkext	= ctype_digit($parkext)		? $parkext	: 70;
	$numslots		= ctype_digit($numslots)		? $numslots	: 8;
	$parkingtime	= ctype_digit($parkingtime)	? $parkingtime	: '';
	$goto		= ($goto) 			? $goto		: 'from-pstn,s,1';

	$parkfields =	array(array($parkinglot_id, 'parkingenabled', "$parkingenabled"),
			array($parkinglot_id, 'parkext', "$parkext"),
			array($parkinglot_id, 'numslots', "$numslots"),
			array($parkinglot_id, 'parkingtime', "$parkingtime"),
			array($parkinglot_id, 'parkingcontext', trim("$parkingcontext")),
			array($parkinglot_id, 'parkalertinfo', trim("$parkalertinfo")),
			array($parkinglot_id, 'parkcid', trim("$parkcid")),
			array($parkinglot_id, 'parkingannmsg', "$parkingannmsg"),
			array($parkinglot_id, 'goto', "$goto"));

	$compiled = $db->prepare('INSERT INTO parkinglot (id, keyword, data) values (?,?,?)');

	$result = $db->executeMultiple($compiled,$parkfields);
	if(DB::IsError($result)) {
		die($result->getDebugInfo()."<br><br>".'error adding to PARKING table');
	}
}


function parking_getconfig($parkinglot_id=1) {
	global $db;
	$sql = "SELECT keyword,data FROM parkinglot WHERE id = '$parkinglot_id'";
	$results = $db->getAssoc($sql);
	if(DB::IsError($results)) {
		$results = null;
	}
	return $results;
}

// Duly stolen from the queues module (since I can't count on it being there, but would not be bad to stuff back in the common include
//
function parking_timeString($seconds, $full = false) {
        if ($seconds == 0) {
                return "0 ".($full ? "seconds" : "s");
        }

        $minutes = floor($seconds / 60);
        $seconds = $seconds % 60;

        $hours = floor($minutes / 60);
        $minutes = $minutes % 60;

        $days = floor($hours / 24);
        $hours = $hours % 24;

        if ($full) {
                return substr(
                                ($days ? $days." day".(($days == 1) ? "" : "s").", " : "").
                                ($hours ? $hours." hour".(($hours == 1) ? "" : "s").", " : "").
                                ($minutes ? $minutes." minute".(($minutes == 1) ? "" : "s").", " : "").
                                ($seconds ? $seconds." second".(($seconds == 1) ? "" : "s").", " : ""),
                               0, -2);
        } else {
                return substr(($days ? $days."d, " : "").($hours ? $hours."h, " : "").($minutes ? $minutes."m, " : "").($seconds ? $seconds."s, " : ""), 0, -2);
        }
}

?>
