<?php
include_once'database.php';
$humidity = $_GET['humidity'];
$temperature = $_GET['temperature'];
$rainfall = $_GET['rainfall'];
$wind_speed = $_GET['wind_speed'];
$wind_direction = $_GET['wind_direction'];
$dailyrainfall = $_GET['dailyrainfall'];
$pressure = $_GET['pressure'];

$result = insertReading($temperature, $humidity, $pressure, $rainfall, $wind_speed, $wind_direction, $dailyrainfall);
echo $result;

$data = trim($data);
$data = stripslashes($data);
$data = htmlspecialchars($data);
return $data;
?>
