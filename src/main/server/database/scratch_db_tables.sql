-- Gonna use sqlite3 or mariadb for simplicity. This is mostly scratch until I actually implement
-- accounts, personas, and race ranking history.

CREATE TABLE "Users" (
	"Id"	INTEGER NOT NULL, -- UID
	"Username"	INTEGER NOT NULL,
	"Email" TEXT NOT NULL,
	"PasswdHash" TEXT NOT NULL,

	-- TODO: friends, such. will have to see about that
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

CREATE TABLE "ScoresTable" (
	"UserId" INTEGER NOT NULL, -- Game sends this as the username, but to save db space map it as the uid
	"GameMode" INTEGER NOT NULL, -- avoids maintaining multiple tables, we can just do `select * from scorestable where GameMode = bla`
	"Rank"	INTEGER NOT NULL,
	"Time"	INTEGER NOT NULL,
	"Event"	INTEGER NOT NULL,
	"Venue"	INTEGER NOT NULL,
	PRIMARY KEY("UserId")
);