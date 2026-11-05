# Liberty City Universal

<!-- PROJECT_STATUS_START -->
## Project status and downloads

- **Build status:** [![Android ARM64 CI](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml/badge.svg?branch=friday%2Fandroid-runtime-audit)](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml)
- **Latest tagged releases:** [GitHub Releases](https://github.com/OMPergo/gta4-universal-/releases)
- **Every build and its logs:** [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- **Commit history:** [Friday integration branch](https://github.com/OMPergo/gta4-universal-/commits/friday/android-runtime-audit)
- **Known issues and regression-prevention ledger:** [Build Failure Ledger](docs/BUILD_FAILURE_LEDGER.md)
- **Galaxy A14 5G findings and recommended test profile:** [Android A14 Performance Audit](docs/ANDROID_A14_PERFORMANCE_AUDIT.md)
- **Release procedure:** create a tag named `android-vX.Y.Z` on the commit to publish; the Android workflow must pass before the APK is attached to that release.

> Android APKs are development builds until signed with the stable release key and validated on target devices. Untagged APK artifacts are available from their corresponding Actions runs. A stable signing key is required for seamless install-over updates across releases.
<!-- PROJECT_STATUS_END -->

An Android-focused GTA IV recompilation project based on the Xbox 360 release. The Android app uses the pinned XHYN/ReXGlue Android port as its gameplay runtime.

> **Android runtime status:** The Android workflow builds the pinned XHYN native GTA IV runtime, SDL3, Vulkan renderer/driver components, installer, and launcher into an ARM64 APK. Physical-device testing has reached gameplay on a Samsung Galaxy A14 5G, but performance and memory stability are not yet acceptable; low-memory termination and a separate native signal-11 exit remain under investigation.

## Android build

- Target ABI: `arm64-v8a`
- Minimum Android API: 29
- Android build workflow: [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- The workflow checks out the pinned XHYN submodule, runs its dependency setup, builds the native runtime and driver proxy, stages native libraries/resources, then packages the app with Gradle.
- APK size ceiling: 200,000,000 bytes; game data remains separate.

A successful CI build proves that native libraries and the Android APK compile and pass packaging checks. It does **not** prove that a device can sustain gameplay without crashes, stutter, or thermal/memory pressure.

## Two native code layers

- **Gameplay runtime:** built from the pinned `vendor/XHYN-IV-recomp` submodule using its ReXGlue/SDL3 Android build scripts. This supplies the actual game entry point, renderer, and installer libraries packaged into the Android APK.
- **Universal support core:** the root `CMakeLists.txt` builds `LibertyUniversalCore` for hardware diagnostics, performance-tier experiments, GPU flags, multiplayer scaffolding, media-ID helpers, and a separate JNI bridge. It is not the game's main execution library and its configuration values must not be assumed to control the upstream renderer.

See [Implementation Status](docs/IMPLEMENTATION_STATUS.md), [Android Runtime Integration Audit](docs/ANDROID_RUNTIME_GAP_AUDIT.md), and [Android A14 Performance Audit](docs/ANDROID_A14_PERFORMANCE_AUDIT.md).

## Game data

This project does not distribute copyrighted GTA IV game assets. The Android installer expects the user to supply compatible game data and the required Title Update. The native installer is packaged as `libliberty_install.so`; the root support-core media-ID helper is separate and is not a substitute for the native installer’s complete validation path.

## iOS status

The iOS target currently builds a SwiftUI launcher/game-folder manager shell. It does not yet contain the Android ReXGlue/SDL3 runtime or an iOS-compatible native gameplay backend. A successful iOS launcher build is not a playable GTA IV iOS port.
