# GTA IV Universal 1.0.2 — Full Integration Execution Gates

This file tracks the release-candidate work. It is not a claim that multiplayer is already functional.

## Source-of-truth repositories

- Universal host: this repository.
- Android runtime submodule: `runtime/xhyn-android`, pinned at `be597d42d12abb04a552fd87e3853028499ae835`.
- The Android APK workflow explicitly checks out that pinned submodule revision. Changes to the runtime must therefore be made in the runtime repository and then consumed by updating the submodule gitlink in this repository.

## Required implementation

### Multiplayer end-to-end
- Trace the GTA IV retail phone multiplayer entry points into the XAM/session APIs.
- Verify community backend configuration, identity/authentication, directory operations, session creation, search, join, leave, invites, and reconnect.
- Verify peer transport initialization and that gameplay datagrams reach the retail multiplayer hooks.
- Verify player/entity/vehicle/event synchronization and host migration behavior actually work; do not infer this from hook presence or documentation.
- Add an explicit capability/status surface for unavailable services. No fake sessions or placeholder success states.

### Mobile in-game integration
- Put multiplayer actions in the retail in-game phone flow, not only in a launcher Activity.
- Extend the existing pause/session menu to expose multiplayer session status and player actions.
- Keep graphics/performance, touch controls, controller, overlay editing, and diagnostics in the same pause/settings system.
- Ensure settings are persisted and applied to the running native runtime, not just displayed in Android UI.

### A14 stability
- Resolve or safely work around Mali-G57 clipping feature gaps with a correctness-preserving fallback.
- Profile CPU/GPU frame costs and address recurring gameplay hitches.
- Fix audio queue starvation and verify queue depth/underruns.
- Diagnose the SIGSEGV with native symbols/backtrace and audit shutdown/lifecycle ordering.
- Add low-memory behavior appropriate to 4–6 GiB devices.

## Build and acceptance gates

1. Host tests and runtime unit tests pass.
2. Android ARM64 release-optimized native runtime and signed debug APK build.
3. APK inspection confirms the expected runtime libraries and Universal package identity.
4. Phone multiplayer flow reaches the configured backend and reports real session state.
5. Two-device test: host, discover/browse, join, exchange gameplay state, disconnect and reconnect.
6. Verify the in-game pause/settings and touch overlay changes persist and affect runtime behavior.
7. A14 play session produces a report with FPS/frame-time distributions, memory, audio underruns, and no native crash.
8. iOS build is attempted against the same runtime changes; report toolchain blockers honestly.

A green compile or APK packaging job is not a substitute for gates 4–8. The release must be described as a build candidate until device-level acceptance passes.
