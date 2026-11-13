# Runtime source manifest

This file records the exact source revisions used by GTA IV Universal. Update it only alongside a reviewed source change.

## iOS runtime source

- Repository: https://github.com/KoreanSeats1/Theft4
- Path in this repository: `runtime/theft4` (Git submodule)
- Pinned commit: `88115fefad59cfcb62d17c0c9548a7a900c01a23`
- Role: iOS runtime, UIKit/Objective-C++ shell, platform integration, renderer, AOT runtime source.
- Build gate: the pinned Theft4 iOS core-only device/simulator matrix. This is not a signed app or IPA.

## Android runtime source

- Repository: https://github.com/OMPergo/XHYN-IV-recomp-multiplayer-1.0
- Pinned audit commit: `be597d42d12abb04a552fd87e3853028499ae835`
- Path in this repository: `runtime/xhyn-android` (Git submodule)
- Role: ARM64 Android runtime and native APK source, including installer, SDL/input, touch controls, GPU/driver and performance implementation.
- Build gate: Universal's opt-in Android native-runtime and debug-APK targets. The APK is unsigned and requires the user's legally obtained game files/TU8 to be installed separately.

## Shared project status

- Universal diagnostics/performance/GPU/session core remains at the repository root.
- The iOS and Android platform runtime sources are both present as separately pinned submodules, each with an opt-in build path.
- Android and iOS retain platform-specific implementations while the Universal root exposes opt-in build targets; generated runtime code is not blindly combined.
- No GTA IV user data, TU8, title-update files, keys, saves, private crash reports, Apple credentials, or provisioning profiles belong in this repository.
- A green core/link build is compile validation only. Device installation, GTA IV startup, playable controls/audio, and two-device multiplayer each require separate proof.
