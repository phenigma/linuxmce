<?php 
// ================================================
// SPAW PHP WYSIWYG editor control
// ================================================
// Hyperlink properties dialog
// ================================================
// Developed: Alan Mendelevich, alan@solmetra.lt
// Copyright: Solmetra (c)2003 All rights reserved.
// ------------------------------------------------
//                                www.solmetra.com
// ================================================
// v.1.0, 2004-01-20
// ================================================

// include wysiwyg config
include '../config/spaw_control.config.php';
include $spaw_root.'class/lang.class.php';

$theme = empty($HTTP_GET_VARS['theme'])?$spaw_default_theme:$HTTP_GET_VARS['theme'];
$theme_path = $spaw_dir.'lib/themes/'.$theme.'/';

$l = new SPAW_Lang($HTTP_GET_VARS['lang']);
$l->setBlock('hyperlink');
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
  <title><?php echo $l->m('title')?></title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Content-Type" content="text/html; charset=<?php echo $l->getCharset()?>">
  <link rel="stylesheet" type="text/css" href="<?php echo $theme_path.'css/'?>dialog.css">
  <script language="javascript" src="utils.js"></script>
  
  <script language="javascript">
  <!--  
  function Init() {
    var aProps = window.dialogArguments;
    if (aProps && (aProps.href || aProps.name))
    {
      // set attribute values
      if (aProps.href) {
        a_prop.chref.value = aProps.href;
      }
      if (aProps.name) {
        a_prop.cname.value = aProps.name;
      }

      setTarget(aProps.target);
      
      if (aProps.title) {
        a_prop.ctitle.value = aProps.title;
      }
    }
	var found = setAnchors(aProps.anchors, aProps.href);
	
	var atype = "link";
	if (aProps.name)
	{
		atype = "anchor";	
	}
	else if (found)
	{
		atype = "link2anchor";
	}
	if (a_prop.canchor.options.length<=1)
	{
		// no anchors found, disable link to anchor feature
		a_prop.catype.options.remove(2);
	}
	changeType(atype);    
  }
  
  function validateParams()
  {
    return true;
  }
  
  function okClick() {
    // validate paramters
    if (validateParams())    
    {
      var aProps = {};
	  if (a_prop.catype.options[a_prop.catype.selectedIndex].value == "link2anchor")
	      aProps.href = (a_prop.canchor.options[a_prop.canchor.selectedIndex].value)?(a_prop.canchor.options[a_prop.canchor.selectedIndex].value):'';
	  else
	      aProps.href = (a_prop.chref.value)?(a_prop.chref.value):'';
      aProps.name = (a_prop.cname.value)?(a_prop.cname.value):'';
      aProps.target = (a_prop.ctarget.value)?(a_prop.ctarget.value):'';
      aProps.title = (a_prop.ctitle.value)?(a_prop.ctitle.value):'';

      window.returnValue = aProps;
      window.close();
    }
  }

  function cancelClick() {
    window.close();
  }
  
  
  function setTarget(target)
  {
    for (i=0; i<a_prop.ctarget.options.length; i++)  
    {
      tg = a_prop.ctarget.options.item(i);
      if (tg.value == target.toLowerCase()) {
        a_prop.ctarget.selectedIndex = tg.index;
      }
    }
  }

  function setAnchors(anchors, anchor)
  {
  	var found = false;
  	for(var i=0; i<anchors.length; i++)
	{
		var opt = document.createElement("OPTION");
		a_prop.canchor.options.add(opt);
		opt.innerText = anchors[i];
		opt.value = '#'+anchors[i];
		if (opt.value == anchor)
		{
			opt.selected = true;
			found = true;
		}
	}
	return found;
  }

  function changeType(new_type)
  {
  	a_prop.catype.selectedIndex = 0;
	if (new_type == "anchor")
	{
		a_prop.catype.selectedIndex = 1;
	}
	else if (new_type == "link2anchor")
	{
		a_prop.catype.selectedIndex = 2;
	}
		
  	url_row.style.display = new_type=="link"?"inline":"none";
  	name_row.style.display = new_type=="anchor"?"inline":"none";
  	anchor_row.style.display = new_type=="link2anchor"?"inline":"none";
  	target_row.style.display = (new_type=="link"||new_type=="link2anchor")?"inline":"none";
	resizeDialogToContent();
  }
  //-->
  </script>
</head>

<body onLoad="Init()" dir="<?php echo $l->getDir();?>">
<table border="0" cellspacing="0" cellpadding="2" width="336">
<form name="a_prop">
<tr>
  <td><?php echo $l->m('a_type')?>:</td>
  <td>
  <select name="catype" size="1" class="input" onchange="changeType(this.options[this.selectedIndex].value);">
  	<option value="link"><?php echo $l->m('type_link')?></option>
  	<option value="anchor"><?php echo $l->m('type_anchor')?></option>
  	<option value="link2anchor"><?php echo $l->m('type_link2anchor')?></option>
  </select>
  </td>
</tr>
<tr id="url_row">
  <td><?php echo $l->m('url')?>:</td>
  <td><input type="text" name="chref" class="input" size="32"></td>
</tr>
<tr id="name_row">
  <td><?php echo $l->m('name')?>:</td>
  <td><input type="text" name="cname" class="input" size="32"></td>
</tr>
<tr id="anchor_row">
  <td><?php echo $l->m('anchors')?>:</td>
  <td>
  <select name="canchor" size="1" class="input">
  	<option></option>
  </select>
  </td>
</tr>
<tr id="target_row">
  <td><?php echo $l->m('target')?>:</td>
  <td align="left">
  <select name="ctarget" size="1" class="input">
    <?php
		foreach($spaw_a_targets as $key=>$value)
		{
			if ($l->m($key,'hyperlink_targets')!='') 
				$value = $l->m($key,'hyperlink_targets');
			echo '<option value="'.$key.'">'.$value."</option>";
		}
	?>
  </select>
  </td>
</tr>
<tr id="title_row">
  <td><?php echo $l->m('title_attr')?>:</td>
  <td align="left">
    <input type="text" name="ctitle" size="32" class="input">
  </td>
</tr>
<tr>
<td colspan="2" nowrap>
<hr width="100%">
</td>
</tr>
<tr>
<td colspan="2" align="right" valign="bottom" nowrap>
<input type="button" value="<?php echo $l->m('ok')?>" onClick="okClick()" class="bt">
<input type="button" value="<?php echo $l->m('cancel')?>" onClick="cancelClick()" class="bt">
</td>
</tr>
</form>
</table>

</body>
</html>
