package com.example.eventtracker;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class EventListActivity extends AppCompatActivity {

    private GridLayout eventGrid;
    private DatabaseHelper databaseHelper;
    private EditText editTextSearch;
    private Button buttonSort;

    private long userId;
    private boolean sortAscending = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_event_list);

        eventGrid = findViewById(R.id.eventGrid);
        editTextSearch = findViewById(R.id.editTextSearch);
        buttonSort = findViewById(R.id.buttonSort);
        Button buttonAddEvent = findViewById(R.id.buttonAddEvent);

        databaseHelper = new DatabaseHelper(this);

        // Get the ID of the user who logged in.
        userId = getIntent().getLongExtra("USER_ID", -1);

        buttonAddEvent.setOnClickListener(v -> {
            Intent intent = new Intent(
                    EventListActivity.this,
                    AddEventActivity.class
            );

            intent.putExtra("USER_ID", userId);
            startActivity(intent);
        });

        // Reload the event list while the user types in the search box.
        editTextSearch.addTextChangedListener(new TextWatcher() {

            @Override
            public void beforeTextChanged(
                    CharSequence s,
                    int start,
                    int count,
                    int after) {
            }

            @Override
            public void onTextChanged(
                    CharSequence s,
                    int start,
                    int before,
                    int count) {

                loadEvents();
            }

            @Override
            public void afterTextChanged(Editable s) {
            }
        });

        // Switch between oldest and newest events first.
        buttonSort.setOnClickListener(v -> {

            sortAscending = !sortAscending;

            if (sortAscending) {
                buttonSort.setText("Oldest First");
            } else {
                buttonSort.setText("Newest First");
            }

            loadEvents();
        });

        buttonSort.setText("Oldest First");
    }

    @Override
    protected void onResume() {
        super.onResume();
        loadEvents();
    }

    private void loadEvents() {

        eventGrid.removeAllViews();

        String searchText =
                editTextSearch.getText().toString().trim();

        Cursor cursor = databaseHelper.searchEventsForUser(
                userId,
                searchText,
                sortAscending
        );

        while (cursor.moveToNext()) {

            int id = cursor.getInt(
                    cursor.getColumnIndexOrThrow("id")
            );

            String name = cursor.getString(
                    cursor.getColumnIndexOrThrow("name")
            );

            String date = cursor.getString(
                    cursor.getColumnIndexOrThrow("date")
            );

            String time = cursor.getString(
                    cursor.getColumnIndexOrThrow("time")
            );

            String location = cursor.getString(
                    cursor.getColumnIndexOrThrow("location")
            );

            String description = cursor.getString(
                    cursor.getColumnIndexOrThrow("description")
            );

            TextView eventNameView = createGridTextView(name);
            TextView eventDateView = createGridTextView(date);

            LinearLayout actionLayout = new LinearLayout(this);
            actionLayout.setOrientation(LinearLayout.VERTICAL);
            actionLayout.setGravity(Gravity.CENTER);

            Button editButton = new Button(this);
            editButton.setText("Edit");

            editButton.setOnClickListener(v -> {

                Intent intent = new Intent(
                        EventListActivity.this,
                        AddEventActivity.class
                );

                intent.putExtra("USER_ID", userId);
                intent.putExtra("eventId", id);
                intent.putExtra("eventName", name);
                intent.putExtra("eventDate", date);
                intent.putExtra("eventTime", time);
                intent.putExtra("eventLocation", location);
                intent.putExtra("eventDescription", description);

                startActivity(intent);
            });

            Button deleteButton = new Button(this);
            deleteButton.setText("Delete");

            deleteButton.setOnClickListener(v -> {

                boolean deleted =
                        databaseHelper.deleteEvent(id, userId);

                if (deleted) {

                    Toast.makeText(
                            this,
                            "Event deleted.",
                            Toast.LENGTH_SHORT
                    ).show();

                    loadEvents();
                }
            });

            actionLayout.addView(editButton);
            actionLayout.addView(deleteButton);

            GridLayout.LayoutParams actionParams =
                    new GridLayout.LayoutParams();

            actionParams.width = 0;
            actionParams.columnSpec = GridLayout.spec(
                    GridLayout.UNDEFINED,
                    1f
            );

            actionLayout.setLayoutParams(actionParams);

            eventGrid.addView(eventNameView);
            eventGrid.addView(eventDateView);
            eventGrid.addView(actionLayout);
        }

        cursor.close();
    }

    private TextView createGridTextView(String text) {

        TextView textView = new TextView(this);

        textView.setText(text);
        textView.setPadding(8, 16, 8, 16);
        textView.setGravity(Gravity.CENTER_VERTICAL);

        GridLayout.LayoutParams params =
                new GridLayout.LayoutParams();

        params.width = 0;
        params.columnSpec = GridLayout.spec(
                GridLayout.UNDEFINED,
                1f
        );

        textView.setLayoutParams(params);

        return textView;
    }
}