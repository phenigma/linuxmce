  <?php
  /*
  setEa - temp utility to set the ea for a new Qorbiter
  */

  //initialization area
  if (isset($_GET["d"])) {
    $deviceID = $_GET['d'];
    if($deviceID==""){
   echo "No Device set";
    return;
    }
  } else {
    die("Please specify the device ID with d=xxxx");
  }
/*
Need to get children devices and do the same as the parent.
*/
echo "Starting";
 $deviceName="Mobile QOrbiter";
 $mobileRoom=-1;
 $mobileEa=-1;
 $server = "localhost";
 $mysqlUser = "root";
 $mysqlPass = "";
 $conn = mysql_connect($server, $mysqlUser, $mysqlPass);
 mysql_select_db("pluto_main", $conn);
 $dupe;

 if($conn){
  $installSql = "SELECT * FROM `Installation` ";
  $iRes=mysql_query($installSql, $conn) or die(mysql_error($conn));
  $inst="";
  
    while($row=mysql_fetch_array($iRes)){
    $inst=$row['PK_Installation'];    
    }
    
    $nameSql = "SELECT `Description` FROM `Device` WHERE `PK_Device` =".$deviceID." LIMIT 0, 30 ";
    $nameRes = mysql_query($nameSql, $conn) or die(mysql_error($conn));
    
    while($row=mysql_fetch_array($nameRes)){
    $deviceName=$row['Description'];   
    }

  $installation=$inst;
  echo "Installation is ".$installation."<br>Device: ".$deviceName." <br> ID::".$deviceID."<br>";
  
    if($installation==""){
    return;
  }
  echo "Connection Found, Starting<br>";

  $roomSql = "SELECT * FROM `Room` WHERE `Description` like 'Mobile' LIMIT 0, 30 ";
  echo "Checking for existing mobile orbiter room <br>";
  $result = mysql_query($roomSql,  $conn) or die (mysql_error($conn));
  $cnt = mysql_num_rows($result);
  
    if($cnt===0){
      global $mobileRoom;
      $iRoomSql ="INSERT INTO `pluto_main`.`Room` (`PK_Room`, `FK_Installation`, `FK_RoomType`, `Description`, `FK_Icon`, `ManuallyConfigureEA`, `HideFromOrbiter`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, ".$installation.", '9', 'Mobile', NULL, '1', '1', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
      $result2=mysql_query($iRoomSql,  $conn) or die (mysql_error($conn));
      $lastId = mysql_insert_id($conn);

      $chkSql=$roomSql = "SELECT * FROM `Room` WHERE `PK_Room` = ".$lastId;
      $result3 = mysql_query($chkSql, $conn) or die(mysql_error($conn));
	while ($row = mysql_fetch_array($result3)){
	  if($row['PK_Room']){
	    $mobileRoom = $row['PK_Room'];
	    echo "Mobile QOrbiters Room is ".$mobileRoom;
          }
        }
      echo "Not Found, so we've added it. Setting up entertain area now<br>";	
      } else {
	while ($row = mysql_fetch_array($result)){
	  if($row['PK_Room']){
	    $mobileRoom = $row['PK_Room'];
	    echo "Mobile QOrbiters Room is ".$mobileRoom."<br>" ;
         }
        }
	  echo " Setting up entertain area now <br>";
      }
    $dupe = checkForDupe($conn, $deviceID);
      if($dupe)	{
	echo "Duplicate, will set to ea for  device #".$deviceID."<br>";
	updateEntertainArea($conn, $deviceID, $mobileRoom);
	} else {
	echo "No Matching Ea found, will set device #".$deviceID." to new EA<br>";
	setEntertainArea($conn, $deviceID, $mobileRoom);
	}
      }


function checkForDupe($connect, $device){
global $mobileRoom;
global $mobileEa; 
$status = false;
echo "<b>Checking for duplicate Ea for device:: " . $deviceName. "</b><br>";
$sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '".$deviceName."' LIMIT 0, 30 ";
$result = mysql_query($sql, $connect) or die(mysql_error($connect));
$i= 0;
$eaRoom=-1;
$fixitSql="";
while ($row = mysql_fetch_array($result)){
  if($row['Description']){    
     
    if($row['FK_Room'] != $mobileRoom){
        $status = true;
     $eaRoom=$row['FK_Room'];
     $fixitSql ="UPDATE EntertainArea SET FK_Room = ".$mobileRoom." WHERE PK_EntertainArea = ".$row["PK_EntertainArea"].";";
    } else {
      $mobileEa = $row['PK_EntertainArea'];
      echo "Found current ea for device #".$device." ==> ".$mobileEa.". It set to the proper room<br>";
      die("nothin to fix");
      }	
    }
  }
  
  if(status && $eaRoom!=-1){
  echo $fixitSql."<br>";
   $fixRes = mysql_query($fixitSql, $connect);
    echo "Location Mismatch. EA room is ".$eaRoom." but Mobile QOrbiters room is ".$mobileRoom." fixing <BR> ";
     
     
      $rc= mysql_affected_rows();
      echo $rc;
      echo "Updated EA for device ".$deviceName."<br>";
      die("no mas trabajo.");
      
  }
return false;
}

function setEntertainArea($connect, $device, $location){
global $deviceName;
echo "Setting device ".$deviceName." to EA: ".$location."<br>";
$sql = "INSERT INTO `pluto_main`.`EntertainArea` (`PK_EntertainArea`, `FK_Room`, `Only1Stream`, `Description`, `Private`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, ".$location.", '0','".$deviceName."' ,'0', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
$result = mysql_query($sql, $connect) or die(mysql_error($connect));
$id=mysql_insert_id($conn);
echo $id;
}

function updateEntertainArea($connect, $device, $location){
echo "Updating device to existing EA <br>";

global $mobileEa;
  if($mobileEa == -1){
  echo "invalid ea, exiting updateEntertainArea()";
  return;
  }
echo "updating...<br>";
$updatesql = "UPDATE `pluto_main`.`Device_EntertainArea` SET `FK_EntertainArea` = ".$mobileEa. " WHERE `Device_EntertainArea`.`FK_Device` = ".$deviceID;
$result = mysql_query($updatesql, $connect) or die(mysql_error($conn));
echo "updated. <br>";
}

?>
