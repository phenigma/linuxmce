<?
class oo_file
{
	var $fp;
	
	/**
	 * @param $filename string
	 * @param $mode string
	 * @param $use_include_path bool
	 * @param $context resource
	 */
	function open($filename, $mode) // , $use_include_path=null, $context=null
	{
		$this->fp = fopen($filename, $mode); // , $use_include_path, $context
		return $this->fp;
	}
	function close()
	{
		return fclose($this->fp);
	}
	function eof()
	{
		return feof($this->fp);
	}
	function flush()
	{
		return fflush($this->fp);
	}
	function getc()
	{
		return fgetc($this->fp);
	}
	/**
	 * @param $length int
	 * @param $delimiter string
	 * @param $enclosure string
	 */
	function getcsv($length) // , $delimiter=null, $enclosure=null
	{
		return fgetcsv($this->fp, $length); // , $delimiter, $enclosure
	}
	/**
	 * @param $length int
	 */
	function gets($length)
	{
		return fgets($this->fp, $length);
	}
	/**
	 * @param $length int
	 * @param $allowable_tags string
	 */
	function getss($length) // , $allowable_tags=null
	{
		return fgetss($this->fp, $length); // , $allowable_tags
	}
	
	//function lock(){return flock($this->fp, $operation, $wouldblock);}

	function passthru()
	{
		return fpassthru($this->fp);
	}
	/**
	 * @param $str string
	 * @param $length int
	 */
	function write($str) // , $length=null
	{
		return fwrite($this->fp, $str); // , $length
	}
	/**
	 * @param $length int
	 */
	function fread($length)
	{
		return fread($this->fp, $length);
	}
	/**
	 * @param $offset int
	 * @param $whence int
	 */
	function seek($offset) // , $whence=null
	{
		return fseek($this->fp, $offset); // , $whence
	}
	function tell()
	{
		return ftell($this->fp);
	}
	function stat()
	{
		return fstat($this->fp);
	}
	/**
	 * @param $size int
	 */
	function truncate($size)
	{
		return ftruncate($this->fp,$size);
	}
	/**
	 * @param $buffer int
	 */
	function set_buffer($buffer)
	{
		return set_file_buffer($this->fp, $buffer);
	}
	function rewind()
	{
		return rewind($this->fp);
	}
}