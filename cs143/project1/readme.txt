=====================================================
Project: 1C
Class:   CS143, Fall 2014
Date:    11/9/2014
Authors: [Zhao Yang, Andrew Forney]
UID:     [904169203, 004130291]
Email:   [yz931129@gmail.com, forns@cs.ucla.edu]
=====================================================


Report:
-----------------------------------------------------
- The project was a team effort by Zhao Yang and
  Andrew Forney.
  
- All project criteria have been met
    - Some additional convenience links have been
      added to add-movie.php and add-person.php to
      allow for easier linking and viewing of each
      newly created actor / director / movie

- Collaboration:
    - We began with a good division of labor:
        - Zhao would work on the search and browse
          pages
        - Andrew would work on the add-*.php pages
        - We met to perform the tests and last minute
          revisions
    - We used a private GitHub repository to sync
      our work and allow for excellent version control
    - We were pleased with the way the division of
      labor and organization turned out, and have no
      improvements to make for the next project


File Contents:
-----------------------------------------------------
P1C.zip
 | 
 +- 004130291: submission folder
    |
    +- src
        |
        +- css: contains all css files and stylings for
        |    the pages and Bootstrap positionings
        +- fonts: contains all fonts and glyphicons used
        |    by Bootstrap for styling
        +- js: contains all Bootstrap and jQuery
        |    library files
        +- index.php: serves as the home and search page
        +- add-movie.php: page to add a Movie
        +- add-person.php: page to add an Actor or
        |    Director
        +- add-movie-actor.php: links an Actor to a
        |    Movie
        +- add-movie-director.php: links a Director to
        |    a Movie
        +- browse-actor.php: provides info on an Actor
        +- browse-director.php: provides info on a
        |    Director
        +- browse-movie.php: provides info on a Movie
             as well as the Review commenting system
             
    +- testcase: contains all Selenium test cases
        |
        +- t1.html: test for search on Tom Hanks
        +- t2.html: test for actor and director creation
        +- t3.html: test for movie creation and actor /
        |    director linking from t2
        +- t4.html: creates two reviews for movie created
        |    in t3
        +- t5.html: test for search on santa
        
    +- readme.txt: this file!
    