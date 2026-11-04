# Android Runtime Gap Audit

## Verified repository findings

The current Android CMake target builds `LibertyUniversalCore` from the universal runtime, diagnostics, performance tiers, GPU compatibility, multiplayer session scaffolding, media-ID helper and JNI bridge.

The following required gameplay components are not linked by that target:

- ReXGlue runtime / Xbox 360 guest environment.
- Generated GTA IV PowerPC translation units and game entry point.
- A real Vulkan renderer/device lifecycle.
- XEX metadata parser and Title Update 8 patch/verification pipeline.

### Runtime behavior corrections on the Friday branch

- Vulkan capability detection is no longer reported as successful Vulkan initialization. Startup stops until a real backend exists.
- Finding `default.xex` no longer causes the runtime to accept hard-coded Title ID / Media ID constants as proof of a valid image.
- Engine startup no longer sets `is_engine_ready` or `is_game_code_executing` without invoking a real game entry point.
- Android release packaging no longer explicitly retains native debug symbols.
- Android CI runs for `friday/**` branches and checks that the ARM64 shared library exists, is AArch64, and the APK is no larger than 100 MiB.

These changes intentionally make the current app report that it is not ready rather than falsely claiming GTA IV is running.

## Integration sequence

1. Pin and inspect compatible upstream ReXGlue and Android port revisions, including their licenses and submodules.
2. Import the required runtime and generated GTA IV source as reproducible pinned dependencies.
3. Integrate the actual SDL/Android lifecycle, guest memory, kernel/XAM services, audio, input and renderer.
4. Implement XEX metadata parsing and exact TU8 compatibility checks; validate user-supplied game data.
5. Build ARM64 and run host/unit tests plus device smoke tests.
6. Test on target hardware, measure memory/thermal behavior and confirm the APK stays under 100 MiB.

## Device compatibility caveat

An ARM64 APK build is not proof that a specific Mali GPU or 4 GB RAM device can run GTA IV. Device support must be established by real runtime integration and sustained on-device testing.
