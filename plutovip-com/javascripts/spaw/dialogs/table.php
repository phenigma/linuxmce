<?php 
// ================================================
// SPAW PHP WYSIWYG editor control
// ================================================
// Table properties dialog
// ================================================
// Developed: Alan Mendelevich, alan@solmetra.lt
// Copyright: Solmetra (c)2003 All rights reserved.
// ------------------------------------------------
//                                www.solmetra.com
// ================================================
// v.1.0, 2003-04-01
// ================================================

// include wysiwyg config
include '../config/spaw_control.config.php';
include $spaw_root.'class/lang.class.php';

$theme = empty($HTTP_GET_VARS['theme'])?$spaw_default_theme:$HTTP_GET_VARS['theme'];
$theme_path = $spaw_dir.'lib/themes/'.$theme.'/';

$l = new SPAW_Lang($HTTP_GET_VARS['lang']);
$l->setBlock('table_prop');

$request_uri = urldecode(empty($HTTP_POST_VARS['request_uri'])?(empty($HTTP_GET_VARS['request_uri'])?'':$HTTP_GET_VARS['request_uri']):$HTTP_POST_VARS['request_uri']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
	<meta http-equiv="Pragma" content="no-cache">
  <title><?php echo $l->m('title')?></title>
  <meta http-equiv="Content-Type" content="text/html; charset=<?php echo $l->getCharset()?>">
  <link rel="stylesheet" type="text/css" href="<?php echo $theme_path.'css/'?>dialog.css">
  <script language="javascript" src="utils.js"></script>
  
  <script language="javascript">
  <!--  
  function showColorPicker(curcolor) {
    var newcol = showModalDialog('colorpicker.php?theme=<?php echo $theme?>&lang=<?php echo $l->lang?>', curcolor, 
      'dialogHeight:250px; dialogWidth:366px; resizable:no; status:no');  
    try {
      table_prop.tbgcolor.value = newcol;
      table_prop.color_sample.style.backgroundColor = table_prop.tbgcolor.value;
    }
    catch (excp) {}
  }

  function showImgPicker()
  {
    var imgSrc = showModalDialog('<?php echo $spaw_dir?>dialogs/img_library.php?theme=<?php echo $theme?>&lang=<?php echo $l->lang?>&request_uri=<?php echo $request_uri?>', '', 
      'dialogHeight:420px; dialogWidth:420px; resizable:no; status:no');
    
    if(imgSrc != null)
	{
		table_prop.tbackground.value = imgSrc;
	}
  }
  
  function Init() {
    var tProps = window.dialogArguments;
    if (tProps)
    {
      // set attribute values
      table_prop.trows.value = '3';
      table_prop.trows.disabled = true;
      table_prop.tcols.value = '3';
      table_prop.tcols.disabled = true;

      table_prop.tborder.value = tProps.border;
      table_prop.tcpad.value = tProps.cellPadding;
      table_prop.tcspc.value = tProps.cellSpacing;
      table_prop.tbgcolor.value = tProps.bgColor;
      table_prop.color_sample.style.backgroundColor = table_prop.tbgcolor.value;
	  table_prop.tbackground.value = tProps.background;
      if (tProps.width) {
        if (!isNaN(tProps.width) || (tProps.width.substr(tProps.width.length-2,2).toLowerCase() == "px"))
        {
          // pixels
          if (!isNaN(tProps.width))
            table_prop.twidth.value = tProps.width;
          else
            table_prop.twidth.value = tProps.width.substr(0,tProps.width.length-2);
          table_prop.twunits.options[0].selected = false;
          table_prop.twunits.options[1].selected = true;
        }
        else
        {
          // percents
          table_prop.twidth.value = tProps.width.substr(0,tProps.width.length-1);
          table_prop.twunits.options[0].selected = true;
          table_prop.twunits.options[1].selected = false;
        }
      }
      if (tProps.height) {
        if (!isNaN(tProps.height) || (tProps.height.substr(tProps.height.length-2,2).toLowerCase() == "px"))
        {
          // pixels
          if (!isNaN(tProps.height))
            table_prop.theight.value = tProps.height;
          else
            table_prop.theight.value = tProps.height.substr(0,tProps.height.length-2);
          table_prop.thunits.options[0].selected = false;
          table_prop.thunits.options[1].selected = true;
        }
        else
        {
          // percents
          table_prop.theight.value = tProps.height.substr(0,tProps.height.length-1);
          table_prop.thunits.options[0].selected = true;
          table_prop.thunits.options[1].selected = false;
        }
      }
      if (tProps.className) {
        table_prop.tcssclass.value = tProps.className;
		css_class_changed();
      }
    }
    else
    {
      // set default values
      table_prop.trows.value = '3';
      table_prop.tcols.value = '3';
      table_prop.tborder.value = '1';
    }
    resizeDialogToContent();
  }
  
  function validateParams()
  {
    // check whether rows and cols are integers
    if (isNaN(parseInt(table_prop.trows.value)))
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_rows_nan')?>');
      table_prop.trows.focus();
      return false;
    }
    if (isNaN(parseInt(table_prop.tcols.value)))
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_columns_nan')?>');
      table_prop.tcols.focus();
      return false;
    }
    // check width and height
    if (isNaN(parseInt(table_prop.twidth.value)) && table_prop.twidth.value != '')
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_width_nan')?>');
      table_prop.twidth.focus();
      return false;
    }
    if (isNaN(parseInt(table_prop.theight.value)) && table_prop.theight.value != '')
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_height_nan')?>');
      table_prop.theight.focus();
      return false;
    }
    // check border, padding and spacing
    if (isNaN(parseInt(table_prop.tborder.value)) && table_prop.tborder.value != '')
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_border_nan')?>');
      table_prop.tborder.focus();
      return false;
    }
    if (isNaN(parseInt(table_prop.tcpad.value)) && table_prop.tcpad.value != '')
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_cellpadding_nan')?>');
      table_prop.tcpad.focus();
      return false;
    }
    if (isNaN(parseInt(table_prop.tcspc.value)) && table_prop.tcspc.value != '')
    {
      alert('<?php echo $l->m('error').': '.$l->m('error_cellspacing_nan')?>');
      table_prop.tcspc.focus();
      return false;
    }
    
    return true;
  }
  
  function okClick() {
    // validate paramters
    if (validateParams())    
    {
      var newtable = {};
      newtable.className = (table_prop.tcssclass.value != 'default')?table_prop.tcssclass.value:null;
      newtable.cols = table_prop.tcols.value;
      newtable.rows = table_prop.trows.value;
	  if (!table_prop.twidth.disabled)
	  {
	      newtable.width = (table_prop.twidth.value)?(table_prop.twidth.value + table_prop.twunits.value):'';
	      newtable.height = (table_prop.theight.value)?(table_prop.theight.value + table_prop.thunits.value):'';
	      newtable.border = table_prop.tborder.value;
	      newtable.cellPadding = table_prop.tcpad.value;
	      newtable.cellSpacing = table_prop.tcspc.value;
	      newtable.bgColor = table_prop.tbgcolor.value;
		  newtable.background = table_prop.tbackground.value;
	  }
      window.returnValue = newtable;
      window.close();
    }
  }

  function cancelClick() {
    window.close();
  }
  
  function setSample()
  {
    try {
      table_prop.color_sample.style.backgroundColor = table_prop.tbgcolor.value;
    }
    catch (excp) {}
  }
  
  function css_class_changed()
  {
  	if (<?php echo (isset($spaw_disable_style_controls) && $spaw_disable_style_controls)?'true':'false'?>)
	{
	  	// disable/enable non-css class controls
		if (table_prop.tcssclass.value && table_prop.tcssclass.value!='default')
		{
			// disable all controls
			table_prop.twidth.disabled = true;
			table_prop.twunits.disabled = true;
			table_prop.theight.disabled = true;
			table_prop.thunits.disabled = true;
			table_prop.tborder.disabled = true;
			table_prop.tcpad.disabled = true;
			table_prop.tcspc.disabled = true;
			table_prop.tbgcolor.disabled = true;
			table_prop.tcolorpicker.src = '<?php echo $theme_path.'img/'?>tb_colorpicker_off.gif';
			table_prop.tcolorpicker.disabled = true;
			table_prop.tbackground.disabled = true;
			table_prop.timg_picker.src = '<?php echo $theme_path.'img/'?>tb_image_insert_off.gif';
			table_prop.timg_picker.disabled = true;
		}
		else
		{
			// enable all controls
			table_prop.twidth.disabled = false;
			table_prop.twunits.disabled = false;
			table_prop.theight.disabled = false;
			table_prop.thunits.disabled = false;
			table_prop.tborder.disabled = false;
			table_prop.tcpad.disabled = false;
			table_prop.tcspc.disabled = false;
			table_prop.tbgcolor.disabled = false;
			table_prop.tcolorpicker.src = '<?php echo $theme_path.'img/'?>tb_colorpicker.gif';
			table_prop.tcolorpicker.disabled = false;
			table_prop.tbackground.disabled = false;
			table_prop.timg_picker.src = '<?php echo $theme_path.'img/'?>tb_image_insert.gif';
			table_prop.timg_picker.disabled = false;
		}
	}
  }
  
  //-->
  </script>
