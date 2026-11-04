# Building for Android

## Native Core Compilation
The native core subsystems (`LibertyUniversalCore`) can be built for Android ARM64 using the Android NDK:

### Prerequisites
- Android SDK (API 35, Build Tools 35.0.0)
- Android NDK (r27+ recommended)
- CMake 3.22+ and Ninja

### Native CMake Build (Android ARM64)
```bash
cmake -B build-android-arm64 \
  -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-35 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-android-arm64 --target LibertyUniversalCore
```

## Android Application Packaging (APK)
Full packaging of the standalone Android APK (`app-debug.apk`) is handled by the Android Gradle application module (`os/android`):

### Native Pipeline Staging
Gradle does not build the C++ recompilation core directly; instead, native binaries (`libmain.so`, `librexruntime.so`, `libSDL3.so`, `libc++_shared.so`) and title assets (`src/main/assets/Resources`) are staged into `os/android/app/src/main/jniLibs/arm64-v8a` via `os/android/scripts/build_native.sh`.

### APK Assembly
Once native libraries are staged:
```bash
cd os/android
./gradlew assembleDebug
```
The output APK is generated at:
`os/android/app/build/outputs/apk/debug/app-debug.apk`

*Note: In CI environments where only the core native library is compiled and tested, an installable APK is not produced until the complete upstream recompiled PowerPC executable tree and staged runtime assets are included.*
