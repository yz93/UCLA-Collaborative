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
          
          <h1>Test Heading</h1>
          <p>Oh man, this project is going to be so sick...</p>
          
        </div>
      </div>
      <!-- END MAIN CONTENT -->  
      
    </div>
    <!-- END WRAPPER -->
    
  </body>
</html>
