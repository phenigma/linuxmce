<?php

function callforward_get_config($engine) {
	$modulename = 'callforward';
	
	// This generates the dialplan
	global $ext;  
	switch($engine) {
		case "asterisk":
			if (is_array($featurelist = featurecodes_getModuleFeatures($modulename))) {
				foreach($featurelist as $item) {
					$featurename = $item['featurename'];
					$fname = $modulename.'_'.$featurename;
					if (function_exists($fname)) {
						$fcc = new featurecode($modulename, $featurename);
						$fc = $fcc->getCodeActive();
						unset($fcc);
						
						if ($fc != '')
							$fname($fc);
					} else {
						$ext->add('from-internal-additional', 'debug', '', new ext_noop($modulename.": No func $fname"));
						var_dump($item);
					}	
				}
			}
		break;
	}
}

// Unconditional Call Forwarding
function callforward_cfon($c) {
	global $ext;

	$id = "app-cf-on"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional'));
	$ext->add($id, $c, '', new ext_playback('please-enter-your&extension'));
	$ext->add($id, $c, '', new ext_read('fromext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_setvar('fromext', '${IF($["foo${fromext}"="foo"]?${CALLERID(number)}:${fromext})}'));	
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, 'startread', new ext_playback('ent-target-attendant'));
	$ext->add($id, $c, '', new ext_read('toext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_gotoif('$["foo${toext}"="foo"]', 'startread'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('DB(CF/${fromext})', '${toext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${toext}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_setvar('DB(CF/${CALLERID(number)})', '${EXTEN:'.$clen.'}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${CALLERID(number)}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

function callforward_cfoff_any($c) {
	global $ext;

	$id = "app-cf-off-any"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_playback('please-enter-your&extension'));
	$ext->add($id, $c, '', new ext_read('fromext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_dbdel('CF/${fromext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('cancelled'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

function callforward_cfoff($c) {
	global $ext;

	$id = "app-cf-off"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	// for this extension
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_dbdel('CF/${CALLERID(number)}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional&de-activated')); // $cmd,n,Playback(...)
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	// for any extension, dial *XX<exten>
	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('fromext', '${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_dbdel('CF/${fromext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-unconditional&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('cancelled'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
	
}

// Call Forward on Busy
function callforward_cfbon($c) {
	global $ext;

	$id = "app-cf-busy-on"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy'));
	$ext->add($id, $c, '', new ext_playback('please-enter-your&extension'));
	$ext->add($id, $c, '', new ext_read('fromext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_setvar('fromext', '${IF($["foo${fromext}"="foo"]?${CALLERID(number)}:${fromext})}'));	
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, 'startread', new ext_playback('ent-target-attendant'));
	$ext->add($id, $c, '', new ext_read('toext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_gotoif('$["foo${toext}"="foo"]', 'startread'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('DB(CFB/${fromext})', '${toext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${toext}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_setvar('DB(CFB/${CALLERID(number)})', '${EXTEN:'.$clen.'}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${CALLERID(number)}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

function callforward_cfboff_any($c) {
	global $ext;

	$id = "app-cf-busy-off-any"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_playback('please-enter-your&extension'));
	$ext->add($id, $c, '', new ext_read('fromext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_dbdel('CFB/${fromext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('cancelled'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

function callforward_cfboff($c) {
	global $ext;

	$id = "app-cf-busy-off"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	// for this extension
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_dbdel('CFB/${CALLERID(number)}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy&de-activated')); // $cmd,n,Playback(...)
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	// for any extension, dial *XX<exten>
	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('fromext', '${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_dbdel('CFB/${fromext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-on-busy&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('cancelled'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
	
}

// Call Forward on No Answer/Unavailable (i.e. phone not registered)
function callforward_cfuon($c) {
	global $ext;

	$id = "app-cf-unavailable-on"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	// prompt for extension
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_playback('call-fwd-no-ans'));
	$ext->add($id, $c, '', new ext_playback('please-enter-your&extension'));
	$ext->add($id, $c, '', new ext_read('fromext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_setvar('fromext', '${IF($["foo${fromext}"="foo"]?${CALLERID(number)}:${fromext})}'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, 'startread', new ext_playback('ent-target-attendant'));
	$ext->add($id, $c, '', new ext_read('toext', 'then-press-pound'));
	$ext->add($id, $c, '', new ext_gotoif('$["foo${toext}"="foo"]', 'startread'));
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('DB(CFU/${fromext})', '${toext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-no-ans&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${toext}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	// assume this extension and forward to number after the feature code
	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_setvar('DB(CFU/${CALLERID(number)})', '${EXTEN:'.$clen.'}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-no-ans&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${CALLERID(number)}'));
	$ext->add($id, $c, '', new ext_playback('is-set-to'));
	$ext->add($id, $c, '', new ext_saydigits('${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

function callforward_cfuoff($c) {
	global $ext;

	$id = "app-cf-unavailable-off"; // The context to be included

	$ext->addInclude('from-internal-additional', $id); // Add the include from from-internal

	// for this extension
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_macro('user-callerid')); // $cmd,n,Macro(user-callerid)
	$ext->add($id, $c, '', new ext_dbdel('CFU/${CALLERID(number)}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-no-ans&de-activated')); // $cmd,n,Playback(...)
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)

	// for any extension, dial *XX<exten>
	$clen = strlen($c);
	$c = "_$c.";
	$ext->add($id, $c, '', new ext_answer('')); // $cmd,1,Answer
	$ext->add($id, $c, '', new ext_wait('1')); // $cmd,n,Wait(1)
	$ext->add($id, $c, '', new ext_setvar('fromext', '${EXTEN:'.$clen.'}'));
	$ext->add($id, $c, '', new ext_dbdel('CFU/${fromext}')); 
	$ext->add($id, $c, '', new ext_playback('call-fwd-no-ans&for&extension'));
	$ext->add($id, $c, '', new ext_saydigits('${fromext}'));
	$ext->add($id, $c, '', new ext_playback('cancelled'));
	$ext->add($id, $c, '', new ext_macro('hangupcall')); // $cmd,n,Macro(user-callerid)
}

?>
