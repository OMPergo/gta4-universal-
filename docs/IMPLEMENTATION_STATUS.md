# Implementation Status

This status distinguishes the gameplay runtime built from the pinned upstream Android submodule from the separate support code in this repository.

| Subsystem | Status | Evidence / remaining work |
| :--- | :--- | :--- |
| XHYN/ReXGlue Android gameplay runtime | Built by CI | Pinned submodule is initialized and its native build script produces the Android gameplay libraries. Physical Galaxy A14 testing has reached gameplay. Stability is not established. |
| Android SDL3 activity and launcher | Integrated from pinned port | `LibertyActivity` hosts SDL3; `LauncherActivity` provides installation, driver, settings, logs, and launch flow. |
| Android native installer | Packaged | CI stages `libliberty_install.so`; installer JNI calls it for source inspection and game installation. |
| Vulkan renderer and driver components | Built by upstream scripts | APK includes the ReXGlue GTA IV GPU plugins and Vulkan proxy. Mali-specific capability/pipeline behavior still needs device validation. |
| Generated GTA IV PowerPC code | Built by upstream native build | Compiled into the gameplay `libmain.so`; not built by the root support-core CMake target. |
| Root hardware capability probing | Implemented | CPU, memory, GPU and Vulkan capability reporting exists in `LibertyRecomp/diagnostics`. |
| Root performance tiers | Implemented as configuration | Standalone support-core configuration; not currently the source of truth for the upstream renderer's Android settings. |
| Root GPU compatibility flags | Implemented as configuration | Standalone flags; actual runtime workarounds are in the pinned upstream renderer/driver path. |
| Multiplayer | Scaffolding only | Session/packet logic exists; no integrated GTA IV world/network multiplayer runtime. |
| Root media-ID validator | Pure validation helper only | Separate from the upstream native installer and not proof of full XEX/TU validation. |
| Root `LibertyUniversalCore` JNI bridge | Built as support library | Its `nativeInitRuntime` bridge is not the SDL gameplay activity's JNI launch contract. |
| Physical-device stability | Not achieved | Galaxy A14 reached gameplay but reported severe frame pacing, low-memory termination, and a separate signal-11 exit. Root cause remains open. |
| iOS gameplay runtime | Not integrated | Current iOS build is a SwiftUI launcher shell; no iOS ReXGlue/SDL3/Metal or MoltenVK gameplay backend is built. |

## Android build checks

The Android workflow initializes pinned dependencies, builds the driver proxy and native gameplay runtime, builds the support core and host tests, checks ARM64 ELF headers and required APK entries, calculates a checksum, and enforces the 200,000,000-byte APK ceiling.

These checks establish build/package integrity, not sustained performance or crash-free device gameplay. See [Android A14 Performance Audit](ANDROID_A14_PERFORMANCE_AUDIT.md).
