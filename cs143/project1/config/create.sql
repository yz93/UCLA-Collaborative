-- [!] To find added constraints, simply search for [C] tags

CREATE TABLE Movie(
    id INT,
    title VARCHAR(100),
    year INT,
    rating VARCHAR(10),
    company VARCHAR(50),
    
    -- [C] IDs of movies must be keys (PKC 1/3)
    PRIMARY KEY(id)
) ENGINE=INNODB;

CREATE TABLE Actor(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    sex VARCHAR(6),
    dob DATE,
    dod DATE,
    
    -- [C] IDs of actors must be keys (PKC 2/3)
    PRIMARY KEY(id),
    
    -- [C] Sex must be either 'Male' or 'Female' (CC 1/3)
    CHECK(sex = 'Male' OR sex = 'Female'),
    
    -- [C] Date of birth must precede date of death (CC 2/3)
    CHECK(dod = NULL OR dob <= dod)
) ENGINE=INNODB;

CREATE TABLE Director(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    dob DATE,
    dod DATE,
    
    -- [C] IDs of directors must be keys (PKC 3/3)
    PRIMARY KEY(id)
) ENGINE=INNODB;

CREATE TABLE MovieGenre(
    mid INT,
    genre VARCHAR(20),
    
    -- [C] MIDs of movie genres must reference ids of Movie table (RIC 1/6)
    FOREIGN KEY(mid) REFERENCES Movie(id)
) ENGINE=INNODB;

CREATE TABLE MovieDirector(
    mid INT,
    did INT,
    
    -- [C] MIDs of movie directors must reference ids of Movie table (RIC 2/6)
    FOREIGN KEY(mid) REFERENCES Movie(id),
    
    -- [C] DIDs of movie directors must reference ids of Director table (RIC 3/6)
    FOREIGN KEY(did) REFERENCES Director(id)
) ENGINE=INNODB;

CREATE TABLE MovieActor(
    mid INT,
    aid INT,
    role VARCHAR(50),
    
    -- [C] MIDs of movie actors must reference ids of Movie table (RIC 4/6)
    FOREIGN KEY(mid) REFERENCES Movie(id),
    
    -- [C] AIDs of movie actors must reference ids of Actor table (RIC 5/6)
    FOREIGN KEY(aid) REFERENCES Actor(id)
) ENGINE=INNODB;

CREATE TABLE Review(
    name VARCHAR(20),
    time TIMESTAMP,
    mid INT,
    rating INT,
    comment VARCHAR(500),
    
    -- [C] MIDs of review must reference ids of Movie table (RIC 6/6)
    FOREIGN KEY(mid) REFERENCES Movie(id),
    
    -- [C] Review rating must be some number between 0 and 5 (CC 3/3)
    CHECK(rating >= 0 AND rating <= 5)
) ENGINE=INNODB;

CREATE TABLE MaxPersonID(
    id INT
) ENGINE=INNODB;

CREATE TABLE MaxMovieID(
    id INT
) ENGINE=INNODB;