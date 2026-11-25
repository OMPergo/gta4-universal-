#!/usr/bin/env python3
"""Apply the reviewed, allowlisted GTA IV Universal Android low-end profile."""
from pathlib import Path
import re
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

# Replace the compact upstream method by matching its full source line. The
# previous literal anchor was brittle across whitespace-only upstream changes.
mobile_file = root / "os/android/app/src/main/java/com/libertyrecomp/MobileConfig.java"
mobile_source = mobile_file.read_text(encoding="utf-8")
mobile_pattern = r"(?m)^    static void ensureDefaults\\(File file\\)throwsIOException \\{[^\\n]*\\}$"
mobile_new = """    static void ensureDefaults(File file)throws IOException {Map<String,String> saved=read(file),missing=new LinkedHashMap<>();for(Map.Entry<String,String> e:DEFAULTS.entrySet())if(!saved.containsKey(e.getKey()))missing.put(e.getKey(),e.getValue());if(!missing.isEmpty())update(file,missing);}
    private static final Map<String,String> LOW_END_DEFAULTS=Collections.unmodifiableMap(new LinkedHashMap<String,String>(){{
        put("gta4_frame_limit","30");
        put("gta4_dynamic_draw_distance","true");
        put("gta4_draw_distance_scale","0.60");
        put("gta4_native_shadow_interval","0");
        put("gta4_reflection_capture_distance","near");
        put("gta4_native_water_reflection_interval","2");
        put("gta4_native_environment_reflection_interval","2");
        put("gta4_motion_blur","false");
        put("audio_adaptive_buffer","true");
        put("android_thermal_guard","true");
    }});
    static void applyLowEndDefaults(File file)throws IOException {
        File marker=new File(file.getParentFile(),"universal-low-end-profile-v1.applied");
        if(marker.isFile())return;
        update(file,LOW_END_DEFAULTS);
        try(FileOutputStream out=new FileOutputStream(marker)) {
            out.write("profile=v1\\ndevice-class=low-end\\n".getBytes(StandardCharsets.UTF_8));
            out.getFD().sync();
        }
    }"""
mobile_source, count = re.subn(mobile_pattern, mobile_new, mobile_source)
if count != 1 and "applyLowEndDefaults(File file)" not in mobile_source:
    raise SystemExit(f"Expected one MobileConfig ensureDefaults method; found {count}")
mobile_file.write_text(mobile_source, encoding="utf-8")

launcher_old = "    private boolean mBusy;"
launcher_new = """    private boolean mBusy;

    /** Conservative defaults for devices with 6 GiB RAM or less. */
    private boolean isLowEndDevice() {
        ActivityManager manager=(ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memory=new ActivityManager.MemoryInfo();
        if(manager!=null)manager.getMemoryInfo(memory);
        return memory.totalMem>0&&memory.totalMem<=6L*1024L*1024L*1024L;
    }"""
replace_once("os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java", launcher_old, launcher_new)
replace_once(
    "os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java",
    "MobileConfig.ensureDefaults(MobileConfig.file(mDataRoot));",
    "MobileConfig.ensureDefaults(MobileConfig.file(mDataRoot));if(isLowEndDevice())MobileConfig.applyLowEndDefaults(MobileConfig.file(mDataRoot));"
)
replace_once("os/android/app/build.gradle.kts", 'applicationId = "com.xhyn.ivrecomp"', 'applicationId = "com.ompergo.gta4universal"')
replace_once(
    "os/android/app/build.gradle.kts",
    'versionCode = 40\\n        versionName   = "0.5.9.3-xhyn-r2-test12"',
    'versionCode = 1\\n        versionName   = "0.1.0-universal-lowend-alpha"'
)
replace_once("os/android/app/src/main/res/values/strings.xml", '<string name="app_name">XHYN IV</string>', '<string name="app_name">GTA IV Universal</string>')
print("Applied Universal low-end defaults using only the pinned JNI settings allowlist.")
print("Detection: Android total memory <=6 GiB; existing player edits are preserved after first application.")
