<?
    include( "../main.inc.php" );
    if( !$CUSER->id )
  {
     header( "Location: ".$CFG->wwwroot."index.php?".get_parameters() );
     exit;
  }
   $page_title = "Edit popup";
    include( $CFG->templatesdir."header.home.inc.php" );
    $show_message = 0;
   ?>
   <table width="100%"  border="0" cellspacing="0" cellpadding="0"  align="center">
   <tr>
   <td><object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="751" height="229">
              <param name="movie" value="<?=$CFG->wwwroot?>games/POISSONS/out/poissons.swf">
              <param name="quality" value="high">
              <embed src="<?=$CFG->wwwroot?>games/POISSONS/out/poissons.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="751" height="229"></embed>
            </object></td>
            </tr>
            </table>
   <?
include( $CFG->templatesdir. "footer.home.inc.php" );
?>