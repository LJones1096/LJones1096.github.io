package com.example.eventtracker;

import android.Manifest;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class SmsPermissionActivity extends AppCompatActivity {

    private static final int SMS_PERMISSION_REQUEST_CODE = 100;

    private EditText editTextPhoneNumber;
    private String phoneNumber = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms_permission);

        editTextPhoneNumber = findViewById(R.id.editTextPhoneNumber);

        Button buttonAllowSms = findViewById(R.id.buttonAllowSms);
        Button buttonNoThanks = findViewById(R.id.buttonNoThanks);

        buttonAllowSms.setOnClickListener(v -> {

            phoneNumber = editTextPhoneNumber
                    .getText()
                    .toString()
                    .trim();

            if (phoneNumber.isEmpty()) {
                Toast.makeText(
                        this,
                        "Please enter a phone number.",
                        Toast.LENGTH_SHORT
                ).show();

                return;
            }

            requestSmsPermission();
        });

        buttonNoThanks.setOnClickListener(v -> {

            clearSmsSettings();
            openEventList();
        });
    }

    private void requestSmsPermission() {

        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.SEND_SMS
        ) == PackageManager.PERMISSION_GRANTED) {

            saveSmsSettings();

            Toast.makeText(
                    this,
                    "SMS notifications enabled.",
                    Toast.LENGTH_SHORT
            ).show();

            openEventList();

        } else {

            ActivityCompat.requestPermissions(
                    this,
                    new String[]{Manifest.permission.SEND_SMS},
                    SMS_PERMISSION_REQUEST_CODE
            );
        }
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode,
            String[] permissions,
            int[] grantResults) {

        super.onRequestPermissionsResult(
                requestCode,
                permissions,
                grantResults
        );

        if (requestCode == SMS_PERMISSION_REQUEST_CODE) {

            if (grantResults.length > 0
                    && grantResults[0]
                    == PackageManager.PERMISSION_GRANTED) {

                saveSmsSettings();

                Toast.makeText(
                        this,
                        "SMS notifications enabled.",
                        Toast.LENGTH_SHORT
                ).show();

            } else {

                clearSmsSettings();

                Toast.makeText(
                        this,
                        "SMS permission denied. The app will still work normally.",
                        Toast.LENGTH_SHORT
                ).show();
            }

            openEventList();
        }
    }

    private void saveSmsSettings() {

        SharedPreferences preferences =
                getSharedPreferences(
                        "EventTrackerPreferences",
                        MODE_PRIVATE
                );

        SharedPreferences.Editor editor =
                preferences.edit();

        editor.putBoolean("smsEnabled", true);
        editor.putString("phoneNumber", phoneNumber);
        editor.apply();
    }

    private void clearSmsSettings() {

        SharedPreferences preferences =
                getSharedPreferences(
                        "EventTrackerPreferences",
                        MODE_PRIVATE
                );

        SharedPreferences.Editor editor =
                preferences.edit();

        editor.putBoolean("smsEnabled", false);
        editor.remove("phoneNumber");
        editor.apply();
    }

    private void openEventList() {

        Intent intent = new Intent(
                SmsPermissionActivity.this,
                EventListActivity.class
        );

        startActivity(intent);
        finish();
    }
}