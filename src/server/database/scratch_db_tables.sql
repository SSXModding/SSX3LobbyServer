-- Gonna use sqlite3 or mariadb for simplicity. This is mostly scratch until I actually implement
-- accounts, personas, and race ranking history.

CREATE TABLE "Users" (
	"Id"	INTEGER NOT NULL,
	"Username"	INTEGER NOT NULL,
	"PasswdHash"	INTEGER NOT NULL,
	PRIMARY KEY("Id")
);

-- both sqlite3 and mariadb don't have a native array type.
-- kinda sucks, although when moving to mariadb i might make
-- personas a JSON field in "Users".
CREATE TABLE "UserPersonas" (
    -- This ID is the same as the user account it is for.
	"Id" INTEGER NOT NULL,

	-- Null in this case means the persona
	-- isn't created/selectable for this user.
	--
	-- Unlike most of our fields, that's a valid possibility.
	"Persona0"	TEXT,
	"Persona1"	TEXT,
	"Persona2"	TEXT,
	"Persona3"	TEXT,
	PRIMARY KEY("Id")
);

CREATE TABLE "RaceScores" (
	"UserId" INTEGER NOT NULL,
	"Rank"	INTEGER NOT NULL,
	"Time"	INTEGER NOT NULL,
	"Event"	INTEGER NOT NULL,
	"Venue"	INTEGER NOT NULL,
	PRIMARY KEY("UserId")
);

CREATE TABLE "FreestyleScores" (
	"UserId" INTEGER NOT NULL,
	"Score"	INTEGER NOT NULL,
	"Rank"	INTEGER NOT NULL,
	"Event"	INTEGER NOT NULL,
	"Venue"	INTEGER NOT NULL,
	PRIMARY KEY("UserId")
);

-- do we need to provide overall?