# GTA IV Universal — iOS/iPadOS Port Plan

**Status:** Active engineering plan  
**Repository:** `OMPergo/gta4-universal-`  
**iOS bundle identifier:** `com.ompergo.gtaivuniversal`  
**Minimum deployment target:** iOS 15.0

## Product objective

The iOS target is a first-class platform in GTA IV Universal, not a separate product and not merely a browser companion. It must share the project's compatibility, game-version, diagnostics, and multiplayer contracts while using native Apple platform APIs and Apple's supported signing/distribution process.

Players supply their own legally obtained GTA IV data and required updates. The project must not package Rockstar game assets.

## Verified starting point

- The repository contains a SwiftUI iOS/iPadOS launcher in `ios/Sources/LauncherView.swift`.
- The launcher can select a folder through Apple's document picker and copy it into app-private storage.
- Settings persist locally through `@AppStorage`.
- The XcodeGen project targets iOS 15 and supports iPhone/iPad device families.
- GitHub Actions has successfully built iPhone Simulator, iPad Simulator, and an unsigned universal device IPA (workflow run 20, commit `cfb352f99b9757b4c9116e3ef7752e6aa9c3e147`).
- The current launcher explicitly reports that the gameplay engine is not connected.
- An unsigned IPA cannot be installed by tapping it on an ordinary iPhone. Apple signing and a supported distribution channel are required.

## Workstreams and exit criteria

### 1. Native app foundation and repeatable CI

- Keep the XcodeGen project in this repository.
- Build iPhone simulator, iPad simulator, and universal iPhone/iPad device targets on macOS CI.
- Validate bundle identifier, minimum OS, device architectures, IPA structure, and checksums.
- Preserve unsigned build artifacts for engineering inspection; label them clearly as not directly installable.
- Add signed export/TestFlight only after the owner configures Apple Developer Program and App Store Connect credentials as protected GitHub secrets.

**Exit:** CI produces validated device and simulator artifacts on every relevant iOS change.

### 2. Game-data and update pipeline

- Keep folder selection through Apple's document picker and security-scoped URL access.
- Implement a non-destructive import workflow with progress, cancellation, free-space checks, duplicate handling, and recovery from interrupted copies.
- Inventory the upstream Android installer and its exact game-data/Title Update 8 validation and extraction requirements before implementing an iOS equivalent.
- Define a platform-neutral manifest for game build, required files, hashes, and update compatibility.
- Do not claim Title Update 8 is installed until the actual required files are validated and the engine confirms the expected game version.

**Exit:** A user can import their own game data, see exactly what is present/missing, and receive a validated compatible game-data manifest.

### 3. Gameplay runtime port

- Audit the pinned XHYN/ReXGlue source and all native dependencies for iOS support, licensing, and platform assumptions.
- Separate portable game/runtime code from Android-only JNI, Android lifecycle, filesystem, input, audio, and graphics code.
- Integrate an iOS-native application lifecycle and SDL3 iOS backend where supported by the upstream code.
- Determine whether the renderer can use a supported iOS graphics path. If it requires Vulkan, evaluate a maintained Vulkan-to-Metal implementation and validate its feature coverage; do not assume Android Vulkan binaries can be reused on iOS.
- Implement controller/touch input, audio session behavior, suspend/resume, app storage, memory pressure handling, and device-specific diagnostics.
- Keep platform-specific code behind explicit interfaces so Android and iOS can share portable runtime and game-version logic.

**Exit:** A signed development build launches the actual game runtime from user-provided files on physical iPhones/iPads, with documented device coverage and repeatable tests.

### 4. Shared multiplayer and cross-play

- Define one versioned, platform-neutral multiplayer protocol and capability handshake.
- Keep transport/session services separate from platform UI and OS-specific networking.
- Integrate multiplayer at actual game-state and gameplay hooks; a standalone socket connection or placeholder session is not sufficient.
- Define authoritative state, player identity, session discovery, reconnect behavior, latency handling, compatibility checks, and anti-cheat/security boundaries.
- Test Android-to-iOS sessions, then Windows and other targets, against the same protocol version.
- Expose connection, latency, packet-loss, session, and compatibility diagnostics.

**Exit:** Real gameplay sessions can be joined across supported platforms and pass repeatable synchronization, reconnect, and compatibility tests.

### 5. Apple distribution

- Use Apple Developer Program/App Store Connect and TestFlight for invited contributor distribution.
- Keep signing certificates, private keys, and API credentials out of source control; store them only as protected CI secrets.
- Publish the iOS TestFlight invitation/instructions beside the Android APK on the project's shared release hub.
- Keep raw unsigned IPA artifacts labelled as engineering artifacts, not user-installable releases.

**Exit:** An invited contributor can install and update the signed iOS app through Apple's TestFlight flow from the shared project page.

## Required physical-device validation

- Install, launch, suspend, resume, and update on a supported iPhone and iPad.
- Import user-supplied game data and verify file access after app relaunch.
- Verify game build and Title Update compatibility using hashes/manifests.
- Measure frame pacing, memory pressure, thermal behavior, storage usage, and crash diagnostics.
- Join a multiplayer session with an Android device and verify gameplay state, reconnect, and latency reporting.
- Record device model, iOS version, build SHA, game-data version, and test result for every test.

## Explicit non-goals

- Do not bypass Apple's signing, sandbox, or distribution requirements.
- Do not distribute copyrighted game data.
- Do not represent the current SwiftUI launcher or unsigned IPA as playable GTA IV.
- Do not advertise cross-play as complete until actual cross-platform gameplay has been tested.
