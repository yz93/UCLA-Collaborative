<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

  <head>
    <title>Movie Database</title>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="./css/bootstrap.min.css" rel="stylesheet" type="text/css">
    <link href="./css/main.css" rel="stylesheet" type="text/css">
    <script src="./js/lib/jquery-2.1.1.min.js"></script>
    <script src="./js/lib/bootstrap.min.js"></script>
  </head>
  
  <body>
    
    <!-- BEGIN WRAP -->
    <div id="wrap">
      
      <!-- BEGIN NAVIGATION -->
      <nav class='navbar navbar-default' role='navigation'>
        <div class='container'>
          <div class='row'>
            <div class='col-md-12'>
              <div class='navbar-header'>
                <button class='navbar-toggle' type='button' data-toggle='collapse' data-target='.navbar-main-collapse'>
                  <span class='sr-only'>Toggle Navigation</span>
                  <span class='icon-bar'></span>
                  <span class='icon-bar'></span>
                  <span class='icon-bar'></span>
                </button>
                <a class='navbar-brand' href='index.php'>
                  <span id='brand-text'>
                    Movie Database
                  </span>
                </a>
              </div>
              
              <div id='nav-main' class='collapse navbar-collapse navbar-main-collapse'>
                <ul class='nav navbar-nav navbar-right'>
                  
                  <li>
                    <a href='index.php'>
                      <p class='text-center'>
                        <span class='glyphicon glyphicon-search'></span>
                        &nbsp;Search
                      </p>
                    </a>
                  </li>
                  
                  <li>
                    <a href='add-person.php'>
                      <p class='text-center'>
                        <span class='glyphicon glyphicon-user'></span>
                        &nbsp;Add Person
                      </p>
                    </a>
                  </li>
                  
                  <li>
                    <a href='add-movie.php'>
                      <p class='text-center'>
                        <span class='glyphicon glyphicon-film'></span>
                        &nbsp;Add Movie
                      </p>
                    </a>
                  </li>
                  
                  <li>
                    <a href='add-movie-actor.php'>
                      <p class='text-center'>
                        <span class='glyphicon glyphicon-star'></span>
                        &nbsp;Add Movie-Actor
                      </p>
                    </a>
                  </li>
                  
                  <li>
                    <a href='add-movie-director.php'>
                      <p class='text-center'>
                        <span class='glyphicon glyphicon-bullhorn'></span>
                        &nbsp;Add Movie-Director
                      </p>
                    </a>
                  </li>
                  
                </ul>
              </div>
            </div>
          </div>
        </div>
      </nav>
      <!-- END NAVIGATION -->
      
      <!-- BEGIN MAIN CONTENT -->
      <div id="main-content" class="container">
        <div class='row'>
          <!-- BEGIN PHP CODE BELOW (may also modify headings / text) -->
          
          <h1>Add Actor to Movie</h1>
          <hr/>
          <p class='alert alert-info'><span class='glyphicon glyphicon-info-sign'></span>&nbsp;Enter the information below to link an Actor with a Movie.</p>
          
          <form method="GET" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" id="input-form" class="form-horizontal" role="form">
            <div class="form-group">
              <label for="mid" class="col-md-2 control-label">Movie</label>
              <div class="col-md-10">
                <select class="form-control" name="mid" id="mid">
                  <?php
                    $dao = mysql_connect("localhost", "cs143", "");
                    mysql_select_db("CS143", $dao);
                    
                    // Query the DB and get info on the returned attrs
                    $movies = mysql_query("SELECT title, id, year FROM Movie ORDER BY title;", $dao);
                    
                    while ($m = mysql_fetch_assoc($movies)) {
                      print "<option value='" . $m["id"] . "'>" . $m["title"] . " (" . $m["year"] . ")</option>";
                    }
                  ?>
                </select>
              </div>
            </div>
            
            <div class="form-group">
              <label for="aid" class="col-md-2 control-label">Actor</label>
              <div class="col-md-10">
                <select class="form-control" name="aid" id="aid">
                  <?php
                    // Query the DB and get info on the returned attrs
                    $actors = mysql_query("SELECT id, first, last FROM Actor ORDER BY last;", $dao);
                    
                    while ($a = mysql_fetch_assoc($actors)) {
                      print "<option value='" . $a["id"] . "'>" . $a["last"] . ", " . $a["first"] . "</option>";
                    }
                  ?>
                </select>
              </div>
            </div>
            
            <div class="form-group">
              <label for="role" class="col-md-2 control-label">Role</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="role" id="role" placeholder="Role" maxlength="50">
              </div>
            </div>
            
            <div class="form-group">
              <div class="col-md-offset-2 col-md-10">
                <button type="submit" class="btn btn-default">Submit!</button>
              </div>
            </div>
          </form>
          
          <?php
            // The current page's input names to parse
            $inputNames = array("mid", "aid", "role", "linkmid", "linkaid");
            $inputVals = array();
            $foundVal = false;
            
            // Helper to get the values associated with the form fields
            // in map form; sets flag $foundVal to true whenever at least
            // one field is populated
            function getInputs ($inputs, &$foundVal) {
              $result = array();
              foreach ($inputs as $i) {
                $result[$i] = mysql_escape_string($_GET[$i]);
                if ($result[$i]) {
                  $foundVal = true;
                }
              }
              return $result;
            }
            
            $inputVals = getInputs($inputNames, $foundVal);
            
            // Sometimes, we just have a link to the page and don't actually
            // want to make an addition:
            if (!empty($inputVals["linkmid"])) {
              print
                "<script>" .
                  "$('#mid').val('" . $inputVals["linkmid"]  . "')" .
                "</script>";
              return;
            }
            if (!empty($inputVals["linkaid"])) {
              print
                "<script>" .
                  "$('#aid').val('" . $inputVals["linkaid"]  . "')" .
                "</script>";
              return;
            }
            
            // Don't do anything else if no inputs were indicated
            if (!$foundVal) {
              return;
            }
            
            // Otherwise, start off by designating the results section
            print "<h2>Result</h2>";
            print "<hr/>";
            
            // Now insert the new record
            $query = "INSERT INTO MovieActor VALUES(" . 
              $inputVals["mid"] . "," .
              $inputVals["aid"] . ",'" .
              $inputVals["role"] . "'" .
            ");";
            $insertResult = mysql_query($query, $dao);
            
            $feedback;
            $options;
            if ($insertResult) {
              $feedback =
                "<p class='alert alert-success well-lg'><span class='glyphicon glyphicon-check'></span>&nbsp;Your addition was successful!</p>";
              $options = "";
            } else {
              $feedback = 
                  "<p class='alert alert-danger well-lg'><span class='glyphicon glyphicon-remove'></span>&nbsp;Looks like something went wrong on our end. Try again, double checking your inputs!</p>";
              $options = "";
            }
            
            print $feedback;
            print $options;
            
            mysql_close($dao);
          ?>
          
        </div>
      </div>
      <!-- END MAIN CONTENT -->  
      
    </div>
    <!-- END WRAPPER -->
    
  </body>
</html>