</head>

<body onLoad="Init()" dir="<?php echo $l->getDir();?>">
<table border="0" cellspacing="0" cellpadding="2" width="336">
<form name="table_prop">
<tr>
  <td><?php echo $l->m('rows')?>:</td>
  <td><input type="text" name="trows" size="3" maxlength="3" class="input_small"></td>
  <td><?php echo $l->m('columns')?>:</td>
  <td><input type="text" name="tcols" size="3" maxlenght="3" class="input_small"></td>
</tr>
<tr>
  <td><?php echo $l->m('css_class')?>:</td>
  <td colspan="3">
    <select id="tcssclass" name="tcssclass" size="1" class="input" onchange="css_class_changed();">
	<?php
	foreach($spaw_dropdown_data["table_style"] as $key => $text)
	{
		echo '<option value="'.$key.'">'.$text.'</option>'."\n";
	}
	?>
    </select>
  </td>
</tr>
<tr>
  <td><?php echo $l->m('width')?>:</td>
  <td nowrap>
    <input type="text" name="twidth" size="3" maxlenght="3" class="input_small">
    <select size="1" name="twunits" class="input_small">
      <option value="%">%</option>
      <option value="px">px</option>
    </select>
  </td>
  <td><?php echo $l->m('height')?>:</td>
  <td nowrap>
    <input type="text" name="theight" size="3" maxlenght="3" class="input_small">
    <select size="1" name="thunits" class="input_small">
      <option value="%">%</option>
      <option value="px">px</option>
    </select>
  </td>
