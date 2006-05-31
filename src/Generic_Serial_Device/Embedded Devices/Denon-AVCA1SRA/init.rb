#31-May-06 13:14 init Denon AVC
$logFile = File.new("/var/log/pluto/DenonAVC.log", "w")
$SoundLevel = 0

#buffer for reading from serial
$line=String.new
$lineIndex=0

$waitTime = 3000      #wait time in comunication
