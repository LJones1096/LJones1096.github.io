package com.example.eventtracker;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "EventTracker.db";

    // Version 4 improves password storage.
    private static final int DATABASE_VERSION = 4;

    // Users table
    private static final String TABLE_USERS = "users";
    private static final String COLUMN_USER_ID = "id";
    private static final String COLUMN_USERNAME = "username";
    private static final String COLUMN_PASSWORD = "password";

    // Events table
    private static final String TABLE_EVENTS = "events";
    private static final String COLUMN_EVENT_ID = "id";
    private static final String COLUMN_EVENT_USER_ID = "user_id";
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
                        COLUMN_EVENT_USER_ID + " INTEGER, " +
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

        if (oldVersion < 3) {
            db.execSQL(
                    "ALTER TABLE " + TABLE_EVENTS +
                            " ADD COLUMN " + COLUMN_EVENT_USER_ID + " INTEGER"
            );
        }

        // Convert existing plaintext passwords to hashed passwords.
        if (oldVersion < 4) {

            Cursor cursor = db.query(
                    TABLE_USERS,
                    new String[]{COLUMN_USER_ID, COLUMN_PASSWORD},
                    null,
                    null,
                    null,
                    null,
                    null
            );

            while (cursor.moveToNext()) {

                int userId = cursor.getInt(
                        cursor.getColumnIndexOrThrow(COLUMN_USER_ID)
                );

                String oldPassword = cursor.getString(
                        cursor.getColumnIndexOrThrow(COLUMN_PASSWORD)
                );

                String hashedPassword = hashPassword(oldPassword);

                ContentValues values = new ContentValues();
                values.put(COLUMN_PASSWORD, hashedPassword);

                db.update(
                        TABLE_USERS,
                        values,
                        COLUMN_USER_ID + " = ?",
                        new String[]{String.valueOf(userId)}
                );
            }

            cursor.close();
        }
    }

    // Convert a password into a SHA-256 hash before storing or comparing it.
    private static String hashPassword(String password) {

        try {

            MessageDigest digest =
                    MessageDigest.getInstance("SHA-256");

            byte[] hash = digest.digest(
                    password.getBytes(StandardCharsets.UTF_8)
            );

            StringBuilder hexString = new StringBuilder();

            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);

                if (hex.length() == 1) {
                    hexString.append('0');
                }

                hexString.append(hex);
            }

            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {

            throw new RuntimeException(
                    "Unable to hash password.",
                    e
            );
        }
    }

    // Add a new user account with a hashed password.
    public boolean createUser(String username, String password) {

        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(COLUMN_USERNAME, username);
        values.put(COLUMN_PASSWORD, hashPassword(password));

        long result = db.insert(TABLE_USERS, null, values);

        return result != -1;
    }

    // Check an existing user's login information.
    public boolean checkUser(String username, String password) {

        SQLiteDatabase db = this.getReadableDatabase();

        String hashedPassword = hashPassword(password);

        String selection =
                COLUMN_USERNAME + " = ? AND " +
                        COLUMN_PASSWORD + " = ?";

        String[] selectionArgs = {
                username,
                hashedPassword
        };

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

    // Get the ID of a user after a successful login.
    public long getUserId(String username, String password) {

        SQLiteDatabase db = this.getReadableDatabase();

        String hashedPassword = hashPassword(password);

        String selection =
                COLUMN_USERNAME + " = ? AND " +
                        COLUMN_PASSWORD + " = ?";

        String[] selectionArgs = {
                username,
                hashedPassword
        };

        Cursor cursor = db.query(
                TABLE_USERS,
                new String[]{COLUMN_USER_ID},
                selection,
                selectionArgs,
                null,
                null,
                null
        );

        long userId = -1;

        if (cursor.moveToFirst()) {
            userId = cursor.getLong(
                    cursor.getColumnIndexOrThrow(COLUMN_USER_ID)
            );
        }

        cursor.close();

        return userId;
    }

    // Save a new event for the logged-in user.
    public boolean addEvent(
            long userId,
            String name,
            String date,
            String time,
            String location,
            String description) {

        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(COLUMN_EVENT_USER_ID, userId);
        values.put(COLUMN_EVENT_NAME, name);
        values.put(COLUMN_EVENT_DATE, date);
        values.put(COLUMN_EVENT_TIME, time);
        values.put(COLUMN_EVENT_LOCATION, location);
        values.put(COLUMN_EVENT_DESCRIPTION, description);

        long result = db.insert(TABLE_EVENTS, null, values);

        return result != -1;
    }

    // Return only the events that belong to the logged-in user.
    public Cursor getEventsForUser(long userId) {

        SQLiteDatabase db = this.getReadableDatabase();

        return db.query(
                TABLE_EVENTS,
                null,
                COLUMN_EVENT_USER_ID + " = ?",
                new String[]{String.valueOf(userId)},
                null,
                null,
                COLUMN_EVENT_DATE + " ASC, " +
                        COLUMN_EVENT_TIME + " ASC"
        );
    }

    // Update an event only if it belongs to the logged-in user.
    public boolean updateEvent(
            int id,
            long userId,
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
                COLUMN_EVENT_ID + " = ? AND " +
                        COLUMN_EVENT_USER_ID + " = ?",
                new String[]{
                        String.valueOf(id),
                        String.valueOf(userId)
                }
        );

        return rowsUpdated > 0;
    }

    // Delete an event only if it belongs to the logged-in user.
    public boolean deleteEvent(int id, long userId) {

        SQLiteDatabase db = this.getWritableDatabase();

        int rowsDeleted = db.delete(
                TABLE_EVENTS,
                COLUMN_EVENT_ID + " = ? AND " +
                        COLUMN_EVENT_USER_ID + " = ?",
                new String[]{
                        String.valueOf(id),
                        String.valueOf(userId)
                }
        );

        return rowsDeleted > 0;
    }

    // Search the logged-in user's events and sort them by date and time.
    public Cursor searchEventsForUser(
            long userId,
            String searchText,
            boolean sortAscending) {

        SQLiteDatabase db = this.getReadableDatabase();

        String selection;
        String[] selectionArgs;

        if (searchText.isEmpty()) {

            selection = COLUMN_EVENT_USER_ID + " = ?";

            selectionArgs = new String[]{
                    String.valueOf(userId)
            };

        } else {

            selection =
                    COLUMN_EVENT_USER_ID + " = ? AND (" +
                            COLUMN_EVENT_NAME + " LIKE ? OR " +
                            COLUMN_EVENT_DATE + " LIKE ? OR " +
                            COLUMN_EVENT_TIME + " LIKE ? OR " +
                            COLUMN_EVENT_LOCATION + " LIKE ? OR " +
                            COLUMN_EVENT_DESCRIPTION + " LIKE ?)";

            String searchPattern = "%" + searchText + "%";

            selectionArgs = new String[]{
                    String.valueOf(userId),
                    searchPattern,
                    searchPattern,
                    searchPattern,
                    searchPattern,
                    searchPattern
            };
        }

        String sortOrder;

        if (sortAscending) {
            sortOrder =
                    COLUMN_EVENT_DATE + " ASC, " +
                            COLUMN_EVENT_TIME + " ASC";
        } else {
            sortOrder =
                    COLUMN_EVENT_DATE + " DESC, " +
                            COLUMN_EVENT_TIME + " DESC";
        }

        return db.query(
                TABLE_EVENTS,
                null,
                selection,
                selectionArgs,
                null,
                null,
                sortOrder
        );
    }
}