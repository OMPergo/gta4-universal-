#!/usr/bin/env python3
"""Apply the GTA IV Universal Android launcher branding to the pinned runtime."""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
if len(sys.argv) != 2:
    raise SystemExit("usage: apply_universal_android_branding.py <runtime/xhyn-android>")
root = Path(sys.argv[1]).resolve()
actual = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
if actual != PIN:
    raise SystemExit(f"Refusing to patch unexpected Android runtime: {actual} (expected {PIN})")

def replace_once(path: str, old: str, new: str) -> None:
    file = root / path
    source = file.read_text(encoding="utf-8")
    if new in source:
        return
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"Expected exactly one branding anchor in {path}; found {count}")
    file.write_text(source.replace(old, new), encoding="utf-8")

launcher = "os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java"
replace_once(launcher, 'ModernUi.text(this,"XHYN IV",38,ModernUi.TEXT)',
             'ModernUi.text(this,"GTA IV UNIVERSAL",32,ModernUi.TEXT)')
replace_once(
    launcher,
    'LinearLayout links=new LinearLayout(this);links.setGravity(Gravity.END|Gravity.CENTER_VERTICAL);addFooterLink(links,"YouTube ↗","https://youtube.com/@xhyn_ph");addFooterLink(links,"Telegram ↗","https://t.me/xhynports");addFooterLink(links,"GitHub ↗","https://github.com/XHYN-PH/XHYN-IV-recomp");HorizontalScrollView linkStrip=new HorizontalScrollView(this);linkStrip.setHorizontalScrollBarEnabled(false);linkStrip.setFillViewport(true);linkStrip.addView(links);footer.addView(linkStrip,new LinearLayout.LayoutParams(0,-2,1));root.addView(footer,matchWrap(dp(3)));',
    'root.addView(footer,matchWrap(dp(3)));'
)
manifest = "os/android/app/src/main/AndroidManifest.xml"
replace_once(manifest, 'android:icon="@mipmap/ic_launcher"', 'android:icon="@drawable/app_icon"')
replace_once(manifest, 'android:roundIcon="@mipmap/ic_launcher_round"', 'android:roundIcon="@drawable/app_icon"')
gradle = "os/android/app/build.gradle.kts"
replace_once(gradle, 'versionCode = 1\n        versionName   = "0.1.0-universal-lowend-alpha"',
             'versionCode = 41\n        versionName   = "0.5.9.4-universal-alpha"')
replace_once(launcher, 'Uri.parse("https://github.com/vaduur/LibertyRecompAndroid")',
             'Uri.parse("https://github.com/OMPergo/gta4-universal-")')
replace_once(launcher, 'toast("github.com/vaduur/LibertyRecompAndroid")',
             'toast("github.com/OMPergo/gta4-universal-")')
replace_once(launcher, 'ClipData.newPlainText("XHYN IV"',
             'ClipData.newPlainText("GTA IV Universal"')
replace_once(launcher, 'Intent.EXTRA_TITLE,"XHYN-IV-"',
             'Intent.EXTRA_TITLE,"GTA4-Universal-"')
replace_once(launcher, 'Intent.EXTRA_TITLE,"XHYN-IV-slot-"',
             'Intent.EXTRA_TITLE,"GTA4-Universal-slot-"')
replace_once(launcher, 'Intent.EXTRA_TITLE,"XHYN-IV-saves.zip"',
             'Intent.EXTRA_TITLE,"GTA4-Universal-saves.zip"')
replace_once(launcher, 'GitHub (vaduur/LibertyRecompAndroid)',
             'GitHub (OMPergo/gta-4-title-8-update)')
if not (root / "os/android/app/src/main/res/drawable-nodpi/app_icon.png").is_file():
    raise SystemExit("Universal launcher icon asset is missing")
print("Applied GTA IV Universal title, pinned Liberty City artwork, custom app icon, and clean footer.")
print("Removed upstream YouTube, Telegram, and GitHub footer links.")
print("Set version 0.5.9.4-universal-alpha (versionCode 41).")
