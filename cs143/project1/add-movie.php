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
          
          <h1>Add Movie</h1>
          <hr/>
          <p class='alert alert-info'><span class='glyphicon glyphicon-info-sign'></span>&nbsp;Enter the information below to insert a new Movie into our collection.</p>
          
          <form method="GET" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" id="input-form" class="form-horizontal" role="form">
            <div class="form-group">
              <label for="title" class="col-md-2 control-label">Title</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="title" id="title" placeholder="Title" maxlength="20">
              </div>
            </div>
            
            <div class="form-group">
              <label for="company" class="col-md-2 control-label">Company</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="company" id="company" placeholder="Company" maxlength="50">
              </div>
            </div>
            
            <div class="form-group">
              <label for="year" class="col-md-2 control-label">Year</label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="year" id="year" placeholder="Year" maxlength="4">
              </div>
            </div>
            
            <div class="form-group">
              <label for="rating" class="col-md-2 control-label">MPAA Rating</label>
              <div class="col-md-10">
                <select class="form-control" name="rating" id="rating">
                  <?php
                    $ratings =  array("G", "PG", "PG-13", "R", "NC-17");
                    foreach ($ratings as $r) {
                      print "<option value=" . $r .  ">" . $r . "</option>";
                    }
                  ?>
                </select>
              </div>
            </div>
            
            <div class="form-group">
              <label for="genre" class="col-md-2 control-label">Genre</label>
              <div class="col-md-10">
                <?php
                  $GLOBALS["genres"] = array(
                    "Action", "Adult", "Adventure", "Animation", 
                    "Comedy", "Crime",
                    "Documentary", "Drama",
                    "Family", "Fantasy",
                    "Horror",
                    "Musical", "Mystery",
                    "Romancy",
                    "Sci-Fi", "Short",
                    "Thriller",
                    "War", "Western"
                  );
                  foreach ($GLOBALS["genres"] as $g) {
                    // Each genre will be a checkbox with equivalent value and label
                    print "<label class='checkbox-inline'><input type='checkbox' name='genre-" . $g . "' value='" . $g . "'>" . $g . "</label>";
                  }
                ?>
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
            $inputNames = array("title", "company", "year", "rating");
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
            
            function getGenres () {
              $result = array();
              foreach ($GLOBALS["genres"] as $g) {
                $currentGenre = $_GET["genre-" . $g];
                if (!empty($currentGenre)) {
                  array_push($result, $currentGenre);
                }
              }
              return $result;
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

            // Fetch and increment the ID for the new movie we're making
            $newID = fetchAndIncID("MaxMovieID", $dao);
            
            // Now insert the new record
            $query = "INSERT INTO Movie VALUES(" . 
              $newID . ",'" .  
              $inputVals["title"] . "'," .
              ((empty($inputVals["year"])) ? 0 : $inputVals["year"]) . ",'" .
              $inputVals["rating"] . "','" .
              $inputVals["company"] . "'" .
            ");";
            $insertResult = mysql_query($query, $dao);
            
            // Finally, we have to handle the genres
            $indicatedGenres = getGenres();
            foreach ($indicatedGenres as $g) {
              mysql_query("INSERT INTO MovieGenre VALUES(" . $newID . ",'" . $g . "');", $dao);
            }
            
            $feedback;
            $options;
            if ($insertResult) {
              $feedback =
                  "<p class='alert alert-success well-lg'><span class='glyphicon glyphicon-check'></span>&nbsp;Your addition was successful! What would you like to do next?</p>";
              $options = 
                  "<a class='btn btn-default' href='browse-movie.php?" .
                    "mtitle=" . urlencode(stripcslashes($inputVals["title"])) . "&" .
                    "myear=" . urlencode(stripcslashes($inputVals["year"])) . "&" .
                    "mid=" . $newID . "&" .
                    "mcompany=" . urlencode(stripcslashes($inputVals["company"])) .
                  "'>View Movie Info</a>" .
                  
                  "<a style='margin-left: 20px;' class='btn btn-default' href='add-movie-actor.php?" .
                    "linkmid=" . $newID .
                  "'>Link Actor to Movie</a>" .
                  
                  "<a style='margin-left: 20px;' class='btn btn-default' href='add-movie-director.php?" .
                    "linkmid=" . $newID .
                  "'>Link Director to Movie</a>";
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
