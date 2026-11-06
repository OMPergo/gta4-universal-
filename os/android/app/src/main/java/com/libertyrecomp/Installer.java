package com.libertyrecomp;

import android.Manifest;
import android.app.Activity;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.provider.Settings;
import android.system.Os;
import android.util.Log;

import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Enumeration;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * The game installer for the launcher: the same native code as the in-game
 * installer (libliberty_install.so, gta4-recomp/src/android/android_install_jni.cpp),
 * one installation per process at a time, plus what the launcher needs around
 * it - file permissions and turning picked documents into file paths.
 */
final class Installer {

    private static final String TAG = "LibertyInstall";

    /** A base-game source check: whether it is supported and what was found. */
    static final class Inspection {
        final boolean supported;
        final String summary;
        final String diagnostics;

        Inspection(boolean supported, String summary, String diagnostics) {
            this.supported = supported;
            this.summary = summary;
            this.diagnostics = diagnostics;
        }
    }

    private static boolean sLoaded;
    private static String sLoadError;

    /** The running or last installation of this process. */
    private static Thread sThread;
    private static volatile boolean sRunning;
    private static volatile String sResult;

    private Installer() {}

    static native String nativeInspect(String path);
    static native String nativeInstallState(String gameRoot);
    static native String nativePrepareAchievements(String gameRoot, String userRoot);
    static native String nativeInstall(String game, String update, String tlad, String tbogt,
                                       String installRoot);
    static native long[] nativeProgress();
    static native void nativeCancel();

    /**
     * Loads the native installer and points it at the title resources (the
     * RPF key) copied out of the APK. False when it cannot be used; see
     * {@link #loadError()}.
     */
    static synchronized boolean load(Context context) {
        if (sLoaded) return true;
        try {
            File resources = LibertyActivity.syncResources(context);
            Os.setenv("REX_RESOURCES_DIR", resources.getAbsolutePath(), true);
            System.loadLibrary("liberty_install");
            sLoaded = true;
        } catch (Throwable e) {
            sLoadError = e.getMessage();
            Log.e(TAG, "native installer unavailable", e);
        }
        return sLoaded;
    }

    static String loadError() {
        return sLoadError;
    }

    /** The folder the game is installed into (files/LibertyRecomp). */
    static File installRoot(File dataRoot) {
        return GameStorage.installRoot(dataRoot);
    }

    /** Null when the installed game can start, otherwise why not. */
    static String installState(File dataRoot) {
        String reason = nativeInstallState(new File(installRoot(dataRoot), "game").getAbsolutePath());
        return reason == null || reason.isEmpty() ? null : reason;
    }

    static Inspection inspect(String path) {
        try {
            JSONObject json = new JSONObject(nativeInspect(path));
            return new Inspection(json.optBoolean("supported"), json.optString("summary"),
                    json.optString("diagnostics"));
        } catch (Exception e) {
            return new Inspection(false, "The source could not be checked: " + e.getMessage(), "");
        }
    }


    /**
     * Accepts the user's regional TU8 ZIP and stages only the USA 6AC07221
     * STFS package. The native installer still verifies the actual XEXP hash,
     * title ID, base revision, and source signature before installation.
     */
    /**
     * Identifies the package from its STFS metadata, not its filename.
     * Regional archives often use opaque package filenames that do not contain
     * the Media ID. XContent metadata begins at 0x344; execution info begins
     * at 0x354 (Media ID) and contains the Title ID at 0x360.
     */
    private static boolean isUsaGta4TitleUpdate(ZipFile zip, ZipEntry entry)
            throws java.io.IOException {
        final int headerBytes = 0x364;
        if (entry.getSize() >= 0 && entry.getSize() < headerBytes) return false;
        byte[] header = new byte[headerBytes];
        int offset = 0;
        try (InputStream input = zip.getInputStream(entry)) {
            while (offset < header.length) {
                int count = input.read(header, offset, header.length - offset);
                if (count < 0) return false;
                if (count == 0) continue;
                offset += count;
            }
        }
        if (header[0] != 'L' || header[1] != 'I'
                || header[2] != 'V' || header[3] != 'E') {
            return false;
        }
        ByteBuffer metadata = ByteBuffer.wrap(header).order(ByteOrder.BIG_ENDIAN);
        int contentType = metadata.getInt(0x344);
        int mediaId = metadata.getInt(0x354);
        int titleId = metadata.getInt(0x360);
        return contentType == 0x000B0000
                && titleId == 0x545407F2
                && mediaId == 0x6AC07221;
    }

