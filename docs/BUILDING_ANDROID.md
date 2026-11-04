# Building for Android

## Prerequisites
- Android Studio / Android SDK (API 35, Build Tools 35.0.0)
- Android NDK (r27+ recommended)
- CMake 3.22+ and Ninja

## Compilation
```bash
cd os/android
./gradlew assembleDebug
```
The APK will be generated at:
`os/android/app/build/outputs/apk/debug/app-debug.apk`
