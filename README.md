# Liberty City Universal

<!-- PROJECT_STATUS_START -->
## Project status and downloads

- **Build status:** [![Android ARM64 CI](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml/badge.svg?branch=friday%2Fandroid-runtime-audit)](https://github.com/OMPergo/gta4-universal-/actions/workflows/android.yml)
- **Latest tagged releases:** [GitHub Releases](https://github.com/OMPergo/gta4-universal-/releases)
- **Every build and its logs:** [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- **Commit history:** [Friday integration branch](https://github.com/OMPergo/gta4-universal-/commits/friday/android-runtime-audit)
- **Known issues and regression-prevention ledger:** [Build Failure Ledger](docs/BUILD_FAILURE_LEDGER.md)
- **Release procedure:** create a tag named `android-vX.Y.Z` on the commit to publish; the Android workflow must pass before the APK is attached to that release.

> Releases are development milestones until an on-device launch and gameplay test is recorded. APK artifacts from untagged commits remain available in their corresponding Actions run. A stable signing key is required for seamless install-over updates across releases.
<!-- PROJECT_STATUS_END -->


An Android-focused GTA IV recompilation project based on the Xbox 360 release. This repository is under active integration work.

> **Important runtime status:** The current Android APK is a launcher/support build, not yet a playable GTA IV port. The ReXGlue runtime, generated PowerPC game code, real Vulkan device backend, and complete XEX/TU8 installation pipeline are not yet integrated into this repository's Android build.

## Android build

- Target ABI: `arm64-v8a`
- Minimum Android API: 29
- Android build workflow: [GitHub Actions](https://github.com/OMPergo/gta4-universal-/actions)
- APK size target: 100 MiB maximum for the application package; game data remains separate.

A successful CI build proves that the current Android application and support library compile. It does **not** prove that GTA IV gameplay works or that a device is compatible.

## Game data

This project does not distribute copyrighted GTA IV game assets. A future playable build will require users to supply compatible game data and the matching Title Update. The current code only checks for the presence of `default.xex`; it does not yet parse its real Title ID / Media ID or validate TU8 contents. Do not treat the present installer/runtime status as game-data verification.

## Current implementation

- Hardware capability reporting and performance-tier selection.
- GPU compatibility/workaround configuration.
- Multiplayer packet/session scaffolding.
- Android JNI bridge and ARM64 support-library build.
- Early game-data detection.

## Not yet integrated

- ReXGlue guest runtime and Xbox 360 kernel/XAM compatibility.
- Generated GTA IV PowerPC translation units and actual game entry point.
- Real Vulkan instance/device/queue/swapchain initialization and renderer.
- XEX metadata parsing and verified Title Update 8 application.
- Complete game-data extraction and launch lifecycle.
- End-to-end gameplay validation on physical Android devices.

See [Implementation Status](docs/IMPLEMENTATION_STATUS.md) and [Android Runtime Gap Audit](docs/ANDROID_RUNTIME_GAP_AUDIT.md) for the verified status and next steps.
