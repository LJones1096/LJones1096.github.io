package com.example.eventtracker;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "EventTracker.db";

    // Version 2 adds the events table.
    private static final int DATABASE_VERSION = 2;

    // Users table
    private static final String TABLE_USERS = "users";
    private static final String COLUMN_USER_ID = "id";
    private static final String COLUMN_USERNAME = "username";
    private static final String COLUMN_PASSWORD = "password";

    // Events table
    private static final String TABLE_EVENTS = "events";
    private static final String COLUMN_EVENT_ID = "id";
    private static final String COLUMN_EVENT_NAME = "name";
    private static final String COLUMN_EVENT_DATE = "date";
    private static final String COLUMN_EVENT_TIME = "time";
    private static final String COLUMN_EVENT_LOCATION = "location";
    private static final String COLUMN_EVENT_DESCRIPTION = "description";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {

        String createUsersTable =
                "CREATE TABLE " + TABLE_USERS + " (" +
                        COLUMN_USER_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                        COLUMN_USERNAME + " TEXT UNIQUE, " +
                        COLUMN_PASSWORD + " TEXT)";

        String createEventsTable =
                "CREATE TABLE " + TABLE_EVENTS + " (" +
                        COLUMN_EVENT_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                        COLUMN_EVENT_NAME + " TEXT, " +
                        COLUMN_EVENT_DATE + " TEXT, " +
                        COLUMN_EVENT_TIME + " TEXT, " +
                        COLUMN_EVENT_LOCATION + " TEXT, " +
                        COLUMN_EVENT_DESCRIPTION + " TEXT)";

        db.execSQL(createUsersTable);
        db.execSQL(createEventsTable);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

        if (oldVersion < 2) {
            String createEventsTable =
                    "CREATE TABLE IF NOT EXISTS " + TABLE_EVENTS + " (" +
                            COLUMN_EVENT_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                            COLUMN_EVENT_NAME + " TEXT, " +
                            COLUMN_EVENT_DATE + " TEXT, " +
                            COLUMN_EVENT_TIME + " TEXT, " +
                            COLUMN_EVENT_LOCATION + " TEXT, " +
                            COLUMN_EVENT_DESCRIPTION + " TEXT)";

            db.execSQL(createEventsTable);
        }
    }

    // Add a new user account.
    public boolean createUser(String username, String password) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(COLUMN_USERNAME, username);
        values.put(COLUMN_PASSWORD, password);

        long result = db.insert(TABLE_USERS, null, values);

        return result != -1;
    }

    // Check an existing user's login information.
    public boolean checkUser(String username, String password) {
        SQLiteDatabase db = this.getReadableDatabase();

        String selection =
                COLUMN_USERNAME + " = ? AND " +
                        COLUMN_PASSWORD + " = ?";

        String[] selectionArgs = {username, password};

        Cursor cursor = db.query(
                TABLE_USERS,
                null,
                selection,
                selectionArgs,
                null,
                null,
                null
        );

        boolean userExists = cursor.getCount() > 0;
        cursor.close();

        return userExists;
    }

    // Save a new event.
    public boolean addEvent(
            String name,
            String date,
            String time,
            String location,
            String description) {

        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(COLUMN_EVENT_NAME, name);
        values.put(COLUMN_EVENT_DATE, date);
        values.put(COLUMN_EVENT_TIME, time);
        values.put(COLUMN_EVENT_LOCATION, location);
        values.put(COLUMN_EVENT_DESCRIPTION, description);

        long result = db.insert(TABLE_EVENTS, null, values);

        return result != -1;
    }

    // Return all saved events.
    public Cursor getAllEvents() {
        SQLiteDatabase db = this.getReadableDatabase();

        return db.query(
                TABLE_EVENTS,
                null,
                null,
                null,
                null,
                null,
                COLUMN_EVENT_ID + " DESC"
        );
    }

    // Update an existing event.
    public boolean updateEvent(
            int id,
            String name,
            String date,
            String time,
            String location,
            String description) {

        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(COLUMN_EVENT_NAME, name);
        values.put(COLUMN_EVENT_DATE, date);
        values.put(COLUMN_EVENT_TIME, time);
        values.put(COLUMN_EVENT_LOCATION, location);
        values.put(COLUMN_EVENT_DESCRIPTION, description);

        int rowsUpdated = db.update(
                TABLE_EVENTS,
                values,
                COLUMN_EVENT_ID + " = ?",
                new String[]{String.valueOf(id)}
        );

        return rowsUpdated > 0;
    }

    // Delete an event.
    public boolean deleteEvent(int id) {
        SQLiteDatabase db = this.getWritableDatabase();

        int rowsDeleted = db.delete(
                TABLE_EVENTS,
                COLUMN_EVENT_ID + " = ?",
                new String[]{String.valueOf(id)}
        );

        return rowsDeleted > 0;
    }
}