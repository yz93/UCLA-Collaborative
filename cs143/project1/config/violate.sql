-- -----------------------------------------------------------
-- PRIMARY KEY CONSTRAINTS -----------------------------------
-- -----------------------------------------------------------

-- [C] (PKC 1 / 3) IDs of movies must be keys
-- PRIMARY KEY(id)
-- The following violation attempts to insert a movie into the
-- Movie table with an ID that matches one already there
INSERT INTO Movie
VALUES(2, 'Failure, The Movie', 2014, 'R', 'Forney Industries');

-- [X] Error message:
-- ERROR 1062 (23000): Duplicate entry '2' for key 1


-- [C] (PKC 2 / 3) IDs of actors must be keys
-- PRIMARY KEY(id)
-- The following violation attempts to insert an actor into the
-- Actor table with an ID that matches one already there
INSERT INTO Actor
VALUES(1, 'Forney', 'Andrew', 'Male', '1989-11-09', NULL);

-- [X] Error message:
-- ERROR 1062 (23000): Duplicate entry '1' for key 1


-- [C] (PKC 3 / 3) IDs of directors must be keys
-- PRIMARY KEY(id)
-- The following violation attempts to insert a director into the
-- Director table with an ID that matches one already there
INSERT INTO Director
VALUES(68626, 'Forney', 'Andrew', '1989-11-09', NULL);

-- [X] Error message:
-- ERROR 1062 (23000): Duplicate entry '68626' for key 1



-- -----------------------------------------------------------
-- REFERENTIAL INTEGRITY CONSTRAINTS -------------------------
-- -----------------------------------------------------------

-- [C] (RIC 1 / 6) MIDs of movie genres must reference ids of Movie table
-- FOREIGN KEY(mid) REFERENCES Movie(id)
-- The following violation attempts to insert a new movie genre
-- with a value for mid that is not within the Movie table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO MovieGenre
VALUES(0, 'Horror');

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))


-- [C] (RIC 2 / 6) MIDs of movie directors must reference ids of Movie table
-- FOREIGN KEY(mid) REFERENCES Movie(id)
-- The following violation attempts to insert a new movie director
-- with a value for mid that is not within the Movie table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO MovieDirector
VALUES(0, 68626);

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))


-- [C] (RIC 3 / 6) DIDs of movie directors must reference ids of Director table
-- FOREIGN KEY(did) REFERENCES Director(id)
-- The following violation attempts to insert a new movie director
-- with a value for did that is not within the Director table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO MovieDirector
VALUES(2, 0);

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`did`) REFERENCES `Director` (`id`))


-- [C] (RIC 4 / 6) MIDs of movie actors must reference ids of Movie table
-- FOREIGN KEY(mid) REFERENCES Movie(id)
-- The following violation attempts to insert a new movie actor
-- with a value for mid that is not within the Movie table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO MovieActor
VALUES(0, 1, 'ERORRRRRR');

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))


-- [C] (RIC 5 / 6) AIDs of movie actors must reference ids of Actor table
-- FOREIGN KEY(aid) REFERENCES Actor(id)
-- The following violation attempts to insert a new movie actor
-- with a value for aid that is not within the Actor table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO MovieActor
VALUES(2, 0, 'ERORRRRRR');

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`aid`) REFERENCES `Actor` (`id`))


-- [C] (RIC 6 / 6) MIDs of review must reference ids of Movie table
-- FOREIGN KEY(mid) REFERENCES Movie(id)
-- The following violation attempts to insert a new review
-- with a value for mid that is not within the Movie table. As such,
-- the foreign key constraint is violated since that foreign key DNE.
INSERT INTO Review
VALUES('Andrew Forney', '2014-10-26 10:14:07', 0, 0, 'This film is so bad, it does not exist');

-- [X] Error message:
-- ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
-- (`CS143/Review`, CONSTRAINT `Review_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))



-- -----------------------------------------------------------
-- CHECK CONSTRAINTS -----------------------------------------
-- -----------------------------------------------------------

-- [C] (CC 1 / 3) Actor sex must be either 'Male' or 'Female'
-- CHECK(sex = 'Male' OR sex = 'Female')
-- Since we wish to adhere to uniform representations of sex in
-- our database, we should enforce that only two (perhaps not
-- a politically correct dichotomy) values are allowed.
INSERT INTO Actor
VALUES(1, 'Forney', 'Andrew', 'WOAHSOMETHING', '1989-11-09', NULL);


-- [C] (CC 2 / 3) Actor dob must precede dod
-- CHECK(dod = NULL OR dob <= dod)
-- We should ensure that none of our actors are time travelers
-- by enforcing the constraint that all actors have died at
-- some point after their birth (if they have indeed died)
INSERT INTO Actor
VALUES(1, 'Forney', 'Andrew', 'Male', '1989-11-09', '1989-10-08');


-- [C] (CC 3 / 3) Review rating must be some number between 0 and 5
-- CHECK(rating >= 0 AND rating <= 5)
-- The spec says that a review's rating must be x out of 5, which
-- establishes a lower bound of 0 and an upper bound of 5.
INSERT INTO Review
VALUES('Andrew Forney', '2014-10-26 10:14:07', 2, -5000, 'This film is Avatar, The Last Airbender by M Night');

