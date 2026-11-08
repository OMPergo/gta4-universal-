# Runtime source manifest

This file records the exact source revisions used by GTA IV Universal. Update it only alongside a reviewed source change.

## iOS runtime source

- Repository: https://github.com/KoreanSeats1/Theft4
- Path in this repository: `runtime/theft4` (Git submodule)
- Pinned commit: `88115fefad59cfcb62d17c0c9548a7a900c01a23`
- Role: iOS runtime, UIKit/Objective-C++ shell, platform integration, renderer, AOT runtime source.
- Build gate: the pinned Theft4 iOS core-only device/simulator matrix. This is not a signed app or IPA.

## Android reference source

- Repository: https://github.com/OMPergo/XHYN-IV-recomp-multiplayer-1.0
- Pinned audit commit: `be597d42d12abb04a552fd87e3853028499ae835`
- Role: Android implementation reference for runtime, installer, SDL/input, touch controls, GPU/driver and performance work.
- Integration policy: keep Android platform glue in the Android adapter; port platform-neutral fixes only after comparing generated-code/API provenance with the iOS runtime.

## Shared project status

- Universal diagnostics/performance/GPU/session core remains at the repository root.
- The iOS runtime source is now present as a pinned submodule.
- The Android runtime is not duplicated as a second submodule; it remains a pinned comparison source to avoid duplicate generated AOT code.
- No GTA IV user data, TU8, title-update files, keys, saves, private crash reports, Apple credentials, or provisioning profiles belong in this repository.
- A green core/link build is compile validation only. Device installation, GTA IV startup, playable controls/audio, and two-device multiplayer each require separate proof.
