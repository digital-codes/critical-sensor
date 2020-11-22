<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");

/* fill database paramteres in config.ini */
/*$cfg = parse_ini_file("../../files/iot/config.ini", false);*/
$cfg = parse_ini_file("config.ini", false);

/* we can check parameters via a filter like so: */
/* multiple parms: see https://www.php.net/manual/de/function.filter-input-array.php */
/* for testing with http, call like:
http GET  "http://127.0.0.1:8080" sens==123 last==2
When add_empty (3. parm) is true, all parms will be returned
missing are null
values failing the filter condition (e.g. validate int) are FALSE
*/

$parms = array(
	"last" => FILTER_VALIDATE_INT,
	"days" => FILTER_VALIDATE_INT,
	"sens" => FILTER_VALIDATE_INT
);

$args = filter_input_array(INPUT_GET, $parms,TRUE);

$last = (false | ($args["last"] > 0));
// default period: 14 
$per = ($args["days"] ? $args["days"] : 14); /* days */

// date computation
$fmt = "Y-m-d h:m:s";
date_default_timezone_set("Europe/Berlin");
// now
$d0 = date($fmt);
//echo "Now: " . $d0 . PHP_EOL;
// target date
$d1 = date_create($d0);
date_sub($d1, date_interval_create_from_date_string($per . " days"));
$d2 = date_format($d1, $fmt);
//echo "New: " . $d2 . PHP_EOL;

$pdo = new PDO('mysql:host=' . $cfg["dbserv"] . ';dbname=' . $cfg["dbname"] , $cfg["dbuser"], $cfg["dbpass"]);
$pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE,PDO::FETCH_ASSOC);
/* $query = "SELECT * from sensors order by `index` asc"; */
/*$query = "SELECT id,count,co2,bat,pres,hum,temp,light,rssi,rfu,date,pkt,rep from sensors order by `index` asc"; */
$query = "SELECT id,req,count,co2,bat,pres,hum,temp,light,rssi,date,pkt,rep from sensors";
/* options to select sensor and to get only the last entry */
if ($args["sens"]) 
	$query .= " where id = " . $args["sens"];
else
	$query .= " where id >= 0"; // defaults to all

// period
$query .= " and `date` > \"" . $d2 . "\"";

$query .= " order by `date`";

if ($args["last"]) 
	$query .= " desc limit 1";
else
	$query .= " asc";


$statement = $pdo->query($query);
$data = array();
foreach ($statement as $row) {
    array_push($data,$row);
}
echo json_encode($data);

?>
