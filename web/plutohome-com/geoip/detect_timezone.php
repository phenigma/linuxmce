<?php
require_once("../globalconfig/globalconfig.inc.php");
define('main_path',dirname(__FILE__));
define('host',$dbPlutoMainServer);
define('user',$dbPlutoMainUser);
define('password',$dbPlutoMainPass);
define('db',$dbPlutoMainDatabase);

if (!isset($_SERVER["REMOTE_ADDR"])) exit;
//print $_SERVER["REMOTE_ADDR"];
include("geoipcity.inc");

//geoip_load_shared_mem(main_path."/GeoLiteCity.dat");
//$gi = geoip_open(main_path."/GeoLiteCity.dat",GEOIP_SHARED_MEMORY);

$gi = geoip_open(main_path."/GeoLiteCity.dat",GEOIP_STANDARD);

$record = geoip_record_by_addr($gi,$_SERVER["REMOTE_ADDR"]);
//$record = geoip_record_by_addr($gi,'68.178.171.146');
$handle = @mysql_connect(host,user,password);
mysql_select_db(db,$handle);
$query="select tz.ZoneName from City c 
left join Country co on UPPER(c.FK_Country)=UPPER(co.PK_Country)
left join Region r on c.FK_Region=r.PK_Region
left join TimeZone tz on tz.PK_TimeZone=c.FK_TimeZone
where UPPER(c.City)=UPPER('".$record->city."') and UPPER(co.country_code)=UPPER('".$record->country_code."') 
and 
	(
	(UPPER(co.country_code) IN ('US', 'CA') and (UPPER(r.Code)=UPPER('".$record->region."')))
	or
	(UPPER(co.country_code) NOT IN ('US', 'CA'))
	)
	";
if(!($res=@mysql_query($query, $handle))) {
//	echo "Cannot exec\n$query\n".mysql_error($handle)."\nEXIT\n=================================\n";
	exit;
};
$tz=mysql_fetch_array($res,MYSQL_ASSOC);
print $tz['ZoneName'];
geoip_close($gi);
?>
