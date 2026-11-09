package com.libertyrecomp;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Button;
import android.widget.LinearLayout;
import android.content.Intent;
import android.view.Gravity;

public class LauncherActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        layout.setPadding(32, 32, 32, 32);

        TextView title = new TextView(this);
        title.setText("Grand Theft Auto IV Universal");
        title.setTextSize(24.0f);
        title.setGravity(Gravity.CENTER);
        layout.addView(title);

        TextView subtitle = new TextView(this);
        subtitle.setText("Xbox 360 Native Recompilation Engine\nARM64 Android Runtime");
        subtitle.setTextSize(14.0f);
        subtitle.setGravity(Gravity.CENTER);
        subtitle.setPadding(0, 16, 0, 32);
        layout.addView(subtitle);

        Button playButton = new Button(this);
        playButton.setText("Launch Game Engine");
        playButton.setOnClickListener(v -> {
            Intent intent = new Intent(this, LibertyActivity.class);
            startActivity(intent);
        });
        layout.addView(playButton);

        setContentView(layout);
    }
}
