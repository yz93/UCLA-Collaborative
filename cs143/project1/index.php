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
          
          <h1>Welcome to the Movie Database</h1>
          <p>Feel free to browse, add to, and comment on our dated collection.</p>

		  <form method="GET" action="<?php echo $_SERVER["PHP_SELF"]; ?>" class="form" id="search-form" role="form">
		  <div class="row">
		  <div class="col-lg-5">
		  <div class="input-group input-group-lg">
                <input type="text"  value="<?php echo $_GET['search']?>" class="form-control input-lg" name="search" id="search" placeholder="Search Movies/Actors"/>
				<span class="input-group-btn">
				<button type="submit" class="btn btn-primary  btn-lg">
					<span class='glyphicon glyphicon-search'></span>
				</button>
				</span>
		  </div>
		  </div>
		  </div>
		  </form>
		  <br>
		  <?php
		    function sanitizeKeyword($keyword) {
						return mysql_real_escape_string(trim($keyword));
					} 
			function displayResults($movieRes, $actorRes)
			{
			if ($movieRes){
					echo '<h3>Movie</h3>';
					echo '<ul class="list-unstyled">';
					while ($row = mysql_fetch_row($movieRes))
					{
						echo '<li>';
						if ($row[0]){
							echo '<a href="browse-movie.php?mtitle='.$row[0].'&myear='.$row[1].'&mid='.$row[2].'&mrating='.$row[3].'&mcompany='.$row[4].'">';
							echo $row[0];
							}
						if ($row[1])
							echo ' ('.$row[1].')';
						echo '</a>';
						echo '</li>';
					}
					echo '</ul>';	
				}
			if ($actorRes){
					echo '<h3>Actor</h3>';
					echo '<ul class="list-unstyled">';
					while ($row = mysql_fetch_row($actorRes))
					{
						echo '<li>';
						if ($row[0]){
							echo '<a href="browse-actor.php?actorname='.$row[0].'&actordob='.$row[1].'&actorsex='.$row[2].'&actordod='.$row[3].'&actorid='.$row[4].'">';
							echo $row[0];
							}
						if ($row[1])
							echo ' ('.$row[1].')';
						echo '</a>';
						echo '</li>';
					}
					echo '</ul>';
				}
			}
		    $db_connection = mysql_connect("localhost","cs143","");

			mysql_select_db("CS143",$db_connection);

			$search = $_GET["search"];
			
			if ($search=="") {}
			else{
				$search = trim($search);
				$search = preg_replace("/\s(\s)+/"," ", $search);
				$keywords = explode(" ",$search);
				$keywords = array_map(sanitizeKeyword, $keywords);
				
				$queryMovie="SELECT title,year,id,rating,company FROM Movie WHERE title LIKE '%";
				$queryActor="SELECT concat(first, ' ', last), dob, sex, dod, id FROM Actor WHERE first LIKE '%";

				$queryMovie .= implode("%' OR title LIKE '%", $keywords) . "%'"." ORDER BY title";
				$queryActor .= implode("%' OR first LIKE '%", $keywords) . "%'";
				$queryActor .= " OR last LIKE '%" . implode("%' OR last LIKE '%",$keywords) . "%'"." ORDER BY first,last";
				
				
				$movieResults = mysql_query($queryMovie, $db_connection);
				$actorResults = mysql_query($queryActor, $db_connection);
				
				displayResults($movieResults, $actorResults);
			}
			mysql_close($db_connection);
		  
		  ?>
        </div>
      </div>
      <!-- END MAIN CONTENT -->  
      
    </div>
    <!-- END WRAPPER -->
    
  </body>
</html>
