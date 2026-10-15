package com.example.eventtracker;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.view.Gravity;
import android.widget.Button;
import android.widget.GridLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class EventListActivity extends AppCompatActivity {

    private GridLayout eventGrid;
    private DatabaseHelper databaseHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_event_list);

        eventGrid = findViewById(R.id.eventGrid);
        Button buttonAddEvent = findViewById(R.id.buttonAddEvent);

        databaseHelper = new DatabaseHelper(this);

        buttonAddEvent.setOnClickListener(v -> {
            Intent intent = new Intent(
                    EventListActivity.this,
                    AddEventActivity.class
            );
            startActivity(intent);
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        loadEvents();
    }

    private void loadEvents() {

        eventGrid.removeAllViews();

        Cursor cursor = databaseHelper.getAllEvents();

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

                boolean deleted = databaseHelper.deleteEvent(id);

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