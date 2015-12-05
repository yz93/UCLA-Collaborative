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
		  <?php
		  $movieTitle=$_GET['mtitle'];
		  $movieID = $_GET['mid'];
		  $movieYear = $_GET['myear'];
		  $movieRating = $_GET['mrating'];
		  $movieCompany = $_GET['mcompany'];

		  $db_connection = mysql_connect("localhost","cs143","");
		  mysql_select_db("CS143",$db_connection);
		  $query = "SELECT concat(first, ' ', last), role, aid, sex, dob, dod FROM Movie, MovieActor, Actor WHERE aid=Actor.id AND Movie.id=mid AND Movie.id=".$movieID;
		  $queryDirector="SELECT concat(first, ' ', last), did, dob, dod FROM MovieDirector, Director WHERE Director.id=MovieDirector.did AND MovieDirector.mid=".$movieID;
		  $queryGenre = "SELECT genre FROM MovieGenre WHERE mid=".$movieID;
		  //$queryAveReview = "SELECT AVG(A.rating) FROM (SELECT rating FROM Review WHERE mid=".$movieID.") A";
		  $queryReview = "SELECT * FROM Review WHERE mid=".$movieID." ORDER BY time;";
		  $actors = mysql_query($query, $db_connection);
		  $director = mysql_query($queryDirector, $db_connection);
		  //$avgReview = mysql_query($queryAveReview, $db_connection);
		  $genres = mysql_query($queryGenre, $db_connection);
		  $reviews = mysql_query($queryReview, $db_connection);

		  mysql_close($db_connection);
		  ?>
          <h1><?php echo $movieTitle;?></h1>
          <hr/>
          <label class="col-md-2 control-label">Title: </label>
		  <p><?php echo $movieTitle; ?></p>
              
            
          <label class="col-md-2 control-label">Year: </label>
		  <p><?php echo $movieYear; ?></p>

		  <label class="col-md-2 control-label">Genre: </label>
		  <p><?php 
				$row = mysql_fetch_row($genres);
				if (!$row)
					echo 'Not Specified';
				else{
				echo $row[0];
				while ($row = mysql_fetch_row($genres))
				{
					if (!$row[0])
						echo 'Not Specified';
					else 
						echo ", ".$row[0];
				}
				}
		  
		  ?></p>
		  
		  <label class="col-md-2 control-label">Average Review Score: </label>
		  <p id='avgrating'><?php 
			$row = mysql_fetch_row($avgReview);
			if (!$row)
			{
				echo 'No Score available. Be the first to review!';
			}
			else
			{
				if ($row[0])
					echo $row[0];
				else
					echo 'No Score available. Be the first to review!';
			}
				
		  ?></p>

          <label class="col-md-2 control-label">Rating: </label>
		  <p><?php echo $movieRating; ?></p>
            

		  <label class="col-md-2 control-label">Company: </label>
		  <p><?php echo $movieCompany; ?></p>
		  
		  <label class="col-md-2 control-label">Director: </label>
		  <div class="col-md-offset-2">
		  <p><?php 
			if ($director)
			{
				echo '<ul class="list-unstyled">';
				$row = mysql_fetch_row($director);
				if (!$row)
					echo 'It\'s a mystery';
				else{
				echo '<li>';
						if (!$row[0])
							echo 'It\'s a mystery';
						else{
							echo '<a href="browse-director.php?directorname='.$row[0].'&directorid='.$row[1].'&directordob='.$row[2].'&directordod='.$row[3].'">';
							echo $row[0];
							echo '</a>';
							}
						echo '</li>';
				while ($row = mysql_fetch_row($director))
					{
						echo '<li>';
						if (!$row[0])
							echo 'It\'s a mystery';
						else{
							echo '<a href="browse-director.php?directorname='.$row[0].'&directorid='.$row[1].'&directordob='.$row[2].'&directordod='.$row[3].'">';
							echo $row[0];
							echo '</a>';
							}
						echo '</li>';
					}
				}
					
				echo '</ul>';
			}
			else
				echo 'It\'s a mystery';
		  ?></p>
		  </div>
		  
		  <label class="col-md-2 control-label">Starring: </label>
		  <div class="col-md-offset-2">
		  <p><?php	
		  if ($actors){
					echo '<ul class="list-unstyled">';
					$ok=true;
					while ($row = mysql_fetch_row($actors))
					{
						if (!$row)
						{
							$ok=false;
							break;
						}
						echo '<li>';
						if ($row[0]){
							//echo '';
							echo '<a href="browse-actor.php?actorname='.$row[0].'&actorid='.$row[2].'&actorsex='.$row[3].'&actordob='.$row[4].'&actordod='.$row[5].'">';
							echo $row[0];
							echo '</a>';
							}
						if ($row[1]){
								echo ' ';
								echo '<i>'.$row[1].'</i>';
							}
						echo '</li>';
					}
					if (!$ok)
						echo 'None';
					
					echo '</ul>';
				}
		    else
					echo 'None';
           ?></p>
		   </div>
          <hr/>
		  <h3>Add Your Review</h3>
		  <form method="GET" action="<?php echo $_SERVER["PHP_SELF"]; ?>" id="input-form" class="form-horizontal" role="form">
		  <?php
				foreach($_GET as $name => $value) {
				  $name = htmlspecialchars($name);
				  $value = htmlspecialchars($value);
				  echo '<input type="hidden" name="'. $name .'" value="'. $value .'">';
				}
			?>
            <div class="form-group">
              <label for="title" class="col-md-2 control-label">Your Name: </label>
              <div class="col-md-10">
                <input type="text" class="form-control" name="reviewerName" id="title" value="Anonymous" maxlength="30">
              </div>
            </div>
            
			<div class="form-group">
              <label for="rating" class="col-md-2 control-label">Rate this movie!</label>
              <div class="col-md-10">
                <select class="form-control" name="reviewerRating" id="rating">
                  <?php
                    $ratings =  array("5-Excellent", "4-Good", "3-Average", "2-Bad", "1-Very Bad");
                    foreach ($ratings as $r) {
                      print "<option value=" . $r .  ">" . $r . "</option>";
                    }
                  ?>
                </select>
              </div>
            </div>
			
			<div class="form-group">
              <label for="title" class="col-md-2 control-label">Comment: </label>
              <div class="col-md-10">
			  <textarea class="form-control" name="input" placeholder="Share your thoughts"></textarea>
			  <br>
				<button type="submit" class="btn btn-default">Add!</button>
			  </div>
            </div>
			<?php
				$reviewerName = $_GET['reviewerName'];
				$pp = $_GET['reviewerRating'];
				$reviewerRating = $pp[0];
				$reviewerComment = $_GET['input'];

				$db2_connection = mysql_connect("localhost","cs143","");
				mysql_select_db("CS143",$db2_connection);
				$reviewerName=mysql_real_escape_string($reviewerName, $db2_connection);
				$reviewerComment=mysql_real_escape_string($reviewerComment, $db2_connection);
				$queryInsert = "INSERT INTO Review VALUES('".$reviewerName."',NOW(),".$movieID.",".$reviewerRating.",'".$reviewerComment."');";
				
				
				$success = mysql_query($queryInsert, $db2_connection);
				$reviews = mysql_query($queryReview, $db2_connection);
				
				
				//mysql_close($db2_connection);
			?>
			</form>
			<?php
			$queryAveReview = "SELECT AVG(rating) FROM Review WHERE mid=".$movieID;
			//$queryAveReview = "SELECT AVG(A.rating) FROM (SELECT rating FROM Review WHERE mid=".$movieID.") A";
			$avgReview = mysql_query($queryAveReview, $db2_connection);
			$row = mysql_fetch_row($avgReview);
			$result = $row[0];
			if (empty($row[0]))
			{
				$result = 'No Score available. Be the first to review!';
			}
			
			echo "<script> $('#avgrating').text('".$result."'); </script>";
			
			//echo "<script> $('#avgrating').text('test'); </script>";
			mysql_close($db2_connection);
			?>
			
			<h3>All Reviews</h3>
			<?php
			if ($reviews)
			{
				while ($row = mysql_fetch_row($reviews))
					{
						//echo '<li>';
						if ($row[0]){  // reviewer name
							echo '<b>'.$row[0].'</b> ';
							}
						else 
							echo '<b>Anonymous Moose</b> ';
						if ($row[1]){
								//echo ' ';
								//echo '<i>'.$row[1].'</i>';
								echo $row[1];
							}
						//echo '</li>';
						if ($row[3]){
								echo '<br>';
								echo '<b>Rating: </b>';
								echo $row[3];
								echo '<br>';
							}
						if ($row[4])
							{
							echo '<b>Comment: </b>';
							echo $row[4].'<br>';
							}
							echo '<br>';
					}
			}
			?>
		  
        </div>
      </div>
      <!-- END MAIN CONTENT -->
    </div>
    <!-- END WRAPPER -->
    
  </body>
</html>