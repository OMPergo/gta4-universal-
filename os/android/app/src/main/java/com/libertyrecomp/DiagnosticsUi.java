package com.libertyrecomp;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.StatFs;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/** Device-side dashboard. Runtime values not yet instrumented remain explicitly unknown. */
final class DiagnosticsUi {
    static void show(Activity a, File dataRoot) {
        LinearLayout body=new LinearLayout(a);body.setOrientation(LinearLayout.VERTICAL);
        section(a,body,"DEVICE SUMMARY");
        row(a,body,"Model",Build.MANUFACTURER+" "+Build.MODEL);
        row(a,body,"Android",Build.VERSION.RELEASE+" (API "+Build.VERSION.SDK_INT+")");
        row(a,body,"GPU / renderer",safeRenderer());
        ActivityManager am=(ActivityManager)a.getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo mi=new ActivityManager.MemoryInfo();if(am!=null)am.getMemoryInfo(mi);
        row(a,body,"RAM",mi.totalMem>0?String.format(Locale.US,"%.1f GB total · %.1f GB free",mi.totalMem/1073741824.0,mi.availMem/1073741824.0):"Unknown");
        try{StatFs fs=new StatFs(dataRoot.getAbsolutePath());row(a,body,"Storage free",String.format(Locale.US,"%.1f GB",fs.getAvailableBytes()/1073741824.0));}catch(Exception e){row(a,body,"Storage free","Unknown");}
        section(a,body,"GAME BUILD");
        row(a,body,"App version",version(a));row(a,body,"Package",a.getPackageName());
        row(a,body,"Game data",new File(dataRoot,"game").isDirectory()?"Game folder detected":"Not detected");
        section(a,body,"GRAPHICS & AUDIO");
        row(a,body,"Renderer","Vulkan · "+safeRenderer());
        row(a,body,"SDL audio","Unknown — not captured");
        row(a,body,"Playback stream","Unknown — not captured");
        row(a,body,"XMA decoder","Unknown — not captured");
        section(a,body,"CREATE TEST REPORT");
        body.addView(ModernUi.text(a,"Select what happened. Device and build details are included automatically.",12,ModernUi.MUTED));
        String[] labels={"Crash / app closed","Black screen","No audio","Stuttering / low FPS","Missing textures","Controls not working","Other"};
        List<CheckBox> checks=new ArrayList<>();
        for(String label:labels){CheckBox c=new CheckBox(a);c.setText(label);c.setTextColor(ModernUi.TEXT);c.setTextSize(14);c.setButtonTintList(android.content.res.ColorStateList.valueOf(0xff43b8ef));body.addView(c);checks.add(c);}
        EditText notes=new EditText(a);notes.setTextColor(ModernUi.TEXT);notes.setHintTextColor(ModernUi.MUTED);notes.setHint("Describe what happened and what you were doing…");notes.setMinLines(3);notes.setGravity(android.view.Gravity.TOP|android.view.Gravity.START);notes.setTextSize(13);notes.setBackground(ModernUi.surface(a,0xff102438,10));notes.setPadding(ModernUi.dp(a,12),ModernUi.dp(a,10),ModernUi.dp(a,12),ModernUi.dp(a,10));body.addView(notes,new LinearLayout.LayoutParams(-1,-2));
        ScrollView scroll=ModernUi.scroll(a,112);scroll.addView(body);
        android.app.AlertDialog dialog=ModernUi.screen(a,"Diagnostics center",scroll,"Share report",()->{
            StringBuilder report=new StringBuilder("GTA IV UNIVERSAL — TEST REPORT\n");
            report.append("Device: ").append(Build.MANUFACTURER).append(" ").append(Build.MODEL).append("\nAndroid: ").append(Build.VERSION.RELEASE).append(" (API ").append(Build.VERSION.SDK_INT).append(")\nGPU: ").append(safeRenderer()).append("\nApp: ").append(version(a)).append("\n");
            report.append("SDL audio: Unknown (not captured)\nPlayback stream: Unknown (not captured)\nXMA decoder: Unknown (not captured)\n\nObserved issues:\n");
            boolean any=false;for(CheckBox c:checks)if(c.isChecked()){report.append("- ").append(c.getText()).append("\n");any=true;}if(!any)report.append("Not specified\n");
            report.append("\nNotes:\n").append(notes.getText());
            Intent send=new Intent(Intent.ACTION_SEND);send.setType("text/plain");send.putExtra(Intent.EXTRA_SUBJECT,"GTA IV Universal diagnostic report");send.putExtra(Intent.EXTRA_TEXT,report.toString());
            a.startActivity(Intent.createChooser(send,"Share diagnostic report"));
        },()->true);
        dialog.show();
    }
    private static void section(Activity a,LinearLayout p,String label){TextView t=ModernUi.text(a,label,11,0xff43b8ef);t.setTypeface(android.graphics.Typeface.DEFAULT_BOLD);t.setPadding(0,ModernUi.dp(a,12),0,ModernUi.dp(a,4));p.addView(t);}
    private static void row(Activity a,LinearLayout p,String key,String value){LinearLayout r=new LinearLayout(a);r.setPadding(ModernUi.dp(a,10),ModernUi.dp(a,8),ModernUi.dp(a,10),ModernUi.dp(a,8));r.setBackground(ModernUi.surface(a,0xff102438,8));TextView k=ModernUi.text(a,key,12,ModernUi.MUTED);r.addView(k,new LinearLayout.LayoutParams(0,-2,1));TextView v=ModernUi.text(a,value,12,ModernUi.TEXT);v.setGravity(android.view.Gravity.RIGHT);r.addView(v,new LinearLayout.LayoutParams(0,-2,1));LinearLayout.LayoutParams lp=new LinearLayout.LayoutParams(-1,-2);lp.topMargin=ModernUi.dp(a,4);p.addView(r,lp);}
    private static String safeRenderer(){try{return GpuInfo.renderer();}catch(Throwable t){return "Unknown";}}
    private static String version(Activity a){try{return a.getPackageManager().getPackageInfo(a.getPackageName(),0).versionName;}catch(Exception e){return "Unknown";}}
    private DiagnosticsUi(){}
}
