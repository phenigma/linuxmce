<?
write_file('/home/users/vali/work/web/plutohome-com/support/UserFiles/bubu.txt','Path: ');

function write_file($filename, $content){

	if (!$handle = fopen($filename, 'a')) {
         echo "Cannot open file ($filename)";
         exit;
   }

   // Write $somecontent to our opened file.
   if (fwrite($handle, $content) === FALSE) {
       echo "Cannot write to file ($filename)";
       exit;
   }
	fclose($handle);

}
?>