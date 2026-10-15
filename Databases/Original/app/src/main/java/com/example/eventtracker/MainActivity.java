package com.example.eventtracker;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    private EditText editTextUsername;
    private EditText editTextPassword;
    private DatabaseHelper databaseHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editTextUsername = findViewById(R.id.editTextUsername);
        editTextPassword = findViewById(R.id.editTextPassword);
        Button buttonLogin = findViewById(R.id.buttonLogin);
        Button buttonCreateAccount = findViewById(R.id.buttonCreateAccount);

        databaseHelper = new DatabaseHelper(this);

        // Check the username and password when the user selects Login.
        buttonLogin.setOnClickListener(v -> {
            String username = editTextUsername.getText().toString().trim();
            String password = editTextPassword.getText().toString();

            if (username.isEmpty() || password.isEmpty()) {
                Toast.makeText(
                        MainActivity.this,
                        "Please enter a username and password.",
                        Toast.LENGTH_SHORT
                ).show();
                return;
            }

            if (databaseHelper.checkUser(username, password)) {
                Toast.makeText(
                        MainActivity.this,
                        "Login successful.",
                        Toast.LENGTH_SHORT
                ).show();

                Intent intent = new Intent(
                        MainActivity.this,
                        SmsPermissionActivity.class
                );
                startActivity(intent);
            } else {
                Toast.makeText(
                        MainActivity.this,
                        "Invalid username or password.",
                        Toast.LENGTH_SHORT
                ).show();
            }
        });

        // Create a new account using the entered username and password.
        buttonCreateAccount.setOnClickListener(v -> {
            String username = editTextUsername.getText().toString().trim();
            String password = editTextPassword.getText().toString();

            if (username.isEmpty() || password.isEmpty()) {
                Toast.makeText(
                        MainActivity.this,
                        "Please enter a username and password.",
                        Toast.LENGTH_SHORT
                ).show();
                return;
            }

            boolean accountCreated =
                    databaseHelper.createUser(username, password);

            if (accountCreated) {
                Toast.makeText(
                        MainActivity.this,
                        "Account created. You can now log in.",
                        Toast.LENGTH_SHORT
                ).show();
            } else {
                Toast.makeText(
                        MainActivity.this,
                        "That username already exists.",
                        Toast.LENGTH_SHORT
                ).show();
            }
        });
    }
}