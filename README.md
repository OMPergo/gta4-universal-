# Liberty City Universal

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
