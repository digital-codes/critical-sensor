<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");

/*$pdo = new PDO('mysql:host=localhost;dbname=cz', 'cz', '1234abcd');*/
/*$cfg = parse_ini_file("../../files/iot/config.ini", false);*/
$cfg = parse_ini_file("config.ini", false);
$pdo = new PDO('mysql:host=' . $cfg["dbserv"] . ';dbname=' . $cfg["dbname"] , $cfg["dbuser"], $cfg["dbpass"]);

$pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE,PDO::FETCH_ASSOC);
/* $query = "SELECT * from sensors order by `index` asc"; */
$query = "SELECT id,count,co2,bat,pres,hum,temp,light,rssi,rfu,date,pkt,rep from sensors order by `index` asc";
$statement = $pdo->query($query);
$data = array();
foreach ($statement as $row) {
    array_push($data,$row);
}
echo json_encode($data);

?>
