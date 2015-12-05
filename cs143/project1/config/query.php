<html>
  <head>
    <title>Query</title>
  </head>
  <body>
    <h1>Query</h1>
    <p>Author: Andrew Forney<br/>10/29/14</p>
    
    <form method="GET" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>">
      <textarea id="query" name="area" rows=8 cols=60><?php print $_GET["area"]; ?></textarea>
      <input type="submit">
    </form>
    
    <?php
      $query = $_GET["area"];
      
      if (empty($query)) {
        return;
      }
      
      // Configure database access object
      $dao = mysql_connect("localhost", "cs143", "");
      
      // Select the CS143 db
      mysql_select_db("CS143", $dao);
      
      // Query the DB and get info on the returned attrs
      $result = mysql_query($query, $dao);
      $fields = mysql_num_fields($result);
      
      // Construct the html table
      print "<table border='1' cellspacing='1' cellpadding='2'><tbody><tr align='center'>";
      
      // Print out the attribute names
      for ($attr = 0; $attr < $fields; $attr++) {
        print "<td><strong>" . mysql_field_name($result, $attr) . "</strong></td>";
      }
      
      print "</tr>";
      
      while ($row = mysql_fetch_row($result)) {
        print "<tr align='center'>";
        foreach ($row as $r) {
          print "<td>" . ((is_null($r)) ? "N/A" : $r) . "</td>";
        }
        print "</tr>";
      }
      
      print "</tbody></table>";
      
      // Close the sql server connection when done
      mysql_close($dao);
    ?>
  </body>
</html>
