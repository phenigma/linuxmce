<?php 
// ================================================
// SPAW PHP WYSIWYG editor control
// ================================================
// Configuration file
// ================================================
// Developed: Alan Mendelevich, alan@solmetra.lt
// Copyright: Solmetra (c)2003 All rights reserved.
// ------------------------------------------------
//                                www.solmetra.com
// ================================================
// v.1.0, 2003-03-27
// ================================================

// directory where spaw files are located
$GLOBALS['spaw_dir'] = $spaw_dir = '/pluto_home/javascripts/spaw/';

// base url for images
$spaw_base_url = 'http://spaw/';

if (!ereg('/$', $_SERVER['DOCUMENT_ROOT']))
  $GLOBALS['spaw_root'] = $_SERVER['DOCUMENT_ROOT'].$spaw_dir;
else
  $GLOBALS['spaw_root'] = $_SERVER['DOCUMENT_ROOT'].substr($spaw_dir,1,strlen($spaw_dir)-1);
  

$GLOBALS['spaw_default_toolbars'] = 'default';
$GLOBALS['spaw_default_theme'] = 'default';
$GLOBALS['spaw_default_lang'] = 'en';
$GLOBALS['spaw_default_css_stylesheet'] = $spaw_dir.'wysiwyg.css';

// add javascript inline or via separate file
$GLOBALS['spaw_inline_js'] = false;

// use active toolbar (reflecting current style) or static
$GLOBALS['spaw_active_toolbar'] = true;

// default dropdown content
$GLOBALS['spaw_dropdown_data']['style']['default'] = 'Normal';

$GLOBALS['spaw_dropdown_data']['table_style']['default'] = 'Normal';

$GLOBALS['spaw_dropdown_data']['td_style']['default'] = 'Normal';

$GLOBALS['spaw_dropdown_data']['font']['Arial'] = 'Arial';
$GLOBALS['spaw_dropdown_data']['font']['Courier'] = 'Courier';
$GLOBALS['spaw_dropdown_data']['font']['Tahoma'] = 'Tahoma';
$GLOBALS['spaw_dropdown_data']['font']['Times New Roman'] = 'Times';
$GLOBALS['spaw_dropdown_data']['font']['Verdana'] = 'Verdana';

$GLOBALS['spaw_dropdown_data']['fontsize']['1'] = '1';
$GLOBALS['spaw_dropdown_data']['fontsize']['2'] = '2';
$GLOBALS['spaw_dropdown_data']['fontsize']['3'] = '3';
$GLOBALS['spaw_dropdown_data']['fontsize']['4'] = '4';
$GLOBALS['spaw_dropdown_data']['fontsize']['5'] = '5';
$GLOBALS['spaw_dropdown_data']['fontsize']['6'] = '6';

$GLOBALS['spaw_dropdown_data']['paragraph']['Normal'] = 'Normal';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 1'] = 'Heading 1';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 2'] = 'Heading 2';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 3'] = 'Heading 3';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 4'] = 'Heading 4';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 5'] = 'Heading 5';
$GLOBALS['spaw_dropdown_data']['paragraph']['Heading 6'] = 'Heading 6';

// image library related config

// allowed extentions for uploaded image files
$GLOBALS['spaw_valid_imgs'] = array('gif', 'jpg', 'jpeg', 'png');

// allow upload in image library
$GLOBALS['spaw_upload_allowed'] = true;

// allow delete in image library
$GLOBALS['spaw_img_delete_allowed'] = true;

// image libraries
$GLOBALS['spaw_imglibs'] = array(
  array(
    'value'   => 'you/need/to/change/this/',
    'text'    => 'Not configured',
  ),
  array(
    'value'   => 'you/need/to/change/this/too/',
    'text'    => 'Not configured',
  ),
);
// file to include in img_library.php (useful for setting $spaw_imglibs dynamically
// $spaw_imglib_include = '';

// allowed hyperlink targets
$GLOBALS['spaw_a_targets']['_self'] = 'Self';
$GLOBALS['spaw_a_targets']['_blank'] = 'Blank';
$GLOBALS['spaw_a_targets']['_top'] = 'Top';
$GLOBALS['spaw_a_targets']['_parent'] = 'Parent';

// image popup script url
$GLOBALS['spaw_img_popup_url'] = $spaw_dir.'img_popup.php';

// internal link script url
$GLOBALS['spaw_internal_link_script'] = 'url to your internal link selection script';

// disables style related controls in dialogs when css class is selected
$GLOBALS['spaw_disable_style_controls'] = true;

?>