    static File extractUsaTu8Package(Context context, String archivePath) throws java.io.IOException {
        File archive = new File(archivePath);
        if (!archive.isFile() || !archive.canRead()) {
            throw new java.io.IOException("The selected Title Update ZIP cannot be read.");
        }

        final long maxPackageBytes = 16L * 1024L * 1024L;
        File directory = new File(context.getFilesDir(), "title-updates/tu8");
        if (!directory.isDirectory() && !directory.mkdirs()) {
            throw new java.io.IOException("Could not create the app's Title Update staging folder.");
        }

        try (ZipFile zip = new ZipFile(archive)) {
            ZipEntry selected = null;
            Enumeration<? extends ZipEntry> entries = zip.entries();
            while (entries.hasMoreElements()) {
                ZipEntry entry = entries.nextElement();
                if (entry.isDirectory()) continue;
                if (entry.getSize() > maxPackageBytes) {
                    continue;
                }
                if (isUsaGta4TitleUpdate(zip, entry)) {
                    if (selected != null) {
                        throw new java.io.IOException("The ZIP contains more than one USA GTA IV Title Update package.");
                    }
                    selected = entry;
                }
            }
            if (selected == null) {
                throw new java.io.IOException(
                        "No USA TU8 package (Media ID 6AC07221) was found in this ZIP. "
                        + "Choose the regional TU8 ZIP or select the USA package file directly.");
            }

            File temporary = new File(directory, "TU8-USA-6AC07221.live.tmp");
            File output = new File(directory, "TU8-USA-6AC07221.live");
            long copied = 0;
            try (InputStream input = zip.getInputStream(selected);
                 FileOutputStream stream = new FileOutputStream(temporary, false)) {
                byte[] buffer = new byte[64 * 1024];
                int count;
                while ((count = input.read(buffer)) != -1) {
                    copied += count;
                    if (copied > maxPackageBytes) {
                        throw new java.io.IOException("The USA Title Update package exceeded the safe size limit.");
                    }
                    stream.write(buffer, 0, count);
                }
                stream.getFD().sync();
            } catch (java.io.IOException error) {
                //noinspection ResultOfMethodCallIgnored
                temporary.delete();
                throw error;
            }

            byte[] magic = new byte[4];
            try (InputStream input = new FileInputStream(temporary)) {
                if (input.read(magic) != magic.length
                        || magic[0] != 'L' || magic[1] != 'I' || magic[2] != 'V' || magic[3] != 'E') {
                    //noinspection ResultOfMethodCallIgnored
                    temporary.delete();
                    throw new java.io.IOException("The selected USA update entry is not a valid Xbox LIVE/STFS package.");
                }
            }
            if (copied < 1024) {
                //noinspection ResultOfMethodCallIgnored
                temporary.delete();
                throw new java.io.IOException("The USA Title Update package is unexpectedly small.");
            }
            if (output.exists() && !output.delete()) {
                temporary.delete();
                throw new java.io.IOException("Could not replace the previously staged Title Update.");
            }
            if (!temporary.renameTo(output)) {
                temporary.delete();
                throw new java.io.IOException("Could not finish staging the USA Title Update package.");
            }
            return output;
        } catch (java.util.zip.ZipException error) {
            throw new java.io.IOException("The selected file is not a readable ZIP archive.", error);
        }
    }

    // -------------------------------------------------------------- the job

    static synchronized boolean start(String game, String update, String tlad, String tbogt,
                                      File dataRoot) {
        if (sRunning) return false;
        sRunning = true;
        sResult = null;
        File root = installRoot(dataRoot);
        //noinspection ResultOfMethodCallIgnored
        root.mkdirs();
        sThread = new Thread(() -> {
            String error;
            try {
                error = nativeInstall(nz(game), nz(update), nz(tlad), nz(tbogt),
                        root.getAbsolutePath());
                if (error == null || error.isEmpty()) {
                    String metadata = nativePrepareAchievements(new File(root, "game").getAbsolutePath(),
                            new File(dataRoot, "LibertyRecomp/saves").getAbsolutePath());
                    if (metadata != null && !metadata.isEmpty()) Log.w(TAG, metadata);
                }
            } catch (Throwable e) {
                error = "The installer stopped: " + e.getMessage();
            }
            sResult = error == null ? "" : error;
            sRunning = false;
        }, "GameInstall");
        sThread.start();
        return true;
    }

