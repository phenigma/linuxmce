<?PHP
include 'fckeditor.php';
$fckEdit = new FCKeditor;
//$fckEdit->BasePath = '/netours/scripts/fckeditor/';
$fckEdit->ToolbarSet = 'Accessibility' ;
$fckEdit->CreateFCKeditor( 'EditorDefault', '100%', 150 ) ;
?>