# Android Runtime Integration Audit

## Corrected runtime picture

The Android workflow does build and package the actual GTA IV gameplay runtime. It checks out the pinned `vendor/XHYN-IV-recomp` submodule at commit `025986db1a24aec73ebccc60aac4c235fd820da4`, runs the upstream dependency setup, builds the Vulkan driver proxy, and invokes:

```sh
bash vendor/XHYN-IV-recomp/os/android/scripts/build_native.sh 2
```

That build stages the gameplay `libmain.so`, `libliberty_install.so`, ReXGlue runtime/GPU plugins, SDL3, C++ runtime, and game resources. Gradle packages those components with the Android launcher and SDL activity.

The earlier claim that the Android APK lacked ReXGlue and generated GTA IV code was incorrect for this workflow and is superseded by this document.

## Keep the two code layers distinct

The root CMake target builds `LibertyUniversalCore` from:

- Hardware capability detection and performance-tier configuration.
- GPU compatibility flags.
- Multiplayer session/packet scaffolding.
- A media-ID validation helper.
- A small JNI bridge.

This is a separate support library. It is not the gameplay engine, does not create the actual Vulkan device/renderer, and its `UniversalRuntime` initialization path is not the SDL activity's native launch path. Do not use its tier settings as evidence that the upstream renderer is applying those settings.

The gameplay path is the pinned XHYN/ReXGlue build and its Android Java/native interface. `LibertyActivity` extends SDL3's `SDLActivity`; `Installer` loads `libliberty_install.so`.

## What is still not proven

- Sustained playable frame rate on the Galaxy A14 5G.
- Freedom from Android low-memory kills and native signal-11 exits.
- The exact allocation or resource lifetime responsible for process RSS growth.
- Whether Mali's missing `shaderClipDistance` capability is causal to any observed rendering or crash issue.
- Crash-free behavior across different Vulkan drivers and Android devices.
- A playable iOS build. The current iOS target is a SwiftUI launcher shell only.

## Required validation

1. Preserve unstripped native symbols for diagnostic builds.
2. Capture a native tombstone and symbolize it against the exact APK's matching `libmain.so`, ReXGlue, and GPU plugin symbols.
3. Repeat the same in-game route with controlled settings and thermal starting conditions.
4. Correlate frame times and pipeline compile/wait intervals with renderer-owned host allocations, Vulkan heap usage/budget, texture evictions, and pending releases.
5. Validate Mali-specific shader/pipeline behavior separately from memory pressure.
6. Record device model, Android build, driver choice, APK checksum, and game-data/update identity for every test.

See [Android A14 Performance Audit](ANDROID_A14_PERFORMANCE_AUDIT.md) for the current evidence and the recommended controlled settings profile.
