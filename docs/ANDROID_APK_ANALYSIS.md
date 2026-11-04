# Android APK Generation Analysis & Technical Audit

## 1. Actual Android Project Location & Structure
In the upstream baseline (`XHYN-IV-recomp`), the Android application project is located at:
- Root project: `os/android`
  - `build.gradle.kts` (Root Gradle configuration referencing AGP 8.7.3)
  - `settings.gradle.kts` (Project settings including `:app`)
  - `gradlew` / `gradlew.bat` / `gradle/wrapper/gradle-wrapper.properties` (Gradle 8.11.1 wrapper)
- Application module: `os/android/app`
  - `build.gradle.kts` (Android application plugin, targeting SDK 35, NDK 29.0.14206865, ARM64)
  - `src/main/AndroidManifest.xml` (Declares `LauncherActivity`, `LibertyActivity`, `TouchEditorActivity`)
  - `src/main/java/com/libertyrecomp/` (Launcher UI, driver management, settings, save tools)
  - `src/main/res/` and `src/main/assets/` (Bundled Turnip, ESMERINGHOY, PanVK drivers and licenses)

## 2. Why Gradle Cannot Build an APK Directly From Raw Checkout
As explicitly documented in `os/android/app/build.gradle.kts`:
> `Gradle never runs CMake. The native side (libmain.so, librexruntime.so, the GPU plugins, libSDL3.so, libc++_shared.so) is millions of lines of recompiled PowerPC and is built by os/android/scripts/build_native.sh, which stages the libraries into src/main/jniLibs and the title resources into src/main/assets/Resources. This module only packages them.`

Furthermore:
1. **Missing SDL Java Classes**: The Gradle build relies on `../../glue/rexglue-sdk-main/thirdparty/sdl3/android-project/app/src/main/java`, requiring the exact pinned SDL3 submodule checkout.
2. **Missing Native Libraries (`jniLibs/arm64-v8a`)**:
   - `libmain.so` (Millions of lines of recompiled Xbox 360 PPC binary)
   - `librexruntime.so` (ReXGlue runtime)
   - `librexgpu-*.so` (Vulkan GPU translation plugins)
   - `libSDL3.so` (SDL3 Android shared library)
   - `libc++_shared.so` (LLVM C++ runtime)
3. **Missing Title Resources**:
   - `src/main/assets/Resources` (Font atlases and private button prompts)
4. **Upstream Release Pipeline**:
   Because compiling the entire static recompilation core and all GPU plugins requires extensive build times and toolchains, upstream releases (`0.5.9.3-xhyn-r2-test12`) package and verify the prebuilt candidate artifact via dedicated release workflows (`.github/workflows/publish-android-test12.yml`), while continuous CI runs regression tests (`android-checks.yml`: PPC calling conventions, UI checks via `aapt2`, Robolectric unit tests, ASan/UBSan host suites, and shader checks).

## 3. Workflow Configuration
In `.github/workflows/android.yml`:
- The workflow compiles and tests `LibertyUniversalCore` natively for ARM64 using the NDK toolchain and runs the complete host test suite.
- If `os/android/app/build/outputs/apk/debug/app-debug.apk` is generated, `actions/upload-artifact@v4` uploads it.
- To prevent silent failures when an APK build is explicitly intended, the upload step fails if expected files are absent when APK generation is active.
