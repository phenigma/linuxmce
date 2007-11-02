<?php

// Unconditional Call Forwarding
$fcc = new featurecode('callforward', 'cfon');
$fcc->setDescription('Call Forward All Activate');
$fcc->setDefault('*72');
$fcc->update();
unset($fcc);

$fcc = new featurecode('callforward', 'cfoff');
$fcc->setDescription('Call Forward All Deactivate');
$fcc->setDefault('*73');
$fcc->update();
unset($fcc);

$fcc = new featurecode('callforward', 'cfoff_any');
$fcc->setDescription('Call Forward All Prompting Deactivate');
$fcc->setDefault('*74');
$fcc->update();
unset($fcc);

// Call Forward on Busy
$fcc = new featurecode('callforward', 'cfbon');
$fcc->setDescription('Call Forward Busy Activate');
$fcc->setDefault('*90');
$fcc->update();
unset($fcc);

$fcc = new featurecode('callforward', 'cfboff');
$fcc->setDescription('Call Forward Busy Deactivate');
$fcc->setDefault('*91');
$fcc->update();
unset($fcc);

$fcc = new featurecode('callforward', 'cfboff_any');
$fcc->setDescription('Call Forward Busy Prompting Deactivate');
$fcc->setDefault('*92');
$fcc->update();
unset($fcc);

// Call Forward on No Answer/Unavailable (i.e. phone not registered)
$fcc = new featurecode('callforward', 'cfuon');
$fcc->setDescription('Call Forward No Answer/Unavailable Activate');
$fcc->setDefault('*52');
$fcc->update();
unset($fcc);

$fcc = new featurecode('callforward', 'cfuoff');
$fcc->setDescription('Call Forward No Answer/Unavailable Deactivate');
$fcc->setDefault('*53');
$fcc->update();
unset($fcc);

?>