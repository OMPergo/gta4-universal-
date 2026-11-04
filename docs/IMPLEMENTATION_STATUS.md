# Implementation Status

This status reflects source code present in this repository, not roadmap descriptions or intended architecture.

| Subsystem | Status | Evidence / remaining work |
| :--- | :--- | :--- |
| Hardware capability probing | Implemented | CPU, memory, GPU and Vulkan capability reporting exists. |
| Performance tiers | Implemented as configuration | Tier selection/settings exist; actual in-game dynamic resolution and FSR rendering are not integrated. |
| GPU compatibility flags | Implemented as configuration | Workaround flags exist; they do not replace a functioning graphics backend. |
| Multiplayer | Scaffolding only | Session/packet logic exists; no integrated GTA IV world/network runtime. |
| Media ID validator | Pure validation helper only | It accepts supplied IDs; runtime does not yet parse IDs from the XEX or validate TU8 files. |
| Android JNI bridge | Implemented | Exposes initialization and status calls to the Android app. |
| Android ARM64 support-library build | CI configured | Does not establish that the game runtime is linked or playable. |
| Vulkan device initialization | Not implemented | Capability detection exists; instance/device/queue/swapchain creation is absent. |
| ReXGlue runtime | Not integrated | Required runtime sources and Android linkage are absent from this build. |
| Generated GTA IV PowerPC code | Not integrated | No generated translation-unit set or actual game entry point is built by the current CMake target. |
| XEX/TU8 installation and verification | Not implemented | Needs metadata parsing, update matching, extraction and integrity checks. |
| Playable Android GTA IV | Not achieved | Must not be claimed until the runtime is integrated and tested on-device. |

## Build size

The Android workflow enforces a 100 MiB APK ceiling for the application package. Game data is expected to remain external. Passing that size check does not imply that the game runtime is complete.
