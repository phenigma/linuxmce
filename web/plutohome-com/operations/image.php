<?
switch ($_REQUEST['img']){
	case 'core':
		$image=(!isset($_REQUEST['alt']))?'core_1.jpg':'core_2.jpg';
		$title='Pluto Core';
	break;
	case 'md':
		$image=(!isset($_REQUEST['alt']))?'media_director_1.jpg':'media_director_2.jpg';
		$title='Pluto Media Director';
	break;
	case 'orbiter':
		$image=(!isset($_REQUEST['alt']))?'orbiter_1.jpg':'orbiter_2.jpg';
		$title='Pluto Orbiter';
	break;
	case 'mobileOrbiter':
		$image=(!isset($_REQUEST['alt']))?'mobile_1.jpg':'mobile_2.jpg';
		$title='Pluto Mobile Orbiter';
	break;
	case 'power':
		$image='screenshot.jpg';
		$title='';
	break;
	default:
		$image=$_REQUEST['img'];
	break;
}
$links=(!isset($_REQUEST['alt']))?'1 | <a href="image.php?img='.$_REQUEST['img'].'&alt=1">2</a>':'<a href="image.php?img='.$_REQUEST['img'].'">1</a> | 2';
$links=(isset($_REQUEST['single']))?'':$links;
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>Pluto Home</title>
<link href="../style/style.css" rel="stylesheet" type="text/css" />
</head>

<body>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" valign="middle" colspan="2"><a href="javascript:self.close();"><img src="../images/<?=$image?>" border="0" alt="Click to close popup."></a></td>
  </tr>
  <tr>
  	<td align="left" valign="middle" class="insidetable2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<SPAN class="title"><?=$title?></SPAN></td>
    <td align="right" valign="middle" class="insidetable2"><?=$links?> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>  
</table>
</body>
</html>
