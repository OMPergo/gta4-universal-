# Android LAN multiplayer validation

This is the Android-only multiplayer path currently implemented in the pinned runtime.

## Current capability

- The Android runtime defaults to the XAM LAN session directory.
- LAN discovery uses UDP port **36002** by default.
- The game network port is reported separately by the runtime; it is not the LAN discovery port.
- The in-game pause/session panel has a **Multiplayer** tab. It reads live runtime state and reports whether the LAN directory is ready, the active session ID, and both ports.
- Host and join operations remain in the retail GTA IV in-game phone/session flow. The pause tab is status and diagnostics, not a simulated lobby.

**Not available yet:** Android community/internet matchmaking. The Android community backend is deliberately an unavailable stub because this build does not yet provide the required Android HTTPS directory client and peer transport. Same-Wi-Fi LAN testing is the supported first validation target; do not claim cross-network or Android-to-iOS matchmaking.

## Two-device test

1. Install the same Android build on both devices.
2. Put both devices on the same Wi-Fi network. Disable VPNs and guest-network/client isolation for this test.
3. Launch the game with the same GTA IV title/update compatibility.
4. Open the GTA IV phone and use its Multiplayer flow to create a LAN session on device A.
5. On device B, use the phone's Multiplayer flow to search for and join that session.
6. Confirm both devices reach the same session, and that each sees the other player's in-game presence.
7. Move, enter a vehicle, and test a gameplay event. Record whether player/entity/vehicle state is actually synchronized.
8. Leave and rejoin. Repeat after one device backgrounds and resumes the game.
9. Open the pause panel's Multiplayer tab on each device and record directory readiness, session ID, discovery port, and game network port.

## Pass criteria

A session appearing in the list is not enough. The test passes only when both clients join the same session and exchange actual gameplay state, including player movement and at least one vehicle/event update, without a crash or stuck session after reconnect.

Attach the in-game diagnostic ZIP and note which steps passed or failed.