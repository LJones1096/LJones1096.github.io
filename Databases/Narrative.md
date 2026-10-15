
# Enhancement Three: Databases

Liam Jones  
CS-499  
10/01/2026

## Artifact Description

The artifact I selected for my database enhancement is the Event Tracker application that I originally created in CS 360: Mobile Architect and Programming. The application was created in Android Studio using Java and SQLite. Its main purpose is to allow users to create an account, log in, and keep track of events. Users can enter information such as an event name, date, time, location, and description. They can also edit or delete events that have already been saved.

## Justification for Inclusion

I selected the Event Tracker application for my ePortfolio because it gave me a good opportunity to demonstrate my database skills. The original application had a working SQLite database, but there were several things that could be improved. One of the biggest problems was that events were not connected to individual users. Even though the application had user accounts, the events were stored separately from those accounts.

For my enhancement, I added a user ID to the event data and changed the application so that events are associated with the user who created them. The logged-in user's ID is passed through the application and stored whenever an event is created. I also changed the database queries so users only see their own events. Updates and deletes use both the event ID and user ID, which helps prevent one account from modifying another user's information.

I also added search and sorting features. Users can search their events using the event name, date, time, location, or description. The search query still checks the user ID, so results are limited to the current user's events. Events can also be sorted by date and time with the oldest or newest events displayed first.

Another improvement I made involved password storage. The original application stored passwords directly in the SQLite database as plain text. I changed the application so passwords are hashed before being stored. I also added a database upgrade that converts passwords from existing accounts when the database version changes. This allowed existing accounts to continue working after the enhancement. Although this is more secure than storing readable passwords, I learned that a production application should use a password-specific hashing method with unique salts instead of relying on SHA-256 by itself.

## Course Outcomes

I believe I met the course outcomes that I planned for this enhancement in Module One. One of the main outcomes I wanted to demonstrate was my ability to design and evaluate computing solutions. Changing the relationship between users and events required me to think about how information should be organized and how different parts of the application would interact with the database.

The enhancement also demonstrates my ability to use computing techniques and tools to implement a solution. I worked with Java, SQLite, database queries, user IDs, and database version upgrades. Instead of retrieving every event from the database, I created more specific queries that search, sort, and restrict data based on the current user.

The security outcome was also addressed by reviewing how user and event information was being handled. Restricting events by user ID improves the privacy of event data, while hashing passwords improves how account information is stored. I do not think I need to change my original outcome-coverage plan because the finished enhancement addresses the database design, programming, and security areas I originally planned to improve.

These areas line up particularly well with program outcomes 3, 4, and 5, which address computing solutions, implementation techniques and tools, and developing a security mindset.

## Reflection

One of the biggest things I learned from this enhancement was that changing a database can affect much more than the database class itself. At first, adding a user ID to an event seemed like a fairly simple change. Once I started working on it, I realized the user ID had to be passed from the login screen through multiple activities before it could be used when creating, displaying, editing, or deleting an event. This helped me better understand how different parts of an application depend on each other.

Testing was also important during this enhancement. I created multiple accounts and made sure that an event created by one account did not appear when another account logged in. I tested creating and editing events, searching for events, and changing the sort order. When I changed the password storage, I tested both an existing account and a newly created account to make sure the database upgrade did not prevent users from logging in.

Another challenge was changing the database without losing information that was already there. Increasing the database version and using the upgrade process allowed me to modify the existing database instead of simply deleting it and starting over. This gave me more experience with database migrations and showed me why they are important when an application changes over time.

Overall, this enhancement helped me understand database development beyond basic create, read, update, and delete operations. I had to think about relationships between data, user privacy, queries, migrations, and security. If I continued working on this application, one improvement I would make would be using a stronger password-storage approach designed specifically for passwords. I would also improve the way dates and times are stored so they use a consistent format that is easier to validate and sort.
