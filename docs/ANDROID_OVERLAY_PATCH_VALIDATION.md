# Android overlay patch validation

This branch exists to run the Android integration workflow against the Universal-owned Android runtime overlay before it is merged into the Android milestone branch.

## Source contract

- Android runtime submodule must resolve to `be597d42d12abb04a552fd87e3853028499ae835`.
- The overlay patch is applied by `tools/apply_android_multiplayer_runtime.py`.
- The patch must fail closed if the pinned source revision or any expected anchor differs.
- The A14 preset must submit only keys present in the pinned JNI `mobile_settings` allowlist.

## Required CI checks

1. Universal host CMake build and CTest.
2. Pinned Android runtime initialization and submodule revision check.
3. All Universal-owned runtime overlays apply without anchor failures.
4. ARM64 native runtime and Vulkan bridge compile.
5. Debug and unsigned release APK assembly.
6. Debug APK package identity, signature and required ARM64 libraries are verified.
7. Artifact SHA-256 values are recorded.

## Device acceptance remains separate

A successful CI build is not proof of game startup, stable frame pacing, correct Mali clipping, crash-free shutdown, or working multiplayer. The Galaxy A14 test must still use legally obtained GTA IV files and TU8, capture diagnostics, and report memory/frame-time/audio/shutdown behavior. Multiplayer passes only after two Android devices join the same LAN session and exchange synchronized player and vehicle/gameplay state.
