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
		  <?php 
		  $directorName=$_GET['directorname'];
		  $directorDOB = $_GET['directordob'];
		  $directorDOD = $_GET['directordod'];
		  $directorID = $_GET['directorid'];
		  $db_connection = mysql_connect("localhost","cs143","");
		  mysql_select_db("CS143",$db_connection);
		  $query = "SELECT title, mid, year, rating, company FROM Movie, MovieDirector WHERE mid=Movie.id AND did=".$directorID;
		  $moviesActedIn = mysql_query($query, $db_connection);
		  mysql_close($db_connection);
		  ?>
          <h1><?php echo $directorName;?></h1>
          <hr/>
          <label class="col-md-2 control-label">Name: </label>
		  <p><?php echo $directorName; ?></p>
              
            
          <label class="col-md-2 control-label">Date of Birth: </label>
		  <p><?php echo $directorDOB; ?></p>
            

		  <label class="col-md-2 control-label">Date of Death: </label>
		  <p><?php if ($directorDOD) echo $directorDOD; else echo Alive; ?></p>
		  
		  <label class="col-md-2 control-label">Filmography: </label>
		  <div class="col-md-offset-2">
		  <p><?php	if ($moviesActedIn){
					echo '<ul class="list-unstyled">';
					while ($row = mysql_fetch_row($moviesActedIn))
					{
						echo '<li>';
						if ($row[0]){
							//echo 'Stars in ';
							echo '<a href="browse-movie.php?mid='.$row[1].'&mtitle='.$row[0].'&myear='.$row[2].'&mrating='.$row[3].'&mcompany='.$row[4].'">';
							echo $row[0];
							echo '</a>';
							}
						echo '</li>';
					}
					echo '</ul>';
				}
				else
					echo None;
           ?></p>
		   </div>
		   
		   
        </div>
      </div>
      <!-- END MAIN CONTENT -->  
      
    </div>
    <!-- END WRAPPER -->
    
  </body>
</html>
