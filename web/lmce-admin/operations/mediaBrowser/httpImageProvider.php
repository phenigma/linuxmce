<?php

$pass=""; $user="root"; $db="pluto_media";
$con= mysqli_connect("localhost", $user, $pass); 	 // connection

	if (!$con) { die('Could not Connect'.mysqli_error($con)); //error messaging
	$connMessage="Fail";
	print_r($connMessage);
	};
	if ($con) { mysqli_select_db($con, "pluto_media") or die(mysqli_error($con)); //connect to media db or error out
	$connMessage="Conn Good";
	};



$test = $_GET['type'];
$val = $_GET['val'];


    if($test=="img"){
	mediaPicsImage($val);
	}
else if ($test=="atr"){
      imdbImage($_GET['file'], $val  );
	}
else if($test=="imdb"){

	imdbImage($con, $_GET['file'],36);
	} 
	else if ($test==="screensaver"){
	screensaverImg($val);
	}
	else{
	echo file_get_contents("operations/mediaBrowser/images/addAll.png");	
}



function mediaPicsImage($fileName){

$img =file_get_contents("mediapics/".$fileName);
echo $img;
}

function attributeImage($fileName, $atr){
//$test=mysql_query("SELECT * FROM Picture_Attribute WHERE FK_Attribute=\"$attribute\" ") or die (mysql_error("MYsql Error"));	

}

function imdbImage($connect, $imdbID, $type ){

$filemarker = substr_replace($imdbID, "", 0, 2);

$sql ="SELECT File_Attribute.FK_File, Picture.PK_Picture, Picture.Extension, File_Attribute.FK_Attribute, Picture_Attribute.FK_Picture, Picture_Attribute.FK_Attribute "
    . "FROM File_Attribute "
    . "JOIN Attribute "
    . "ON File_Attribute.FK_Attribute = Attribute.PK_Attribute "
    . "JOIN "
    . "Picture_Attribute "
    . "ON Picture_Attribute.FK_Attribute = Attribute.PK_Attribute "
    . "JOIN "
    . "Picture "
    . "ON Picture_Attribute.FK_Picture = Picture.PK_Picture "
    . "WHERE File_Attribute.FK_File ="
    .$filemarker
    ." && Attribute.FK_AttributeType=".$type;
    
$cleansql = mysqli_real_escape_string($connect, $sql);

$res=mysqli_query($connect, $cleansql) or die( "MySQL error ".mysql_errno().": ".mysql_error($connect)."\n<br>When executing:<br>\n$res\n<br>" );

	while($row = mysqli_fetch_array($res)){
	$file=$row['PK_Picture'].".".$row['Extension'];

	} 

echo file_get_contents("mediapics/".$file);

}

function screensaverImg($ssimg){
echo file_get_contents($ssimg);
}

?>
