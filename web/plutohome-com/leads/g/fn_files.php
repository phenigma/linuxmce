<?

function file_get_extension($filename)
{
	return is_int($pos = strrpos($filename,'.')) ? substr($filename,$pos+1) : '';
}

function file_get_image($filename)
{
	if (!in_array(file_get_extension($filename),array('png','jpg','gif'))) return false;

	if (!function_exists('file_get_contents')) // PHP 4.3.0
	{
		function file_get_contents($filename) // TODO: filesize doesn't work on remote files
		{
			$handle = fopen ($filename, 'rb');
			$contents = fread ($handle, filesize($filename));
			fclose ($handle);
			return $contents;
		}
	}
	
	return file_get_contents($filename);
}