#!/usr/bin/env python3
"""Capture Android's native-crash tombstone for the previous :game process."""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
if len(sys.argv) != 2:
    raise SystemExit("usage: apply_native_tombstone_capture.py <runtime/xhyn-android>")
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

capture_path = "os/android/app/src/main/java/com/libertyrecomp/PreviousExitCapture.java"
capture_old = "    SessionHistory.save(data,id,p);if(captured.size()==2)break;\n"
capture_new = """    SessionHistory.save(data,id,p);
    if (process.equals(a.getPackageName()+":game") &&
        (exit.getStatus()==11 || exit.getReason()==ApplicationExitInfo.REASON_CRASH_NATIVE)) {
     captureNativeTrace(exit,data);
    }
    if(captured.size()==2)break;
"""
capture_helper = """
 private static void captureNativeTrace(ApplicationExitInfo exit,File data) {
  if(Build.VERSION.SDK_INT<31)return;
  File folder=SessionHistory.directory(data);
  File traceTemporary=null,metadataTemporary=null;
  try(InputStream trace=exit.getTraceInputStream()) {
   if(trace==null)return;
   if(!folder.exists()&&!folder.mkdirs())return;
   File target=new File(folder,"previous-native-crash.pb");
   File metadata=new File(folder,"previous-native-crash.properties");
   traceTemporary=File.createTempFile("native-crash-",".pb.tmp",folder);
   int total=0;
   try(FileOutputStream out=new FileOutputStream(traceTemporary)) {
    byte[] buffer=new byte[8192];int count;final int limit=4*1024*1024;
    while(total<limit&&(count=trace.read(buffer,0,Math.min(buffer.length,limit-total)))!=-1) {
     out.write(buffer,0,count);total+=count;
    }
    out.flush();out.getFD().sync();
   }
   String details=String.join(System.lineSeparator(),
     "format=Android ApplicationExitInfo native tombstone protobuf",
     "process="+exit.getProcessName(),
     "timestamp="+exit.getTimestamp(),
     "reason="+label(exit.getReason())+" ("+exit.getReason()+")",
     "status="+exit.getStatus(),
     "trace_bytes="+total,
     "");
   metadataTemporary=File.createTempFile("native-crash-meta-",".tmp",folder);
   try(FileOutputStream out=new FileOutputStream(metadataTemporary)) {
    out.write(details.getBytes(java.nio.charset.StandardCharsets.UTF_8));
    out.flush();out.getFD().sync();
   }
   java.nio.file.Files.move(traceTemporary.toPath(),target.toPath(),
     java.nio.file.StandardCopyOption.REPLACE_EXISTING);
   traceTemporary=null;
   java.nio.file.Files.move(metadataTemporary.toPath(),metadata.toPath(),
     java.nio.file.StandardCopyOption.REPLACE_EXISTING);
   metadataTemporary=null;
  } catch(Exception ignored) {
  } finally {
   if(traceTemporary!=null)traceTemporary.delete();
   if(metadataTemporary!=null)metadataTemporary.delete();
  }
 }
"""
replace_once(capture_path, capture_old, capture_new)
source=(root/capture_path).read_text(encoding="utf-8")
anchor=" static String label(int reason){"
if capture_helper.strip() not in source:
    if source.count(anchor)!=1: raise SystemExit("Could not locate exit label helper")
    (root/capture_path).write_text(source.replace(anchor,capture_helper+"\n"+anchor),encoding="utf-8")

archive_path = "os/android/app/src/main/java/com/libertyrecomp/DebugLogArchive.java"
replace_once(archive_path,
 'new String[]{"last-session.properties","previous-exit.properties","previous-launcher-exit.properties","launcher-crash.txt"}',
 'new String[]{"last-session.properties","previous-exit.properties","previous-launcher-exit.properties","previous-native-crash.properties","previous-native-crash.pb","launcher-crash.txt"}')
replace_once(archive_path,
 'if(f.isFile()&&!java.nio.file.Files.isSymbolicLink(f.toPath())&&f.length()<=65536)add(zip,f,"context/"+name);',
 'if(f.isFile()&&!java.nio.file.Files.isSymbolicLink(f.toPath())&&(f.length()<=65536||(name.equals("previous-native-crash.pb")&&f.length()<=4L*1024*1024)))add(zip,f,"context/"+name);')
print("Added bounded API 31+ native tombstone capture for signal-11 / native crashes in the game process.")
print("The next debug archive includes raw previous-native-crash.pb plus a readable metadata sidecar when Android provides a trace.")