</tr>
<tr>
  <td><?php echo $l->m('border')?>:</td>
  <td colspan="3"><input type="text" name="tborder" size="2" maxlenght="2" class="input_small"> <?php echo $l->m('pixels')?></td>
</tr>
<tr>
  <td><?php echo $l->m('cellpadding')?>:</td>
  <td><input type="text" name="tcpad" size="3" maxlenght="3" class="input_small"></td>
  <td><?php echo $l->m('cellspacing')?>:</td>
  <td><input type="text" name="tcspc" size="3" maxlenght="3" class="input_small"></td>
</tr>
<tr>
  <td colspan="4"><?php echo $l->m('bg_color')?>: <img src="spacer.gif" id="color_sample" border="1" width="30" height="18" align="absbottom">&nbsp;<input type="text" name="tbgcolor" size="7" maxlenght="7" class="input_color" onKeyUp="setSample()">&nbsp;
  <img id="tcolorpicker" src="<?php echo $theme_path.'img/'?>tb_colorpicker.gif" border="0" onClick="showColorPicker(tbgcolor.value)" align="absbottom">
  </td>
</tr>
<tr>
  <td colspan="4">
	<?php echo $l->m('background')?>: <input type="text" name="tbackground" size="20" class="input" >&nbsp;<img id="timg_picker" src="<?php echo $theme_path.'img/'?>tb_image_insert.gif" border="0" onClick="showImgPicker();" align="absbottom">	
  </td>
</tr>
<tr>
<td colspan="4" nowrap>
<hr width="100%">
</td>
</tr>
<tr>
<td colspan="4" align="right" valign="bottom" nowrap>
<input type="button" value="<?php echo $l->m('ok')?>" onClick="okClick()" class="bt">
<input type="button" value="<?php echo $l->m('cancel')?>" onClick="cancelClick()" class="bt">
</td>
</tr>
</form>
</table>

</body>
</html>
