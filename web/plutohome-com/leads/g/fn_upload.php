<?
function check_upload(&$file_post, $file_type=false)
{
	$message = '';
	if ($file_post['tmp_name']=='none')
	{
		$message.='ERROR: no file uploaded <br />';
	}
	elseif ($file_post['size']==0)
	{
		$message.='ERROR: empty file uploaded <br />';
	}
	elseif (is_string($file_type) && substr($file_post['type'], 0, strlen($file_type))!=$file_type)
	{
		$message.="ERROR: file is not of type $file_type<br />";
	}
	elseif (!is_uploaded_file($file_post['tmp_name']))
	{
		$message.="ERROR: possible file upload attack. Filename: $file_post[name]<br />";
	}
	if ($file_post['error'])
	{
		$message.='ERROR uploading file: ';
		if ($file_post['error'] < 3) $message.='file is too large';
		elseif($file_post['error'] == 3) $message.='The uploaded file was only partially uploaded';
		else $message.='No file was uploaded';
		$message.='<br />';
	} 
	return $message;
}

function ftp_connect_login($host, $user='', $pass='')
{
	if (is_array($host))
	{
		$user = $host['user'];
		$pass = $host['pass'];
		$host = $host['host'];
	}
	
	$message = '';
	$conn = ftp_connect($host);
	if ($conn)
	{
		if (! @ftp_login($conn,$user,$pass))
		{
			$message.="ERROR: Could not log on as $user<br />";
			ftp_quit($conn);
		}
	}
	else $message.='ERROR: could not connect to ftp server<br />';		
	return ($message=='') ? $conn : $message;
}