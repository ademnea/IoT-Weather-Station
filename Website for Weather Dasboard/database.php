<?php error_reporting (E_ALL ^ E_NOTICE);
   
  $humidity = $_GET['humidity'];
  $temperature = $_GET['temperature'];
  $rainfall = $_GET['rainfall'];
  $wind_speed = $_GET['wind_speed'];
  $wind_direction = $_GET['wind_direction'];
  $dailyrainfall = $_GET['dailyrainfall'];
  $pressure = $_GET['pressure'];

  $username = 'id18942558_espdatabase';
  $password = 'ArnoldMuhairwe@30';
  $servername = 'localhost';
  $dbname = 'id18942558_sensordata_table';
  $tablename = 'SensorData';
  
  // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
   if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

$query = "INSERT INTO $tablename (Temperature, Humidity, Pressure, Rainfall, WindSpeed, WindDirection, DailyRainfall) VALUES ('$temperature', '$humidity', '$pressure', '$rainfall', '$wind_speed', '$wind_direction', '$dailyrainfall')";

if (mysqli_query($conn, $query)) {
    return "Weather Data has been Uploaded Successfully.";
} else {
    return "Error: " . $sql . "<br>" . mysqli_error($conn);
}
mysqli_close($conn);


 
    function getAllReadings($limit) {
    global $servername, $username, $password, $dbname;

    // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $query = "SELECT Temperature, Humidity, Pressure, Rainfall, WindSpeed, WindDirection, DailyRainfall, reading_time FROM {$tablename} order by reading_time desc limit " . $limit;
    if ($result = $conn->query($query)) {
      return $result;
    }
    else {
      return false;
    }
    mysqli_close($conn);
  }
  
  function getLastReadings() {
    global $servername, $username, $password, $dbname;

    // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $query = "SELECT Temperature, Humidity, Pressure, Rainfall, WindSpeed, WindDirection, DailyRainfall, reading_time FROM {$tablename} order by reading_time desc limit 1" ;
    if ($result = $conn->query($query)) {
      return $result->fetch_assoc();
    }
    else {
      return false;
    }
    mysqli_close($conn);
  }

  function minReading($limit, $value) {
     global $servername, $username, $password, $dbname;

    // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $query = "SELECT MIN(" . $value . ") AS minimumValue FROM (SELECT " . $value . " FROM {$tablename} order by reading_time desc limit " . $limit . ") AS min";
    if ($result = $conn->query($query)) {
      return $result->fetch_assoc();
    }
    else {
      return false;
    }
   mysqli_close($conn);
  }

  function maxReading($limit, $value) {
     global $servername, $username, $password, $dbname;

    // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $query = "SELECT MAX(" . $value . ") AS maximumValue FROM (SELECT " . $value . " FROM {$tablename} order by reading_time desc limit " . $limit . ") AS max";
    if ($result = $conn->query($query)) {
      return $result->fetch_assoc();
    }
    else {
      return false;
    }
    mysqli_close($conn);
  }

  function avgReading($limit, $value) {
     global $servername, $username, $password, $dbname;

    // Create connection
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $query = "SELECT AVG(" . $value . ") AS averageValue FROM (SELECT " . $value . " FROM {$tablename} order by reading_time desc limit " . $limit . ") AS avg";
    if ($result = $conn->query($query)) {
      return $result->fetch_assoc();
    }
    else {
      return false;
    }
    mysqli_close($conn);
  }

?>
