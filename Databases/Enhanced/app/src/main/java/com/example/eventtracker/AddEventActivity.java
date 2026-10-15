package com.example.eventtracker;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

public class AddEventActivity extends AppCompatActivity {

    private EditText editTextEventName;
    private EditText editTextEventDate;
    private EditText editTextEventTime;
    private EditText editTextEventLocation;
    private EditText editTextEventDescription;

    private DatabaseHelper databaseHelper;

    private int eventId = -1;
    private long userId = -1;
    private boolean editMode = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_event);

        TextView textViewAddEventTitle =
                findViewById(R.id.textViewAddEventTitle);

        editTextEventName = findViewById(R.id.editTextEventName);
        editTextEventDate = findViewById(R.id.editTextEventDate);
        editTextEventTime = findViewById(R.id.editTextEventTime);
        editTextEventLocation = findViewById(R.id.editTextEventLocation);
        editTextEventDescription =
                findViewById(R.id.editTextEventDescription);

        Button buttonSaveEvent = findViewById(R.id.buttonSaveEvent);
        Button buttonCancel = findViewById(R.id.buttonCancel);

        databaseHelper = new DatabaseHelper(this);

        // Get the ID of the currently logged-in user.
        userId = getIntent().getLongExtra("USER_ID", -1);

        // Check whether this screen was opened to edit an existing event.
        if (getIntent().hasExtra("eventId")) {

            editMode = true;

            eventId = getIntent().getIntExtra("eventId", -1);

            String name =
                    getIntent().getStringExtra("eventName");

            String date =
                    getIntent().getStringExtra("eventDate");

            String time =
                    getIntent().getStringExtra("eventTime");

            String location =
                    getIntent().getStringExtra("eventLocation");

            String description =
                    getIntent().getStringExtra("eventDescription");

            editTextEventName.setText(name);
            editTextEventDate.setText(date);
            editTextEventTime.setText(time);
            editTextEventLocation.setText(location);
            editTextEventDescription.setText(description);

            textViewAddEventTitle.setText("Edit Event");
            buttonSaveEvent.setText("Update Event");
        }

        buttonSaveEvent.setOnClickListener(v -> saveEvent());

        buttonCancel.setOnClickListener(v -> finish());
    }

    private void saveEvent() {

        String name =
                editTextEventName.getText().toString().trim();

        String date =
                editTextEventDate.getText().toString().trim();

        String time =
                editTextEventTime.getText().toString().trim();

        String location =
                editTextEventLocation.getText().toString().trim();

        String description =
                editTextEventDescription.getText().toString().trim();

        if (name.isEmpty() || date.isEmpty()) {

            Toast.makeText(
                    this,
                    "Please enter an event name and date.",
                    Toast.LENGTH_SHORT
            ).show();

            return;
        }

        // An event must be associated with a valid logged-in user.
        if (userId == -1) {

            Toast.makeText(
                    this,
                    "Unable to identify the logged-in user.",
                    Toast.LENGTH_SHORT
            ).show();

            return;
        }

        boolean success;

        if (editMode) {

            success = databaseHelper.updateEvent(
                    eventId,
                    userId,
                    name,
                    date,
                    time,
                    location,
                    description
            );

            if (success) {

                Toast.makeText(
                        this,
                        "Event updated.",
                        Toast.LENGTH_SHORT
                ).show();

                finish();

            } else {

                Toast.makeText(
                        this,
                        "Unable to update event.",
                        Toast.LENGTH_SHORT
                ).show();
            }

        } else {

            success = databaseHelper.addEvent(
                    userId,
                    name,
                    date,
                    time,
                    location,
                    description
            );

            if (success) {

                Toast.makeText(
                        this,
                        "Event saved.",
                        Toast.LENGTH_SHORT
                ).show();

                sendSmsAlert(name, date, time);

                finish();

            } else {

                Toast.makeText(
                        this,
                        "Unable to save event.",
                        Toast.LENGTH_SHORT
                ).show();
            }
        }
    }

    private void sendSmsAlert(
            String eventName,
            String eventDate,
            String eventTime) {

        SharedPreferences preferences =
                getSharedPreferences(
                        "EventTrackerPreferences",
                        MODE_PRIVATE
                );

        boolean smsEnabled =
                preferences.getBoolean("smsEnabled", false);

        String phoneNumber =
                preferences.getString("phoneNumber", "");

        // Do nothing if the user did not enable SMS notifications.
        if (!smsEnabled || phoneNumber.isEmpty()) {
            return;
        }

        // Make sure SMS permission is still granted.
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.SEND_SMS
        ) != PackageManager.PERMISSION_GRANTED) {

            return;
        }

        String message;

        if (eventTime.isEmpty()) {

            message =
                    "Event reminder: " +
                            eventName +
                            " is scheduled for " +
                            eventDate +
                            ".";

        } else {

            message =
                    "Event reminder: " +
                            eventName +
                            " is scheduled for " +
                            eventDate +
                            " at " +
                            eventTime +
                            ".";
        }

        try {

            SmsManager smsManager = SmsManager.getDefault();

            smsManager.sendTextMessage(
                    phoneNumber,
                    null,
                    message,
                    null,
                    null
            );

            Toast.makeText(
                    this,
                    "SMS alert sent.",
                    Toast.LENGTH_SHORT
            ).show();

        } catch (Exception e) {

            Toast.makeText(
                    this,
                    "Unable to send SMS alert.",
                    Toast.LENGTH_SHORT
            ).show();
        }
    }
}