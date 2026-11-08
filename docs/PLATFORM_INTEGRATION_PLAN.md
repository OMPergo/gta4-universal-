# GTA IV Universal: Theft4 + XHYN IV platform integration plan

**Status:** architecture and source audit; no runtime merge or multiplayer claim is made by this document.

**Primary project:** `OMPergo/gta4-universal-`

**Reference implementations:**
- iOS/iPadOS: [KoreanSeats1/Theft4](https://github.com/KoreanSeats1/Theft4)
- Android: [OMPergo/XHYN-IV-recomp-multiplayer-1.0](https://github.com/OMPergo/XHYN-IV-recomp-multiplayer-1.0)
- Shared upstream foundations credited by those projects: [OZORDI/LibertyRecomp](https://github.com/OZORDI/LibertyRecomp), [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk)

## 1. Product decision

GTA IV Universal should be one cross-platform product with multiple platform builds, not one platform-specific app forced to share an identical launcher.

The target is:

- The same GTA IV title/recompilation, supported game revision and TU8 behavior on every platform.
- One shared gameplay/runtime and multiplayer protocol wherever the platform APIs allow it.
- Platform adapters for graphics, audio, storage, lifecycle, networking and controller input.
- A consistent settings model and recognizable Universal design language.
- Native platform UI where necessary: Android can retain/adapt the XHYN IV launcher and controller/settings experience; iOS can retain Theft4's native iOS shell and adapt its visual design to Universal.
- Separate, clearly versioned platform releases (Android, iOS/iPadOS, desktop and future platforms), all published through the Universal release hub.

The app shell and UI may differ. The game simulation, compatible content rules, multiplayer protocol, player identity/session model, and shared settings semantics must not silently diverge.

## 2. What the source audit establishes

### Theft4 (iOS)

Theft4 is not merely a generic emulator wrapper. Its README describes a title-specific Xbox 360 ARM64 ahead-of-time recompilation built on LibertyRecomp and ReXGlue. The project has an iOS UIKit application, iOS runtime integration, and a direct Metal renderer in its 0.3 line. Its 0.3.2 public sideload guide requires iOS/iPadOS 26+, an ARM64 device, a signed installation of its unsigned IPA, and a specifically validated Xbox 360 USA retail game/TU8 pair.

The Theft4 release/build documentation also identifies significant source/build prerequisites: full Xcode on macOS, pinned submodules, offline iPhoneOS shader libraries and host shader libraries, plus the existing legacy MoltenVK/SPIRV-Cross-related archives. A clean source build is therefore more involved than downloading the published IPA.

**Important:** Theft4's own development records distinguish loader/runtime initialization from actual game execution and gameplay. A successful install, XEX/TU8 load, runtime startup, shader compilation, or IPA audit is not by itself proof of a playable build. Use the current release notes and logs for the exact milestone being claimed.

### XHYN IV (Android)

The XHYN IV README identifies it as a fork of the Android LibertyRecomp port. It inherits the game runtime, Android packaging, touch controls, physical controller input, driver support and performance foundations. Its own refinements include:

- A redesigned launcher and in-game menus.
- Separate editable mobile and Xbox touch layouts.
- Physical-controller tuning and preview.
- Graphics/performance settings and overlay editing.
- Save tools, mod management and shareable diagnostic reports.

The XHYN documentation says controller support is built on Android/SDL input. Those settings and interaction patterns are valuable product requirements, but Android-specific Java/Kotlin UI and Android input APIs cannot simply be copied into an iOS target. Their *behavior and settings model* can be shared; each platform needs a native input/UI adapter.

### Current Universal repository

The current default branch is a small CMake project with hardware diagnostics, performance tiers, GPU compatibility helpers, a protocol header, an in-memory authoritative-session model, installer validation and platform workflow files. The iOS workflow currently builds `LibertyUniversalCore`; it does not package Theft4 or a complete GTA IV iOS application.

The Android workflow stages the Universal core into an Android Gradle project, but the repository's own `docs/BUILDING_ANDROID.md` warns that a complete APK requires the actual upstream recompiled PowerPC executable tree and staged runtime assets. The top-level `CMakeLists.txt` does not currently include the full LibertyRecomp/ReXGlue game runtime.

**Multiplayer reality check:** the current Universal `multiplayer_protocol.h` defines packet structures and validation, and `authoritative_session.cpp` maintains an in-memory peer table. The inspected implementation does not yet provide a socket transport, session discovery/relay, cryptographic peer handshake, packet serialization/reliability, game-engine player spawning, or live GTA IV entity synchronization. The documentation's “completed” label must not be interpreted as end-to-end multiplayer being implemented.

## 3. Recommended architecture

Keep one shared game/runtime foundation and place platform-specific app shells and backends around it:

```text
                 GTA IV Universal product/release hub
                               |
              Shared game/runtime + title compatibility
              (LibertyRecomp / ReXGlue / generated AOT)
                               |
        +----------------------+----------------------+
        |                      |                      |
  Shared game services   Shared multiplayer     Shared settings
  save/config/logging    protocol + simulation   schema/profiles
        |                      |                      |
  Platform adapters      Transport adapters      UI adapters
        |                      |                      |
  +-----+------+          +----+-----+          +-----+------+
  |            |          |          |          |            |
 Android      iOS       Android     iOS       XHYN-style   Theft4-style
 SDL/Vulkan   UIKit/     UDP/       Network    Android UI   iOS UI
             Metal      sockets    framework
```

This is a target architecture, not a statement that every box exists today.

### Shared settings and controller model

Create a platform-neutral settings schema for:
- Control layout/profile and button mapping.
- Physical controller deadzones, sensitivity, response curve, inversion, trigger threshold and vibration preference.
- Touch layout geometry, opacity, scaling and context.
- Graphics quality, resolution scaling, frame cap, reflections and compatibility workarounds.
- Audio, accessibility, diagnostics and multiplayer preferences.

Each app renders those values using its native UI. Each platform translates physical inputs into a shared logical gamepad/action model. Android SDL/GameController and Apple's GameController framework (or the SDL iOS backend actually selected by the runtime) remain platform adapters. Do not claim iOS Bluetooth controller parity until a real iOS device test confirms it.

### Shared multiplayer model

Keep a versioned, platform-neutral protocol and gameplay state model. Put transport behind an interface so Android and iOS can use supported UDP/socket APIs and, if direct connectivity fails, a relay. The transport must not depend on Android-only APIs.

Before implementing internet matchmaking, prove the smallest vertical slice:
1. Two clients exchange protocol-version/build/content compatibility information.
2. One client hosts; the other joins over the same Wi-Fi network.
3. Authenticated peer IDs and sequence numbers reject malformed, stale and spoofed packets.
4. A real game hook spawns/updates a guest player entity in the host's world.
5. Position/rotation/animation and disconnect state synchronize without corrupting host story state.
6. The same test passes on Android + Android, then iOS + iOS, then Android + iOS.

Do not start with a large public lobby, 32-player support, voice chat, or cloud accounts. First prove two-player local networking, then add NAT traversal/relay and internet matchmaking.

## 4. Integration strategy: reuse, do not blindly merge

1. **Pin and map source lineage.** Record exact commit SHAs, submodule SHAs, license files, generated-code provenance and platform patches for Theft4 and XHYN IV.
2. **Choose a common runtime baseline.** Compare their LibertyRecomp/ReXGlue revisions and generated game-code revisions. Prefer the common upstream source where possible. Do not copy two divergent full runtimes into Universal and try to reconcile them after the fact.
3. **Treat Theft4 as the iOS platform reference.** Reuse/adapt its UIKit lifecycle, iOS runtime changes, Metal path, file access and build recipes after checking source licensing and dependencies. Keep its tested iOS renderer intact initially; avoid simultaneous renderer rewrites.
4. **Treat XHYN IV as the Android UX/input reference.** Port the product behavior and settings concepts through shared interfaces. Preserve platform-specific controller and touch handling where it is already proven.
5. **Implement shared settings before duplicate UI work.** Define versioned config structures and migration tests, then have Android/iOS frontends edit the same semantics.
6. **Replace the current multiplayer placeholder with a tested vertical slice.** Do not wire menu buttons to pretend networking exists. Add transport, protocol tests, engine hooks, entity lifecycle and two-device tests as separate verifiable changes.
7. **Keep platform builds independent.** Produce separate Android and iOS release artifacts from pinned common runtime commits and platform-specific build configurations. A shared release hub can publish both without pretending the binaries are identical.

## 5. Licensing, provenance and repository hygiene

Both Theft4 and the XHYN IV fork declare GPL-3.0, but their submodules and bundled third-party dependencies may have different licenses. Before copying code, verify each file's origin, license and attribution requirements. Retain upstream notices and artwork credits. Do not copy copyrighted GTA IV game data, title updates, keys, private device logs, shader captures, or user caches into the repository or CI artifacts.

Avoid importing either full repository wholesale. Theft4's repository is very large and contains generated assets, build outputs and platform-specific history. Use pinned source revisions and a deliberate source manifest; include only source and redistributable build inputs.

## 6. Immediate implementation milestones

### M0 — Baseline and provenance (this branch)
- Document the product architecture and verified source boundaries.
- Record exact upstream commits/submodule revisions before any source promotion.
- Correct Universal's status labels so scaffolding/tests are not described as completed gameplay features.

### M1 — Runtime convergence
- Identify the common LibertyRecomp/ReXGlue base revision and generated GTA IV code used by Theft4 and XHYN Android.
- Select the runtime source of truth and create a repeatable pinned dependency import/build process.
- Build the existing Android and iOS platform targets from that shared baseline without changing their proven rendering/input behavior.

### M2 — Settings and controller parity
- Define shared settings/config schema and migration tests.
- Map XHYN's controller and touch-control behavior to platform-neutral actions.
- Implement the iOS controller adapter and native UI controls; test supported Bluetooth controllers on actual iPhones/iPads.

### M3 — Two-player multiplayer proof
- Transport-independent protocol tests and packet validation.
- LAN host/join and real engine entity hooks.
- Android/Android proof, iOS/iOS proof, then Android/iOS cross-play proof.
- Add relay/NAT traversal only after LAN vertical slice is reliable.

### M4 — Universal releases
- Separate platform release versions with a shared compatibility/build manifest.
- Universal branding and consistent feature names, with native platform-specific UI.
- Publish verified artifacts and exact test status; never label compile-only builds playable.

## 7. Acceptance criteria (no false positives)

A platform build is **compile-validated** only when its intended target compiles and the artifact is inspected.

It is **install-validated** only when the artifact is signed appropriately, installs on a supported physical device and opens.

It is **gameplay-validated** only when the user-supplied supported game data passes validation, the actual GTA IV game reaches a controllable 3D scene, and audio/input are checked.

It is **multiplayer-validated** only when two physical devices join the same session and real in-game player state is synchronized in both directions. A lobby, discovery packet, unit test, mock player, or successful socket connection alone does not meet this bar.

## 8. First engineering actions

- Inspect exact upstream and fork commit ancestry and submodule revisions; do not assume the two implementations can be combined without conflicts.
- Build a source manifest that identifies the Theft4 iOS-only changes versus shared LibertyRecomp/ReXGlue changes, and XHYN-only Android UI/input changes versus shared runtime changes.
- Add tests around the current Universal protocol/session scaffolding, then replace it with the real transport/game-hook implementation in incremental pull requests.
- Keep the main branch stable; perform all integration work on feature branches and require CI plus source/license review before merging.
