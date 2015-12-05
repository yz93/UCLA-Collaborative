-- Q1: The first and last names of every actor in Die Another Day
SELECT concat(first, ' ', last)
FROM Actor A,
     MovieActor MA
WHERE A.id = MA.aid AND MA.mid = (
    SELECT id
    FROM Movie
    WHERE title = 'Die Another Day'
);

-- [!] Returns:
-- John Cleese
-- Paul Darrow
-- Judi Dench
-- Thomas Ho
-- Michael Madsen
-- Rosamund Pike
-- Halle Berry
-- Toby Stephens
-- Rick Yune
-- Pierce Brosnan


-- Q2: The count of all the actors who acted in multiple movies
SELECT count(*)
FROM (
    SELECT *
    FROM MovieActor
    GROUP BY aid
    HAVING count(*) > 2
) C;

-- [!] Returns:
-- 3198


-- Q3: The names of all films with 'Of the Dead' in the title
SELECT title
FROM Movie M
WHERE title LIKE '%Of the Dead%';

-- [!] Returns:
-- Prison of the Dead
-- Vengeance of the Dead

