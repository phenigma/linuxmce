<?
function home($output,$conn){
		
$out='<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="553" height="353">
          <param name="movie" value="images/intro.swf" />
          <param name="quality" value="high" />
          <embed src="images/intro.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="553" height="353"></embed>
        </object><br><br><img src="images/main_text.gif">';



    $output->setBody($out);
    $output->setImgName("img.jpg");
    $output->setTitle(APPLICATION_NAME);
    $output->output();
}
?>