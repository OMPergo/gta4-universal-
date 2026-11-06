# Liberty City Universal

<!-- PROJECT_STATUS_START -->
## Project status and downloads

- **Build status:** [![Android ARM64 CI](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml/badge.svg?branch=friday%2Fandroid-runtime-audit)](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml)
- **Latest Android APK (public direct download):** [Download app-debug.apk](https://github.com/OMPergo/gta4-universal-/releases/download/android-latest/app-debug.apk)
- **Latest Android release page:** [GTA IV Universal — Latest Android Build](https://github.com/OMPergo/gta4-universal-/releases/tag/android-latest)
- **Versioned releases:** [GitHub Releases](https://github.com/OMPergo/gta4-universal-/releases)
- **Every build and its logs:** [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- **Commit history:** [Friday integration branch](https://github.com/OMPergo/gta4-universal-/commits/friday/android-runtime-audit)
- **Known issues and regression-prevention ledger:** [Build Failure Ledger](docs/BUILD_FAILURE_LEDGER.md)
- **Galaxy A14 5G findings and recommended test profile:** [Android A14 Performance Audit](docs/ANDROID_A14_PERFORMANCE_AUDIT.md)
- **iOS/iPadOS CI:** [Apple build workflow](https://github.com/OMPergo/gta4-universal-/actions/workflows/ios.yml)
- **iOS architecture and port milestones:** [Universal iOS Port Plan](docs/IOS_UNIVERSAL_PORT_PLAN.md)
- **iOS installation and TestFlight roadmap:** [iOS installation roadmap](docs/IOS_INSTALLATION_ROADMAP.md)
- **Automatic latest Android build:** every successful Android push build on this integration branch refreshes the public `android-latest` release and replaces its APK/checksum assets. Versioned `android-vX.Y.Z` releases remain available separately.
- **iOS status:** CI builds an unsigned universal iPhone/iPad launcher package plus a simulator archive. The IPA requires Apple signing before device installation; it is not a playable GTA IV port yet.

> The `android-latest` link is a shared development APK for contributors. It is refreshed only after CI succeeds. It is not yet a stable signed release; until a stable signing key is configured, updates from different builds may require a data-safe reinstall.
<!-- PROJECT_STATUS_END -->

GTA IV Universal is intended to be one cross-platform project—not unrelated platform launchers. The goal is to support Android, iOS/iPadOS, Windows, and other viable targets with platform-native builds, user-supplied legitimate game data, shared compatibility components, diagnostics, and genuine cross-platform multiplayer.

> **Current reality:** Android builds the pinned XHYN native GTA IV runtime and has reached physical-device gameplay, but performance and memory stability remain under investigation. iOS currently builds a SwiftUI launcher/game-folder manager shell; the GTA IV gameplay runtime and multiplayer integration have not yet been ported and validated on iOS.

## Universal project requirements

- Each platform must use its own supported native build, graphics, input, audio, storage, and lifecycle integration.
- Players provide their own legally obtained GTA IV game files and required updates; the project does not distribute Rockstar game assets.
- Game-version and Title Update compatibility must be validated rather than assumed.
- Multiplayer must use a shared, versioned protocol and compatible game-state integration. A protocol header or networking scaffold alone is not working cross-play.
- Diagnostics should report platform, build, runtime, graphics, memory, performance, and network health where the platform permits.
- Platform builds and installation instructions belong under this same repository and shared release hub.

## Android build

- Target ABI: `arm64-v8a`
- Minimum Android API: 29
- Android build workflow: [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- The workflow checks out the pinned XHYN submodule, runs its dependency setup, builds the native runtime and driver proxy, stages native libraries/resources, then packages the app with Gradle.
- APK size ceiling: 200,000,000 bytes; game data remains separate.

A successful CI build proves that native libraries and the Android APK compile and pass packaging checks. It does **not** prove that a device can sustain gameplay without crashes, stutter, or thermal/memory pressure.

## iOS and iPadOS build

- Xcode project specification: `ios/project.yml`
- Bundle identifier: `com.ompergo.gtaivuniversal`
- Minimum deployment target: iOS 15.0
- CI builds iPhone and iPad simulator targets and an unsigned universal device IPA.
- The unsigned IPA is a build artifact, not a tap-to-install iPhone download. Device installation requires Apple-approved signing and distribution.
- See [Universal iOS Port Plan](docs/IOS_UNIVERSAL_PORT_PLAN.md) for the staged engine, multiplayer, and validation work.

## Runtime and shared code

- **Android gameplay runtime:** built from the pinned `vendor/XHYN-IV-recomp` submodule using its ReXGlue/SDL3 Android build scripts.
- **Universal support core:** the root `CMakeLists.txt` builds support code for diagnostics, performance-tier experiments, GPU flags, multiplayer scaffolding, media-ID helpers, and a JNI bridge. It is not the game's main execution library.
- **iOS target:** currently SwiftUI launcher and game-folder management only. Porting the gameplay runtime requires separate engineering for iOS platform services and a supported graphics backend.

See [Implementation Status](docs/IMPLEMENTATION_STATUS.md), [Android Runtime Integration Audit](docs/ANDROID_RUNTIME_GAP_AUDIT.md), [Multiplayer](docs/MULTIPLAYER.md), and [Universal iOS Port Plan](docs/IOS_UNIVERSAL_PORT_PLAN.md).