    static boolean running() {
        return sRunning;
    }

    /** Called on a worker after the launcher has blocked other file actions. */
    static String prepareAchievements(Context context, File dataRoot) {
        if (!load(context)) return "Achievement preparation is unavailable: " + loadError();
        if (sRunning) return "Wait for installation to finish.";
        try {
            return nativePrepareAchievements(new File(installRoot(dataRoot), "game").getAbsolutePath(),
                    new File(dataRoot, "LibertyRecomp/saves").getAbsolutePath());
        } catch (Throwable error) {
            return "Achievement preparation stopped: " + error.getMessage();
        }
    }

    /** Null while nothing finished; "" after a success; the error otherwise. */
    static String takeResult() {
        String result = sResult;
        sResult = null;
        return result;
    }

    static long[] progress() {
        return sLoaded ? nativeProgress() : new long[] {0, 0};
    }

    static void cancel() {
        if (sLoaded) nativeCancel();
    }

    private static String nz(String value) {
        return value == null ? "" : value;
    }

    // ----------------------------------------------------------- permissions

    /** Whether files outside the app's own folder can be opened by path. */
    static boolean hasFileAccess(Context context) {
        if (Build.VERSION.SDK_INT >= 30) return Environment.isExternalStorageManager();
        return context.checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE)
                == PackageManager.PERMISSION_GRANTED;
    }

    /** Opens the system screen (or prompt) that grants {@link #hasFileAccess}. */
    static void requestFileAccess(Activity activity, int requestCode) {
        if (Build.VERSION.SDK_INT >= 30) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION,
                    Uri.parse("package:" + activity.getPackageName()));
            try {
                activity.startActivity(intent);
            } catch (Exception e) {
                activity.startActivity(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
            }
        } else {
            activity.requestPermissions(new String[] {Manifest.permission.READ_EXTERNAL_STORAGE},
                    requestCode);
        }
    }

    // ------------------------------------------------------- documents → paths

    /**
     * The file path behind a document picked in the system file picker, or
     * null when it has none (cloud storage). The installer reads sources by
     * path, which works for the device storage and SD cards once
     * {@link #hasFileAccess} holds.
     */
    static String pathFromUri(Context context, Uri uri) {
        if (uri == null) return null;
        if ("file".equals(uri.getScheme())) return uri.getPath();
        try {
            if (DocumentsContract.isDocumentUri(context, uri)) {
                String authority = uri.getAuthority();
                String id = DocumentsContract.getDocumentId(uri);
                if ("com.android.externalstorage.documents".equals(authority)) {
                    int colon = id.indexOf(':');
                    String volume = colon < 0 ? id : id.substring(0, colon);
                    String relative = colon < 0 ? "" : id.substring(colon + 1);
                    File base = "primary".equalsIgnoreCase(volume)
                            ? Environment.getExternalStorageDirectory()
                            : new File("/storage/" + volume);
                    return new File(base, relative).getAbsolutePath();
                }
                if ("com.android.providers.downloads.documents".equals(authority)) {
                    if (id.startsWith("raw:")) return id.substring(4);
                    if (id.startsWith("msf:")) {
                        return dataColumn(context, ContentUris.withAppendedId(
                                MediaStore.Files.getContentUri("external"),
                                Long.parseLong(id.substring(4))));
                    }
                    try {
                        String path = dataColumn(context, ContentUris.withAppendedId(
                                Uri.parse("content://downloads/public_downloads"), Long.parseLong(id)));
                        if (path != null) return path;
                    } catch (NumberFormatException ignored) {
                    }
                }
                if ("com.android.providers.media.documents".equals(authority)) {
                    int colon = id.indexOf(':');
                    if (colon > 0) {
                        return dataColumn(context, ContentUris.withAppendedId(
                                MediaStore.Files.getContentUri("external"),
                                Long.parseLong(id.substring(colon + 1))));
                    }
                }
            }
            return dataColumn(context, uri);
        } catch (Exception e) {
            Log.w(TAG, "no path for " + uri, e);
            return null;
        }
    }

    @SuppressWarnings("deprecation")
    private static String dataColumn(Context context, Uri uri) {
        try (Cursor cursor = context.getContentResolver().query(uri,
                new String[] {MediaStore.MediaColumns.DATA}, null, null, null)) {
            if (cursor != null && cursor.moveToFirst() && !cursor.isNull(0)) {
                return cursor.getString(0);
            }
        } catch (Exception ignored) {
        }
        return null;
    }
}
