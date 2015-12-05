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
          
          <h1>Add Person</h1>
          <hr/>
          <p class='alert alert-info'><span class='glyphicon glyphicon-info-sign'></span>&nbsp;Enter the information below to insert a new Actor or Director into our collection.</p>
          
          <form method="GET" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" id="input-form" class="form-horizontal" role="form">
            <div class="form-group">
              <label for="genre" class="col-md-2 control-label">Identity</label>
              <div class="col-md-10">
                <label class='radio-inline'><input type='radio' name='identity' value='Actor' checked>Actor</label>
                <label class='radio-inline'><input type='radio' name='identity' value='Director'>Director</label>
              </div>
            </div>
            
            <div class="form-group">
              <label for="first" class="col-md-2 control-label">First Name</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="first" id="first" placeholder="First Name" maxlength="20">
              </div>
            </div>
            
            <div class="form-group">
              <label for="last" class="col-md-2 control-label">Last Name</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="last" id="last" placeholder="Last Name" maxlength="20">
              </div>
            </div>
            
            <div class="form-group">
              <label for="sex" class="col-md-2 control-label">Sex</label>
              <div class="col-md-10">
                <label class='radio-inline'><input type='radio' name='sex' value='Male' checked>Male</label>
                <label class='radio-inline'><input type='radio' name='sex' value='Female'>Female</label>
              </div>
            </div>
            
            <div class="form-group">
              <label for="dob" class="col-md-2 control-label">Date of Birth</label>
              <div class="col-md-10">
                <input type="date" class="form-control" name="dob" id="dob" maxlength="10">
              </div>
            </div>
            
            <div class="form-group">
              <label for="dod" class="col-md-2 control-label">Date of Death</label>
              <div class="col-md-10">
                <input type="date" class="form-control" name="dod" id="dod" maxlength="10">
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
            $inputNames = array("identity", "first", "last", "sex", "dob", "dod");
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
            
            function dateConvert ($inputDate) {
              if (empty($inputDate)) {
                return "NULL";
              }
              return "'" . date("Y-m-d", strtotime($inputDate)) . "'";
            }
            
            function fetchAndIncID ($collection, $dao) {
              // Since we're inserting, we'll need the MaxMovieID
              $newIDResult = mysql_query("SELECT * FROM " . $collection . ";", $dao);
              $newID = mysql_fetch_row($newIDResult);
              
              // Then increment the id
              $updateResult = mysql_query("UPDATE " . $collection . " SET id=id+1;");
              return $newID[0];
            }
            
            $inputVals = getInputs($inputNames, $foundVal);
            
            // Don't do anything else if no inputs were indicated
            if (!$foundVal) {
              return;
            }
            
            // Otherwise, start off by designating the results section
            print "<h2>Result</h2>";
            print "<hr/>";
            
            // Configure database access object
            $dao = mysql_connect("localhost", "cs143", "");
            
            // Select the CS143 db
            mysql_select_db("CS143", $dao);

            // Fetch and increment the ID for the new person we're making
            $newID = fetchAndIncID("MaxPersonID", $dao);
            
            // Now insert the new record
            $query = "INSERT INTO " . $inputVals["identity"] . " VALUES(" . 
              $newID . ",'" .
              $inputVals["last"] . "','" .
              $inputVals["first"] . "'," .
              (($inputVals["identity"] == "Actor") ? "'" . $inputVals["sex"] . "'," : "") .
              dateConvert($inputVals["dob"]) . "," .
              dateConvert($inputVals["dod"]) .
            ");";
            $insertResult = mysql_query($query, $dao);
            
            $feedback;
            $options;
            if ($insertResult) {
              $feedback =
                "<p class='alert alert-success well-lg'><span class='glyphicon glyphicon-check'></span>&nbsp;Your addition was successful! What would you like to do next?</p>";
              $options = 
                (($inputVals["identity"] == "Actor")
                  ? "<a class='btn btn-default' href='browse-actor.php?" .
                      "actorname=" . urlencode(stripcslashes($inputVals["first"])) . "%20" . urlencode(stripcslashes($inputVals["last"])) . "&" .
                      "actordob=" . $inputVals["dob"] . "&" .
                      "actordod=" . $inputVals["dod"] . "&" .
                      "actorsex=" . $inputVals["sex"] . "&" .
                      "actorid=" . $newID .
                    "'>View Person's Info</a>"
                
                  : "<a class='btn btn-default' href='browse-director.php?" .
                      "directorname=" . urlencode(stripcslashes($inputVals["first"])) . "%20" . urlencode(stripcslashes($inputVals["last"])) . "&" .
                      "directordob=" . $inputVals["dob"] . "&" .
                      "directordod=" . $inputVals["dod"] . "&" .
                      "directorid=" . $newID .
                    "'>View Person's Info</a>"
                )
                
                .
                
                (($inputVals["identity"] == "Actor")
                  ? "<a style='margin-left: 20px;' class='btn btn-default' href='add-movie-actor.php?" .
                       "linkaid=" . $newID .
                    "'>Link Actor to Movie</a>"
                
                  : "<a style='margin-left: 20px;' class='btn btn-default' href='add-movie-director.php?" .
                      "linkdid=" . $newID .
                    "'>Link Director to Movie</a>"
                );
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
