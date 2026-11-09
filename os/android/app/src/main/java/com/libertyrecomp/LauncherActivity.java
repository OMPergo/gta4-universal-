package com.libertyrecomp;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class LauncherActivity extends Activity {
    private int dp(float value) {
        return Math.round(value * getResources().getDisplayMetrics().density);
    }

    private TextView label(String value, float size, int color) {
        TextView view = new TextView(this);
        view.setText(value);
        view.setTextSize(size);
        view.setTextColor(color);
        view.setGravity(Gravity.CENTER);
        view.setTypeface(Typeface.create("sans-serif-condensed", Typeface.NORMAL));
        return view;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        FrameLayout root = new FrameLayout(this);
        root.setBackgroundResource(com.libertyrecomp.R.drawable.launcher_art);

        View shade = new View(this);
        shade.setBackgroundColor(0x99070B10);
        root.addView(shade, new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ));

        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        content.setGravity(Gravity.CENTER);
        content.setPadding(dp(28), dp(12), dp(28), dp(12));

        ImageView logo = new ImageView(this);
        logo.setImageResource(com.libertyrecomp.R.drawable.launcher_logo);
        logo.setAdjustViewBounds(true);
        logo.setScaleType(ImageView.ScaleType.FIT_CENTER);
        LinearLayout.LayoutParams logoParams = new LinearLayout.LayoutParams(dp(300), dp(112));
        logoParams.gravity = Gravity.CENTER;
        content.addView(logo, logoParams);

        TextView title = label("GTA IV UNIVERSAL", 25, Color.WHITE);
        title.setTypeface(Typeface.create("sans-serif-condensed", Typeface.BOLD));
        title.setLetterSpacing(0.12f);
        LinearLayout.LayoutParams titleParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        titleParams.topMargin = dp(4);
        content.addView(title, titleParams);

        TextView subtitle = label("LIBERTY CITY  •  NATIVE ANDROID RUNTIME", 11, 0xFFE2D5B8);
        subtitle.setLetterSpacing(0.08f);
        LinearLayout.LayoutParams subParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        subParams.topMargin = dp(7);
        subParams.bottomMargin = dp(22);
        content.addView(subtitle, subParams);

        Button playButton = new Button(this);
        playButton.setText("PLAY");
        playButton.setTextSize(16);
        playButton.setTextColor(0xFF171717);
        playButton.setAllCaps(true);
        playButton.setTypeface(Typeface.create("sans-serif-condensed", Typeface.BOLD));
        GradientDrawable buttonBg = new GradientDrawable();
        buttonBg.setColor(0xFFE0C58F);
        buttonBg.setCornerRadius(dp(4));
        playButton.setBackground(buttonBg);
        playButton.setPadding(dp(42), dp(7), dp(42), dp(7));
        playButton.setOnClickListener(v -> {
            Intent intent = new Intent(this, LibertyActivity.class);
            startActivity(intent);
        });
        content.addView(playButton);

        TextView footer = label("GTA IV UNIVERSAL  |  ANDROID ARM64", 10, 0xFFCBC7BE);
        footer.setLetterSpacing(0.08f);
        LinearLayout.LayoutParams footerParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        footerParams.topMargin = dp(24);
        content.addView(footer, footerParams);

        FrameLayout.LayoutParams contentParams = new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        );
        root.addView(content, contentParams);
        setContentView(root);
    }
}
