<?php


$verz="1.0";
$comPort = "/dev/cu.usbmodem1421"; /*change to correct com port */
    date_default_timezone_set('Portugal');
    $date=date('l jS \of F Y h:i:s A');
    echo "<h2 align=center>";
    echo $date;
    echo "</h2>";
if (isset($_POST["rcmd"])) {
$rcmd = $_POST["rcmd"];
switch ($rcmd) {
     case "ON":
  $fp =fopen($comPort, "w");
  fwrite($fp, 1); /* this is the number that it will write */
  fclose($fp);
  break;
     case "OFF":
  $fp =fopen($comPort, "w");
  fwrite($fp, 0); /* this is the number that it will write */
  fclose($fp);
  break;
  default:
  die('Crap, something went wrong. The page just puked.');
}

}
    if (isset($_POST["rega"])) {
        $rega = $_POST["rega"];
        switch($rega)
        {
        case "Watering":
        {
            $fp =fopen($comPort, "w");
            fwrite($fp, 2); /* this is the number that it will write */
            fclose($fp);
            break;
        }
        case "DRILL":
        {
            $fp =fopen($comPort, "w");
            fwrite($fp, 3); /* this is the number that it will write */
            fclose($fp);
            break;
        }
        case "Watertable":
        {
            $fp =fopen($comPort, "w");
            fwrite($fp, 4); /* this is the number that it will write */
            fclose($fp);
            break;
        }
            default:
    die('Crap, something went wrong. The page just puked.');
    }
    }
?>
<html>
<body>

<center><h1>Arduino Watering control page</h1><b>Version <?php echo $verz; ?></b></center>
<h3 align=center>Company Water:</h3><br />
<form  align=center method="post" action="arduino.php">
&nbsp&nbsp&nbsp&nbsp
<input type="submit" value="ON" name="rcmd" style="background-color: red;">
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
<input type="submit" value="OFF" name="rcmd" style="background-color: green;"><br/>
<br />
<br />
<h3>Force Watering:</h3>
<form method="post" action="arduino.php">
&nbsp&nbsp&nbsp&nbsp
<input type="submit" value="Watering" name="rega" style="background-color: yellow;"><br /><br /><br /><br />
<h3>Force Drill/Water_table:</h3><br />
<input  align=right type="submit" value="DRILL" name="rega" style="background-color: brown;"><br />
<input  align=right type="submit" value="Watertable" name="rega" style="background-color: blue;">
</form>
</body>
</html>