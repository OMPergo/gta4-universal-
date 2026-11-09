#!/usr/bin/env python3
"""Apply the reviewed GTA IV Universal Android low-end build profile.

The Android runtime is a pinned submodule. Keep the patch here so builds are
reproducible without silently modifying the upstream mirror.
"""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
if len(sys.argv) != 2:
    raise SystemExit("usage: apply_android_low_end_profile.py <runtime/xhyn-android>")
root = Path(sys.argv[1]).resolve()
actual = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
if actual != PIN:
    raise SystemExit(f"Refusing to patch unexpected Android runtime: {actual} (expected {PIN})")

def replace_once(path: str, old: str, new: str) -> None:
    file = root / path
    source = file.read_text(encoding="utf-8")
    if new in source:
        return
    if source.count(old) != 1:
        raise SystemExit(f"Expected exactly one patch anchor in {path}; found {source.count(old)}")
    file.write_text(source.replace(old, new), encoding="utf-8")

mobile_old = '    static void ensureDefaults(File file)throws IOException {Map<String,String> saved=read(file),missing=new LinkedHashMap<>();for(Map.Entry<String,String> e:DEFAULTS.entrySet())if(!saved.containsKey(e.getKey()))missing.put(e.getKey(),e.getValue());if(!missing.isEmpty())update(file,missing);}'
mobile_new = '''
    // The low-end profile is selected from device RAM/GPU by the launcher.
    // It only fills missing settings; a player can still change every option.
    private static final Map<String,String> LOW_END_DEFAULTS=Collections.unmodifiableMap(new LinkedHashMap<String,String>(){{
        put("resolution","720p");
        put("gta4_native_upscaler","fsr1");
        put("gta4_fsr1_quality","performance");
        put("gta4_frame_limit","30");
        put("gta4_dynamic_draw_distance","true");
        put("gta4_draw_distance_scale","0.65");
        put("gta4_shadow_map_base_size","256");
        put("gta4_shadow_distance_scale","1.0");
        put("gta4_native_shadow_interval","0");
        put("gta4_reflection_resolution","quarter");
        put("gta4_water_reflection_resolution","quarter");
        put("gta4_mirror_reflection_resolution","quarter");
        put("gta4_reflection_aa","off");
        put("gta4_reflection_capture_distance","near");
        put("gta4_native_max_queued_frames","2");
        put("gta4_native_pipeline_workers","1");
        put("gta4_native_pipeline_prewarm","true");
        put("gta4_native_pipeline_lookahead","true");
        put("gta4_native_converted_texture_disk_cache","true");
        put("gta4_traffic_density_scale","0.30");
        put("gta4_parked_car_density_scale","0.30");
        put("gta4_ped_density_scale","0.30");
        put("gta4_scenario_ped_density_scale","0.30");
        put("android_perf_hint","true");
        put("android_thermal_guard","true");
        put("android_gpu_turbo","false");
        put("audio_adaptive_buffer","true");
    }});
    static void ensureDefaults(File file)throws IOException {ensureDefaults(file,false);}
    static void ensureDefaults(File file,boolean lowEnd)throws IOException {
        Map<String,String> saved=read(file),missing=new LinkedHashMap<>();
        for(Map.Entry<String,String> e:DEFAULTS.entrySet())if(!saved.containsKey(e.getKey()))missing.put(e.getKey(),e.getValue());
        if(!missing.isEmpty())update(file,missing);
        if(lowEnd) {
            File marker=new File(file.getParentFile(),"universal-low-end-profile-v1.applied");
            if(!marker.isFile()) {
                // Apply the measured A14 preset once; preserve later player edits.
                update(file,LOW_END_DEFAULTS);
                try(FileOutputStream out=new FileOutputStream(marker)) {
                    out.write("profile=v1\\ndevice-class=low-end\\n".getBytes(StandardCharsets.UTF_8));
                    out.getFD().sync();
                }
            }
        }
    }'''
replace_once("os/android/app/src/main/java/com/libertyrecomp/MobileConfig.java",mobile_old,mobile_new)

launcher_old='    private boolean mBusy;'
launcher_new='''    private boolean mBusy;

    /** Conservative defaults for devices with <=6 GiB RAM or Mali-G57-class GPU. */
    private boolean isLowEndDevice() {
        ActivityManager manager=(ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memory=new ActivityManager.MemoryInfo();
        if(manager!=null)manager.getMemoryInfo(memory);
        String renderer=GpuInfo.renderer().toLowerCase(Locale.ROOT);
        boolean lowMemory=memory.totalMem>0&&memory.totalMem<=6L*1024L*1024L*1024L;
        boolean targetMali=renderer.contains("mali-g57");
        return lowMemory||targetMali;
    }'''
replace_once("os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java",launcher_old,launcher_new)
replace_once("os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java",
    "MobileConfig.ensureDefaults(MobileConfig.file(mDataRoot));",
    "MobileConfig.ensureDefaults(MobileConfig.file(mDataRoot),isLowEndDevice());")
replace_once("os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java",
    'TextView brand=ModernUi.text(this,"XHYN IV",38,ModernUi.TEXT);',
    'TextView brand=ModernUi.text(this,"GTA IV UNIVERSAL",34,ModernUi.TEXT);')
replace_once("os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java",
    'LinearLayout footer=new LinearLayout(this);footer.setGravity(Gravity.CENTER_VERTICAL);footer.addView(ModernUi.text(this,versionLine(),11,ModernUi.MUTED),new LinearLayout.LayoutParams(-2,-2));LinearLayout links=new LinearLayout(this);links.setGravity(Gravity.END|Gravity.CENTER_VERTICAL);addFooterLink(links,"YouTube ↗","https://youtube.com/@xhyn_ph");addFooterLink(links,"Telegram ↗","https://t.me/xhynports");addFooterLink(links,"GitHub ↗","https://github.com/XHYN-PH/XHYN-IV-recomp");HorizontalScrollView linkStrip=new HorizontalScrollView(this);linkStrip.setHorizontalScrollBarEnabled(false);linkStrip.setFillViewport(true);linkStrip.addView(links);footer.addView(linkStrip,new LinearLayout.LayoutParams(0,-2,1));root.addView(footer,matchWrap(dp(3)));',
    'LinearLayout footer=new LinearLayout(this);footer.setGravity(Gravity.CENTER_VERTICAL);footer.addView(ModernUi.text(this,"GTA IV UNIVERSAL  ·  "+versionLine(),11,ModernUi.MUTED),new LinearLayout.LayoutParams(-2,-2));root.addView(footer,matchWrap(dp(3)));')

replace_once("os/android/app/build.gradle.kts",
    'applicationId = "com.xhyn.ivrecomp"',
    'applicationId = "com.ompergo.gta4universal"')
replace_once("os/android/app/build.gradle.kts",
    'versionCode = 40\n        versionName   = "0.5.9.3-xhyn-r2-test12"',
    'versionCode = 1\n        versionName   = "0.1.0-universal-lowend-alpha"')
replace_once("os/android/app/src/main/res/values/strings.xml",
    '<string name="app_name">XHYN IV</string>',
    '<string name="app_name">GTA IV Universal</string>')
print("Applied GTA IV Universal branding and low-end defaults.")
print("Detection: <=6 GiB RAM or Mali-G57 renderer.")
print("Preset: FSR1 performance, 30 FPS, 2 queued frames, reduced draw/reflection/shadow load, thermal guard.")
print("Existing per-user settings are preserved; only missing settings receive defaults.")
