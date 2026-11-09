#!/usr/bin/env python3
"""Move the optional portable game location into public Downloads storage."""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
if len(sys.argv) != 2:
    raise SystemExit("usage: apply_public_game_storage.py <runtime/xhyn-android>")
root = Path(sys.argv[1]).resolve()
actual = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
if actual != PIN:
    raise SystemExit(f"Refusing to patch unexpected Android runtime: {actual} (expected {PIN})")

def replace_once(path, old, new):
    file = root / path
    source = file.read_text(encoding="utf-8")
    if new in source:
        return
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"Expected exactly one anchor in {path}; found {count}")
    file.write_text(source.replace(old, new), encoding="utf-8")

storage = "os/android/app/src/main/java/com/libertyrecomp/GameStorage.java"
replace_once(storage,
'''    static File accessible(Activity a)throws IOException {
        File[] media=a.getExternalMediaDirs();if(media!=null)for(File folder:media)if(folder!=null){File target=new File(folder,"XHYN-IV");Files.createDirectories(target.toPath());if(target.canRead()&&target.canWrite())return target.getCanonicalFile();}
        throw new IOException("This device does not provide an accessible app media folder.");
    }''',
'''    static File accessible(Activity a)throws IOException {
        if(android.os.Build.VERSION.SDK_INT>=30&&!android.os.Environment.isExternalStorageManager())
            throw new IOException("Enable All files access for GTA IV Universal in Android Settings, then try again.");
        File downloads=android.os.Environment.getExternalStoragePublicDirectory(android.os.Environment.DIRECTORY_DOWNLOADS);
        File target=new File(downloads,"GTA IV Universal");
        Files.createDirectories(target.toPath());
        if(!target.canRead()||!target.canWrite())throw new IOException("The public Downloads folder is not accessible. Check All files access and free storage.");
        return target.getCanonicalFile();
    }''')

launcher = "os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java"
replace_once(launcher, '"Use accessible Android/media folder"', '"Use portable Downloads folder"')
replace_once(launcher,
'''target+"\\n\\nExisting game files are copied and verified first. This needs enough free storage for a second copy. The original remains available."''',
'''target+"\\n\\nExisting game files are copied and verified first. This needs enough free storage for a second copy. The source remains available. Downloads is public storage and is not removed when the app is uninstalled."''')
print("Changed the portable game folder from app-specific Android/media to public Download/GTA IV Universal.")
print("This keeps the game installation outside the app sandbox so it survives app uninstall/reinstall.")
