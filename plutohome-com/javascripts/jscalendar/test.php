<?php

/*$lang = $_GET['lang'];
if (!$lang) {
    $lang = $_REQUEST['lang'];
}
if (!$lang) {
    $lang = 'en';
}
setcookie('lang', $lang);*/
$lang="ro";

?>
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
<head>
<title>
Test for calendar.php
</title>

<?php
require_once ('../jscalendar/calendar.php');
$calendar = new DHTML_Calendar('../jscalendar/', $lang, 'calendar2', false);
$calendar->load_files();
?>

</head>

<body>

<form action="test.php" method="get">
<table><tr>
     <td>Date:</td>
     <td>
       <?
        $calendar->make_input_field(
           // calendar options go here; see the documentation and/or calendar-setup.js
           array('firstDay'       => 1, // show Monday first
                 'showsTime'      => true,
                 'showOthers'     => true,
                 'ifFormat'       => '%Y-%m-%d',
                 'timeFormat'     => '24'),
           // field attributes go here
           array('style'       => 'width: 80px; height:15px; color: #840; border: 1px solid #000; font-size:9px; text-align: center',
                 'name'        => 'datastart',
                 'value'       => strftime('%Y-%m-%d', strtotime('now')))); 
      ?>
     </td>
     </tr>
     </table>

     <hr />
     </form>
</body>
</html>
